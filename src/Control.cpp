#include "Control.h"

Control::Control() {
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
        _eSocket.ID = i+1; // 1, 2, 3
        /* 
        * Represent time in seconds since epoch and 
        * use it in calculating time difference
        * between current (clock) time and target time.
        * For action scheduled at a fixed time each day, 
        * pick any reference date for year, month and day.
        * Gurudev's b'day is 05/13/1956. Use it.
        */
        _eSocket.t_ON.tm_year = 56;
        _eSocket.t_ON.tm_mon = 4;
        _eSocket.t_ON.tm_mday = 13;
        _eSocket.t_OFF.tm_year = 56;
        _eSocket.t_OFF.tm_mon = 4;
        _eSocket.t_OFF.tm_mday = 13;
        this->_eSockets[i] = _eSocket;
    }
}

void Control::initialize_deltas(struct tm t_now) {
    for (int i=0; i < NUMBER_OF_ESOCKETS; i++) { 
        time_t raw_t_now = mktime(&t_now);

        time_t raw_t_ON     = mktime(&(this->_eSockets[i].t_ON)); 
        time_t raw_t_OFF    = mktime(&(this->_eSockets[i].t_OFF));
        
        this->_eSockets[i].delta_on_seconds     = difftime(raw_t_ON, raw_t_now);
        this->_eSockets[i].delta_off_seconds    = difftime(raw_t_OFF, raw_t_now);
    }
};

void Control::advanceCursor1s() {
    for (int i=0; i < NUMBER_OF_ESOCKETS; i++) { 
        this->_eSockets[i].delta_on_seconds     -= 1;
        this->_eSockets[i].delta_off_seconds    -= 1;
    }
}

// void Control::set_target(int ID, struct tm t_target, bool ON=true) {
//     for (int i=0; i < NUMBER_OF_ESOCKETS; i++) {
//         if (this->_eSockets[i].ID == ID) {
//             ON ? this->_eSockets[ID].t_ON = t_target : this->_eSockets[ID].t_OFF = t_target;
//         }
//     }
// }

// double Control::delta2_now(int ID, struct tm t_now, bool ON=true) {
//     /*
//     * Give this method the current time 
//     * and obtain delta to target ON/OFF time
//     * for a given outlet.
//     */
//    for (int )
//     time_t raw_time_now = mktime(&t_now);
//     time_t raw_time_target = ON ? mktime(&this->_eSockets[ID].t_ON) : mktime(&this->_eSockets[ID].t_OFF);
//     return difftime(raw_time_target, raw_time_now);
// }

// void Control::update_eSocket_state(int ID, bool new_state) {
//     this->_eSockets[ID].state_t_minus_one = this->_eSockets[ID].state_t;
//     this->_eSockets[ID].state_t = new_state;
// }

// bool Control::flag_eSocket_state_transition(int ID) {
//     return this->_eSockets[ID].state_t != this->_eSockets[ID].state_t_minus_one;
// }

// bool Control::get_eSocket_state(int ID) {
//     return this->_eSockets[ID].state_t;
// }
