#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

/*

    firefly.cpp

    May 2019    Bob McGrath, Neil Verplank

    https://github.com/bobwmcgrath/firefly


    Code intended for an ATtiny 10 or similar.  Board consists of a visible LED, an infrared LED, and uses an infrared transistor as a receiver.
    Basic operation is to blink in the millisecond range to a) create a carrier signal, and b) control the power consumption (also for a visible fade).
    Essentially, they blink really really fast to create a carrier signal, but visibly appear to blink slowly on and off.  They don't blink if they 
    continuously sense "daylight" to conserve power.  They synch by speeding up slightly when they detect a carrier signal (because recieving a signal
    while you are transmitting immplies you're out of synch).

*/

int lumens = 1;  		//ADC value from the phototransistor
int lastLumens = 1;  		// previous lumens value for comparison
int carrier = 1;  		// a variable that is 1 if the ADC reading is higher than the previous reading and 0 if it is lower
int lastCarrier = 1; 		// the previous carrier value for comparison
int carrierCounter = 1; 	// number of times in a row that the number of lumens reversed direction i.e.  higher or lower
int shineCounter = 1; 		// number of times through the bloop loop, on and off
int senseCounter1 = 1; 		// number of times through the sense loop
int senseCounter2 =1; 		// becuse I cant do floats so this increments one for every 10 senseCounter1s
int dayCounter =1;
//int dayFlag =0;
int pwm=1;



int setup(void){
	//DDRB |= _BV(DDB2); // set as output
	TCCR0A = 2<<COM0A0 | 3<<WGM00; // 10-bit PWM on OC0A (PB0), non-inverting mode
	TCCR0B = 0<<WGM02 | 1<<CS00;   // Divide clock by 1
	DDRB = 0b0101;
}


/* 
	Transmit

	Blink on and off rapidly, or just delay if it's bright out 
*/
void shine (void) {

	if (dayCounter<990){
		PORTB |= _BV(PORTB2);  		// turn on
		_delay_ms(1);

		PORTB &= ~_BV(PORTB2);		// turn off
		_delay_ms(1);

		pwm = 23+2*shineCounter;	// adjust visible blink rate for fade effect
		OCR0A = pwm;

	} else {
		_delay_ms(2);			// too bright out, just do the delay to stay in synch
	}

	shineCounter +=1;

}





/*
	Receive

	Look for a) lumens and b) a fluctuation between on and off (a carrier signal)

  	If we see a regular bright / dim pattern, in perfect alignment with our sensing, that's another unit
        transmitting while we're receiveing, so *we* speed up.  A little if at the beginning of receiving, a lot
	towards the end.  They synch within about 10 blinks.

*/
void sense () {

	_delay_ms(1);
	lastLumens = lumens;
	lastCarrier=carrier;

        // Read sensor
	ADMUX = 1<<MUX0;              	// ADC0 (PB0)
	ADCSRA = 1<<ADEN | 3<<ADPS0;   	// Enable ADC, 125kHz clock
	ADCSRA = ADCSRA | 1<<ADSC;     	// Start
	while (ADCSRA & 1<<ADSC);      	// Wait while conversion in progress
	lumens = ADCL;               	// Copy result to lumens

        // Adjust fade	
	if (pwm>5)pwm -= 3;
	if (pwm<5)pwm = 0;
	OCR0A = pwm;
	
	// Using a high order and low order counter (no float on the ATtiny)
	senseCounter1 += 1;
	if (lumens > 235) dayCounter+=1;
	if (senseCounter1 % 10 == 0) senseCounter2 += 1;

	// State change equals carrier detect
	if (lumens > lastLumens) carrier = 1;
	else carrier = 0;
	if (carrier != lastCarrier) carrierCounter += 1;
	else carrierCounter=0;

	// Eight recurring detects looks like a signal!
	if (carrierCounter==8) {
		senseCounter1 += senseCounter2;	// this is the multiplier that modulates the phase variance
		dayCounter=0;			// continusouly reset daycounter if we're receiving
	}	
	
}



void loop (void) {
	while (senseCounter1<1000) sense();	// Receive / dark
	
	while(shineCounter <500) shine(); 	// Transmit / blink
	
	shineCounter 	= 0;
	senseCounter1 	= 0;
	carrierCounter	= 0;
	senseCounter2	= 0;
	dayCounter	= 0;
}



int main(void) {
	setup();
	for(;;) loop();
}
