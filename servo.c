/*
 * servo.c
 *
 * Created: 2/14/2015 8:01:07 AM
 *  Author: Steven Kordell
 */ 

#include "main.h"

void init_servos() {
	//set OC0A for gripper servo to output
	DDRB |= (GRIPPER_SERVO_PIN);
	//set OC2A/B for rotation/flex servos to output
	DDRD |= (WRIST_ROTATE_SERVO_PIN | WRIST_FLEX_SERVO_PIN);
	
	//initialize timer 0 in phase-correct PWM mode for the gripper
	//InitTimer(0, CTC, 18432); //interrupt every 1 ms (clock speed is 18432000)
	
	//initialize timer 2 in phase-correct PWM mode for the wrist rotation/flex
	//InitTimer(1, CTC, 921); //interrupt every 20.013 ms (clock speed is 18432000)
	
	//todo: move into timer.c
	
	//initiailize timer to 0	
	TCNT1 = 0;
	//configure timer 1 for phase correct PWM mode, prescaler = 8
	//TCCR1A = (1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0) | (1 << WGM11) | (1 << WGM10); //use OCR1A as top and inverting PWM output
	TCCR1A = (1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0) | (1 << WGM11); //use ICR1 as top and inverting PWM output
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11); //use ICR1 as top and non-inverting PWM output
	TCCR1B = (1 << WGM13) | (1 << CS10);
	TCCR1C = 0;
	//set top comparison value // period is 0.013 ms with a 18432000 hz clock and no prescaler
	ICR1 = 921;

	//set both servos to 0 position by setting duty cycle to 5% (1 ms pulse) (max servo range occurs at 10% duty (2 ms pulse)
//	OCR1A = 875; //(921 - (921*0.05)) 	
//	OCR1B = 875; 
	OCR1A = 46; //(ICR1*0.05)
	OCR1B = 46; //(ICR1*0.05)						

}

void set_servo(int servo, double angle){
	switch (servo) {
		case 0:
			OCR1A = int(ICR1 * (0.05*(angle/180)+0.05));
			break;			
		case 1:
			OCR1B = int(ICR1 * (0.05*(angle/180)+0.05));
			break;
	}
}

void servo_test(int servo) {
	while(1) {
		for (unsigned int i = 0; i < 180; i++) {
			set_servo(servo, i);
			_delay_ms(10);
		}
		for (unsigned int i = 180; i > 0; i--) {
			set_servo(servo, i);
			_delay_ms(10);
		}
	}
}