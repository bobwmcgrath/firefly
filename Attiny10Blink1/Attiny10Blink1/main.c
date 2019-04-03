#include <util/delay.h>
#include <avr/io.h>

int main() {
	DDRB |= _BV(DDB2); // set as output
	
	while (1) {
		PORTB &= ~_BV(PORTB2); // turn off
		_delay_ms(500);
		
		PORTB |= _BV(PORTB2); // turn on
		_delay_ms(500);
	}
	
	return 0;
}
