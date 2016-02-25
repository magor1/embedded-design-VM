/*
 * MagoDA0.asm
 *
 *  Created: 2/11/2016 1:27:37 PM
 *   Author: magor

 * ATMega328P
 * - Write an assembly code to add five random numbers >30 and <60. If the sum produces 
 * an overflow set PORTB.2 pin = HIGH else PORTB.2 pin = LOW.
 * - Determine the execution time / # cycles of your algorithm using the simulation, set
 *  CLOCK speed = 8MHz.
 */ 

 ; Overflow example
 
 LDI R17, 56		; 1st integer
 LDI R18, 56		; 2nd integer
 ADD R17, R18		; R20 = sum of 1st and 2nd integer

 LDI R18, 56		; 3rd integer
 ADD R17, R18		; add 3rd integer to sum
 LDI R18, 56		; 4th integer
 ADD R17, R18		; add 4th integer to sum
 LDI R18, 56		; 5th integer
 ADD R17, R18		; add 5th integer to sum

 BRCC noCarry		; branch if carry clear [bit in status register is 0 (no overflow)]
 SBI PORTB, 2		; if there is an overflow, then set pin PB2 = 1
 jmp done

 noCarry:
 CBI PORTB, 2		; if there is no overflow, then clear pin PB2 (PB2 = 0)

 done:
