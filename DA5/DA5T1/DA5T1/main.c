/*
 * DA5T1.c
 *
 * Created: 4/20/2016 1:48:54 AM
 * Author : r
 */ 
 
 //TASK 1: Control the speed of the DC Motor using a potentiometer
 //connected to any analog port.

#define F_CPU 16000000UL //clock frequency
#include <util/delay.h> //needed for delays
#include <avr/io.h>

void motor_delay_ms(int); //delay function in ms

void motor_delay_ms(int n) //delay function
{
	int i;
	for(i=0; i<n; i++)	//loop for "n" ms
	_delay_ms(1);
};

int main()
{
	uint8_t trimpot_val;	//8-bit int type for potentiometer values
	float motorSpeed; //holds value to determine speed of DC motor
	
	DDRD |= (1<<PD6); //(OC0A)
	DDRC |= (0<<PC0); //potentiometer analog input

		//configure ADC
	ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);	//ADCSRA=0b10000111 => division factor 128
	
	ADMUX |= (0<<REFS1) | (1<<REFS0) | (1<<ADLAR);	//VRef = AVCC with external capacitor at AREF pin.
	// ADLAR high, ADCH = 0b00000000 and ADCL = 0b00xxxxxx
	// https://sites.google.com/site/qeewiki/books/avr-guide/analog-input
	
	while(1)
	{
		ADCSRA |= (1<<ADSC); //ADC conversion of potentiometer value to servo position
		
		while((ADCSRA &(1<<ADIF)) == 0); //loop until ADC done
		
		trimpot_val = ADCH << 1;			//potentiometer value = ADC
	
		motorSpeed =  trimpot_val * 5.0 / 1024.0;	//scale the potentiometer value
													//5 Volts divided by 2^10 (10 bit ADC)
		
		PORTD |= (1<<PD6);			//turn on motor
		motor_delay_ms(motorSpeed * 30);	//delay determined by potentiometer
		
		PORTD &= ~(1<<PD6);			//turn off motor
		motor_delay_ms((1 - motorSpeed) * 30); //stop motor (1 minus speed of motor)
	}
}
