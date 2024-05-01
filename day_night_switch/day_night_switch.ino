//#include <ArduinoLowPower.h>
//#include <avr/sleep.h>

// First test of changing one of the lights on seth's cage

#include <DS3231.h>
#include <Wire.h>
#include <time.h>
#include <LiquidCrystal_I2C.h>

//#include <DateTimeStrings.h>
//#include <RTClib.h>

#define CLINT 2
volatile bool tick = 0;

byte alarmBits;
bool alarmDayIsDay;
bool alarmH12;
bool alarmPM;

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

//initialize the liquid crystal library
//the first parameter is  the I2C address
//the second parameter is how many rows are on your screen
//the  third parameter is how many columns are on your screen
//LiquidCrystal_I2C lcd(0x27,  16, 4);

void setup() {
  Serial.begin(9600);

  Wire.begin();

  // set the current date and time
  datetimenow.year = 24;
  datetimenow.month = 4;
  datetimenow.day = 26;
  datetimenow.hour = 21;
  datetimenow.minute = 57;
  datetimenow.second = 0;

  rtc.setYear(datetimenow.year);
  rtc.setMonth(datetimenow.month);
  rtc.setDate(datetimenow.day);
  rtc.setHour(datetimenow.hour);
  rtc.setMinute(datetimenow.minute);
  rtc.setSecond(datetimenow.second);

//  //initialize lcd screen
//  lcd.init();
//  // turn on the backlight
//  lcd.backlight();

  // set alarm 1
//  alarmBits = 0b00001111; // Alarm 1 every second
  alarmBits = 0b00001000;
  alarmDayIsDay = true;
  alarmH12 = false;
  alarmPM = false;    
  
//  rtc.turnOffAlarm(1);
//  rtc.setA1Time(
//       datetimenow.day, datetimenow.hour, datetimenow.minute+1, datetimenow.second,
//       alarmBits, alarmDayIsDay, alarmH12, alarmPM);
//  // enable Alarm 1 interrupts
//  rtc.turnOnAlarm(1);
//  // clear Alarm 1 flag
//  rtc.checkIfAlarm(1);
   // Setup alarm one to fire every second
    rtc.turnOffAlarm(1);
    rtc.setA1Time(10, 21, 57, 10, alarmBits, false, false, false);
    rtc.turnOnAlarm(1);
    rtc.checkIfAlarm(1);

  // Prevent Alarm 2 altogether by assigning a 
  // nonsensical alarm minute value that cannot match the clock time,
  // and an alarmBits value to activate "when minutes match".
//  alarmMinute = 0xFF; // a value that will never match the time
  alarmBits = 0b01100000; // Alarm 2 when minutes match, i.e., never
    
  // Upload the parameters to prevent Alarm 2 entirely
//  rtc.setA2Time(
//        datetimenow.day, datetimenow.hour, 0xFF,
//       alarmBits, alarmDayIsDay, alarmH12, alarmPM);
//  // disable Alarm 2 interrupt
//  rtc.turnOffAlarm(2);
//  // clear Alarm 2 flag
//  rtc.checkIfAlarm(2);

  // NOTE: both of the alarm flags must be clear
  // to enable output of a FALLING interrupt

  // attach clock interrupt
  pinMode(CLINT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CLINT), isr_TickTock, FALLING);
//  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
//  sleep_enable();
//  sleep_cpu();
}


void loop() {
  int curr_year = rtc.getYear();
  Serial.print(curr_year, DEC);
  Serial.print("-");
  int curr_month = rtc.getMonth(century);
  Serial.print(curr_month, DEC);
  Serial.print("-");
  int curr_day = rtc.getDate();
  Serial.print(curr_day, DEC);
  Serial.print(" ");
  int curr_hour = rtc.getHour(h12Flag, pmFlag);
  Serial.print(curr_hour, DEC); //24-hr
  Serial.print(":");
  int curr_minute = rtc.getMinute();
  Serial.print(curr_minute, DEC);
  Serial.print(":");
  int curr_sec = rtc.getSecond();
  Serial.println(curr_sec, DEC);
  delay(3000);
//  LowPower.deepSleep(2000);
  if (tick==1) {
    Serial.print("Alarm triggered");
    // clear alarm state
    tick = 0;
//    rtc.checkIfAlarm(1, true);
    alarmBits = 0b00001000;
    rtc.turnOffAlarm(1);
    rtc.setA1Time(curr_day, curr_hour, curr_minute, curr_sec+11, alarmBits, false, false, false);
    rtc.turnOnAlarm(1);
    rtc.checkIfAlarm(1);
    
  }

}
//
void isr_TickTock() {
    // interrupt signals to loop
    tick = 1;
    return;
}
