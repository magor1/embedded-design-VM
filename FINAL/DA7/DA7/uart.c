#define F_CPU 16000000UL
#define BAUD 9600
//#define BAUD_PRESCALER (((F_CPU/(BAUD*16UL))) - 1 )

#include "uart.h"
#include <util/setbaud.h>


// initialize UART
void uart_init() {
   
  UBRR0H = UBRRH_VALUE; //(uint8_t)(BAUD_PRESCALER >> 8);
  UBRR0L = UBRRL_VALUE; //(uint8_t)(BAUD_PRESCALER);

  UCSR0A &= ~(_BV(U2X0));

  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // 8-bit data
  UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);   // Enable RX and TX

  
}

void uart_putchar(char data) {
    while ( !( UCSR0A & (1<<UDRE0)) );
    UDR0 = data;
}

char uart_getchar(void) {
    while ( !(UCSR0A & (1<<RXC0)) ) ;
    return UDR0;
}

void uart_putstring(char * s) {
  int i = 0;
  while(s[i]!='\0'){
    uart_putchar(s[i]);
    ++i;
  };
  
}

void uart_putint16(int16_t data) {
	uint8_t a = data & 0xFF;
	uart_putchar(a);
	a = data >> 8;
	uart_putchar(a);
}


void uart_putdouble(double data){
	
	char datastring[10];
	snprintf(datastring, 10, "%f", data);
	uart_putstring(datastring);
	
}



