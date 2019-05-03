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

// Tracking the carrier signal

int  lumens         = 1;      // ADC value from the phototransistor
int  lastLumens     = 1;      // previous lumens value for comparison
int  carrier        = 1;      // a variable that is 1 if the ADC reading is higher than the previous reading and 0 if it is lower
int  lastCarrier    = 1;      // the previous carrier value for comparison
int  carrierCounter = 1;      // number of times in a row that the number of lumens reversed direction i.e.  higher or lower
int  blinkCounter   = 1;      // number of times through the bloop loop, on and off
int  senseCounter1  = 1;      // number of times through the sense loop
int  senseCounter2  = 1;      // becuse I cant do floats so this increments one for every 10 senseCounter1s 

int  blinkLength    = 10;     // milliseconds between on and off of carrier blink


// Daylight detection

int  dayThreshold   = 125;    // minimum lumens we think equals daylight (based on 0-255 ADC range - TOTAL GUESS)
long daytimeDetect  = 0L;     // number of continuous times we detect daylight
long dayPause       = 1000L;  // minimum number of times of continuous daylight before we dont blink (NO IDEA WHAT THIS SHOULD BE)


// Clock drift

int  driftCounter    = 0;      // number of sense loops skipped
int  detectCounter   = 0;      // number of times we've adjusted sync
long loopCounter     = 0L;     // total loop count while tracking drift
int  shortBlink;               // one-tenth of blinkLength, but in microseconds
int  adjustedBlink;


void setup(void){
    //DDRB |= _BV(DDB2);      // set as output
    DDRB = 0b0101;
    shortBlink = blinkLength * 100;
}


/*
 *  bloop turns the visible and the infrared led on and off with some frequency
 */
void bloop (void) {

    if (daytimeDetect < dayPause) {

        PORTB |= _BV(PORTB2);
        PORTB |= _BV(PORTB0);  // turn on

        //_delay_ms(blinkLength);
        var_delay();

        PORTB &= ~_BV(PORTB2);
        PORTB &= ~_BV(PORTB0); // turn off

//        _delay_ms(blinkLength);
        var_delay();

    } else {
//        _delay_ms(2*blinkLength);
        var_delay();
        var_delay();
    }

    blinkCounter +=1;

}


/* 
 *  shortBlink is one-tenth the blink length, eg BL = 1 millisecond, SB = 100.
 *  
 *  We take the number of times we've re-synched, times the total amount we re-synched, * the length in milliseconds of BL
 *  (so, the total amount of milliseconds we've "skipped"), as a percentage of the number of total loops, then divided by ten.
 *  
 *  In theory, this calculation yields some small number of microseconds LESS that we should delay each time, in orderr
 *  to speed up our overall pulse rate.
*/
void fix_delay(void) {


   shortBlink = shortBlink - ( (detectCounter * driftCounter * blinkLength) / loopCounter ) / 10;

   // in case of something weird, reset to default
   if (shortBlink < 1) shortBlink = blinkLength *100;

   loopCounter   = 0L;
   detectCounter = 0;
   driftCounter  = 0;

}


void var_delay(void) {

    for (int i=0; i++; i<10) {
        _delay_us(shortBlink);
    }
    
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

//    _delay_ms(blinkLength);
    var_delay();

    
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

    if (carrierCounter == 5) {
        senseCounter1 += senseCounter2;  // this is the multiplier that modulates the phase variance

        detectCounter++;                 // DD - detected a carrier signal
        driftCounter += senseCounter2;   // DD - amount of adjusment
    }
  
    if (lumens > dayThreshold) daytimeDetect += 1;           // if "daytime", increment counter
    else daytimeDetect = 0;                                  // else reset to 0

}




void loop (void) {

    while (senseCounter1 < 100) sense();   // look for carrier signal (blinking light)

    while (blinkCounter < 50)   bloop();   // send out carrier signal (if not daytime)

//  BIG NOTE - there's now a bunch of code meant to vary the total delay (blinkLength)
//  by a small amount (some microseconds).
//  First, we should get daytime detect working.
//
//  THEN, if we ucomment the following line, we can test the clock drift code.  NOTE that
//  uncommenting this line causes the program to *balloon* from 670 bytes to 996 bytes.

//    if (detectCounter > 500) fix_delay();
    
    blinkCounter   = 0; 
    senseCounter1  = 0;
    carrierCounter = 0;
    senseCounter2  = 0;

    loopCounter += 1L;

}



int main(void) {

    setup();
    for(;;) loop();

}
