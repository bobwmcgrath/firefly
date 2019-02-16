#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <avr/io.h>
#include <util/delay.h>
 
#define BLINK_DELAY_MS 2
/* Example code for the Adafruit TCS34725 breakout library */

/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */

/* Initialise with default values (int time = 2.4ms, gain = 60x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_60X);
int poo = 1;
int lastPoo = 1; 
int counter1 = 1;
int counter2 = 1;
long y,z,w,a,us;
int x = 1;
uint16_t lastLux = 1;
uint16_t butt = 1;
//uint16_t lux = 1;
void setup(void) {
  Serial.begin(115200);
  
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  // Now we're ready to get readings!
}

void loop (void) {
  //shine();
  sense();
  if (x<250)sense();
  else shine();
  //lux();
}
void shine (void) {
  counter2 +=1;
  if (counter2 <500) {
    bloop();
    shine();
    }
  else {counter2 = 0; x = 0; 
  counter1=0;}
  
}
void sense (void) {
  us = micros();
  lastLux = butt;
  lastPoo=poo;
  if (us%4000 >= 3950) lux();
  else sense();
  if (lux > lastLux) poo = 1;
  else poo = 0;
  if (poo != lastPoo) counter1+=1;
  else counter1=0;
  if (counter1==16) x *= 1.1;
  }


void lux() {
  tcs.setInterrupt(true);
  uint16_t r, g, b, c, lux, colorTemp;
  //Serial.print(lux);
  tcs.getRawData(&r, &g, &b, &c);
  // colorTemp = tcs.calculateColorTemperature(r, g, b);
  //colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);
  x += 1;
  butt=lux;
  
  //Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
  Serial.print(x);
  Serial.print("  ");
  Serial.print(counter1);
  //Serial.print("poo: "); Serial.print(x, DEC); Serial.print(" - ");
  //Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  //Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  //Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  //Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");
  
}
int bloop (void){
 /* set pin 5 of PORTB for output*/
 DDRB |= _BV(DDB5);
 
 //while(1) {
  /* set pin 5 high to turn led on */
  PORTB |= _BV(PORTB3);
  _delay_ms(BLINK_DELAY_MS);
 
  /* set pin 5 low to turn led off */
  PORTB &= ~_BV(PORTB3);
  _delay_ms(BLINK_DELAY_MS);
 //}
}
