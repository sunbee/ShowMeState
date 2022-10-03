#include "Control.h"

Control::Control(CRUDaLittle* _CRUD) {
    // Configure Tx
    this->_switch.enableTransmit(0);     // ESP8266-12E GPIO#0 ~ D3
    this->_switch.setProtocol(1);        // Protocol no. (default is 1, will work for most outlets)
    this->_switch.setPulseLength(320);   // Pulse length (optional)
    this->_switch.setRepeatTransmit(3);  // Number of tx repetitions (optional)

    this->_CRUD = _CRUD;
    // char configJSON[] = CONFIG_JSON;
    this->_CRUD->readaLittle(&this->configJSON, this->configJSON_path);
    StaticJsonDocument<384> configDOC;
    DeserializationError err = deserializeJson(configDOC, this->configJSON.c_str());
    
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
        const char* t_ON = configDOC[i]["t_ON"];
        const char* t_OFF = configDOC[i]["t_OFF"];
       
        strptime(t_ON, "%H:%M:%S", &_eSocket.t_ON);
        strptime(t_OFF, "%H:%M:%S", &_eSocket.t_OFF);

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
    time_t raw_t_now = mktime(&t_now);

    for (int i=0; i < NUMBER_OF_ESOCKETS; i++) { 
        time_t raw_t_ON     = mktime(&(this->_eSockets[i].t_ON)); 
        time_t raw_t_OFF    = mktime(&(this->_eSockets[i].t_OFF));
        
        this->_eSockets[i].delta_on     = difftime(raw_t_ON, raw_t_now);
        this->_eSockets[i].delta_off    = difftime(raw_t_OFF, raw_t_now);
    }

    struct tm midnight_minus_one; // 23:59:59 is 1 sec to midnight
    midnight_minus_one.tm_year   = 56;
    midnight_minus_one.tm_mon    = 4;
    midnight_minus_one.tm_mday   = 13;
    midnight_minus_one.tm_hour   = 23;
    midnight_minus_one.tm_min    = 59;
    midnight_minus_one.tm_sec    = 59;
    
    time_t raw_midnight_minus_one = mktime(&midnight_minus_one);

    this->delta_midnight = difftime(raw_midnight_minus_one, raw_t_now) + 1; // Add 1 sec
};

bool Control::is_midnight() {
    return this->delta_midnight == 0;
};

void Control::advanceCursor1s() {
    for (int i=0; i < NUMBER_OF_ESOCKETS; i++) { 
        this->_eSockets[i].delta_on     -= 1;
        this->_eSockets[i].delta_off    -= 1;
    }

    this->delta_midnight -= 1;
}

void Control::executeTask(int ID, bool ON) {
    for (int i=0; i < NUMBER_OF_ESOCKETS; i++) { 
        if (this->_eSockets[i].ID == ID) {
            ON == true ? this->_switch.send(this->_eSockets[i].code_ON, 32) : this->_switch.send(this->_eSockets[i].code_OFF, 32);    
        }
    }
}
