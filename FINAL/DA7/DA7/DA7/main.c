/*
 * DA7.c
 *
 * Created: 5/4/2016 12:48:55 AM
 * Author : r
 */ 

// Muscle Movement Detection Unit
//
// Captures data from Gyro/Accelerometer
// Implements UART and I2C using BT


#define F_CPU 16000000UL	//clock frequency (Fosc)
#define BAUD 9600		// Baud Rate 9600, matching 

#define GYRO_SENSITIVITY 65.536		// referenced from [1]
#define M_PI 3.14159265359		//pi

#include <avr/io.h>		//AVR input/output
#include <util/setbaud.h> //requires that entry values already defined for F_CPU and BAUD
#include <math.h>

#include <inttypes.h> //necessary to use uint types from stdint.h, and printf functions
#include <util/delay.h>	//_delay_ms procedure for millisecond delay
#include <avr/interrupt.h> //interrupt enable for serial terminal

#include "i2c.h"
#include "mpu6050.h"
#include "mpu6050_reg.h"
#include "uart.h"

void timer_setup();			// referenced from [2]
void get_time(double* dt);
volatile double count;
const double unit_t = 8/16000000;

int main(void){		//variable declarations and procedures referenced from [2]

	sei();
	uart_init();
	i2c_init();

	DDRB |= _BV(5);

	uint8_t ret;

	int16_t accel_buff[3], gyro_buff[3];
	double accelX, accelY, accelZ;
	double gyroX, gyroY, gyroZ;
	double dt;		
	float pitch;	//angle around x axis
	float roll;		//angle around y axis
	float pitch_accel, roll_accel;
	float pitch_count = 0;
	float roll_count = 0;
	int count_previous_x = 0;
	int count_previous_y = 0;

	// initialize & test MPU5060 availability
	ret = i2c_start(MPU6050_ADDRESS+I2C_WRITE);
	if(~ret){
		PORTB |= _BV(5);
		_delay_ms(200);
		PORTB &= ~(_BV(5));
	}

	mpu6050_init();
	timer_setup();

	mpu6050_read_accel_ALL(accel_buff);

	while(1){
		get_time(&dt);
		mpu6050_read_accel_ALL(accel_buff);
		mpu6050_read_gyro_ALL(gyro_buff);
		
		// acceleration (m/s^2)
		accelX = accel_buff[0]*9.8*2/32768;
		accelY = accel_buff[1]*9.8*2/32768;
		accelZ = accel_buff[2]*9.8*2/32768;
		
		// gyro rate (degrees/s)
		gyroX = gyro_buff[0]*(180/M_PI)*1000/32768;
		gyroY = gyro_buff[1]*(180/M_PI)*1000/32768;
		gyroZ = gyro_buff[2]*(180/M_PI)*1000/32768;
		
/////////////////////////////////////////
// Complementary Filter referenced from [1]
/////////////////////////////////////////
		
		// Integrate the gyroscope data -> int(angularSpeed) = angle
		pitch = pitch + ((float)gyro_buff[0] / GYRO_SENSITIVITY) * dt;
		roll = roll - ((float)gyro_buff[1] / GYRO_SENSITIVITY) * dt;
		
    // Compensate for drift with accelerometer data if !bull
    // Sensitivity = -2 to 2 G at 16Bit -> 2G = 32768 && 0.5G = 8192
		int approx_force_magnitude = abs(accel_buff[0]) + abs(accel_buff[1]) + abs(accel_buff[2]);
		
		if(approx_force_magnitude > 8192 && approx_force_magnitude < 32768)
		{
			// Turning around the X axis results in a vector on the Y-axis
			// Also known as "Roll"
			pitch_accel = atan2f((float)accel_buff[1], (float)accel_buff[2]) * 180 / M_PI; //in degrees
			pitch = pitch * 0.98 + pitch_accel * 0.02;
			if((pitch > 25 || pitch <-25) && count_previous_x == 0)
			{
				pitch_count += 1;
				count_previous_x = 1;
			}
			
			else if (!(pitch > 25 || pitch <-25)) count_previous_x = 0;
			
			// Turning around the Y axis results in a vector on the X-axis
			// Also known as "Pitch"
			roll_accel = atan2f((float)accel_buff[0], (float)accel_buff[2]) * 180 / M_PI; //in degrees
			roll = roll * 0.98 + roll_accel * 0.02;
			if((roll > 25 || roll <-25) && count_previous_y == 0)
			{
				roll_count += 1;
				count_previous_y = 1;
			}
			
			else if(!(roll > 25 || roll <-25)) count_previous_y = 0;
		}
		
		//App: Bluetooth Terminal/Graphics (Android)
		//The string output to the serial terminal must be in this form: 
		//"Eval1,val2,val3\n"
		//Where val1, val2, ... , are the value variables and are separated by commas.
		
		_delay_ms(10);
		uart_putstring("E");
		uart_putdouble(gyroZ);
		uart_putstring(",");
		uart_putdouble(pitch);
		uart_putstring(",");
		uart_putdouble(roll);
		uart_putstring("\n");
		uart_putstring("                                                                  X-axis turns: ");
		uart_putdouble(pitch_count);
		uart_putstring("        Y-axis turns: ");
		uart_putdouble(roll_count);
		uart_putstring("\n");
		
	}

}//end of main


void timer_setup(){		//referenced from [2]
	TCCR1A = 0x00;
	TIMSK1 |= _BV(TOIE1);
	TCCR1B |= _BV(CS11);
	TCCR1B &= ~( _BV(CS12)  | _BV(CS10)); // prescaler=8

}

void get_time(double * dt){  //referenced from [2]
	cli();
	uint8_t l = TCNT1L;
	uint8_t h = TCNT1H;
	uint16_t step = h<<8 | l;
	*dt = (double)step*5e-7 + count*0.032768;
	count = 0;
	sei();
}

// timer 1 overflow interrupt handler
SIGNAL(TIMER1_OVF_vect){	//referenced from [2]
	count += 1;
	//TCNT1H = 0x00;
	//TCNT1L = 0x00;

}
