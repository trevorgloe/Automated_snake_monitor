# Automated_snake_monitor
I have a snake (named Seth). This is an embedded systems project to create an automated system that monitors his cage's temperature and humidity, and controls heat lamps and misters to adjust. 

## Basic Project Info
This is meant to be mostly an embedded system's software project, rather than a hardware one. The hardware was all chosen to be the simplest possible to impliment. As such, the project used an Arduino Uno as the main brain of the controller. Most the choices for the project were made to ensure safety for either myself or Seth. 

The basic architecture for the system is a set of 433MHz wireless controlled outlet switches. Using the 433MHz RF module, the arduino can switch those outlets on or off. The temperature control loop (executed once every several minutes) checks the temperature at two separate locations of the cage (one on the hot side, one of the cold side), and will change the state of the outlets, to switch the lights on and off, depending on if the temperature needs to change. The arduino will also output all data being taken to both a serial monitor, and an LCD screen placed in front of the cage. The system also features an internal timer, automatically switching Seth's lamps between the 'day' light and 'night' light depending on the state of the timer. In this way, the system acts as a safety shutoff for when the cage gets too hot, an automatic light switch for the day and night lamps, and temperature and humidity sensor via the LCD display. 

## Hardware Used (with links to where I got them)
- [Arduino Uno](https://www.amazon.com/dp/B008GRTSV6?ref=ppx_yo2ov_dt_b_product_details&th=1)
- [HiLetgo 433MHz RF Wireless Transmitter and Receiver Module for Arduino](https://www.amazon.com/dp/B01DKC2EY4?psc=1&ref=ppx_yo2ov_dt_b_product_details)
- Etekcity Zap Remote Outlet Switch Model ZAP 5LX-S (This one I could not find at any stores. They may not make it any more. But I was able to find a set on ebay.)
- [DS3231 AT24C32 IIC Real Time Clock](https://www.amazon.com/dp/B07Q7NZTQS?psc=1&ref=ppx_yo2ov_dt_b_product_details)
- [Sunfounder 20x4 LCD Module I2C with interface chip](https://www.amazon.com/dp/B0CLGVYPYW?psc=1&ref=ppx_yo2ov_dt_b_product_details)
- [Wall outlet power supply](https://www.amazon.com/dp/B018OLREG4?psc=1&ref=ppx_yo2ov_dt_b_product_details)
- [Temperature and humidity sensors](https://www.amazon.com/gp/product/B0CPHQC9SF/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1)

## Wiring
![wiring diagram](./doc_files/ASM_wiring_diagram.drawio.png "wiring diagram")

## Software Documentation
![state diagram](./doc_files/ASM_state_diagram.drawio.png "state diagram")

The software is designed around the state diagram above. All the firmware lives within [ASM_firmware](ASM_firmware/ASM_firmware.ino) effectively implementing the finite state machine. The software leans heavily on the [RCSwitch](https://github.com/sui77/rc-switch/tree/436a74b03f3dc17a29ee327af29d5a05d77f94b9), LiquidCrystalI2C, DS3231, and AM2302-Sensor libraries. The dependent libraries and their functions are listed below:

- [RCSwitch](https://github.com/sui77/rc-switch/tree/436a74b03f3dc17a29ee327af29d5a05d77f94b9) - Controls the outlet switches through the 433MHz wireless transmitter. Also used to determine the corresponding codes for the switches via the 433MHz receiver. The library has great documentation and is fairly straightforward to use.
- LiquidCrystalI2C - Controls the Sunfound 20x4 LCD Module over I2C. Using I2C here greatly reduces the pin load.
- [DS3231](https://github.com/NorthernWidget/DS3231) - Controls the RTC, allowing for alarms with interupts, setting the time, etc.
- [AM2302-Sensor](https://github.com/hasenradball/AM2302-Sensor?tab=readme-ov-file#library-usage) - Controls the temperature and humidity sensors

Most of the execution of the state machine happens in the `loop()` function, with the exception of a few functions I created to avoid clutter. 

The first is `set_clock(mydatetime obj)` which sets the RTC's current time using the DS3231 library. Its input is a struct called `mydatetime` which I created to store all the necessary parts of the date and time that the clock sets. This made it easier than having 6 inputs into the function. 

The next function is `is_day(mydatetime obj, mydatetime day, mydatetime night)` taking in 3 `mydatetime` objects. It returns a bool if the current time (given by the first input `obj`) is day. Here "day" means that the time is between that of the second `mydatetime` input, `day` and third, `night`. If the time is not between those, it returns `false`. This function is used on program start so I can just set the time without having to also initially set the date. 

The next function is `set_lights(bool day, float temp_hi, float hot_temp, float cold_temp)` taking in a bool representing whether it is day or night, the maximum allowed temperature, and the temperature on each side of the cage. As the state diagram suggests, the function will determine whether or not to turn on/off the "current" light. If the time is day, the current light will be the day light. If the time is night, it will be the night light. To determine whether to turn on/off the current light, the code compares the temperature at the side corresponding to whether it is day or night to `temp_hi`. This function is designed to simply turn off the correct light when the cage temperature on the correspnding side gets too hot. 

Finally, `test_features()` is a process that will run through all the basic tests on startup. This includes turning on/off the LED, flipping both lights on/off, and reading the sensors/verifying the output is valid. 

If test_features detects a fault or if there are 5 consecutive invalid readings of the sensors (invalid readings mean the sensor returns something other than 0, indicating some osrt of fault), we enter an error state, where the LED will stay lit, the lights will stay shut off, and the LCD screen will display `error`. 

In each cycle of the loop, the LCD will only display for 5 seconds. There is a wait at the end of each loop for ~50 seconds. This low duty cycle for the LCD is done both to conserve power, and the prevent screen burning. 

### Etekcity Zap remote codes
| Outlet | On | Off |
| ---------- | ------------- | ------------- |
| 1 | 5248307 | 5248316 |
| 2 | 5248451 | 5248460 |
| 3 | 5248771 | 5248780 |
| 4 | 5250307 | 5250316 |
| 5 | 5256451 | 5256460 |

All codes featured a pulse length of between 185 and 188. Typically a message was picked up several times, each with slightly different pulse lengths. 
