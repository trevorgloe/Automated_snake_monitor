#include <Wire.h>
#include <RCSwitch.h>

#define RC_PIN_TX A0
#define PW_OUT 13 // stupid 5V header is broken so just using a digital output pin as 5V

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

RCSwitch sendSwitch = RCSwitch();

void setup() {
  pinMode(PW_OUT, OUTPUT);
  digitalWrite(PW_OUT, 1);

  sendSwitch.enableTransmit(RC_PIN_TX);
  sendSwitch.setProtocol(1);
  sendSwitch.setPulseLength(188);

}

void loop() {
  sendSwitch.send(codes[out3on], 24);
  delay(1000);
  sendSwitch.send(codes[out3off], 24);
  delay(1000);

}
