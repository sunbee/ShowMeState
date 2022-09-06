#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <time.h>

#include "config.h"

const char* ssid = SSID;
const char* pass = PASS;

WiFiUDP NTP_UDP;
NTPClient timeClient(NTP_UDP, "pool.ntp.org");

#include "LittleFS.h"

TFT_eSPI _tft = TFT_eSPI();

#include "Keypad.h"
Keypad myKeypad = Keypad();

#include "AnalogClock.h"
AnalogClock myClock = AnalogClock();

#include "Control.h"
Control myControl = Control();
double delta2_ON;
double delta2_OFF;

int time_target = millis() + 1000;

bool first = true;

void setup() {
  // put your setup code here, to run once:
    // Use serial port
  Serial.begin(9600);

  // Connect to WiFi
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay (500);
    Serial.print (".");
  }

  // Connect to  the NTP client
  timeClient.begin();
  timeClient.setTimeOffset(-18000); // Central time GMT-5 ~ -5 x 3600 

  // Initialise the TFT screen
  _tft.init();
  myKeypad.init(&_tft);
  myClock.init(&_tft);

  // Set the rotation before we calibrate
  // _tft.setRotation(1);

  // Clear the screen
  _tft.fillScreen(TFT_BLACK);

  // Calibrate the touch screen and retrieve the scaling factors
  // myKeypad.touchCalibrate();

  // Draw keypad
  // myKeypad.drawKeypad();

  // Draw clock
  myClock.drawClock();

  // Test time
  myControl.set_target(0, 23, 55, 0, true);
  myControl.set_target(0, 23, 55, 30, false);
  delta2_ON = myControl.delta2_now(0, 23, 54, 0, true);
  delta2_OFF = myControl.delta2_now(0, 23, 54, 0, false);
  Serial.print("Switching 1 on in ");
  Serial.println(delta2_ON);
  Serial.print("Switching 1 off in ");
  Serial.println(delta2_OFF);
}

void loop() {
  if (first == true) {
    timeClient.update();
    Serial.println(timeClient.getFormattedTime());
    myClock.set_hh(timeClient.getHours());
    myClock.set_mm(timeClient.getMinutes());
    myClock.set_ss(timeClient.getSeconds());

    myClock.showTime();
    first = false;
  }
  
  if (millis() > time_target) {
    time_target += 1000;
    myClock.advanceTime1s();
  }
  if (abs(millis() / 1000.00 - delta2_ON) < 1) {
    myControl.update_socket_state(0, true);
  }
  if (abs(millis() / 1000.00 - delta2_OFF) < 1) {
    myControl.update_socket_state(0, false);
  }
  if (myControl.flag_socket_state_transition(0)) Serial.println(myControl.get_socket_state(0) ? "ON" : "OFF");

  //myKeypad.senseTouch();
}

