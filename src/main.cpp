#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <time.h>
struct tm t = {0};

time_t secondsSinceEpoch(int hh, int mm, int ss) {
  struct tm t = {0};
  /* 
  * Represent time in seconds since epoch and 
  * use it in calculating time difference
  * between current (clock) time and target time.
  * For action scheduled at a fixed time each day, 
  * pick any reference date for year, month and day.
  * Gurudev's b'day is 05/13/1956. Use it.
  */
  t.tm_year = 56;   // Ref 1900  
  t.tm_mon = 4;     // 0 to 11
  t.tm_mday = 13;
  t.tm_hour = hh;
  t.tm_min = mm;
  t.tm_sec = ss;
  time_t raw_time = mktime(&t);
  Serial.println(ctime(&raw_time));
  return raw_time;
}

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

int time_target = millis() + 1000;

bool first = true;

double deltaTo_one_on;
double deltaTo_one_off;
bool one = false;
bool one_t_minus_one = false;

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
  time_t raw = secondsSinceEpoch(17, 34, 10);
  Serial.print("Secs: ");
  Serial.println(long(&raw));
  deltaTo_one_on = difftime(secondsSinceEpoch(18, 32, 0), secondsSinceEpoch(18, 31, 30));
  deltaTo_one_off = difftime(secondsSinceEpoch(18, 32, 30), secondsSinceEpoch(18, 31, 30));
  Serial.print("Switching 1 on in ");
  Serial.println(deltaTo_one_on);
  Serial.print("Switching 1 off in ");
  Serial.println(deltaTo_one_off);
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
  if (abs(millis() / 1000.00 - deltaTo_one_on) < 1) {
    one_t_minus_one = one;
    one = true;
  }
  if (abs(millis() / 1000.00 - deltaTo_one_off) < 1) {
    one_t_minus_one = one;
    one = false;
  }
  if (one_t_minus_one != one) Serial.println(one ? "ON" : "OFF");

  //myKeypad.senseTouch();
}

