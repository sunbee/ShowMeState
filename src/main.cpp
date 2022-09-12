#include <Arduino.h>
#include <time.h>
#include "LittleFS.h"

#include <ESP8266WiFi.h>
#include "config.h"
const char* ssid = SSID;
const char* pass = PASS;

#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP NTP_UDP;
NTPClient timeClient(NTP_UDP, "pool.ntp.org");

#include <SPI.h>
#include <TFT_eSPI.h>
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

void reset_cursor();

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
  
  // Get the ball rolling
  reset_cursor();
}

void loop() {  
  short int task_target;      // Socket ID, -1 for SELECT NO SOCKET
  bool      task;             // Tx code ON if true, OFF if false
  String    log_record;       // Log record 
  
  // Update and publish the plan every second
  if (millis() > time_target) {
    task_target = -1;  // SELECT NO SOCKET
    time_target += 1000;
    myClock.advanceTime1s();
    myControl.advanceCursor1s();

    for (int i=0; i < NUMBER_OF_ESOCKETS; i++) {
      log_record += (String)"_ON_" + myControl._eSockets[i].delta_on;
      if (myControl._eSockets[i].delta_on == 0) {
        task_target = myControl._eSockets[i].ID;
        task = true;
      }
      log_record += (String) "_OFF_" + (myControl._eSockets[i].delta_off);
      if (myControl._eSockets[i].delta_off == 0) {
        task_target = myControl._eSockets[i].ID;
        task = false;
      }
    }
    
    if (task_target > 0) log_record += (String)"__Task ID " + task_target + "__Code " + (task ? "ON" : "OFF");
    Serial.println (log_record);

    if (myControl.is_midnight() == true) {
      delay(1000); // Give it one second 
      reset_cursor();
    }

    if (millis() > 60*60*1000) { // Refresh at regular interval
      reset_cursor();
    }
  }

  // Execute redundantly with repetition controlled by a small delay
  if (task_target > 0) {
    myControl.executeTask(task_target, task);
  }
  delay(100);  // 10 ms = 100x redundancy, i.e. on/off signal sent 100 times every planning cycle.
  //myKeypad.senseTouch();
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
  myClock.set_hh(t_now.tm_hour);
  myClock.set_mm(t_now.tm_min);
  myClock.set_ss(t_now.tm_sec);
  myClock.showTime();

  // Reset timer deltas 
  myControl.initialize_deltas(t_now);
  for (int i=0; i < NUMBER_OF_ESOCKETS; i++) {
    time_t tt_ON = mktime(&myControl._eSockets[i].t_ON); Serial.print(ctime(&tt_ON));  
    time_t tt_OFF = mktime(&myControl._eSockets[i].t_OFF); Serial.print(ctime(&tt_OFF));
    time_t tt_now = mktime(&t_now); Serial.print(ctime(&tt_now));
    Serial.print("Switching ON in "); Serial.println(myControl._eSockets[i].delta_on);
    Serial.print("Switching OFF in "); Serial.println(myControl._eSockets[i].delta_off);
  }
};