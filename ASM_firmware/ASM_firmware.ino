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

// Code for table LEDs
unsigned long code = 5476865;

RCSwitch receiveSwitch = RCSwitch();
RCSwitch sendSwitch = RCSwitch();

void setup()
{
  Serial.begin(9600);
  receiveSwitch.enableReceive(INTERRUPT_RX);

  // set up transmit
  sendSwitch.enableTransmit(RC_PIN_TX);
  sendSwitch.setProtocol(1);
  sendSwitch.setPulseLength(386);
}

void loop()
{
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

  sendSwitch.send(code, 24);
  Serial.println("Send ON/OFF code");
  delay(500);  
}
