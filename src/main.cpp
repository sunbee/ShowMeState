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

struct tm t_now = {0};

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
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
  t_now.tm_year   = 56;
  t_now.tm_mon    = 4;
  t_now.tm_mday   = 13;
  t_now.tm_hour   = timeClient.getHours();
  t_now.tm_min    = timeClient.getMinutes();
  t_now.tm_sec    = timeClient.getSeconds();

  myClock.drawClock();
  myClock.set_hh(t_now.tm_hour);
  myClock.set_mm(t_now.tm_min);
  myClock.set_ss(t_now.tm_sec);
  myClock.showTime();

  // Test time - all eSockets have same on/off cycle
  // TODO: Load settings from config.json to configure eSockets via class Control's constructor 
  struct tm t_on = t_now;  
  struct tm t_off = t_now;  
  for (int i=0; i < NUMBER_OF_ESOCKETS; i++) {
    t_on.tm_sec += i+30;
    t_off.tm_sec += i+60;
    myControl._eSockets[i].t_ON = t_on;
    myControl._eSockets[i].t_OFF = t_off;
  }
  myControl.initialize_deltas(t_now);
  for (int i=0; i < NUMBER_OF_ESOCKETS; i++) {
    Serial.print("Switching ON in ");
    Serial.println(myControl._eSockets[i].delta_on);
    Serial.print("Switching OFF in ");
    Serial.println(myControl._eSockets[i].delta_off);
  }
}

void loop() {  
  if (millis() > time_target) {
    time_target += 1000;
    myClock.advanceTime1s();
    myControl.advanceCursor1s();
    for (int i=0; i < NUMBER_OF_ESOCKETS; i++) {
      Serial.print(myControl._eSockets[i].delta_on);
      if (myControl._eSockets[i].delta_on == 0) Serial.println("SENT MSG ON");
      Serial.print(myControl._eSockets[i].delta_off);
      if (myControl._eSockets[i].delta_off == 0) Serial.println("SENT MSG OFF");
    }
  }

  //myKeypad.senseTouch();
}

