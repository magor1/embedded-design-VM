/*
 * DA2T4.c
 *
 * Created: 3/8/2016 5:24:22 PM
 * Author : r
 */ 

// REQUIREMENTS:	Blink PC0 with 50% DC and 0.5sec period.

#define F_CPU 8000000UL		// 8MHz Frequency for Simulator

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRC |= (1 << PC0);		// PC0 is output
	
	while(1)				// loop forever blinking PC0
	{
		_delay_ms(250);		// 250ms delay
		PORTC |= 0x01;		// set PC1 high
		_delay_ms(250);		// 250ms delay (completes 0.5sec period)
		PORTC = 0;			// set PC0 low
	}
	
}
