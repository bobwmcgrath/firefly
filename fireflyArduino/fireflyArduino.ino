#include <Wire.h>
#include <avr/io.h>
#include <util/delay.h>
 
#define BLINK_DELAY_MS 10

long us = 1;
int lumens = 1;
int lastLumens = 1;
int carrier = 1;
int lastCarrier = 1; 
int carrierCounter = 1;
int blinkCounter = 1;
int senseCounter = 1;


void setup(void) {
   DDRB |= _BV(DDB5);
  //Serial.begin(9600);
}

void loop (void) {
  sense();  // read the value of the sensor
  shine();  // blink the carier frequency
}

void sense (void) {
  lastLumens = lumens;
  lastCarrier=carrier; 
  lux();
  if (lumens > lastLumens) carrier = 1;
  else carrier = 0;
  if (carrier != lastCarrier) carrierCounter+=1;
  else carrierCounter=0;
  if (carrierCounter==5) senseCounter *= 1.1; // this is the multiplier that modulates the phase variance
  if (senseCounter<100) sense();
  }


void lux() {
  _delay_ms(BLINK_DELAY_MS);
  lumens = (analogRead(A0)/4);
  senseCounter += 1;
  //print();
  
}

void shine (void) {
  blinkCounter +=1;
  //print();
  if (blinkCounter <50) { //run loop for 1 second
    bloop(); // blink on 10ms and off 10ms
    shine(); // restart blinking loop
    }
  else {blinkCounter = 0; senseCounter = 0; 
  carrierCounter=0;} // set counters back to zero and restart main loop
  
}

int bloop (void){
 /* set pin 5 of PORTB for output*/
 
  /* set pin 5 high to turn led on */
  PORTB |= _BV(PORTB5);
  _delay_ms(BLINK_DELAY_MS);
 
  /* set pin 5 low to turn led off */
  PORTB &= ~_BV(PORTB5);
  _delay_ms(BLINK_DELAY_MS);
}

void print(void){
  Serial.print(lumens);
  //Serial.print("  ");
  //Serial.print(senseCounter);
  //Serial.print("  ");
  //Serial.print(blinkCounter);
  //Serial.print(" ");
  //Serial.print(carrierCounter);
  Serial.println(" ");
}
