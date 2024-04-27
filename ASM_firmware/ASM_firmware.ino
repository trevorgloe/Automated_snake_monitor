// Arduino/MicroView code to sniff Etekcity power outlets codes with the RCSwitch library
// Outlets: http://www.amazon.com/gp/product/B00DQELHBS/
// Generic 433 Module: http://www.amazon.com/gp/product/B00M2CUALS (if you search for the code on the board you can find it in many other retailers)
// MicroView Pinout Guide: http://kit.microview.io/pinout/
// RCSwitch: https://code.google.com/p/rc-switch/
// Raspberry PI port:  http://timleland.com/wireless-power-outlets/

#include <RCSwitch.h>

// The interrupt number, on the MicroView this is PIN 11. On Arduino it is PIN 2. See http://arduino.cc/en/Reference/AttachInterrupt
#define INTERRUPT_RX 0
#define RC_PIN_TX A0
#define LED_PIN 10
#define SWITCH_PIN 12

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

int state = 0;

//RCSwitch receiveSwitch = RCSwitch();
RCSwitch sendSwitch = RCSwitch();

void setup()
{
  Serial.begin(9600);
//  receiveSwitch.enableReceive(INTERRUPT_RX);

  // set up transmit
  sendSwitch.enableTransmit(RC_PIN_TX);
  sendSwitch.setProtocol(1);
  sendSwitch.setPulseLength(188);

  // pinout
  pinMode(LED_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT);
}

void loop()
{
//  int state = digitalRead(SWITCH_PIN);
  if (state==1){
      digitalWrite(LED_PIN, 1);
      sendSwitch.send(codes[out4on], 24);
      Serial.println("Send ON code");
//      delay(100);  
      state = 0;
  } else {
      digitalWrite(LED_PIN, 0);
      sendSwitch.send(codes[out4off], 24);
      Serial.println("Send OFF code");
//      delay(100);
      state = 1;
  }
  delay(1000);

}
