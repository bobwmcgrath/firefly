#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

int lumens = 1;  //ADC value from the phototransistor
int lastLumens = 1;  // previous lumens value for comparison
int carrier = 1;  // a variable that is 1 if the ADC reading is higher than the previous reading and 0 if it is lower
int lastCarrier = 1; // the previous carrier value for comparison
int carrierCounter = 1; // number of times in a row that the number of lumens reversed direction i.e.  higher or lower
int blinkCounter = 1; // number of times through the bloop loop, on and off
int senseCounter1 = 1; // number of times through the sense loop
int senseCounter2 =1; // becuse I cant do floats so this increments one for every 10 senseCounter1s 

int setup(void){
	//DDRB |= _BV(DDB2); // set as output
	DDRB = 0b0101;
}


int bloop (void) {
PORTB |= _BV(PORTB2);
PORTB |= _BV(PORTB0);// turn on
_delay_ms(10);

PORTB &= ~_BV(PORTB2);
PORTB &= ~_BV(PORTB0); // turn off
_delay_ms(10);

blinkCounter +=1;
return blinkCounter;
}

//int shine (void) {	
	//_delay_ms(5000);
	//bloop();
	//bloop();
	//blinkCounter +=1;
	//if (blinkCounter <50) { //run loop for a number of times
		//bloop(); // blink
		//shine(); // restart blinking loop
	//}
	//else 
	//{blinkCounter = 0; senseCounter = 0;
	//carrierCounter=0;} // set counters back to zero and restart main loop
	//return blinkCounter;
	
//}

void sense () {
_delay_ms(10);
lastLumens = lumens;
lastCarrier=carrier;

ADMUX = 1<<MUX0;               // ADC0 (PB0)
ADCSRA = 1<<ADEN | 3<<ADPS0;   // Enable ADC, 125kHz clock
ADCSRA = ADCSRA | 1<<ADSC;     // Start
while (ADCSRA & 1<<ADSC);      // Wait while conversion in progress
lumens = ADCL;               // Copy result to lumens

senseCounter1 += 1;
if (senseCounter1 % 10 == 0) senseCounter2 += 1;
if (lumens > lastLumens) carrier = 1;
else carrier = 0;
if (carrier != lastCarrier) carrierCounter += 1;
else carrierCounter=0;
if (carrierCounter==5) senseCounter1 += senseCounter2; // this is the multiplier that modulates the phase variance
//return senseCounter1;
}

int loop (void) {
	//sense();
	while (senseCounter1<100) sense(); 
	//shine();
	while(blinkCounter <50) bloop(); // blink
		//shine(); // restart blinking loop
		//}
		//else
		blinkCounter = 0; 
		senseCounter1 = 0;
		carrierCounter=0;
		senseCounter2=0;

}

int main(void) {
	setup();
	for(;;) loop();
}
