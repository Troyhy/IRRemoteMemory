/* RGB Remote Control Memory
   by: Toni Röyhy

   date: March 5, 2017

   This sketch uses Ken Shirriff's *awesome* IRremote library:
       https://github.com/shirriff/Arduino-IRremote

   IR Remote Memory is for Banggoods 10W IP65 LED Flood Light With Remote Control Outdoor Lamp AC85-265V
   (http://www.banggood.com/10W-IP65-LED-Flood-Light-With-Remote-Control-Outdoor-Lamp-AC85-265V-p-932033.html)
   witch does not have memory over last used light. So every time Light turns on it will be RED.
   This code is meant for Arduino device witch will be using IR sensor from original light. (INPUt  PIN 11 )
   Arduino output pin ( PIN 10) is connected in place ir light's IR sensor data line. 
   Code is repeating all IR commands to the light and has a save/memory funtionality.
   If you press ON while light is on, last command ( Current color) is stored into memory and restored 
   when power is restored.

    +-------+
    |   ||  |
    |   ||  |
    |   U   |
    +-------+
     |  |  |
     |  |  |
     1. 2. 3.

     1. Data   -> Arduino PIN11
     2. Ground -> Arduino GROUND
     3. +5V    -> Arduino +5V

       LED controlled board 9V output ( withe wire) -> Arduino VIN

     Arduino is powered from +9V supply witch is driving 10W RGB led ( white wire going into LED module.
     +5V powe output from 3. pin is not enough to powe up arduino

*/

#include <IRremote.h> // Include the IRremote library
#include <EEPROM.h>

/* Setup constants for China RGB IR Remote: */
#define NUM_BUTTONS (4*6) // The remote has 9 buttons
#define NEC_BITS          32
#define NEC_HDR_MARK    9000
#define NEC_HDR_SPACE   4500
#define NEC_BIT_MARK     560
#define NEC_ONE_SPACE   1690
#define NEC_ZERO_SPACE   560
#define NEC_RPT_SPACE   2250

#define OUTPUT_HIGH digitalWrite(OUT_PIN, HIGH);
#define OUTPUT_LOW  digitalWrite(OUT_PIN, LOW)


/* Connect the output of the IR receiver diode to pin 11. */
const int RECV_PIN = 11;
const int OUT_PIN  = 10;

/* Define the IR remote button codes. We're only using the
   least signinficant two bytes of these codes. Each one
   should actually have 0x007F in front of it. Find these codes
   by running the IRrecvDump example sketch included with
   the IRremote library. 

   NOTE: 0x0000 means that button is not defined, this code has no function for that button.
    
   */

const uint16_t BUTTON_BRIGHT = 0x20DF;
const uint16_t BUTTON_DIM    = 0x20DF;
const uint16_t BUTTON_OFF    = 0x40BF;
const uint16_t BUTTON_ON     = 0xC03F;

const uint16_t BUTTON_R = 0x20DF;
const uint16_t BUTTON_G = 0xA05F;
const uint16_t BUTTON_B = 0x609F;
const uint16_t BUTTON_W = 0xE01F;

const uint16_t BUTTON_R1 = 0x0000;
const uint16_t BUTTON_G1 = 0x0000;
const uint16_t BUTTON_B1 = 0x50AF;
const uint16_t BUTTON_FLASH = 0x0000;

const uint16_t BUTTON_R2 = 0x0000;
const uint16_t BUTTON_G2 = 0x0000;
const uint16_t BUTTON_B2 = 0x0000;
const uint16_t BUTTON_STROBE = 0x0000;

const uint16_t BUTTON_R3   = 0x0000;
const uint16_t BUTTON_G3   = 0x0000;
const uint16_t BUTTON_B3   = 0x0000;
const uint16_t BUTTON_FADE = 0x0000;

const uint16_t BUTTON_R4 = 0x0000;
const uint16_t BUTTON_G4 = 0x0000;
const uint16_t BUTTON_B4 = 0x0000;
const uint16_t BUTTON_SMOOTH = 0x0000;

const uint16_t BUTTON_REPEAT = 0xFFFFFFFF;

uint32_t lastCommand = 0;
uint8_t  echo = 1;
uint8_t  light_on = 0;

/* Initialize the irrecv part of the IRremote  library */
IRrecv irrecv(RECV_PIN);
decode_results results; // This will store our IR received codes

uint32_t get_saved_command() {
  // deserialize lastCommand from EEPROM
  if (0x55 == EEPROM[0]) {
    return  ((uint32_t)EEPROM[1]) << 24 | ((uint32_t)EEPROM[2]) << 16 | ((uint32_t)EEPROM[3]) << 8 | EEPROM[4];
  }
  return BUTTON_G;
}

