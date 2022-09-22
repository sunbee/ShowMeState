#ifndef GURU_WEBSERVER_H
#define GURU_WEBSERVER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>



#include "LittleFS.h"

class  GuruWebServer {
    public:
        GuruWebServer();
        void serveWWW();
        void initializeaLittle();
        void readaLittle(String*, const char*);
        void writeaLittle(const char*, const char*);
        bool initializeWiFi();
    private:
        /* data */
        bool status = false;
        String message;
};

#endif