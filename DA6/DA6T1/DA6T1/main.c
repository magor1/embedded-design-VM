/*
 * DA6.c - 
 *
 * Created: 4/23/2015 11:31:09 PM
 *  Author: r
 */ 

 // Write a C AVR program that will monitor the LM34 connected to an Analog pin to
 // display the temperature in F on the Nokia 5110 GLCD module.

#define F_CPU 1000000UL		//1MHz for test purposes
#include <avr/io.h>			//input
#include <avr/pgmspace.h> 	//program space utilities -> uint_XXt filetype
#include <stdio.h>			//for character display on LCD
#include <util/delay.h>		//_delay_ms function

#include "pcd8544.h"		//header file for PCD8544 LCD (similar to Nokia 5110)
#include "pcd8544.c"		//driver for PCD8544 LCD (similar to Nokia 5110)
#include "main.h"			//header file for "Please Wait" icon displayed on LCD

int main()
{
        LcdInit();	//initialize  LCD (black rectangle)
        LcdContrast(0x7F);	 //display image on full contrast
        _delay_ms(1000);	//1 sec delay
        LcdClear();			//clear LCD output
       
        LcdContrast(0x3F);	//low contrast to make letters visible
        _delay_ms(1000);	//1 second delay
        LcdClear();			//clear LCD output
        
        LcdImage(waitImage);	//"Please Wait" icon
		LcdGotoXYFont(1,6);		//6th row, 1st cursor
		LcdFStr(FONT_1X,(unsigned char*)PSTR ("Loading DA6..."));	//fancy output for DA6
        LcdUpdate();			//update output to LCD screen
        _delay_ms(4000);		//4 second delay
		
		ADCSRA |= (1<<ADEN);	//ADC enable
		ADCSRA |=(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);   //clock prescaler 128
		
		ADMUX |= (1<<REFS0)|(1<<ADLAR);	//external capacitor, ADC0 is input
		
		DDRC = 0x00;	//ADC0 input, PC0
		
		uint16_t temp0, temp1, temp2, temp3;		//create temporary veriables for ADC conversion

        while (1)
        {
				ADCSRA |= (1<<ADSC);	//begin ADC
				
				while(!(ADCSRA &(1<<ADSC))==0);		//while not end of conversion
				temp0=(ADCH<<8)|ADCL;	//get ADC bits
                temp1 = temp0 % 10;		//for digit X in temp output XY.Z
                temp2 = temp0 % 100 / 10;	//for digit Y in temp output XY.Z
                temp3 = temp0 % 1000 / 100;	//for digit Z in temp output XY.Z
				
                char displayTemp[4];			//create string array of size 4
				
                sprintf(displayTemp, "%d%d.%d ", temp1, temp2, temp3); //display temperature XY.Z
                LcdClear();	//clear display for final output
				
                LcdGotoXYFont(6,2); //2nd row, 6th cursor
                LcdFStr(FONT_1X,(unsigned char*)displayTemp);	//display temperature XY.Z
		        LcdGotoXYFont(6,4); //4th row, 6th cursor
		        LcdFStr(FONT_1X,(unsigned char*)PSTR ("o"));	//display degrees symbol
		        LcdGotoXYFont(7,6); //6th row, 7th cursor
		        LcdFStr(FONT_2X,(unsigned char*)PSTR ("F"));	//display "F" for Fahrenheit
				
				LcdUpdate();	//update output to LCD screen
				_delay_ms(2000); //2 second delay
				
        }
        return 0;
}