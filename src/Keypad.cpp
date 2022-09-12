#include "Keypad.h"

// Create 6 keys for the keypad
char keyLabel[6][6] = {"1 ON", "1 OFF", "2 ON", "2 OFF", "3 ON", "3 OFF"};
TFT_eSPI_Button key[6];

Keypad::Keypad() {
  
}

void Keypad::init(TFT_eSPI* display) {
   _tft = display;
}

void Keypad::set_control(Control* control) {
  _control = control;
}

void Keypad::drawKeypad() {
  // Draw keypad background
  // (*this->_tft).fillRect(0, 0, 240, 320, TFT_DARKGREY);

  // Draw number display area and frame
  // (*this->_tft).fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, TFT_BLACK);
  // (*this->_tft).drawRect(DISP_X, DISP_Y, DISP_W, DISP_H, TFT_WHITE);

  // Draw the keys
  for (uint8_t row = 0; row < 3; row++) {
    for (uint8_t col = 0; col < 2; col++) {
      uint8_t b = col + row * 2;
  
      (*this->_tft).setFreeFont(LABEL3_FONT);

      /* key[b].initButton(this->_tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, TFT_BLUE, TFT_WHITE,
                        keyLabel[b], KEY_TEXTSIZE); */
      key[b].initButton(this->_tft, KEY_X + col * (KEY_SPACING_X),
                      KEY_Y + row * (KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                      KEY_W, KEY_H, TFT_WHITE, TFT_BLUE, TFT_WHITE,
                      keyLabel[b], KEY_TEXTSIZE);
      key[b].drawButton();
    }
  }
};

void Keypad::touchCalibrate() {
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // Check file system exists
  if (!LittleFS.begin()) {
    Serial.println("Formating file system");
    LittleFS.format();
    LittleFS.begin();
  }

  // check if calibration file exists and size is correct
  if (LittleFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      LittleFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = LittleFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    (*this->_tft).setTouch(calData);
  } else {
    // data not valid so recalibrate
    (*this->_tft).fillScreen(TFT_BLACK);
    (*this->_tft).setCursor(20, 0);
    (*this->_tft).setTextFont(2);
    (*this->_tft).setTextSize(1);
    (*this->_tft).setTextColor(TFT_WHITE, TFT_BLACK);

    (*this->_tft).println("Touch corners as indicated");

    (*this->_tft).setTextFont(1);
    (*this->_tft).println();

    if (REPEAT_CAL) {
      (*this->_tft).setTextColor(TFT_RED, TFT_BLACK);
      (*this->_tft).println("Set REPEAT_CAL to false to stop this running again!");
    }

    (*this->_tft).calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    (*this->_tft).setTextColor(TFT_GREEN, TFT_BLACK);
    (*this->_tft).println("Calibration complete!");

    // store data
    File f = LittleFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }

};

void Keypad::senseTouch() {
  //TFT_eSPI myDisplay = * this->_tft;
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates

  // Pressed will be set true is there is a valid touch on the screen
  bool pressed = (*this->_tft).getTouch(&t_x, &t_y);

  // / Check if any key coordinate boxes contain the touch coordinates
  for (uint8_t b = 0; b < 6; b++) {
    if (pressed && key[b].contains(t_x, t_y)) {
      key[b].press(true);  // tell the button it is pressed
    } else {
      key[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // Check if any key has changed state
  for (uint8_t b = 0; b < 6; b++) {

    (*this->_tft).setFreeFont(LABEL3_FONT);

    if (key[b].justReleased()) key[b].drawButton();     // draw normal

    if (key[b].justPressed()) {
      key[b].drawButton(true);  // draw invert

      if ((b % 2) == 0) { // Even no, ON
        (*this->_control).executeTask(atoi(&keyLabel[b][0]), true);
      } else { // Odd no, OFF
        (*this->_control).executeTask(atoi(&keyLabel[b][0]), false);
      }
      delay(10); // UI debouncing
    }
  }
};

// Print something in the mini status bar
void Keypad::status(const char *msg) {
  //TFT_eSPI myDisplay = * this->_tft;

  (*this->_tft).setTextPadding(240);
  //tft.setCursor(STATUS_X, STATUS_Y);
  (*this->_tft).setTextColor(TFT_WHITE, TFT_DARKGREY);
  (*this->_tft).setTextFont(0);
  (*this->_tft).setTextDatum(TC_DATUM);
  (*this->_tft).setTextSize(1);
  (*this->_tft).drawString(msg, STATUS_X, STATUS_Y);
}