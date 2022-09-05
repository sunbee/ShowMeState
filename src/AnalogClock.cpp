#include "AnalogClock.h"

AnalogClock::AnalogClock() {

}

void AnalogClock::init(TFT_eSPI* display) {
    _tft = display; 
}

// Get/set hours, minutes and seconds for showing time
void AnalogClock::set_hh(int hh) {
    this->_hh_tminus1 = this->_hh;
    this->_hh = hh;
};
void AnalogClock::set_mm(int mm) {
    this->_mm_tminus1 = this->_mm;
    this->_mm = mm;
};
void AnalogClock::set_ss(int ss) {
    this->_ss_tminus1 = this->_ss;
    this->_ss = ss;
};
// Get/set center and circumference of clock face
void AnalogClock::set_x0(int x0) {
    this->_x0 = x0;
};
void AnalogClock::set_y0(int y0) {
    this->_y0 = y0;
};
void AnalogClock::set_radius(int radius) {
    this->_radius = radius;
};

void AnalogClock::advanceTime1s() {
    int ss = this->_ss;
    int mm = this->_mm;
    int hh = this->_hh;
    ss += 1;
    if (ss == 60) {
        ss = 0;
        mm += 1;
        if(mm > 59) {
            mm = 0;
            hh += 1;
            if (hh == 24) {
                hh = 0;
            }
        }
    }
    this->set_ss(ss);
    this->set_mm(mm);
    this->set_hh(hh);

    String hhmmss = (String)hh + ":" + mm + ":" + ss;
    if (dEbUg) Serial.println(hhmmss); 

    this->showTime();
}

void AnalogClock::drawClock() {
    // Get center and circumference
    int x0 = this->_x0;
    int y0 = this->_y0;
    int radius = this->_radius;

    // Draw clock face
    (*this->_tft).fillCircle(x0, y0, radius, TFT_CYAN);
    (*this->_tft).fillCircle(x0, y0, radius-8, TFT_BLACK);

    // Draw 60 tics for minutes / seconds
    for (int theta=0; theta < 360; theta+= 6) {
        int theta_adj = theta - 90;
        float theta_radians = (theta_adj)*0.0174532925;
        int outerRadius = radius-4;
        int innerRadius = radius-18;

        float x_outer = x0 + (outerRadius * cos(theta_radians));
        float y_outer = y0 + (outerRadius * sin(theta_radians));
        float x_inner = x0 + (innerRadius * cos(theta_radians));
        float y_inner = y0 + (innerRadius * sin(theta_radians));

        (*this->_tft).drawLine(x_outer, y_outer, x_inner, y_inner, TFT_DARKCYAN);

        if (theta_adj % 90 == 0) (*this->_tft).fillCircle(x_inner, y_inner, 3, TFT_DARKCYAN);

        if (theta_adj % 5 == 0) (*this->_tft).fillCircle(x_inner, y_inner, 2, TFT_BLUE);
    }

    (*this->_tft).fillRect(LCD_DISPLAY_X, LCD_DISPLAY_Y, LCD_DISPLAY_WIDTH, LCD_DISPLAY_HEIGHT, TFT_BLACK);
    (*this->_tft).drawRect(LCD_DISPLAY_X, LCD_DISPLAY_Y, LCD_DISPLAY_WIDTH, LCD_DISPLAY_HEIGHT, TFT_WHITE);
    this->showTime();
}

void AnalogClock::renderHands(bool erase=false) {
    int color = TFT_RED;
    int hh = this->_hh;
    int mm = this->_mm;
    int ss = this->_ss;
    if (erase == true) {
        color = TFT_BLACK;
        hh = this->_hh_tminus1;
        mm = this->_mm_tminus1;
        ss = this->_ss_tminus1;
    }

    float ssdeg = ss*6 - 90;
    float mmdeg = mm*6 + ssdeg*(6.0/360) - 90;
    float hhdeg = hh*30 + mmdeg*(30.0/360) - 90;

    float ssrad = ssdeg * 0.0174532925;
    float mmrad = mmdeg * 0.0174532925;
    float hhrad = hhdeg * 0.0174532925;

    float hhradius = this->_radius * 0.6;
    float mmradius = this->_radius * 0.75;
    float ssradius = this->_radius * 0.90;

    int x0 = this->_x0;
    int y0 = this->_y0;

    // Hour hand
    float x_hh = x0 + hhradius*cos(hhrad);
    float y_hh = y0 + hhradius*sin(hhrad);
    (*this->_tft).drawLine(x0, y0, x_hh, y_hh, color);

    // Minute hand
    float x_mm = x0 + mmradius*cos(mmrad);
    float y_mm = y0 + mmradius*sin(mmrad);
    (*this->_tft).drawLine(x0, y0, x_mm, y_mm, color);

    // Second hand
    float x_ss = x0 + ssradius*cos(ssrad);
    float y_ss = y0 + ssradius*sin(ssrad);
    (*this->_tft).drawLine(x0, y0, x_ss, y_ss, color);

    // 7 Segment LCD Display
    /*
    * Draw the HHMMSS string inside the rectangular LCD display
    * and erase the vacant part of the display after the string
    * by drawing a black rectangle. Thus avoid flicker arising
    * from repeated draw-erase cycles over the whole display area.
    */
    char HMS[9];
    snprintf(HMS, 9, "%02d:%02d:%02d", hh, mm, ss);
    int HMS_width = (*this->_tft).drawString(HMS, LCD_DISPLAY_X+4, LCD_DISPLAY_Y+10, 7);
    (*this->_tft).fillRect(LCD_DISPLAY_X+4+HMS_width, LCD_DISPLAY_Y+10, LCD_DISPLAY_WIDTH-5-HMS_width, LCD_DISPLAY_HEIGHT-12, TFT_BLACK);
    /*
    String HHMMSS = (String)hh + ":" + mm + ":" + ss;
    int HHMMSS_width = (*this->_tft).drawString(HHMMSS, LCD_DISPLAY_X+4, LCD_DISPLAY_Y+10, 7);
    (*this->_tft).fillRect(LCD_DISPLAY_X+4+HHMMSS_width, LCD_DISPLAY_Y+10, LCD_DISPLAY_WIDTH-5-HHMMSS_width, LCD_DISPLAY_HEIGHT-12, TFT_BLACK);
    */
}

void AnalogClock::showTime() {
    this->renderHands(true);
    this->renderHands();
}
