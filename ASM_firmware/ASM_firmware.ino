/* Firmware for the automated snake monitor. This program does the following:
- Switched the snake's lights between the day light and the night light based on the time
from a RTC
- Records the temperature and humidity from two different sensors
- Prints the temperature and humidity onto an LCD display
- Shuts off the lights if the temperature gets to be too hot, and activates 
a red LED if it gets WAY too hot.

Author: Trevor Loe 
*/

#include <DS3231.h>
#include <Wire.h>
#include <time.h>
#include <LiquidCrystal_I2C.h>

#include <RCSwitch.h>

#include <AM2302-Sensor.h>
#define HOTPIN 8
#define COLDPIN 6

// define temperature sensors
AM2302::AM2302_Sensor hot{HOTPIN};
AM2302::AM2302_Sensor cold{COLDPIN};

// define the control temperatures
float temp_hi = 80;     // the temperature at which it will turn off the light
float temp_err = 90;    // the temperature at which the device will go into an error state

bool error;

int numInvaldRead = 0;  // number of consecutive invalid reads

bool hotLightOn = false;
bool coldLightOn = false;

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

//initialize the liquid crystal library
//the first parameter is  the I2C address
//the second parameter is how many rows are on your screen
//the  third parameter is how many columns are on your screen
LiquidCrystal_I2C lcd(0x27,  16, 4);

// flag for a serial port being plugged in
bool serialport;

void setup() {
  if (Serial){
    Serial.begin(9600);
    serialport = true;
  } else {
    serialport = false;
  }
  

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

 //initialize lcd screen
  lcd.init();
 // turn on the backlight
  lcd.backlight();
  lcd.setCursor(0,0);
  // tell the screen to write “hello, from” on the top  row
  lcd.print("Starting Program :)");
  
  if (is_day(datetimenow, night2day, day2night)){
    day = true;
    set_clock(day2night);
  } else {
    day = false;
    set_clock(night2day);
  }

  // attach clock interrupt
  pinMode(CLINT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CLINT), isr_TickTock, FALLING);

  // warning LED
  pinMode(LED_PIN, OUTPUT);
  LED_state = false;

  // set up transmit
  sendSwitch.enableTransmit(RC_PIN_TX);
  sendSwitch.setProtocol(1);
  sendSwitch.setPulseLength(188);

  // turn off both switchs to start
  sendSwitch.send(codes[out4off], 24);
  sendSwitch.send(codes[out5off], 24);

  // temperature sensors
  hot.begin();
  cold.begin();

  // set the error state flag
  error = false;
}


