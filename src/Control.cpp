#include "Control.h"

Control::Control() {
    for (int i=0; i < NUMBER_OF_OUTLETS; i++) {
        struct Outlet _socket;
        _socket.ID = i;
          /* 
        * Represent time in seconds since epoch and 
        * use it in calculating time difference
        * between current (clock) time and target time.
        * For action scheduled at a fixed time each day, 
        * pick any reference date for year, month and day.
        * Gurudev's b'day is 05/13/1956. Use it.
        */
        _socket.t_ON.tm_year = 56;
        _socket.t_ON.tm_mon = 4;
        _socket.t_ON.tm_mday = 13;
        _socket.t_OFF.tm_year = 56;
        _socket.t_OFF.tm_mon = 4;
        _socket.t_OFF.tm_mday = 13;
        this->_sockets[i] = _socket;
    }
}

void Control::set_target(int ID, int hh_target, int mm_target, int ss_target, bool ON=true) {
    if (ON) {
        this->_sockets[ID].t_ON.tm_hour = hh_target;
        this->_sockets[ID].t_ON.tm_min  = mm_target;
        this->_sockets[ID].t_ON.tm_sec  = ss_target;
    } else {
        this->_sockets[ID].t_OFF.tm_hour = hh_target;
        this->_sockets[ID].t_OFF.tm_min  = mm_target;
        this->_sockets[ID].t_OFF.tm_sec  = ss_target;
    }
}

double Control::delta2_now(int ID, int hh_now, int mm_now, int ss_now, bool ON=true) {
    /*
    * Give this method the current time 
    * and obtain delta to target ON/OFF time
    * for a given outlet.
    */
    struct tm t_now = {0};
    t_now.tm_year = 56;   // Ref 1900  
    t_now.tm_mon = 4;     // 0 to 11
    t_now.tm_mday = 13;
    t_now.tm_hour = hh_now;
    t_now.tm_min = mm_now;
    t_now.tm_sec = ss_now;
    time_t raw_time_now = mktime(&t_now);
    time_t raw_time_target = ON ? mktime(&this->_sockets[ID].t_ON) : mktime(&this->_sockets[ID].t_OFF);
    return difftime(raw_time_target, raw_time_now);
}

void Control::update_socket_state(int ID, bool new_state) {
    this->_sockets[ID].state_t_minus_one = this->_sockets[ID].state_t;
    this->_sockets[ID].state_t = new_state;
}

bool Control::flag_socket_state_transition(int ID) {
    return this->_sockets[ID].state_t != this->_sockets[ID].state_t_minus_one;
}

bool Control::get_socket_state(int ID) {
    return this->_sockets[ID].state_t;
}