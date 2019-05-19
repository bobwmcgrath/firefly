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

int lumens = 1;  //ADC value from the phototransistor
int lastLumens = 1;  // previous lumens value for comparison
int carrier = 1;  // a variable that is 1 if the ADC reading is higher than the previous reading and 0 if it is lower
int lastCarrier = 1; // the previous carrier value for comparison
int carrierCounter = 1; // number of times in a row that the number of lumens reversed direction i.e.  higher or lower
int shineCounter = 1; // number of times through the bloop loop, on and off
int senseCounter1 = 1; // number of times through the sense loop
int senseCounter2 =1; // becuse I cant do floats so this increments one for every 10 senseCounter1s
int dayCounter =1;

int setup(void){
	//DDRB |= _BV(DDB2); // set as output
	DDRB = 0b0101;
}


int shine (void) {
	if (dayCounter < 98){
		PORTB |= _BV(PORTB2);
		_delay_ms(1);
		PORTB |= _BV(PORTB0);// turn on


		PORTB &= ~_BV(PORTB2);
		_delay_ms(1);
		PORTB &= ~_BV(PORTB0); // turn off


	shineCounter +=1;}
	else {shineCounter +=1;
	_delay_ms(2);}
	return shineCounter;

}



void sense () {
	_delay_ms(1);
	lastLumens = lumens;
	lastCarrier=carrier;

	ADMUX = 1<<MUX0;               // ADC0 (PB0)
	ADCSRA = 1<<ADEN | 3<<ADPS0;   // Enable ADC, 125kHz clock
	ADCSRA = ADCSRA | 1<<ADSC;     // Start
	while (ADCSRA & 1<<ADSC);      // Wait while conversion in progress
	lumens = ADCL;               // Copy result to lumens

	senseCounter1 += 1;
	if (lumens > 235) dayCounter+=1;
	if (senseCounter1 % 10 == 0) senseCounter2 += 1;
	if (lumens > lastLumens) carrier = 1;
	else carrier = 0;
	if (carrier != lastCarrier) carrierCounter += 1;
	else carrierCounter=0;
	if (carrierCounter==8) {
		senseCounter1 += senseCounter2; // this is the multiplier that modulates the phase variance
		dayCounter=0;}
	//return senseCounter1;
}

int loop (void) {
	//sense();
	while (senseCounter1<1000) sense();
	//shine();
	while(shineCounter <500) shine(); // blink
	//shine(); // restart blinking loop
	//}
	//else
	shineCounter = 0;
	senseCounter1 = 0;
	carrierCounter=0;
	senseCounter2=0;
	dayCounter=0;
}

int main(void) {
	setup();
	for(;;) loop();
}
