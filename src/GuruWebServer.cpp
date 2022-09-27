#include "GuruWebServer.h"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Search for parameter in HTTP POST request
const char* POSTed_SSID     = "ssid";
const char* POSTed_PWD      = "pass";
const char* POSTed_IP       = "ip";
const char* POSTed_Gateway  = "gateway";
//Variables to save values from HTML form
String SSID;
String PWD;
String IP;
String Gateway;
// File paths to save input values permanently
const char* Path2_SSID      = "/ssid";
const char* Path2_PWD       = "/pass";
const char* Path2_IP        = "/ip";
const char* Path2_Gateway   = "/gateway";
const char* Path2_Dummy     = "/dummy";
// WiFi Configuration Settings
IPAddress localIP;        //IPAddress localIP(192, 168, 1, 200); // hardcoded
IPAddress localGateway;   //IPAddress localGateway(192, 168, 1, 1); //hardcoded
IPAddress subnet(255, 255, 255, 0);
IPAddress DNS(1, 1, 1, 1);

// Get config. JSON
String i1ON     = "input_1ON";
String i1OFF    = "input_1OFF";
String i2ON     = "input_2ON";
String i2OFF    = "input_2OFF";
String i3ON     = "input_3ON";
String i3OFF    = "input_3OFF";

GuruWebServer::GuruWebServer(/* args */) {

}

// File System Operations
void GuruWebServer::initializeaLittle() {
    /*
    * Mount the file-system
    */
    if (!LittleFS.begin()) {
        Serial.println("Mounted no LittleFS!");
        return;
    }
    Serial.println("Mounted LittleFS successfully!");
}

void GuruWebServer::readaLittle(String* file_content, const char* file_path) {
    /*
    * Read contents of the named file into a string.
    * Args:
    *   file_content is a String and passed by reference. (I.e. a pointer to String is passed.)
    *   file_path is a C string (array of char) specifying the path to file on local file-system. 
    * Result:
    *   The output, when file is read successfully, is printed to the String passed as arg.
    * Note:
    *   The implementation does not return a result. Take a reference to String and pass it as arg.
    *   The function writes the result at the location provided, making it available.
    */
    Serial.printf("Reading file: %s\r\n", file_path);

    File file = LittleFS.open(file_path, "r");
    if (!file) {
        Serial.println("Read NO file!");
        return;
    }

    Serial.print("File Contents: ");
    while (file.available()) {
        *file_content = file.readStringUntil('\n');
        break;
    }
    Serial.println(*file_content);
    file.close();
}

void GuruWebServer::writeaLittle(const char* message, const char* file_path) {
    /*
    * Scribe a message to the named file.
    * Args: 
    *   message is a C string (i.e. array of char) containing the message.
    *   file_path is a C string specifying the path to file on local file-system.
    * Result:
    *   The text message is persisted to file, previous contents over-written.
    */
    Serial.printf("Writing file: %s\r\n", file_path);

    File file = LittleFS.open(file_path, "w");
    if (!file) {
        Serial.println("Wrote NO file!");
        return;
    }

    Serial.println(file.print(message) ? "Wrote message successfully!" : "Wrote no message!"); 
    file.close(); 
}

bool GuruWebServer::initializeWiFi() {
    /*
    * Make WiFi connection when SSID and PWD are available
    * and set up station for web-service.
    */
    // Check
    this->initializeaLittle();

    // Connect to WiFi
    this->readaLittle(&SSID, Path2_SSID); 
    this->readaLittle(&PWD, Path2_PWD);
    this->readaLittle(&IP, Path2_IP);
    this->readaLittle(&Gateway, Path2_Gateway);
    String conn_ack = (String)"SSID <" + SSID + "> | PWD <" + PWD + "> | IP <" + IP + "> | Gateway <" + Gateway + ">";

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

        server.on("/schedule", HTTP_POST, [](AsyncWebServerRequest* request) {
            // Process
            if (request->hasParam(i1ON)) {
                Serial.print(request->getParam(i1ON)->name());
                Serial.println(request->getParam(i1ON)->value());
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
                        this->writeaLittle(SSID.c_str(), Path2_SSID); 
                    } 
                    if (p->name() == POSTed_PWD) {
                        PWD = p->value().c_str();
                        this->writeaLittle(PWD.c_str(), Path2_PWD);
                    }
                        if (p->name() == POSTed_IP) {
                        IP = p->value().c_str();
                        this->writeaLittle(IP.c_str(), Path2_IP);
                    } 
                    if (p->name() == POSTed_Gateway) {
                        Gateway = p->value().c_str();
                        this->writeaLittle(Gateway.c_str(), Path2_Gateway);
                    }           // end IF name()
                }               // end IF isPost()
            }                   // end FOR
            request->send(200, "text/plain", "Done! Will restart and make WiFi connection. Go to IP address: " + IP);
            delay(3000);
            ESP.restart();
        });                     // end ON HTTP POST request
        server.begin();
    }                           // end IF WiFi initialized
} // end Fn


  




