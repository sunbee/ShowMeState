#ifndef CONTROL_H
#define CONTROL_H

#include <time.h>

#define NUMBER_OF_OUTLETS   3

struct Outlet {
    /*
    * Save the state and the target on/off times.
    * We will use the time library native to C.
    * This offers two interchangeable data structures 
    * for a timestamp as follows:
    * 1. struct tm 
    * 2. time_t
    * The first is a C structure with slots 
    * for year, month, day and hours, mins, secs.
    * The second is the number of seconds since epoch
    * that is midnight at the turn to January 1, 1970.
    * These two are interchangeable. 
    * In practical case, our app is concerned
    * with a daily schedule of on/off operations.
    * So we will use a set date (Gurudev's birthday)
    * and specify the time in hours, minutes and seconds,
    * storing the data with the struct tm 
    * and making calculations such as time-difference
    *  using time_t.
    */
    int  ID;
    bool state_t            = false;
    bool state_t_minus_one  = false;
    struct tm t_ON          = {0};
    struct tm t_OFF         = {0};
};

class Control {
    public:
        Control();
        void switch_me(int, bool);
        void set_target(int, int, int, int, bool);
        double delta2_now(int, int, int, int, bool);
        void update_socket_state(int, bool);
        bool flag_socket_state_transition(int);
        bool get_socket_state(int);
    private:
        struct Outlet _sockets[NUMBER_OF_OUTLETS];
};

#endif