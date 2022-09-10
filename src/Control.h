#ifndef CONTROL_H
#define CONTROL_H

#include <Arduino.h>
#include <time.h>

#include <ArduinoJson.h>

#include <RCSwitch.h>

#define CONFIG_JSON "[{'ID':1,'t_ON':'23:45','t_OFF':'23:46','code_ON':1949669955,'code_OFF':1949672782},{'ID':2,'t_ON':'23:45:30','t_OFF':'23:46:30','code_ON':1949668413,'code_OFF':1949674324},{'ID':3,'t_ON':'17:30','t_OFF':'21:00','code_ON':1949670469,'code_OFF':1949672268}]"
#define NUMBER_OF_ESOCKETS 3

struct eSocket {
    int  ID;
    struct tm t_ON              = {0};      // Timestamp for switching outlet ON..
    struct tm t_OFF             = {0};      // and OFF
    double delta_on             = -1;       // Time (secs) to event, tx ON..
    double delta_off            = -1;       // and OFF 
    long int code_ON;    
    long int code_OFF;
};

class Control {
    public:
        Control();
        void initialize_deltas(struct tm);
        void advanceCursor1s();
        void executeTask(int, bool);        
        struct eSocket _eSockets[NUMBER_OF_ESOCKETS];
        bool is_midnight();
    private:
        double delta_midnight; 
        RCSwitch _switch = RCSwitch();
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

/* The BN-LINK rf devices use protocol no. 2 and bit-length 32 for communication.
   The remote for 3 on/off controls maps to the following codes:
   - 1 - ON  1949669955
   - 1 - OFF 1949672782
   - 2 - ON  1949668413
   - 2 - OFF 1949674324
   - 3 - ON  1949670469
   - 3 - OFF 1949672268
*/