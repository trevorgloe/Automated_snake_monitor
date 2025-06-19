// System for switching the light on and off for a fish tank
// Single light, switches it on/off at day/night
// No LCD

#include <DS3231.h>
#include <Wire.h>
#include <time.h>

#include <RCSwitch.h>

#define CLINT 2
volatile bool tick = 0;

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
  // check for serial port
  if (Serial && !serialport){
    Serial.begin(9600);
    serialport = true;
    Serial.print("Started serial port");
  }

  if (day){
    lcd.backlight();
  }
  int curr_year = rtc.getYear();
  int curr_month = rtc.getMonth(century);
  int curr_day = rtc.getDate();
  int curr_hour = rtc.getHour(h12Flag, pmFlag);
  int curr_minute = rtc.getMinute();
  int curr_sec = rtc.getSecond();
  if (serialport) {
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
  }
  
  LED_state = !LED_state;
  digitalWrite(LED_PIN, LED_state);
   // print stuff onto LCD display
  lcd.setCursor(0,0);
  // tell the screen to write “hello, from” on the top  row
  // lcd.print("Year:");
  lcd.print(curr_year);
  lcd.print("-");
  lcd.print(curr_month);
  lcd.print("-");
  lcd.print(curr_day);
  lcd.print(" ");
  lcd.print(curr_hour);
  lcd.print("-");
  lcd.print(curr_minute);
  lcd.print("-");
  lcd.print(curr_sec);
  lcd.setCursor(0,1);
  // lcd.print(rtc.getDate());
  // lcd.print(" Hour:");
  // lcd.print(rtc.getHour(h12Flag, pmFlag));
  // lcd.setCursor(0,2);
  // lcd.print("Minute:");
  // lcd.print(rtc.getMinute());
  // lcd.print(" Second:");
  // lcd.print(rtc.getSecond());
  lcd.print("Day: ");
  if (day){
    lcd.print("true ");
  } else {
    lcd.print("false");
  }
  lcd.print("Ser: ");
  if (serialport){
    lcd.print("true");
  } else {
    lcd.print("false");
  }
  delay(5000);
  lcd.noBacklight();
//  LowPower.deepSleep(2000);
  if (day) {
    sendSwitch.send(codes[out5on], 24);
    
    sendSwitch.send(codes[out4off], 24);
    if (serialport) {
      Serial.println("Sent 5 ON code");
      Serial.println("Sent 4 OFF code");
    }
    
  } else {
    sendSwitch.send(codes[out4on], 24);
    sendSwitch.send(codes[out5off], 24);
    if (serialport){
      Serial.println("Sent 4 ON code");
      Serial.println("Sent 5 OFF code");
    }
  }
  if (tick==1) {
    Serial.print("Alarm triggered");
    // clear alarm state
    
//    rtc.checkIfAlarm(1, true);
    // alarmBits = 0b00001000;
    // rtc.turnOffAlarm(1);
    if (day){
      // it is now night, set the timer for the next morning
      // rtc.setA1Time(10, 7, 10, 0, alarmBits, false, false, false);
      set_clock(night2day);
      // sendSwitch.send(codes[out5on], 24);
      // Serial.println("Sent 5 ON code");
      // sendSwitch.send(codes[out4off], 24);
      // Serial.println("Sent 4 OFF code");
      day = false;
    } else {
      // it is now day, set the timer for the evening
      // rtc.setA1Time(10, 20, 10, 0, alarmBits, false, false, false);
      set_clock(day2night);
      // sendSwitch.send(codes[out4on], 24);
      // Serial.println("Sent 4 ON code");
      // sendSwitch.send(codes[out5off], 24);
      // Serial.println("Sent 5 OFF code");
      day = true;
    }
    // rtc.setA1Time(curr_day, curr_hour, curr_minute, curr_sec+11, alarmBits, false, false, false);
    // rtc.turnOnAlarm(1);
    // rtc.checkIfAlarm(1);
    
    tick = 0; // do this at the end in case the turning off and on causes any interferance
  }

}
//
void isr_TickTock() {
    // interrupt signals to loop
    tick = 1;
    return;
}

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