void save_command() {
  // serialize 32b last command to EEPROM
  EEPROM[0] = 0x55;
  EEPROM[1] = (lastCommand >> 24) & 0xFF;
  EEPROM[2] = (lastCommand >> 16) & 0xFF;
  EEPROM[3] = (lastCommand >> 8) & 0xFF;
  EEPROM[4] = (lastCommand & 0XFF);
  return BUTTON_G;
}


void setup()
{
  Serial.begin(9600); // Use serial to debug.
  irrecv.enableIRIn(); // Start the receiver
  pinMode(OUT_PIN, OUTPUT);
  space(0);
  delay(300);
  sendNEC(0xFFFFFFFF, 32);
  delay(10);

  lastCommand = get_saved_command();
  light_on = 1;
  sendNEC(lastCommand, 32);

}


//+=============================================================================
// Custom delay function that circumvents Arduino's delayMicroseconds limit

void custom_delay_usec(unsigned long uSecs) {
  if (uSecs > 4) {
    unsigned long start = micros();
    unsigned long endMicros = start + uSecs - 4;
    if (endMicros < start) { // Check if overflow
      while ( micros() > start ) {} // wait until overflow
    }
    while ( micros() < endMicros ) {} // normal wait
  }
  //else {
  //  __asm__("nop\n\t"); // must have or compiler optimizes out
  //}
}

void  sendNEC (unsigned long data,  int nbits)
{
  space(1);
  // Header
  mark(NEC_HDR_MARK);
  space(NEC_HDR_SPACE);

  // Data
  for (unsigned long  mask = 1UL << (nbits - 1);  mask;  mask >>= 1) {
    if (data & mask) {
      mark(NEC_BIT_MARK);
      space(NEC_ONE_SPACE);
    } else {
      mark(NEC_BIT_MARK);
      space(NEC_ZERO_SPACE);
    }
  }

  // Footer
  mark(NEC_BIT_MARK);
  space(0);  // Always end with the LED off
}

//+=============================================================================
// Sends an IR mark for the specified number of microseconds.
// The mark output is modulated at the PWM frequency.
//
void  mark (unsigned int time) {
  OUTPUT_LOW;
  if (time > 0) custom_delay_usec(time);
}

//+=============================================================================
// Leave pin off for time (given in microseconds)
// Sends an IR space for the specified number of microseconds.
// A space is no output, so the PWM output is disabled.
//
void  space (unsigned int time) {
  OUTPUT_HIGH;
  if (time > 0) custom_delay_usec(time);
}

void  sendRaw (const unsigned int buf[],  unsigned int len,  unsigned int hz)
{

  for (unsigned int i = 0;  i < len;  i++) {
    //    if (i & 1)  space(buf[i]*USECPERTICK) ;
    //    else        mark (buf[i]*USECPERTICK) ;
    if (i & 1)  mark(buf[i]*USECPERTICK) ;
    else        space (buf[i]*USECPERTICK) ;
  }

  space(0);  // Always end with the LED off
}



// loop() constantly checks for any received IR codes. At the
// end it updates the RGB LED.
void loop() {
  if (irrecv.decode(&results)) {
    /* read the RX'd IR into a 16-bit variable: */
    uint16_t resultCode = (results.value & 0xFFFF);

    // This switch statement checks the received IR code against
    // all of the known codes. Each button press produces a
    // serial output, and has an effect on the LED output.


    Serial.println("decoded:");
    Serial.print("Bits read: ");
    Serial.println(results.bits, DEC);
    Serial.print("Value Read 0x");
    Serial.println(results.value, HEX);
    Serial.print("Decoded code type: ");
    Serial.println(results.decode_type, DEC);
    Serial.println("");

    echo = 1;
    switch (resultCode) {
      case BUTTON_OFF:
        light_on = 0;
        break;

      case BUTTON_ON:
        if ( light_on) {
          sendNEC((0xF7L << 16) | BUTTON_R, 32);
          delay(200);
          sendNEC((0xF7L << 16) | BUTTON_G, 32);
          delay(200);
          sendNEC((0xF7L << 16) | BUTTON_B, 32);
          delay(200);
          sendNEC(lastCommand, 32);
          save_command();
          Serial.println("save");
          echo = 0;
        } else {
          sendNEC(get_saved_command(), 32);
          light_on = 1;
          echo = 0;
        }
        break;
      default:
        if ( light_on) {
          if (results.bits == 32 && results.decode_type == NEC) {
            lastCommand = results.value;
          }
          echo = 1;
        } else {
          sendNEC(get_saved_command(), 32);
          light_on = 1;
          echo = 0;
        }
        break;
    }
    if (echo) {
      Serial.println("***");
      sendRaw( results.rawbuf, results.rawlen, 0);
    }
    irrecv.resume(); // Receive the next value
  }

}
