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
	
	//todo: move timer configuration stuff into timer.c
		
	
	//initialize timer 0 in phase-correct PWM mode for the gripper
	//Initialize timer count to 0
	TCNT0 = 0;
	//configure timer 0 for phase correct PWM mode, prescaler = clk/8 //note, if changing prescaler, look at open/close gripper commands and other places the timer might be stopped as that will change too
	TCCR0A = (1 << COM0A1) | (1 << WGM00); //enable non-inverting PWM output on pin OC0A (we're not using 0C0B)
	TCCR0B = (1 << WGM02) | (1 << CS01); //prescaler = clk/8
	//set both servos to 0 position by setting duty cycle to 5% (1 ms pulse) (max servo range occurs at 10% duty (2 ms pulse)
	//float T0TOP = 115.2; //period is 20 ms with a 18432000 hz clock and clk/8 prescaler
	OCR0A = 6; //T0TOP*0.05;	
	//stop the timer for the gripper servo because it's continuous rotation
	TCCR0B &= ~(1 << CS01);
	
	//initialize timer 2 in phase-correct PWM mode for the wrist rotation/flex
	//initialize timer count to 0	
	TCNT1 = 0;
	//configure timer 1 for phase correct PWM mode, no prescaler
	//TCCR1A = (1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0) | (1 << WGM11) | (1 << WGM10); //use OCR1A as top and inverting PWM output
	TCCR1A = (1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0) | (1 << WGM11); //use ICR1 as top and inverting PWM output
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11); //use ICR1 as top and non-inverting PWM output
	TCCR1B = (1 << WGM13) | (1 << CS10);
	TCCR1C = 0;
	//set top comparison value // period is 20.013 ms with a 18432000 hz clock and no prescaler
	ICR1 = 921;
	//set servos to 0 position by setting duty cycle to 5% (1 ms pulse) (max servo range occurs at 10% duty (2 ms pulse)
//	OCR1A = 875; //(921 - (921*0.05)) 	
//	OCR1B = 875; 
	OCR0A = 46; //(ICR1*0.05)					

}

void set_servo(int servo, double angle){
	static float T0TOP = 115.2;
	switch (servo) {
		case GRIPPER_SERVO:
			OCR0A = (char)(T0TOP * (0.05*(angle/180)+0.05));
			break;
		case WRIST_FLEX_SERVO:
			OCR1A = (char)(ICR1 * (0.05*(angle/180)+0.05));
			break;
		case WRIST_ROTATE_SERVO:
			OCR1B = (char)(ICR1 * (0.05*(angle/180)+0.05));
			break;			
	}
}

void servo_test(int servo) {
	//todo: if gripper servo, need to start timer
	
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

void gripper_open() {
	cli();
	//start the PWM timer 		
	TCNT0 = 0; //Initialize timer count to 0
	TCCR0B |= (1 << CS01); //prescaler = clk/8
	sei();
	set_servo(GRIPPER_SERVO,180);
	
	//todo: check limit switch and stop when hit
	
	cli();
	//stop the timer
	TCCR0B &= ~(1 << CS01);	
	sei();
}

void gripper_close() {
	cli();
	//start the PWM timer
	TCNT0 = 0; //Initialize timer count to 0
	TCCR0B |= (1 << CS01); //prescaler = clk/8
	sei();
	set_servo(GRIPPER_SERVO,0);
	//todo: check limit switch and stop when hit
	cli();
	//stop the timer
	TCCR0B &= ~(1 << CS01);	
	sei();
}