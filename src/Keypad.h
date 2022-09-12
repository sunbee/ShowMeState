#ifndef KEYPAD_H
#define KEYPAD_H

#include <TFT_eSPI.h>
#include "LittleFS.h"

#include "Control.h"

// This is the file name used to store the calibration data
// You can change this to create new calibration files.
// The LittleFS file name must start with "/".
#define CALIBRATION_FILE "/calibration"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false

// Keypad start position, key sizes and spacing
#define KEY_X 35 // Centre of key
#define KEY_Y 40
#define KEY_W 60 // Width and height
#define KEY_H 60
#define KEY_SPACING_X 250   // X and Y gap
#define KEY_SPACING_Y 80
#define KEY_TEXTSIZE 0.66   // Font size multiplier

// Using two fonts since numbers are nice when bold
#define LABEL1_FONT &FreeSansOblique12pt7b  // Key label font 1
#define LABEL2_FONT &FreeSansBold12pt7b     // Key label font 2
#define LABEL3_FONT &FreeSansBold9pt7b      // Key label font 3

// We have a status line for messages
#define STATUS_X 120 // Centred on this
#define STATUS_Y 65

class Keypad {
    public:
        Keypad();
        void init(TFT_eSPI* _tft);
        void set_control(Control*);
        void drawKeypad();
        void touchCalibrate();
        void senseTouch();
        void status(const char*);
    private:
        TFT_eSPI* _tft;
        Control* _control;
};

#endif