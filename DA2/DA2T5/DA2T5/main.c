/*
 * DA2T5.c
 *
 * Created: 3/8/2016 11:55:01 PM
 * Author : r
 */ 

// REQUIREMENTS : Output 8-bit counter on PB0-PB7. 0.5sec delay. PC4 toggled every 5th
//				  rising edge and PC5 is toggled every 10th rising edge.

#define F_CPU 8000000UL		// 8MHz Frequency for Simulator

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	int binaryCounter = 0;		// set counter for LED bargraph starting at 0
	int risingCounter = 0;		// set counter for 5th/10th rising edge starting at 0
	DDRB = 0xFF;			// PB0-PB7 set to outputs
	DDRC = 0x30;			// PC4 and PC5 set to outputs
	
	while(1)				//count sequence loop forever
	{
		_delay_ms(250);		// 250ms delay
		binaryCounter++;			// increment counter after delay		
		PORTB = binaryCounter;		// output counter to LED
		
		risingCounter++;			//used to detect every 5th and 10th rising edge
		
		if(risingCounter%5 == 0) //PC4 toggles every 5 rising pulses
		PORTC ^= (1<<PC4);
		
		if(risingCounter%10 == 0) //PC5 toggles every 10 rising pulses
		PORTC ^= (1<<PC5);
		
		_delay_ms(250);			// 250ms delay (completes 0.5sec period)
	}
	
}


