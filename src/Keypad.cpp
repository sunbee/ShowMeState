#include "Keypad.h"

char numberBuffer[NUM_LEN + 1] = "";
uint8_t numberIndex = 0;

uint16_t keyColor[15] = {TFT_RED, TFT_DARKGREY, TFT_DARKGREEN,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE
                        };
// Create 15 keys for the keypad
char keyLabel[15][5] = {"New", "Del", "Send", "1", "2", "3", "4", "5", "6", "7", "8", "9", ".", "0", "#" };

TFT_eSPI_Button key[15];

Keypad::Keypad() {
  
}

void Keypad::init(TFT_eSPI* display) {
    _tft = display;
}

void Keypad::drawKeypad() {
  // Clear the screen
  (*this->_tft).fillScreen(TFT_BLACK);

  // Draw keypad background
  (*this->_tft).fillRect(0, 0, 240, 320, TFT_DARKGREY);

  // Draw number display area and frame
  (*this->_tft).fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, TFT_BLACK);
  (*this->_tft).drawRect(DISP_X, DISP_Y, DISP_W, DISP_H, TFT_WHITE);
  
  // Draw the keys
  for (uint8_t row = 0; row < 5; row++) {
    for (uint8_t col = 0; col < 3; col++) {
      uint8_t b = col + row * 3;
  
      if (b < 3) (*this->_tft).setFreeFont(LABEL1_FONT);
      else (*this->_tft).setFreeFont(LABEL2_FONT);

      key[b].initButton(this->_tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, keyColor[b], TFT_WHITE,
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
  TFT_eSPI myDisplay = * this->_tft;
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates

  // Pressed will be set true is there is a valid touch on the screen
  bool pressed = (*this->_tft).getTouch(&t_x, &t_y);

  // / Check if any key coordinate boxes contain the touch coordinates
  for (uint8_t b = 0; b < 15; b++) {
    if (pressed && key[b].contains(t_x, t_y)) {
      key[b].press(true);  // tell the button it is pressed
    } else {
      key[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // Check if any key has changed state
  for (uint8_t b = 0; b < 15; b++) {

    if (b < 3) (*this->_tft).setFreeFont(LABEL1_FONT);
    else (*this->_tft).setFreeFont(LABEL2_FONT);

    if (key[b].justReleased()) key[b].drawButton();     // draw normal

    if (key[b].justPressed()) {
      key[b].drawButton(true);  // draw invert

      // if a numberpad button, append the relevant # to the numberBuffer
      if (b >= 3) {
        if (numberIndex < NUM_LEN) {
          numberBuffer[numberIndex] = keyLabel[b][0];
          numberIndex++;
          numberBuffer[numberIndex] = 0; // zero terminate
        }
        status(""); // Clear the old status
      }

      // Del button, so delete last char
      if (b == 1) {
        numberBuffer[numberIndex] = 0;
        if (numberIndex > 0) {
          numberIndex--;
          numberBuffer[numberIndex] = 0;//' ';
        }
        status(""); // Clear the old status
      }

      if (b == 2) {
        status("Sent value to serial port");
        Serial.println(numberBuffer);
      }
      // we dont really check that the text field makes sense
      // just try to call
      if (b == 0) {
        status("Value cleared");
        numberIndex = 0; // Reset index to 0
        numberBuffer[numberIndex] = 0; // Place null in buffer
      }

      // Update the number display field
      (*this->_tft).setTextDatum(TL_DATUM);        // Use top left corner as text coord datum
      (*this->_tft).setFreeFont(&FreeSans18pt7b);  // Choose a nicefont that fits box
      (*this->_tft).setTextColor(DISP_TCOLOR);     // Set the font colour

      // Draw the string, the value returned is the width in pixels
      int xwidth = (*this->_tft).drawString(numberBuffer, DISP_X + 4, DISP_Y + 12);

      // Now cover up the rest of the line up by drawing a black rectangle.  No flicker this way
      // but it will not work with italic or oblique fonts due to character overlap.
      (*this->_tft).fillRect(DISP_X + 4 + xwidth, DISP_Y + 1, DISP_W - xwidth - 5, DISP_H - 2, TFT_BLACK);

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