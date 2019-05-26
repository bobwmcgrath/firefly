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
//int dayFlag =0;
int pwm=1;

void setup(void){
	//DDRB |= _BV(DDB2); // set as output
	TCCR0A = 2<<COM0A0 | 3<<WGM00; // 10-bit PWM on OC0A (PB0), non-inverting mode
	TCCR0B = 0<<WGM02 | 1<<CS00;   // Divide clock by 1
	DDRB = 0b0101;
}


void shine (void) {
	if (dayCounter<240){
		PORTB |= _BV(PORTB2);
		_delay_ms(4);
		//PORTB |= _BV(PORTB0);// turn on


		PORTB &= ~_BV(PORTB2);
		_delay_ms(4);
		//PORTB &= ~_BV(PORTB0); // turn off

		pwm = 23+2*shineCounter;
		OCR0A = pwm;

		} else {
		_delay_ms(8);
	}
	shineCounter +=1;

	//return shineCounter;

}



void sense () {
	_delay_ms(4);
	lastLumens = lumens;
	lastCarrier=carrier;

	ADMUX = 1<<MUX0;               // ADC0 (PB0)
	ADCSRA = 1<<ADEN | 3<<ADPS0;   // Enable ADC, 125kHz clock
	ADCSRA = ADCSRA | 1<<ADSC;     // Start
	while (ADCSRA & 1<<ADSC);      // Wait while conversion in progress
	lumens = ADCL;               // Copy result to lumens

	
	if (pwm>5)pwm -= 3;
	if (pwm<5)pwm = 0;
	OCR0A = pwm;
	
	senseCounter1 += 1;
	if (lumens > 225) dayCounter+=1;
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

void loop (void) {
	//shine();
	//pwm = 0;
	while(shineCounter <125) shine(); // blink
	dayCounter=0;
	//sense();
	while (senseCounter1<250) sense();
	//if (dayCounter<991) dayFlag=0;
	//if (dayCounter>990) dayFlag=1;
	//shine(); // restart blinking loop
	//}
	//else
	
	shineCounter = 0;
	senseCounter1 = 0;
	carrierCounter=0;
	senseCounter2=0;

	//return dayFlag;
}

int main(void) {
	setup();
	for(;;) loop();
}
