// System for switching the light on and off for a fish tank
// Single light, switches it on/off at day/night
// No LCD

#include <DS3231.h>
#include <Wire.h>
#include <time.h>

#include <RCSwitch.h>

#define LED_PIN 10

#define RC_PIN_TX A0

byte alarmBits;
bool alarmDayIsDay;
bool alarmH12;
bool alarmPM;

bool LED_state;
bool day;

DS3231 rtc;
bool century = false;
bool h12Flag = false;
bool pmFlag = false;

struct mydatetime {
  int year;
  int month;
  int day;
  int hour; // in 24-hour format
  int minute;
  int second;
};
mydatetime datetimenow;

mydatetime day2night = {24, 5, 10, 20, 15, 0};  // when the light should switch from day to night
mydatetime night2day = {24, 5, 10, 7, 10, 0};   // when the light should switch from night to day

// Code for Etekcity Zap outlets
unsigned long codes[10] = 
  {5248307, 5248316, // outlet 1
  5248451, 5248460,  // outlet 2
  5248771, 5248780,  // outlet 3
  5250307, 5250316,  // outlet 4
  5256451, 5256460};

// enum to help index 
enum code_idx{
  out1on,
  out1off,
  out2on,
  out2off,
  out3on,
  out3off,
  out4on,
  out4off,
  out5on,
  out5off
};
// day light is switch 4, night light is switch 5

RCSwitch sendSwitch = RCSwitch();
  

  Wire.begin();

  // set the current date and time
  datetimenow.year = 24;
  datetimenow.month = 5;
  datetimenow.day = 14;
  datetimenow.hour = 18;
  datetimenow.minute = 52;
  datetimenow.second = 5;

  rtc.setYear(datetimenow.year);
  rtc.setMonth(datetimenow.month);
  rtc.setDate(datetimenow.day);
  rtc.setHour(datetimenow.hour);
  rtc.setMinute(datetimenow.minute);
  rtc.setSecond(datetimenow.second);


  if (is_day(datetimenow, night2day, day2night)){
    day = true;
    set_clock(day2night);
  } else {
    day = false;
    set_clock(night2day);
  }
  
  pinMode(LED_PIN, OUTPUT);
  LED_state = true;

  // set up transmit
  sendSwitch.enableTransmit(RC_PIN_TX);
  sendSwitch.setProtocol(1);
  sendSwitch.setPulseLength(188);

  // turn off switch to start
  sendSwitch.send(codes[out3off], 24);
}


void loop() {
  delay(30000);

  if (day){
    lcd.backlight();
  }
  int curr_year = rtc.getYear();
  int curr_month = rtc.getMonth(century);
  int curr_day = rtc.getDate();
  int curr_hour = rtc.getHour(h12Flag, pmFlag);
  int curr_minute = rtc.getMinute();
  int curr_sec = rtc.getSecond();
  Serial.print(curr_year, DEC);
  Serial.print("-");
    
  Serial.print(curr_month, DEC);
  Serial.print("-");
    
  Serial.print(curr_day, DEC);
  Serial.print(" ");
    
  Serial.print(curr_hour, DEC); //24-hr
  Serial.print(":");
    
  Serial.print(curr_minute, DEC);
  Serial.print(":");
    
  Serial.println(curr_sec, DEC);

  Serial.print("day: ");
  Serial.println(day);
  
  LED_state = !LED_state;
  digitalWrite(LED_PIN, LED_state);
  

  if (day) {
    sendSwitch.send(codes[out3on], 24);
    
    Serial.println("Sent 3 ON code");
  } else {
    sendSwitch.send(codes[out3off], 24);
    Serial.println("Sent 3 OFF code");
  }

  // Set the day variable based on the current readings from the RTC
  if (day) {
    // it is currently day, so check if the clock says its night
    if (is_day(currdatetime, night2day, day2night)){
      // it is actually day, dont really do anything
      Serial.print("no time change required");
    } else {
      // it is not actually day, according to the clock 
      set_clock(night2day);
      day = false;
    }

    } else {
    // it is currently night, so check if the clock says its day
      if (is_day(currdatetime, night2day, day2night)){
        // it is actually day, according to the clock
        set_clock(day2night);
        day = true;
      } else {
        // it is actually night, dont really do anything 
        Serial.print("no time change required");
      }
    }

}
//

void set_clock(mydatetime obj){
  // function to set the alarm 1 for the RTC
  alarmBits = 0b00001000; // alarm will trip when date and time match
  rtc.turnOffAlarm(1);

  // setA1Time(day, hour, minute, second, alarmbits, false, false, false)
  int day = obj.day;
  int hour = obj.hour;
  int min = obj.minute;
  int sec = obj.second;
  rtc.setA1Time(day, hour, min, sec, alarmBits, false, false, false);

  rtc.turnOnAlarm(1);
  rtc.checkIfAlarm(1);
}

bool is_day(mydatetime obj, mydatetime day, mydatetime night){
  // checks if the date/time given by the input is in the day or night
  if (obj.hour != night.hour && obj.hour != day.hour){
    if (obj.hour < day.hour || obj.hour > night.hour){
      return false;
    } else {
      return true;
    }
  } else {
    if (obj.hour == night.hour) {
      if (obj.minute > night.minute){
        return false;
      } else {
        return true;
      }
    } else {
      if (obj.minute < day.minute) {
        return false;
      } else {
        return true;
      }
    }
  }
}
