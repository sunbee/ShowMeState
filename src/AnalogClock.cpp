#include "AnalogClock.h"

AnalogClock::AnalogClock() {

}

void AnalogClock::init(TFT_eSPI* display) {
    _tft = display; 
}

// Get/set hours, minutes and seconds for showing time
int  AnalogClock::get_hh() {
    return this->hh;
};
void AnalogClock::set_hh(int hh) {
    this->hh_tminus1 = this->hh;
    this->hh = hh;
};
int  AnalogClock::get_mm() {
    return this->mm;
};
void AnalogClock::set_mm(int mm) {
    this->mm_tminus1 = this->mm;
    this->mm = mm;
};
int  AnalogClock::get_ss() {
    return this->ss;
};
void AnalogClock::set_ss(int ss) {
    this->ss_tminus1 = this->ss;
    this->ss = ss;
};
// Get/set center and circumference of clock face
int  AnalogClock::get_x0() {
    return this->x0;
};
void AnalogClock::set_x0(int x0) {
    this->x0 = x0;
};
int  AnalogClock::get_y0() {
    return this->y0;
};
void AnalogClock::set_y0(int y0) {
    this->y0 = y0;
};
int  AnalogClock::get_radius() {
    return this->radius;
};
void AnalogClock::set_radius(int radius) {
    this->radius = radius;
};

void AnalogClock::drawClock() {
    // Get center and circumference
    int x0 = this->x0;
    int y0 = this->y0;
    int radius = this->radius;

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

        this->showTime();
    }
}

void AnalogClock::showTime() {
    this->eraseTime();

    int hh = this->hh;
    int mm = this->mm;
    int ss = this->ss;

    float ssdeg = ss*6 - 90;
    float mmdeg = mm*6 + ssdeg*(6.0/360) - 90;
    float hhdeg = hh*30 + mmdeg*(30.0/360) - 90;

    float ssrad = ssdeg * 0.0174532925;
    float mmrad = mmdeg * 0.0174532925;
    float hhrad = hhdeg * 0.0174532925;

    float hhradius = this->radius * 0.6;
    float mmradius = this->radius * 0.75;
    float ssradius = this->radius * 0.90;

    int x0 = this->x0;
    int y0 = this->y0;

    // Hour hand
    float x_hh = x0 + hhradius*cos(hhrad);
    float y_hh = y0 + hhradius*sin(hhrad);
    (*this->_tft).drawLine(x0, y0, x_hh, y_hh, TFT_RED);

    // Minute hand
    float x_mm = x0 + mmradius*cos(mmrad);
    float y_mm = y0 + mmradius*sin(mmrad);
    (*this->_tft).drawLine(x0, y0, x_mm, y_mm, TFT_RED);

    // Second hand
}

void AnalogClock::eraseTime() {
    int hh = this->hh_tminus1;
    int mm = this->mm_tminus1;
    int ss = this->ss_tminus1;

    float ssdeg = ss*6 - 90;
    float mmdeg = mm*6 + ssdeg*(6/360) - 90;
    float hhdeg = hh*30 + mmdeg*(30/360) - 90;

    float ssrad = ssdeg * 0.0174532925;
    float mmrad = mmdeg * 0.0174532925;
    float hhrad = hhdeg * 0.0174532925;

    float hhradius = this->radius * 0.6;
    float mmradius = this->radius * 0.75;
    float ssradius = this->radius * 0.90;

    int x0 = this->x0;
    int y0 = this->y0;

    // Hour hand
    float x_hh = x0 + hhradius*cos(hhrad);
    float y_hh = y0 + hhradius*sin(hhrad);
    (*this->_tft).drawLine(x0, y0, x_hh, y_hh, TFT_BLACK);

    // Minute hand
    float x_mm = x0 + mmradius*cos(mmrad);
    float y_mm = y0 + mmradius*cos(mmrad);
    (*this->_tft).drawLine(x0, y0, x_mm, y_mm, TFT_BLACK);

    // Second hand
}