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

void set_time_now(struct tm* t_now);
void set_clock_time(struct tm* t_now);
void set_timeline(struct tm* t_now);

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
  set_timeline(&t_now);
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
    _control.scan_sockets_and_flag();
    Serial.println(_control.scan_sockets_and_log_record());

    if (_control.is_midnight() == true) {
      delay(1000); // Give it one second 
      set_timeline(&t_now);
    }

    if (millis() > 60*60*1000) { // Refresh at regular interval
      set_timeline(&t_now);
    }
  }

  // Execute redundantly with repetition controlled by a small delay
  _control.scan_sockets_and_execute();
  /* DEPRECATE!!!
  if (task_target > 0) {
    _control.executeTask(task_target, task);
  }
  */
  delay(10);  // 10 ms = 100x redundancy, i.e. on/off signal sent 100 times every planning cycle.
  _keypad.senseTouch();
}

void set_time_now(struct tm* t_now) {
  // Get current time
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());  
  // Update arg passed by ref
  t_now->tm_year   = 56;
  t_now->tm_mon    = 4;
  t_now->tm_mday   = 13;
  t_now->tm_hour   = timeClient.getHours();
  t_now->tm_min    = timeClient.getMinutes();
  t_now->tm_sec    = timeClient.getSeconds();
}

void set_clock_time(struct tm* t_now) {
  // Set clock time 
  _clock.set_hh(t_now->tm_hour);
  _clock.set_mm(t_now->tm_min);
  _clock.set_ss(t_now->tm_sec);
  // Refresh display 
  _clock.showTime();
}

void set_timeline(struct tm* t_now) {
  set_time_now(t_now);
  set_clock_time(t_now);  
  _control.set_deltas(t_now);
  _control.scan_sockets_and_print_timetable(t_now);
};