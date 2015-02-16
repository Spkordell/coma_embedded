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
	//Initialize timer count to 0
	TCNT0 = 0;
	//configure timer 0 for phase correct PWM mode, prescaler = clk/1024
	TCCR0A = (1 << COM0A1) | (1 << WGM00); //enable non-inverting PWM output on pin OC0A (we're not using 0C0B)
	TCCR0B = (1 << CS02) | (1 << CS00); //prescaler = clk/1024	
	OCR0A = 9;
	//stop the timer for the gripper servo because it's continuous rotation
	stop_timer0();
	
	//initialize timer 2 in phase-correct PWM mode for the wrist rotation/flex
	//initialize timer count to 0	
	TCNT1 = 0;
	//configure timer 1 for phase correct PWM mode, prescale = 8	
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11); //use ICR1 as top and non-inverting PWM output		
	TCCR1B = (1 << WGM13) | (1 << CS11); //phase-correct, prescale = 8
	TCCR1C = 0;
	//set top comparison value // period is 20 ms with a 18432000 hz clock and prescale = clk/8
	ICR1 = 23040; // ((0.02 * F_CPU)/8)/2 //division by 8 because of prescale, division by 2 because of phase correct mode (don't divide by 2 for fast mode)		
	//set servos to 0 position by setting duty cycle to 5% (1 ms pulse) (max servo range occurs at 12.5% duty (2.5 ms pulse)
	OCR1A = ICR1*0.05;
	OCR1B = ICR1*0.05;
}

void set_servo(int servo, double angle){
	static char T0TOP = 180;
	switch (servo) {
		case GRIPPER_SERVO:
			OCR0A = (uint8_t)(T0TOP * (0.075*(angle/180)+0.05));
			break;
		case WRIST_FLEX_SERVO:
			OCR1A = (uint16_t)(ICR1 * (0.075*(angle/180)+0.05));
			break;
		case WRIST_ROTATE_SERVO:
			OCR1B = (uint16_t)(ICR1 * (0.075*(angle/180)+0.05));
			break;			
	}
}

inline void stop_timer0() {
	TCCR0B &= ~((1 << CS02) | (1 << CS00)); //prescaler = clk/1024
}

inline void restart_timer0() {
	TCNT0 = 0; //Initialize timer count to 0
	TCCR0B |= ((1 << CS02) | (1 << CS00)); //prescaler = clk/1024
}


void servo_test(int servo) {	
	if (servo == GRIPPER_SERVO) {
		restart_timer0();
	}	
	while(1) {					
		for (unsigned int i = 180; i > 0; i--) {
			set_servo(servo, i);
			_delay_ms(50);
		}
		for (unsigned int i = 0; i < 180; i++) {
			set_servo(servo, i);
			_delay_ms(50);
		}	
			
	}
}

void gripper_open() {
	restart_timer0();
	set_servo(GRIPPER_SERVO,180);
	
	//todo: check limit switch and stop when hit instead of delay
	_delay_ms(5000);

	stop_timer0();
}

void gripper_close() {
	restart_timer0();
	set_servo(GRIPPER_SERVO,0);
	
	//todo: check limit switch and stop when hit instead of delay
	_delay_ms(5000);
	
	stop_timer0();
}