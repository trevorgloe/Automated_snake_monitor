#include <AM2302-Sensor.h>
#define HOTPIN 8
#define COLDPIN 6
#include <LiquidCrystal_I2C.h>

AM2302::AM2302_Sensor hot{HOTPIN};
AM2302::AM2302_Sensor cold{COLDPIN};

//initialize the liquid crystal library
//the first parameter is  the I2C address
//the second parameter is how many rows are on your screen
//the  third parameter is how many columns are on your screen
LiquidCrystal_I2C lcd(0x27,  16, 4);

void setup() {
  Serial.begin(9600);
  hot.begin();
  cold.begin();

  auto status = hot.read();
  Serial.print("\n\nstatus of sensor read(): ");
  Serial.println(AM2302::AM2302_Sensor::get_sensorState(status));

  Serial.print("Temperature: ");
  Serial.println(hot.get_Temperature());

  Serial.print("Humidity:    ");
  Serial.println(hot.get_Humidity());

  Wire.begin();
  //initialize lcd screen
  lcd.init();
 // turn on the backlight
  lcd.backlight();
  lcd.setCursor(0,0);
}

void loop() {
  lcd.setCursor(0,0);
  auto hotstatus = hot.read();
  Serial.print("\n\nstatus of sensor read(): ");
  Serial.println(AM2302::AM2302_Sensor::get_sensorState(hotstatus));
//  lcd.print("Stat: ");
//  lcd.print(AM2302::AM2302_Sensor::get_sensorState(hotstatus));
  

  Serial.print("Hot Temperature: ");
  Serial.println(hot.get_Temperature() * 9.0 / 5.0 + 32.0);
  lcd.print("T: ");
  lcd.print(hot.get_Temperature() * 9.0 / 5.0 + 32.0);
  
  Serial.print("Hot Humidity:    ");
  Serial.println(hot.get_Humidity());
  lcd.print("Hum: ");
  lcd.print(hot.get_Humidity());

  auto coldstatus = cold.read();
  Serial.print("Cold Temperature: ");
  Serial.println(cold.get_Temperature() * 9.0 / 5.0 + 32.0);

  Serial.print("Cold Humidity:    ");
  Serial.println(cold.get_Humidity());
  delay(2000);
}
