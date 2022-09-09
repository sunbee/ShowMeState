#ifndef CONTROL_H
#define CONTROL_H

#include <Arduino.h>
#include <time.h>

#define NUMBER_OF_ESOCKETS   3

struct eSocket {
    int  ID;
    struct tm t_ON              = {0};      // Timestamp for switching outlet ON..
    struct tm t_OFF             = {0};      // and OFF
    double delta_on             = -1;       // Time (secs) to event,
    double delta_off            = -1;       // 
    long int code_ON            = 1949669955;    
    long int code_OFF           = 1949672782;
};

class Control {
    public:
        Control();
        void initialize_deltas(struct tm);
        void advanceCursor1s();
        bool flag_state_transition(struct eSocket*);
        struct eSocket _eSockets[NUMBER_OF_ESOCKETS];
};

#endif

/* Time
* We will use the time library native to C.
* This offers two interchangeable data structures 
* for a timestamp as follows:
* 1. struct tm 
* 2. time_t
* The first is a C structure with slots 
* for year, month, day and hours, mins, secs.
* The second is the number of seconds since epoch
* that is midnight at the turn of January 1, 1970.
* These two are interchangeable. 
*/

/*
* Our app is concerned with a daily schedule of 
* on/off operations that repeats ever 24 hours.
* We shall use the struct tm to store a timestamp
* and set a fixed day (Gurudev's birthday) always
* to use with the time hours, minutes and seconds.
* Then convert to time_t as needed to apply methods 
* such as those for calculating time-difference .
*/

/* eSocket 
* Save the state and the target on/off times.
* Save the time-deltas to event (on/off) and
* update every second in main's loop or 
* when current time is reset.
*/