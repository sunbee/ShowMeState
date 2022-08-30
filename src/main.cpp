#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

#include "LittleFS.h"

TFT_eSPI _tft = TFT_eSPI();

#include "Keypad.h"
Keypad myKeypad = Keypad();

void setup() {
  // put your setup code here, to run once:
    // Use serial port
  Serial.begin(9600);

  // Initialise the TFT screen
  _tft.init();
  myKeypad.init(&_tft);

  // Set the rotation before we calibrate
  //_tft.setRotation(1);

  // Calibrate the touch screen and retrieve the scaling factors
  myKeypad.touchCalibrate();
  //littleNamaste();

  // Clear the screen
  _tft.fillScreen(TFT_BLACK);

  // Draw keypad background
  _tft.fillRect(0, 0, 240, 320, TFT_DARKGREY);

  // Draw number display area and frame
  _tft.fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, TFT_BLACK);
  _tft.drawRect(DISP_X, DISP_Y, DISP_W, DISP_H, TFT_WHITE);

  // Draw keypad
  myKeypad.drawKeypad();
}

void loop() {
  myKeypad.senseTouch();
}

