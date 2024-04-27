#include <RCSwitch.h>

// The interrupt number, on the MicroView this is PIN 11. On Arduino it is PIN 2. See http://arduino.cc/en/Reference/AttachInterrupt
#define INTERRUPT_RX 0
#define RC_PIN_TX A0

// Code for Etekcity Zap outlets
unsigned long code[10] = 
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


#define LED_PIN 10

RCSwitch receiveSwitch = RCSwitch();

void setup() {
  Serial.begin(9600);
  receiveSwitch.enableReceive(INTERRUPT_RX);
  pinMode(LED_PIN, OUTPUT);

}

void loop() {

  if (receiveSwitch.available())
  {
    int value = receiveSwitch.getReceivedValue();
    
    if (value == 0)
    {
      Serial.println("Unknown encoding");
    }
    else
    {
      Serial.print("Received ");
      Serial.print(receiveSwitch.getReceivedValue());
      Serial.print(" / ");
      Serial.print(receiveSwitch.getReceivedBitlength());
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println(receiveSwitch.getReceivedProtocol());
      Serial.print("Delay (Pulse Length): ");
      Serial.println(receiveSwitch.getReceivedDelay());
    }
    
    receiveSwitch.resetAvailable();
  }
  digitalWrite(LED_PIN, 1);

}
