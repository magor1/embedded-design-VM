;
; DA2T1.asm
;
; Created: 3/8/2016 3:11:13 PM
; Author : r
;

; REQUIREMENTS : Generate a 50% DC with 0.5sec period on PORTC.0

; FOR 8MHZ CLOCK
	SBI DDRC, 0			; set PC0 output
	CBI DDRC, 0			; clear PC0 bit

PC0_Blink:
	CALL delay_250ms	; call 1/4 second delay procedure (current counter?250ms)
	LDI  R17, 1			; R17 = 1
	OUT PORTC, R22		; set PC0 high

	CALL delay_250ms	; call 1/4 second delay procedure (current counter?500ms)
	LDI  R17, 0			; R17 = 0
	OUT PORTC, R22		; set PC0 low
	JMP PC0_Blink		; repeat procedure

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
