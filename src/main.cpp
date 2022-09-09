#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();

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

  // Configure Tx
  mySwitch.enableTransmit(0);     // ESP8266-12E GPIO#0 ~ D3
  mySwitch.setProtocol(1);        // Protocol no. (default is 1, will work for most outlets)
  mySwitch.setPulseLength(320);   // Pulse length (optional)
  mySwitch.setRepeatTransmit(3);  // Number of tx repetitions (optional)

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
    t_on.tm_sec += (i+1)*10;
    t_off.tm_sec += (i+1)*10 + 30;
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
  short int task_ID   = 0;
  long int  task_code = 0;
  String log_record;  
  
  // Update and publish the plan every second
  if (millis() > time_target) {
    time_target += 1000;
    myClock.advanceTime1s();
    myControl.advanceCursor1s();

    for (int i=0; i < NUMBER_OF_ESOCKETS; i++) {
      log_record += (String)"__" + myControl._eSockets[i].delta_on;
      if (myControl._eSockets[i].delta_on == 0) {
        task_ID = (i*2)+1;   // 1, 3, 5
        task_code = myControl._eSockets[i].code_ON;
      }
      log_record += (String) "__" + (myControl._eSockets[i].delta_off);
      if (myControl._eSockets[i].delta_off == 0) {
        task_ID = (i*2)+2;  // 2, 4, 6
        task_code = myControl._eSockets[i].code_OFF;
      }
    }
    
    if (task_ID != 0) log_record += (String)"__Task ID " + task_ID + "__Code " + task_code;
    
    switch(task_ID) { // Print out the plan
      case 1:
        Serial.println("LED#1 ON");
        Serial.println(log_record);
        break;
      case 2: 
        Serial.println("LED#1 OFF");
        Serial.println(log_record);
        break;
      case 3:
        Serial.println("LED#2 ON");
        Serial.println(log_record);
        break;
      case 4:
        Serial.println("LED#2 OFF");
        Serial.println(log_record);
        break;
      case 5:
        Serial.println("Fan ON");
        Serial.println(log_record);
        break;
      case 6:
        Serial.println("Fan OFF");
        Serial.println(log_record);
        break;
      default:
        Serial.println(log_record);
        break;
    }
  }

  // Execute with redundancy controlled  by delay
  switch(task_ID) { 
    case 1:
      mySwitch.send(task_code, 32);
      break;
    case 2: 
      mySwitch.send(task_code, 32);
      break;
    case 3:
      mySwitch.send(task_code, 32);
      break;
    case 4:
      mySwitch.send(task_code, 32); 
      break;
    case 5:
      mySwitch.send(task_code, 32);
      break;
    case 6:
      mySwitch.send(task_code, 32);
      break;
    default:
      break;
  }
  delay(100);  // 100 ms = 10x redundancy, i.e. on/off signal sent 10 times every planning cycle.
  //myKeypad.senseTouch();
}

/* The BN-LINK rf devices use protocol no. 2 and bit-length 32 for communication.
   The remote for 3 on/off controls maps to the following codes:
   - 1 - ON  1949669955
   - 1 - OFF 1949672782
   - 2 - ON  1949668413
   - 2 - OFF 1949674324
   - 3 - ON  1949670469
   - 3 - OFF 1949672268
*/
