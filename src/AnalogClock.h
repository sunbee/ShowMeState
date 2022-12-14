#ifndef ANALOGCLOCK_H
#define ANALOGCLOCK_H

#include <TFT_eSPI.h>

#include <time.h>

#define dEbUg false // Verbose if true

/*
* Paramters for LCD display
*/
#define LCD_DISPLAY_X      80
#define LCD_DISPLAY_Y      172
#define LCD_DISPLAY_WIDTH  160
#define LCD_DISPLAY_HEIGHT 68
#define LCD_DISPLAY_FONT   7
#define LCD_DISPLAY_COLOR  TFT_ORANGE

class AnalogClock {
    public:
        AnalogClock();
        void init(TFT_eSPI*);
        void drawClock();
        void showTime();
        void eraseTime();
        void advanceTime(int);
        void advanceTime1s();
        void renderHands(bool);
        // Get/set hours, minutes and seconds for showing time
        void set_hh(int);
        void set_mm(int);
        void set_ss(int);
        // Get/set center and circumference of clock face
        void set_x0(int);
        void set_y0(int);
        void set_radius(int);
    private:
        TFT_eSPI* _tft;
        int _hh = 10;        // 0 to 23
        int _mm = 10;        // 0 to 59
        int _ss = 30;        // 0 to 59
        int _x0 = 120;       // Center X
        int _y0 = 120;       // Center Y
        int _radius = 118;   // Radius
        int _hh_tminus1;
        int _mm_tminus1;
        int _ss_tminus1;
};


#endif