void loop() {
  delay(30000); //wait 30 seconds
  // check for serial port
  if (Serial && !serialport){
    Serial.begin(9600);
    serialport = true;
    Serial.print("Started serial port");
  }

  // read temperature and humidity sensors
  auto hotstatus = hot.read();
  auto coldstatus = cold.read();
  float hot_temp = hot.get_Temperature() * 9.0 / 5.0 + 32.0; // convert to ferenheit
  float cold_temp = cold.get_Temperature() * 9.0 / 5.0 + 32.0; // convert to ferenheit
  float hot_hum = hot.get_Humidity();
  float cold_hum = hot.get_Humidity();

  bool valid_read = false;
  // check for certain errors
  if (hotstatus == 0 && coldstatus == 0){
    // check for if we should be going into an error state
    if (hot_temp > temp_err || cold_temp > temp_err){
      // the read is valid and the temperature is actually this high, go into an error state
      error = true;
    }
    valid_read = true;
    numInvaldRead = 0;
  } else {
    valid_read = false;
    numInvaldRead++;
  }

  // if we get 5 invalid reads in a row, go into error state
  if (numInvaldRead>5){
    error = true;
  }

  if (day){
    lcd.backlight(); // we only want the display on during the day, when I would actually be looking 
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

    Serial.print("\n\nstatus of hot sensor read(): ");
    Serial.println(AM2302::AM2302_Sensor::get_sensorState(hotstatus));

    Serial.print("\n\nstatus of cold sensor read(): ");
    Serial.println(AM2302::AM2302_Sensor::get_sensorState(coldstatus));

    Serial.print("Hot side: Temp: ");
    Serial.print(hot_temp);
    Serial.print(" Humidity: ");
    Serial.println(hot_hum);

    Serial.print("Cold side: Temp: ");
    Serial.print(cold_temp);
    Serial.print(" Humidity: ");
    Serial.println(cold_hum);
  }
  
   // print stuff onto LCD display
  lcd.setCursor(0,0);
  // tell the screen to write “hello, from” on the top  row
  // lcd.print("Year:");

  // print date and time
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
  // print where its night or day
  lcd.print(" day:");
  lcd.print(day);
  lcd.print("      ");

  // only update the display if there is a valid read
  if (valid_read){
    lcd.setCursor(0,1);
    //print hot side environmental information
    lcd.print("Hot: ");
    lcd.print(hot_temp);
    lcd.print("F ");
    lcd.print(hot_hum);
    lcd.print("%       ");
    //print cold side environmental information
    lcd.print("Cold: ");
    lcd.print(cold_temp);
    lcd.print("F ");
    lcd.print(cold_hum);
    lcd.print("%       ");
  }

  // print information about which lights on on
  lcd.setCursor(0,2);
  lcd.print("Hot:");
  lcd.print(hotLightOn);
  lcd.print(" Cold:");
  lcd.print(coldLightOn);

  if (error){
      // show error message on LCD
    lcd.setCursor(0,0);
    lcd.print("ERROR");
  }

  delay(5000); // display LCD stuff for only 5 seconds
  lcd.noBacklight();
  // first check that we are not in an error state
  if (error) {
    Serial.print("ERROR");
    digitalWrite(LED_PIN, true);
    // turn off both lights
    sendSwitch.send(codes[out4off], 24);
    sendSwitch.send(codes[out5off], 24);
    hotLightOn = false;
    coldLightOn = false;
  } else {
    // set the correct day/night lights on based on the value of the 'day' variable
    // if (day) {
    //   sendSwitch.send(codes[out5on], 24);
    //   hotLightOn = true;
    //   sendSwitch.send(codes[out4off], 24);
    //   coldLightOn = false;
    //   if (serialport) {
    //     Serial.println("Sent 5 ON code");
    //     Serial.println("Sent 4 OFF code");
    //   }
      
    // } else {
    //   sendSwitch.send(codes[out4on], 24);
    //   coldLightOn = true;
    //   sendSwitch.send(codes[out5off], 24);
    //   hotLightOn = false;
    //   if (serialport){
    //     Serial.println("Sent 4 ON code");
    //     Serial.println("Sent 5 OFF code");
    //   }
    // }
    if (valid_read){
      set_lights(day, temp_hi, hot_temp, cold_temp);
    }
  }
  
  if (tick==1) {
    // the alarm was triggered
    Serial.println("Alarm triggered");
    if (day){
      // it is now night, set the timer for the next morning
      set_clock(night2day);
      day = false;
    } else {
      // it is now day, set the timer for the evening
      set_clock(day2night);
      day = true;
    }
    
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
  // the alarm will be set to go off at the input hour, minute and second
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
  // returns true if the input time is day, false otherwise
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

void set_lights(bool day, float temp_hi, float hot_temp, float cold_temp){
  // this will turn the lights on or off depending on the value of day and the values of the two temperatures of each side of the cage
  if (day) {
    // daytime so we care about controlling the hot side lamp
    sendSwitch.send(codes[out4off], 24);
    coldLightOn = false;
    // check if temperature is above temp_hi threshold
    if (hot_temp > temp_hi){
      sendSwitch.send(codes[out5on], 24);
      hotLightOn = true;
    } else {
      sendSwitch.send(codes[out5off], 24);
      hotLightOn = false;
    }
    if (serialport) {
      Serial.println("Sent 5 ON code");
      Serial.println("Sent 4 OFF code");
    }
  } else {
    // night time so we only care about controlling the night lamp
    sendSwitch.send(codes[out5off], 24);
    hotLightOn = false;
    // check if temperature is above temp_hi threshold
    if (cold_temp > temp_hi){
      sendSwitch.send(codes[out4on], 24);
      coldLightOn = true;
    } else {
      sendSwitch.send(codes[out4off], 24);
      coldLightOn = false;
    }
    if (serialport){
      Serial.println("Sent 4 ON code");
      Serial.println("Sent 5 OFF code");
    }
  }
}
