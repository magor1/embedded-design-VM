/*
 * DA5T3.c
 *
 * Created: 4/19/2016 9:17:46 PM
 * Author : r
 */ 

 //TASK 3: Control the position of the Servo motor using a potentiometer
 //connected to any analog-in. Pot value = 0, Servo at 0 degrees.
 //Pot value = MAX (5V), Servo at 180 degrees.
 
#define F_CPU 16000000UL //clock frequency
#define preScaler 64	//prescaler value

#include <avr/io.h>

int main()
{
	uint8_t trimpot_val;	//8-bit int type for potentiometer values
	float servoPosition; //holds value to determine servo position
	
	DDRB |= (1<<PB1); //PWM pin (OC1A)
	DDRC |= (0<<PC0); //potentiometer analog input
	
	//configure timer1
	TCCR1A |= (1<<COM1A1); //COM1A1=1 and COM1A0=0 (Non-inverted mode) (HIGH at bottom, LOW on match)
	
	TCCR1A |= (1<<WGM11) | (0<<WGM10);	//WGM13=1, WGM12=1, WGM11=1, WGM10=0
	TCCR1B |= (1<<WGM13) | (1<<WGM12); //1 1 1 0 => fast PWM using ICR1 (mode 14)
	//FAST PWM is more suited for servo
	
	TCCR1B |= (0<<CS12) | (1<<CS11) | (1<<CS10); //0 1 1 => prescaler 64
	
	//we want T1 to repeat with a period of 20ms or 50Hz.
	ICR1 = ((double)F_CPU) / (preScaler * 50); //value of ICR1 = [(F_CPU)/(prescaler * 50Hz)]
	
	//configure ADC
	ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);	//ADCSRA=0b10000111 => division factor 128
	
	ADMUX |= (0<<REFS1) | (1<<REFS0) | (1<<ADLAR);	//VRef = AVCC with external capacitor at AREF pin.
	// ADLAR high, ADCH = 0b00000000 and ADCL = 0b00xxxxxx
	// https://sites.google.com/site/qeewiki/books/avr-guide/analog-input
	
	while(1)
	{
		ADCSRA |= (1<<ADSC); //ADC conversion of potentiometer value to servo position
		
		while((ADCSRA &(1<<ADIF)) == 0); //loop until ADC done
		
		trimpot_val = ADCH << 1;			//store high byte of ADC
		servoPosition = trimpot_val * 5 / 1024.0;	//scale the potentiometer value
													//5 Volts divided by 2^10 (10 bit ADC)
		
		//OCR1A = 150 => 0 degrees (min)	150 ohms
		//OCR1A = 535 => 180 degrees (max)	535 ohms
		servoPosition =  (servoPosition * 385) + 150; //equation to determine position. Min = 150, Max = 535
		
		OCR1A = (int)servoPosition; //update servo position
	}
}


