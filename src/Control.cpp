#include "Control.h"

Control::Control() {
    char configJSON[] = CONFIG_JSON;
    StaticJsonDocument<256> configDOC;
    DeserializationError err = deserializeJson(configDOC, configJSON);
    /*
    * TODO 
    * RREPLACE BY MECHANISM TO READ CONFIGURATION FILE   
    * AND INITIALIZE THE SET OF ESOCKET STRUCTURES FROM JSON.
    * AN ESOCKET REPRESENTS 1 H/W DEVICE's ONE ON/OFF CYCLE 
    * IN A 24-HOUR PERIOD. EACH H/W DEVICE HAS A UNIQUE NUMBER ID.
    * SOCKETS REPRESENTING DIFFERENT ON/OFF CYCLES OF THE SAME 
    * DEVICE WILL SHARE THE ID. 
    */
    for (int i=0; i < NUMBER_OF_ESOCKETS; i++) { 
        struct eSocket _eSocket;
        _eSocket.ID = configDOC[i]["ID"]; // 1, 2, 3
        /* 
        * Represent time in seconds since epoch and 
        * use it in calculating time difference
        * between current (clock) time and target time.
        * For action scheduled at a fixed time each day, 
        * pick any reference date for year, month and day.
        * Gurudev's b'day is 05/13/1956. Use it.
        */
        _eSocket.t_ON.tm_year   = 56;
        _eSocket.t_ON.tm_mon    = 4;
        _eSocket.t_ON.tm_mday   = 13;
        _eSocket.t_OFF.tm_year  = 56;
        _eSocket.t_OFF.tm_mon   = 4;
        _eSocket.t_OFF.tm_mday  = 13;
        _eSocket.code_ON        = configDOC[i]["code_ON"];
        _eSocket.code_OFF       = configDOC[i]["code_OFF"];
        this->_eSockets[i] = _eSocket;
    }
}

void Control::initialize_deltas(struct tm t_now) {
    for (int i=0; i < NUMBER_OF_ESOCKETS; i++) { 
        time_t raw_t_now = mktime(&t_now);

        time_t raw_t_ON     = mktime(&(this->_eSockets[i].t_ON)); 
        time_t raw_t_OFF    = mktime(&(this->_eSockets[i].t_OFF));
        
        this->_eSockets[i].delta_on     = difftime(raw_t_ON, raw_t_now);
        this->_eSockets[i].delta_off    = difftime(raw_t_OFF, raw_t_now);
    }
};

void Control::advanceCursor1s() {
    for (int i=0; i < NUMBER_OF_ESOCKETS; i++) { 
        this->_eSockets[i].delta_on     -= 1;
        this->_eSockets[i].delta_off    -= 1;
    }
}

