;
; DA2T3.asm
;
; Created: 3/8/2016 4:59:29 PM
; Author : r
;

.ORG 0x00			; PC = 0x00
RJMP INIT			; Initialize Stack at PC = 0
.ORG 0x16			; TIM1_COMPA in interrupt table is 0x16
RJMP TIM1_COMPA		; Timer 1 Compare A Handler

INIT:				; Macro that initializes stack pointer
.MACRO INITSTACK
	LDI	R16, HIGH(RAMEND)	
	OUT SPH, R16	
	LDI R16, LOW(RAMEND)	
	OUT SPL, R16	
.ENDMACRO
INITSTACK
RJMP main

main:
	SBI DDRB, 0			; output PB0
	SBI DDRB, 1			; output PB1
	SBI DDRB, 2			; output PB2
	SBI DDRB, 3			; output PB3
	SBI DDRB, 4			; output PB4
	SBI DDRB, 5			; output PB5
	SBI DDRB, 6			; output PB6
	SBI DDRB, 7			; output PB7
	SBI DDRC, 0			; output PC0 (pulse)
	SBI DDRC, 4			; output PC4 (toggle 5th pulse)
	SBI DDRC, 5			; output PC6 (toggle 10th pulse)

	LDI R20, 0x00			; R20 is counter used for 8-bit counter
	LDI R23, 0x00			; R23 is counter for 5th or 10th rising pulse check

	LDI R24, 0x0F		; R24 used as temporary value register. load 0x0F into R24
	STS OCR1AH, R24		; store 0x0F into the high register of OCR1A
	LDI R24, 0x42		; load 0x42 into R24
	STS OCR1AL, R24		; store 0x42 into the low register of OCR1A
	LDI R24, 0x00		; load 0x00 into R24
	STS TCCR1A, R24		; set CTC and have OC1A be disconnected to not change bits on bargraph
	LDI R24, 0x0D		; load 0x0D into R24
	STS TCCR1B, R24		; set prescaler to 1024 and WGM12 bit high
	LDI R24, 0x02		; load 0x02 into R24
	STS TIMSK1, R24		; set OCIEA1 high to enable interrupts for OCF1A
	
	SEI					; enable global interrupts
	
loop:
	RJMP loop	; relative jump, looping forever while interrupt occurs at 0.5 sec intervals

; INTERRUPT PROCEDURE

TIM1_COMPA:				; Timer 1 Compare A Handler
	INC R20				; binary counter is incremented
	OUT PORTB, R20		; update value of counter on LED bargraph
	
	INC R23				; increment register checking for 5th/10th rising edge
	
; DETECT RISING PULSES

	CPI R23, 5			; compare immediate, 5th pulse
	BREQ rising_5		; if it is the 5th pulse, branch
	CPI R23, 10			; compare immediate, 10th pulse
	BREQ rising_10		; if it is the 10th pulse, branch
	CPI R23, 15			; compare immediate, 15th pulse
	BREQ rising_15		; if it is the 15th pulse, branch
	CPI R23, 20			; compare immediate, 20th pulse
	BREQ rising_20		; if it is the 20th pulse, branch
	JMP negEdge			; jump to negative edge if not a 5th or 10th pulse
	
		rising_5:
			LDI R16, 0x10
			OUT PORTC, R16	; PC4 = 0 and PC5 = 1, Toggle PC4
			JMP negEdge		; toggle done, jump to negative edge
		rising_10:
			LDI R16, 0x20
			OUT PORTC, R16	; PC4 = 1 and PC5 = 0, Toggle PC5
			JMP negEdge		; toggle done, jump to negative edge
		rising_15:
			LDI R16, 0x30
			OUT PORTC, R16	; PC4 = 1 and PC5 = 1, Toggle PC4 and PC5
			JMP negEdge		; toggle done, jump to negative edge
		rising_20:
			LDI R16, 0x00
			OUT PORTC, R16		; second 10th pulse where PC4 = 0 and PC5 = 0
			LDI R23, 0x00		; set counter back to zero to redo toggle check
			JMP negEdge		    ; toggle done, jump to negative edge
	
		negEdge:
			LDI R24, 0x02		; load 0x02 into R24
			STS TIFR1, R24		; clear OCF1A flag
			RETI				; return and enable interrupts
