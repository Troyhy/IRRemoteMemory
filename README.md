# IRRemoteMemory
## General  
  IR Remote Memory is for Banggoods 10W IP65 LED Flood Light With Remote Control.
  This LED lamp does not have memory over last used color. So every time Light 
  turns on it will be RED.
    
  This Arduino program is repeating all IR commands to the light and has a save/memory funtionality.
  If you press ON while light is on, last command ( Current color) is stored into memory and restored
  when power is restored.

## Hardware Connections

* Banggoods 10W IP65 LED Flood Light With Remote Control Outdoor Lamp AC85-265V (http://www.banggood.com/10W-IP65-LED-Flood-Light-With-Remote-Control-Outdoor-Lamp-AC85-265V-p-932033.html)
* Arduino controller
* IR sensor ( reused from LED lamp itsef)

This code is meant for Arduino device witch will be using IR sensor from original light. (INPUT  PIN 11 )
Arduino output pin ( PIN 10) is connected in place ir light's IR sensor data line. 
Code is repeating all IR commands to the light and has a save/memory funtionality.
If you press ON while light is on, last command ( Current color) is stored into memory and restored 
when power is restored.

```
    +-------+                               RDB LED controll board
    |   ||  |   IR-Sensor                +------------------------+
    |   ||  |   IR          LEDwhite. ---|   [1.2.3.]             |
    |   U   |               LEDred.   ---|  (orig IR ^ )          |---- 230V~
    +-------+               LEDgreen. ---|                        |
     |  |  |                LEDblue.  ---|                        |---- 230V~
     |  |  |                             +------------------------+
     1. 2. 3.

     Connections:
     IR1. Data   -> Arduino PIN11 (AR11.)
     IR2. Ground -> Arduino GROUND
     IR3. +5V    -> Arduino +5V
     LEDwhite.  LED controlled board 9V output ( withe wire) -> Arduino VIN
     AR10. Repeated IR signal -> LED1. LED controller board wire where IR1. used to be connected
 ```
Arduino is powered from +9V supply witch is driving 10W RGB led ( white wire going into LED module.
 +5V powe output from 3. pin is not enough to powe up arduino

# Requirements
 * Arduino and electrical skills
 * Ken Shirriff's *awesome* IRremote library:  https://github.com/shirriff/Arduino-IRremote
 * Arduino UNO ( should work with Arduino Mini, but still untested)
