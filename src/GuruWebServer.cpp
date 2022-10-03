#include "GuruWebServer.h"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// WiFi Configuration Settings
IPAddress localIP;        //IPAddress localIP(192, 168, 1, 200); // hardcoded
IPAddress localGateway;   //IPAddress localGateway(192, 168, 1, 1); //hardcoded
IPAddress subnet(255, 255, 255, 0);
IPAddress DNS(1, 1, 1, 1);

GuruWebServer::GuruWebServer(/* args */) {

}

bool GuruWebServer::initializeWiFi() {
    /*
    * Make WiFi connection when SSID and PWD are available
    * and set up station for web-service.
    */
    // Check

    // Connect to WiFi
    this->_CRUD->readaLittle(&this->SSID, this->Path2_SSID); 
    this->_CRUD->readaLittle(&this->PWD, this->Path2_PWD);
    this->_CRUD->readaLittle(&this->IP, this->Path2_IP);
    this->_CRUD->readaLittle(&this->Gateway, this->Path2_Gateway);
    String conn_ack = (String)"SSID <" + this->SSID + "> | PWD <" + this->PWD + "> | IP <" + IP + "> | Gateway <" + Gateway + ">";

    if (SSID == "" || PWD == "") {  // Files not populated!
        Serial.println("Found NO SSID or PWD!");
        return false;
    }

    WiFi.mode(WIFI_STA);
    /* Configure no IP address!
    localIP.fromString(IP.c_str());
    localGateway.fromString(Gateway.c_str());

    if (!WiFi.config(localIP, localGateway, subnet)) {
        Serial.println("Configured NO station!");
        return false;
    }
    */
    WiFi.begin(SSID, PWD);

    unsigned long _now   = millis();
    unsigned long _then  = _now;    // Marker for ticker
    unsigned long _stop  = _now;    // Marker for ATTEMPT NO FURTHER
    unsigned long delta_minor = 0;  // Time since last tick
    unsigned long delta_major = 0;  // Time since start
    while (WiFi.status() != WL_CONNECTED) { 
        _now = millis();
        delta_minor = _now - _then; // Time since last tick
        delta_major = _now - _stop; // Time since start

        if (delta_minor > 1000) {   // Mark time and reset delta
            _then = _now;
            Serial.print (".");
        }
        if (delta_major > 60000) {  // Stop attempting connection
            String timely = (String)"Now " + _now + " Stop " + _stop + " Diff " + delta_major;
            Serial.println(timely);
            Serial.println("Made NO WiFi connection!");
            return false;           // Exit false
        }                           // end IF
    }                               // end WHILE WiFi 

    Serial.println(WiFi.localIP());
    return true;
}

void GuruWebServer::serveWWW() {
    if (this->initializeWiFi()) {
        /*
        * Serve the UI for configuring home automation settings.
        */
        server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
            request->send(LittleFS, "./scheduler.html", "text/html");
        });

        server.on("/schedule", HTTP_POST, [&, this](AsyncWebServerRequest* request) {
            // Process
            if (request->hasParam(this->i1ON.c_str(), true)) {
                Serial.print(request->getParam(this->i1ON, true)->name());
                Serial.println(request->getParam(this->i1ON, true)->value());
            } else {
                Serial.print(request->getParam(this->i1ON, true)->name());
                Serial.println(request->getParam(this->i1OFF, true)->value());
            }
            request->send(LittleFS, "./scheduler.html", "text/html");

        });

        server.serveStatic("/", LittleFS, "/");

        server.begin();
    } else {
        /*
        * Serve the UI for configuring WiFi settings.
        */
        Serial.println("Setting up WAP!");
        WiFi.softAP("ESP-WIFI-MANAGER", NULL);

        IPAddress WAP_IP = WiFi.softAPIP();
        Serial.print("Got IP address as: ");
        Serial.println(WAP_IP);
        server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
            request->send(LittleFS, "./wifimanager.html", "text/html");
        });

        server.serveStatic("/", LittleFS, "/");

        server.on("/", HTTP_POST, [&, this](AsyncWebServerRequest* request) {
            /*
            * The path operation function is a lamda. As such,
            * the `this` self-reference of class GuruWebServer 
            * is no longer visible in the body of the lamda fn, 
            * making class attributes and methods inaccessible. 
            * To fix this, provide the lamda function context 
            * via the lamda-introducer. The prototypical lamda:
            * []  ()  mutable throw() -> type {lambda body}
            * Here, lamda introducer is the rectangular brackets
            * and captures context for  the lamda' use. 
            * For details, refer the thread on Arduino forum:
            * https://forum.arduino.cc/t/problem-with-is-not-captured-error-on-compile/394052/8
            */ 
            int params = request->params();
            for (int i=0; i<params; i++) {
                AsyncWebParameter* p = request->getParam(i);
                if (p->isPost()) {
                    if (p->name() == POSTed_SSID) {
                        SSID = p->value().c_str();
                        this->_CRUD->writeaLittle(SSID.c_str(), Path2_SSID); 
                    } 
                    if (p->name() == POSTed_PWD) {
                        PWD = p->value().c_str();
                        this->_CRUD->writeaLittle(PWD.c_str(), Path2_PWD);
                    }
                        if (p->name() == POSTed_IP) {
                        IP = p->value().c_str();
                        this->_CRUD->writeaLittle(IP.c_str(), Path2_IP);
                    } 
                    if (p->name() == POSTed_Gateway) {
                        Gateway = p->value().c_str();
                        this->_CRUD->writeaLittle(Gateway.c_str(), Path2_Gateway);
                    }           // end IF name()
                }               // end IF isPost()
            }                   // end FOR
            request->send(200, "text/html", "Done! Will restart and make WiFi connection. Go to IP address: " + IP);
            delay(3000);
            ESP.restart();
        });                     // end ON HTTP POST request
        server.begin();
    }                           // end IF WiFi initialized
} // end Fn


  




