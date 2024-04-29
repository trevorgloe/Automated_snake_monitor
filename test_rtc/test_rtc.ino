#include <DS3231.h>
#include <Wire.h>
#include <time.h>
#include <LiquidCrystal_I2C.h>

//#include <DateTimeStrings.h>
//#include <RTClib.h>

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
}

void loop() {
  // Send Day-of-Week
//  Serial.print(rtc.getDOWStr());
  delay(1000);
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

//  // print stuff onto LCD display
//  lcd.setCursor(0,0);
//  // tell the screen to write “hello, from” on the top  row
//  lcd.print("Year:");
//  lcd.print(rtc.getYear());
//  lcd.print(" Month:");
//  lcd.print(rtc.getMonth(century));
//  lcd.setCursor(0,1);
//  lcd.print("Day:");
//  lcd.print(rtc.getDate());
//  lcd.print(" Hour:");
//  lcd.print(rtc.getHour(h12Flag, pmFlag));
//  lcd.setCursor(0,2);
//  lcd.print("Minute:");
//  lcd.print(rtc.getMinute());
//  lcd.print(" Second:");
//  lcd.print(rtc.getSecond());
}
