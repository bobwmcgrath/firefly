#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

int lumens = 1;  //ADC value from the phototransistor
int lastLumens = 1;  // previous lumens value for comparison
int carrier = 1;  // a variable that is 1 if the ADC reading is higher than the previous reading and 0 if it is lower
int lastCarrier = 1; // the previous carrier value for comparison
int carrierCounter = 1; // number of times in a row that the number of lumens reversed direction i.e.  higher or lower
int blinkCounter = 1; // number of times through the bloop loop, on and off
int senseCounter = 1; // number of times through the sense loop

int setup(void){
	DDRB |= _BV(DDB2); // set as output
}


int bloop (void) {
PORTB |= _BV(PORTB2);
PORTB |= _BV(PORTB0);// turn on
_delay_ms(50);

PORTB &= ~_BV(PORTB2);
PORTB &= ~_BV(PORTB0); // turn off
_delay_ms(50);


}

void shine (void) {	
	blinkCounter +=1;
	if (blinkCounter <50) { //run loop for a number of times
		bloop(); // blink
		shine(); // restart blinking loop
	}
	else {blinkCounter = 0; senseCounter = 0;
	carrierCounter=0;} // set counters back to zero and restart main loop
	
}

int sense (void) {
_delay_ms(50);

lastLumens = lumens;
lastCarrier=carrier;

ADMUX = 1<<MUX0;               // ADC0 (PB0)
ADCSRA = 1<<ADEN | 3<<ADPS0;   // Enable ADC, 125kHz clock
ADCSRA = ADCSRA | 1<<ADSC;     // Start
while (ADCSRA & 1<<ADSC);      // Wait while conversion in progress
lumens = ADCL;               // Copy result to lumens

senseCounter += 1;
if (lumens > lastLumens) carrier = 1;
else carrier = 0;
if (carrier != lastCarrier) carrierCounter+=1;
else carrierCounter=0;
if (carrierCounter==5) senseCounter += 10; // this is the multiplier that modulates the phase variance
if (senseCounter<100) sense(); //why does everything break for values over 11
}

int loop (void) {
	sense();
	shine();

}

int main(void) {
	setup();
	for(;;) loop();
}