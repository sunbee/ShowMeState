#include <Arduino.h>
#include <time.h>
#include "LittleFS.h"

#include "CRUDaLittle.h"
CRUDaLittle _littleCRUD = CRUDaLittle();

#include <ESP8266WiFi.h>
#include "GuruWebServer.h"
GuruWebServer _WWW = GuruWebServer(&_littleCRUD);

#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP NTP_UDP;
NTPClient timeClient(NTP_UDP, "pool.ntp.org");

#include <SPI.h>
#include <TFT_eSPI.h>
TFT_eSPI _tft = TFT_eSPI();

#include "Keypad.h"
Keypad _keypad = Keypad();

#include "AnalogClock.h"
AnalogClock _clock = AnalogClock();

#include "Control.h"
Control _control = Control(&_littleCRUD);
double delta2_ON;
double delta2_OFF;

struct tm t_now = {0};
int time_target = millis() + 1000;
bool first = true;

void reset_cursor();

void setup() {
  // put your setup code here, to run once:
  // Use serial port
  Serial.begin(9600);
  
  // Set up web-server
  _WWW.serveWWW();

  // Connect to  the NTP client
  delay(3000);
  timeClient.begin();
  timeClient.setTimeOffset(-18000); // Central time GMT-5 ~ -5 x 3600 
  
  // Initialise the TFT screen
  _tft.init();
  _tft.setRotation(1);          // Set the rotation before we calibrate
  _tft.fillScreen(TFT_PINK);    // Clear the screen

  // Draw keypad
  _keypad.init(&_tft);
  _keypad.touchCalibrate();    // Calibrate the touch screen and retrieve the scaling factors
  _keypad.drawKeypad();
  _keypad.set_control(&_control);

  // Draw clock
  _clock.init(&_tft);
  _clock.set_x0(160);
  _clock.set_y0(90);
  _clock.set_radius(90);
  _clock.drawClock();
  
  // Get the ball rolling
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());

  _control.init();
  reset_cursor();
}

void loop() {  
  short int task_target;      // Socket ID, -1 for SELECT NO SOCKET
  bool      task;             // Tx code ON if true, OFF if false
  String    log_record;       // Log record 
  
  // Update and publish the plan every second
  if (millis() > time_target) {
    task_target = -1;  // SELECT NO EL. SOCKET
    time_target += 1000;
    _clock.advanceTime1s();
    _control.advanceCursor1s();

    for (int i=0; i < NUMBER_OF_ESOCKETS; i++) {
      log_record += (String)"_ON_" + _control._eSockets[i].delta_on;
      if (_control._eSockets[i].delta_on == 0) {
        task_target = _control._eSockets[i].ID;
        task = true;
      }
      log_record += (String) "_OFF_" + (_control._eSockets[i].delta_off);
      if (_control._eSockets[i].delta_off == 0) {
        task_target = _control._eSockets[i].ID;
        task = false;
      }
    }
    
    if (task_target > 0) log_record += (String)"__Task ID " + task_target + "__Code " + (task ? "ON" : "OFF");
    Serial.println (log_record);

    if (_control.is_midnight() == true) {
      delay(1000); // Give it one second 
      reset_cursor();
    }

    if (millis() > 60*60*1000) { // Refresh at regular interval
      reset_cursor();
    }
  }

  // Execute redundantly with repetition controlled by a small delay
  if (task_target > 0) {
    _control.executeTask(task_target, task);
  }
  delay(10);  // 10 ms = 100x redundancy, i.e. on/off signal sent 100 times every planning cycle.
  _keypad.senseTouch();
}

void reset_cursor() {
  // Get current time
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
  t_now.tm_year   = 56;
  t_now.tm_mon    = 4;
  t_now.tm_mday   = 13;
  t_now.tm_hour   = timeClient.getHours();
  t_now.tm_min    = timeClient.getMinutes();
  t_now.tm_sec    = timeClient.getSeconds();
  
  // Refresh displayed clock 
  _clock.set_hh(t_now.tm_hour);
  _clock.set_mm(t_now.tm_min);
  _clock.set_ss(t_now.tm_sec);
  _clock.showTime();

  // Reset timer deltas 
  _control.set_deltas(t_now);
  _control.print_timetable(t_now);
};