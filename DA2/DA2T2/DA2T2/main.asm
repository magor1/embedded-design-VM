;
; DA2T2.asm
;
; Created: 3/8/2016 4:32:04 PM
; Author : User
;

; REQUIREMENTS:		Use PORTB to display an 8-bit binary counter. PC4 will toggle every 5th pulse
;					and PC5 will toggle every 10th pulse. Non-interrupt method.

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

	LDI R22, 0x00			; R22 is counter used for 8-bit counter
	LDI R23, 0x00			; R23 is counter for 5th or 10th rising pulse check

binaryCounter:
	CALL delay_250ms		; 250ms delay procedure
	INC R22					; increment 8-bit counter
	OUT PORTB, R22			; output 8-bit counter on PORTB
	INC R23					; increment count for detecting 5th or 10th rising pulse

; PULSE CHECK PROCEDURE
	CPI R23, 5			; first 5th pulse
	BREQ rising_5		; branch if equal to 5
	CPI R23, 10			; next 5th (10th pulse)
	BREQ rising_10		; branch if equal to 10
	CPI R23, 15			; next 5th (15th pulse)
	BREQ rising_15		; branch if equal to 15
	CPI R23, 20			; next 5th (20th pulse)
	BREQ rising_20		; branch if equal to 15

	JMP next_250ms			; if does not match any case, toggle nothing

rising_5:
	LDI R24, 0x10
	OUT PORTC, R24		; 5th pulse where PC4 = 1 and PC5 = 0
	JMP next_250ms		; finish pulse output, complete the delay
rising_10:
	LDI R24, 0x20
	OUT PORTC, R24		; 10th pulse where PC4 = 0 and PC5 = 1
	JMP next_250ms		; finish pulse output, complete the delay
rising_15:
	LDI R24, 0x30
	OUT PORTC, R24		; 5th pulse where PC4 = 1 and PC5 = 1
	JMP next_250ms		; finish pulse output, complete the delay
rising_20:
	LDI R24, 0x00
	OUT PORTC, R24		; second 10th pulse where PC4 = 0 and PC5 = 0
	LDI R23, 0x00		; set pulse check counter back to zero to redo cases
	JMP next_250ms		; finish pulse output, complete the delay
next_250ms:
	CALL delay_250ms	; delay another 250ms to complete 0.5sec period
	JMP binaryCounter	; count again, don't care about overflow

;	DELAY CALCULATION:
;	4 * 55 * 90 * 100 = 1,980,000
;	We are using 8MHz frequency, so 1,980,000 / 8,000,000 = 0.2475 ? 250ms
;
delay_250ms:
	PUSH R18			; push R18
	PUSH R19			; push R19
	PUSH R20			; push R20 (registers for creating looping branch cycles for delay)

	LDI R18, 55			; R18 = 55
delay1:					
	LDI R19, 90			; R19 = 90
delay2:					
	LDI R20, 100		; R20 = 100
delay3:					
	NOP					; NOP adds extra cycle
	DEC R20
	BRNE delay3			; repeats previous loops to add to clock cycle
	DEC R19
	BRNE delay2
	DEC R18
	BRNE delay1
	
	POP R20				; pop back registers used for loops
	POP R19
	POP R18				
	RET					; finished delay procedure, go back to main
