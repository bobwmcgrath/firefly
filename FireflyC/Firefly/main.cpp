/*
 * 
 *   Bob McCgrath, Neil Verplank
 *  
 *   Firefly.ccp is a simple program for the attiny series microcontroller that turns
 *   an LED on and off slowly (like a firefly), but is also "secretly" turning the LED
 *   and a corresponding infrared LED on and off every few milliseconds to create a 
 *   "carrier" signal.  We look for that signal, and when detected, "speed up" a bit to 
 *   try and stand in synch with our neighbor. NOT seeing a signal means we're in synch,
 *   or there's no nearby neighbor.
 *   
 *     
 */



#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

int  lumens         = 1;      // ADC value from the phototransistor
int  lastLumens     = 1;      // previous lumens value for comparison
int  carrier        = 1;      // a variable that is 1 if the ADC reading is higher than the previous reading and 0 if it is lower
int  lastCarrier    = 1;      // the previous carrier value for comparison
int  carrierCounter = 1;      // number of times in a row that the number of lumens reversed direction i.e.  higher or lower
int  blinkCounter   = 1;      // number of times through the bloop loop, on and off
int  senseCounter1  = 1;      // number of times through the sense loop
int  senseCounter2  = 1;      // becuse I cant do floats so this increments one for every 10 senseCounter1s 

int  blinkLength    = 10;
int  dayThreshold   = 125;    // minimum lumens we think equals daylight
long daytimeDetect  = 0L;     // number of continuous times we detect daylight
long dayPause       = 1000L;  // minimum number of times of continuous daylight before we dont blink (NO IDEA WHAT THIS SHOULD BE)



void setup(void){
    //DDRB |= _BV(DDB2);   // set as output
    DDRB = 0b0101;
}


/*
 *  bloop turns the visible and the infrared led on and off with some frequency
 */
void bloop (void) {

    if (daytimeDetect < dayPause) {

        PORTB |= _BV(PORTB2);
        PORTB |= _BV(PORTB0);  // turn on

        _delay_ms(blinkLength);

        PORTB &= ~_BV(PORTB2);
        PORTB &= ~_BV(PORTB0); // turn off

        _delay_ms(blinkLength);

    } else {
        _delay_ms(2*blinkLength);
    }

    blinkCounter +=1;

}


/*
 * 
 * Sense is detecting the incoming lumens.  It is also looking for a periodic "on/off" lumen signal.
 * If the lights are in synch, no carrier is detected.  If we detect at least 5 "blips", we *are* seeing
 * a signal, but it's out of phase, and we "speed up".  Speed up equates to completing the total sense
 * loop count faster by incrementing the loop counter.  The later in the sense / loop cycle, the more
 * we increment the loop counter.
 * 
 * We are also looking for "daytime" and incrementing a counter if detected.
 * 
 */
void sense () {

    _delay_ms(blinkLength);

    lastLumens  = lumens;
    lastCarrier = carrier;

    ADMUX  = 1<<MUX0;                                        // ADC0 (PB0)
    
    
    ADCSRA = 1<<ADEN | 3<<ADPS0;                             // Enable ADC, 125kHz clock
    ADCSRA = ADCSRA  | 1<<ADSC;                              // Start

    while (ADCSRA & 1<<ADSC);                                // Wait while conversion in progress
    lumens = ADCL;                                           // Copy result to lumens

    senseCounter1 += 1;                                      // Least significant digit
    if (senseCounter1 % 10 == 0) senseCounter2 += 1;         // Most signfificant digit (10's)

    if (lumens > lastLumens) carrier = 1;
    else carrier = 0;

    if (carrier != lastCarrier) carrierCounter += 1;
    else carrierCounter = 0;

    if (carrierCounter == 5)  senseCounter1 += senseCounter2;  // this is the multiplier that modulates the phase variance
  
    if (lumens > dayThreshold) daytimeDetect += 1;           // if "daytime", increment counter
    else daytimeDetect = 0;                                  // else reset to 0

}




void loop (void) {

    while (senseCounter1 < 100) sense();   // look for carrier signal (blinking light)

    while (blinkCounter < 50)   bloop();   // send out carrier signal (if not daytime)

    blinkCounter   = 0; 
    senseCounter1  = 0;
    carrierCounter = 0;
    senseCounter2  = 0;

}



int main(void) {

    setup();
    for(;;) loop();

}
