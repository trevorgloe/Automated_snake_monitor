// First test of changing one of the lights on seth's cage

#include <DS3231.h>
#include <Wire.h>
#include <time.h>
#include <LiquidCrystal_I2C.h>

//#include <DateTimeStrings.h>
//#include <RTClib.h>

#define CLINT 2
volatile bool tick = 1;

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
  datetimenow.second = 25;

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
  
  rtc.turnOffAlarm(1);
  rtc.setA1Time(
       datetimenow.day, datetimenow.hour, datetimenow.minute+1, datetimenow.second,
       alarmBits, alarmDayIsDay, alarmH12, alarmPM);
  // enable Alarm 1 interrupts
  rtc.turnOnAlarm(1);
  // clear Alarm 1 flag
  rtc.checkIfAlarm(1);

  // Prevent Alarm 2 altogether by assigning a 
  // nonsensical alarm minute value that cannot match the clock time,
  // and an alarmBits value to activate "when minutes match".
//  alarmMinute = 0xFF; // a value that will never match the time
  alarmBits = 0b01100000; // Alarm 2 when minutes match, i.e., never
    
  // Upload the parameters to prevent Alarm 2 entirely
  rtc.setA2Time(
        datetimenow.day, datetimenow.hour, 0xFF,
       alarmBits, alarmDayIsDay, alarmH12, alarmPM);
  // disable Alarm 2 interrupt
  rtc.turnOffAlarm(2);
  // clear Alarm 2 flag
  rtc.checkIfAlarm(2);

  // NOTE: both of the alarm flags must be clear
  // to enable output of a FALLING interrupt

  // attach clock interrupt
  pinMode(CLINT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CLINT), isr_TickTock, FALLING);
}


void loop() {
  if (tick) {
    tick = 0;
    Serial.print("Interupt triggered");
    
  }
  Serial.print(rtc.getYear(), DEC);
  Serial.print("-");
  Serial.print(rtc.getMonth(century), DEC);
  Serial.print("-");
  Serial.print(rtc.getDate(), DEC);
  Serial.print(" ");
  Serial.print(rtc.getHour(h12Flag, pmFlag), DEC); //24-hr
  Serial.print(":");
  Serial.print(rtc.getMinute(), DEC);
  Serial.print(":");
  Serial.println(rtc.getSecond(), DEC);
  delay(100);

}

void isr_TickTock() {
    // interrupt signals to loop
    tick = 1;
    return;
}
