/*
 * MagoDA1.asm
 *
 *  Created: 2/24/2016 11:10:26 PM
 *   Author: magor
 */ 

/*
 AVR Assembly - DA1

Program tasks: 
a) Store 25 numbers starting from the RAMEND/2 location. [RAMEND/2 is RAM_MIDDLE]
	- Capture the lower 8-bits (lower byte) of the variable/memory location RAM_MIDDLE address and
	use them as your values.
	- RAM_MIDDLE is incremented or decremented to get the subsequent 24 numbers.
	- X/Y/Z registers are used as pointers to fill up 25 numbers from RAM_MIDDLE.
b) Use X/Y/Z register to parse through the 25 numbers and add all numbers divisible by
	7 and place the result in R20:21.
c) Same, only add all numbers divisible by 3 and place result in R23:24.
d) Check and set register R07.3 if the sum is greater than 8-bits.
e) Determine the execution time @ 16MHz/#cycles of your algorithm using the simulation.
	-	The execution time is Stop Watch divided by Cycle Counter.
*/ 

LDI R24, 0		;zero register
LDI R25, 25		;load 25 as counter, 25 numbers

LDI ZH, 0x00	; high byte of RAMBEGIN
LDI ZL, 0x18	; low byte of RAMBEGIN (arbitrary value)
LDI YL, LOW(RAMEND)	; low byte of RAMEND
LDI YH, HIGH(RAMEND) ; high byte of RAMEND

ADC YL, ZL	; add low byte to set RAM_MIDDLE
ADC YH, ZH	; add with carry high byte

LSR YH		; logical shift right, divides unsigned value by 2 (RAMEND/2)
ROR YL		; rotate right with carry (right Y = RAM_MIDDLE = RAMEND/2)

; Sum of #s divisible by 7 are in X, #s divisible by 3 are in Z
LDI XH, 0	; high byte clear
LDI XL, 0	; low byte clear
LDI ZH, 0	; high byte clear
LDI ZL, 0	; low byte clear

/*
------------------------------------------------------------------------
START OF PROCEDURE
------------------------------------------------------------------------
*/

sumLoop: 
MOV R16, YL ; copy YL into R16
ST Y, R16   ; store contents of R16 the data space of Y

MOV R17, R16 ; copy R16 to check if div by 7

/*
------------------------------------------------------------------------
DIVISIBLE BY 7
------------------------------------------------------------------------
*/

div7_Check:
CPI R17, 0 ; compare immediate, # div by 7 is remainder = 0
BREQ div7_Done ; skip next subtraction if remainder is 0
SUBI R17, 7	 ; subtract 7 to simulate division
BRCC div7_Check ;branch if carry clear, branch if > 0 

div7_Done:
CPI R17, 0 ; check if remainder is 0 (div by 7)
BRNE div7_False ; skip addition if number not divisible by 7
ADC XL, R16 ; add number to sum of numbers divisible by 7
ADC XH, R24	; add carry to R24

div7_False:
MOV R17, R16 ;copy R16 to check if div by 3

/*
------------------------------------------------------------------------
DIVISIBLE BY 3
------------------------------------------------------------------------
*/

div3_Check:
CPI R17, 0   ;check if remainder is 0 (means number is divisible by 3)
BREQ div3_Done ;skip next subtraction if remainder is 0
SUBI R17, 3  ;subtract 3 to simulate division
BRCC div3_Check  ;branch if > 0

div3_Done:
CPI R17, 0	;check if remainder is 0 (div by 3)
BRNE div3_False ;skip addition if number not divisible by 3
ADC ZL, R16	;add number to sum of numbers divisible by 3
ADC ZH, R24	;add carry if any

div3_False:
ADIW YH:YL, 1	;add immediate to word, point to next location in ram
DEC R25			;decrement counter
CPI R25, 0		;stops looping after storing 25 numbers
BRNE sumLoop	;loop back for next number


/*
------------------------------------------------------------------------
OVERFLOW CHECK
------------------------------------------------------------------------
*/

ADIW XH:XL, 0	; add immediate 0 to word, use X register to check for overflow
BRCS sumOverflow_Check	; branch if carry set, check carry for overflow

MOV R20, XH		; if no overflow, save sum of numbers div by 7 into R20:R21
MOV R21, XL

sumOverflow_Check:
ADIW ZH:ZL, 0	;pause here to check for overflow
BRCS done		;check carry for overflow on numbers div by 3
MOV R23, ZH		;if no overflow, save sum of numbers div by 3 into R23:R24
MOV R24, ZL		;

MOV R18, R23	; sum registers R23:24
ADC R18, R24
ADC R18, R20	; sum registers R20:21 and store in R18
ADC R18, R21
LDI R19, 8		; set bit 3 (0001000)

CPI R18, 0		; compare if R18 has carry
BRCS setReg7bit3	; branch with carry set if there is carry

setReg7bit3:
MOV R7, R19		; set bit 3 to R7 if the sum of R23:R24 and R20:R21 is over 8 bits

done:
