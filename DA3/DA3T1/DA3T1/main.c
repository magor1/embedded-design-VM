/*
 * DA3T1.c
 *
 * Created: 3/24/2016 5:01:14 PM
 *  Author: magor
 */ 

#define F_CPU 16000000UL //16 MHz clock
#define BAUD 9600		//9600 Baud Rate (default)
#define MYUBRR (double)F_CPU/16/BAUD - 1	//given baud rate calculation
#define TCNT1_VALUE 65536 - ((double)F_CPU/256) //calculation for Timer 1 Counter (1s Period)
#include <avr/io.h>
#include <avr/interrupt.h>

void init_Timer1 (void)
{
	TCNT1 = TCNT1_VALUE; //overflow in 1 sec
	TCCR1A = 0; //normal mode
	TCCR1B = 4; //prescaler = 256
}

int main(void)
{
	DDRC &= (0<<PORTC0);	//PC0 = ADC0 for ADC input
	
	UCSR0B = (1<<TXEN0); //enable TX
	UCSR0C = ((1<<UCSZ01)|(1<<UCSZ00)); //set Asynchronous Mode
	UBRR0L = MYUBRR;	// Baud Rate 9600, 16MHZ
	
	ADCSRA = 0x87; 
//0b10000111, 7th bit set for ADEN. Bit 0-3 set to 111 for prescaler 128
	ADMUX |= (1<<REFS0) | (1<<REFS1); //use internal VREF for LM34
	sei(); //enable interrupt
	
	init_Timer1 ();	//configure timer 1 to interrupt every second
	
	TIMSK1	|= (1<<TOIE1); //enable Timer Overflow Interrupt
	
	while(1);
	{
		// loop forever with interrupts
	}
	
	return 0;
}

void USART_TX_Out(char *ch) //takes a string and sends it serially
{
	while(*ch != '\0')	//send chars until NULL is found
	{
		while(!(UCSR0A & (1<<UDRE0))); //wait for UDRE0 to be 1
		UDR0 = *ch;	//send char serially
		ch++; //point to next char
	}
}

ISR(TIMER1_OVF_vect) //timer1 overflow ISR
{
	TCCR1B = 0; //stop timer 1
	TIFR1 = 1; //clear overflow flag

	int adc_temp;	//stores ADC temporarily
	float adc_temp_Float; //float for calculations
	int adc_LeftSide; //left side of the value before decimal point
	int adc_RightSide; //right side of the value before decimal point
	
	ADCSRA |= (1<<ADSC); //ADC start conversion
	while((ADCSRA &(1<<ADIF)) == 0); //while conversion isn't finished
	
	adc_temp = ADC;	//get ADC value (LM34 Vout)
	
	adc_temp_Float = (float)adc_temp * (1.1 / 1024) / 0.01; 
//AREF = 1.1V, 10 bit ADC = 1024, 10mV°F Scale Factor
	adc_LeftSide = (int)adc_temp_Float; //integer part before decimal point
	
	adc_temp_Float = adc_temp_Float - adc_LeftSide; //subtract left side
	adc_RightSide = (int)(adc_temp_Float * 10); //decimal part before decimal point

	char string_temp[18] = "LM34 TEMPERATURE: ";

	char TempInteger[27]; //stores temperature to be displayed
	sprintf(TempInteger, "%d", adc_LeftSide); //integer pAart to string
	strcat(TempInteger, "."); //concatenate dot
	
	char TempDecimal[6]; //stores decimal part
sprintf(TempDecimal, "%d°F\r\n", adc_RightSide); 
// %d displays integer value of decimal value
	//output degrees Fahrenheit to the end of the
	//transmission. \r\n combination of carriage return
	//and line feed prevents scattered data in serial terminal

	strcat(TempInteger, TempDecimal); //concatenate integer and decimal parts
	
	USART_TX_Out(string_temp);	//display finalized temperature value string on serial terminal
	
	init_Timer1 ();	//configure timer 1 to interrupt every second
	
	return;
}
