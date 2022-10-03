#ifndef CRUDALITTLE_H
#define CRUDALITTLE_H

#include <Arduino.h>
#include "LittleFS.h"

class CRUDaLittle {
    public:
        CRUDaLittle();
        void readaLittle(String*, const char*);
        void writeaLittle(const char*, const char*);
    private:
};

#endif