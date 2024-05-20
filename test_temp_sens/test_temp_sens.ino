#include <AM2302-Sensor.h>
#define HOTPIN 8
#define COLDPIN 6

AM2302::AM2302_Sensor hot{HOTPIN};
AM2302::AM2302_Sensor cold{COLDPIN};

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
}

void loop() {
  auto hotstatus = hot.read();
  Serial.print("\n\nstatus of sensor read(): ");
  Serial.println(AM2302::AM2302_Sensor::get_sensorState(hotstatus));

  Serial.print("Hot Temperature: ");
  Serial.println(hot.get_Temperature() * 9.0 / 5.0 + 32.0);

  Serial.print("Hot Humidity:    ");
  Serial.println(hot.get_Humidity());

  auto coldstatus = cold.read();
  Serial.print("Cold Temperature: ");
  Serial.println(cold.get_Temperature() * 9.0 / 5.0 + 32.0);

  Serial.print("Cold Humidity:    ");
  Serial.println(cold.get_Humidity());
  delay(2000);
}
