#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

#include "LittleFS.h"

TFT_eSPI _tft = TFT_eSPI();

#include "Keypad.h"
Keypad myKeypad = Keypad();

#include "AnalogClock.h"
AnalogClock myClock = AnalogClock();

void setup() {
  // put your setup code here, to run once:
    // Use serial port
  Serial.begin(9600);

  // Initialise the TFT screen
  _tft.init();
  myKeypad.init(&_tft);
  myClock.init(&_tft);

  // Set the rotation before we calibrate
  //_tft.setRotation(1);

  // Calibrate the touch screen and retrieve the scaling factors
  // myKeypad.touchCalibrate();

  // Draw keypad
  // myKeypad.drawKeypad();

  // Draw clock
  myClock.drawClock();
}

void loop() {
  //myKeypad.senseTouch();
}

