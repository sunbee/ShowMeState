#ifndef ANALOGCLOCK_H
#define ANALOGCLOCK_H

#include <TFT_eSPI.h>

class AnalogClock {
    public:
        AnalogClock();
        void init(TFT_eSPI*);
        void drawClock();
        void showTime();
        void eraseTime();
        void advanceTime(int millisecs);
        // Get/set hours, minutes and seconds for showing time
        int  get_hh();
        void set_hh(int);
        int  get_mm();
        void set_mm(int);
        int  get_ss();
        void set_ss(int);
        // Get/set center and circumference of clock face
        int  get_x0();
        void set_x0(int);
        int  get_y0();
        void set_y0(int);
        int  get_radius();
        void set_radius(int);
    private:
        TFT_eSPI* _tft;
        int hh = 10;        // 1 to 12
        int mm = 10;        // 0 to 59
        int ss = 30;        // 0 to 59
        int x0 = 120;       // Center X
        int y0 = 120;       // Center Y
        int radius = 118;   // Radius
        int hh_tminus1;
        int mm_tminus1;
        int ss_tminus1;

};


#endif