#ifndef GURUWEBSERVER_H
#define GURUWEBSERVER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>

#include "LittleFS.h"
#include "CRUDaLittle.h"

class  GuruWebServer {
    public:
        GuruWebServer();
        void serveWWW();
        bool initializeWiFi();
        CRUDaLittle* _CRUD;
    private:
        /* data */
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
        // Get config. JSON
        String i1ON     = "input_1ON";
        String i1OFF    = "input_1OFF";
        String i2ON     = "input_2ON";
        String i2OFF    = "input_2OFF";
        String i3ON     = "input_3ON";
        String i3OFF    = "input_3OFF";
};

#endif