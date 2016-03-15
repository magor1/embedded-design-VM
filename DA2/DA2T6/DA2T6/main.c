/*
 * DA2T6.c
 *
 * Created: 3/9/2016 12:13:03 AM
 * Author : r
 */ 

// REQUIREMENTS : Output 8-bit counter on PB0-PB7. PC0 generates 50% DC and 0.5sec period.
//				  PC4 toggles every 5th rising pulse, PC5 toggles every 10th rising pulse.
//				  Toggles are handled by interrupts

#define F_CPU 8000000UL //set clock frequency to 16MHz for compiler

#include <avr/io.h>
#include <avr/interrupt.h>		//use interrupt library

void delay_msec(int ms); //delay function procedure
						//calculation using clock freq. (here we are using 8MHz)

int main(void)
{
	DDRC |= 0x31;	// set PC4 an PC5 as outputs on PORTC
	DDRB |= 0xFF;	//set every pin on PORTB as an output
	
//////////////
//TIMER 0
//////////////
	DDRD &= ~(1<<PD4); //set PORTD.4 as input. pin PD4 uses TIMER0 and PC0 connects to this pin
						//for the binary counter to properly count on the LEDs.
	
	TCCR0A = 0; //timer 0 in normal mode
	TCCR0B = 0x07; //external clock on TIMER0
	TCNT0 = 0;	//initialize binary count sequence
	
//////////////
//INTERRUPTS
//////////////
	PCICR |= (1<<PCIE1); //turn on pin change interrupts for PORTC
	PCIFR |= (1<<PCIF1); //turn on pin change interrupt flag for PORTC
	PCMSK1 |= (1<<PCINT8); //turn on PC0 (PCINT8) for interrupt usage (PCMSK1 is for PORTC)
	
	sei(); //set interrupts
	
//////////////
//BINARY COUNTER
//////////////
	
	while(1) //generate waveform of 50% DC with 0.5 sec period
	{
		PORTC &= ~(1<<PC0);	//PORTC.0 low
		delay_msec(250); //delay for 250 ms
		
		PORTC |= (1<<PC0);	//PORTC.0 high
		PORTB = TCNT0+1;	//binary count sequence incremented by 1 each loop
		
		delay_msec(250); //delay for 250 ms
	}
}

//////////////
//DELAY PROCEDURE
//////////////
//////////////

void delay_msec(int ms)
{
	double timerCalc = 65536 - (((double)ms)/1000)*((double)F_CPU/64);
	//set timer calculation = 2^16 - delay in sec / (clock period/ prescaler 64)

	TCNT1H = ((int)timerCalc) >> 0x08; //move upper byte of TCNT
	TCNT1L = ((int)timerCalc) & 0x00FF; //move lower byte of TCNT
	
	TCCR1A = 0; //Timer 1 Normal Mode
	TCCR1B = 0x03; //Prescaler = 64 (we can easily change prescaler here for emulation)
	
	while(!(TIFR1 & (1<<TOV1))) {	//loop until overflow
	}
	TCCR1B = 0; // stop timer 1
	TIFR1 = 1; // clear overflow flag
	
	return;
}

//////////////
//INTERRUPT SERVICE ROUTINE
//////////////

ISR (PCINT1_vect) //PC0 is in the PCINT1_vect
{
	if(!(PORTC & (1<<PC0)) == 0)	//if PC0 is changed from low to high
	{
		static int risingCounter = 0; //used to detect every 5th and 10th rising edge
		risingCounter++;
		
		if(risingCounter%5 == 0) //PC4 toggles every 5 rising pulses
		PORTC ^= (1<<PC4);
		
		if(risingCounter%10 == 0) //PC5 toggles every 10 rising pulses
		PORTC ^= (1<<PC5);
	}
	
	return;
}
