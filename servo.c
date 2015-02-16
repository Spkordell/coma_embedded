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
	//configure timer 1 for phase correct PWM mode, prescale = 8	
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11); //use ICR1 as top and non-inverting PWM output		
	TCCR1B = (1 << WGM13) | (1 << CS11); //phase-correct, prescale = 8
	TCCR1C = 0;
	//set top comparison value // period is 20 ms with a 18432000 hz clock and prescale = clk/8
	ICR1 = 23040; // ((0.02 * F)/8)/2 //division by 8 because of prescale, division by 2 because of phase correct mode (don't divide by 2 for fast mode)		
	//set servos to 0 position by setting duty cycle to 5% (1 ms pulse) (max servo range occurs at 12.5% duty (2.5 ms pulse)
	OCR1A = ICR1*0.05;
	OCR1B = ICR1*0.05;

}

void set_servo(int servo, double angle){
	static float T0TOP = 115.2;
	cli();
	switch (servo) {
		case GRIPPER_SERVO:
			OCR0A = (char)(T0TOP * (0.05*(angle/180)+0.05));
			/*
			char buff[40];
			itoa(OCR0B,buff,10);
			uart_put_string(buff);
			uart_putchar('\n');
			*/
			break;
		case WRIST_FLEX_SERVO:
			OCR1A = (uint16_t)(ICR1 * (0.075*(angle/180)+0.05));
			break;
		case WRIST_ROTATE_SERVO:
			OCR1B = (uint16_t)(ICR1 * (0.075*(angle/180)+0.05));
			break;			
	}
	sei();
}

void servo_test(int servo) {
	//todo: if gripper servo, need to start timer
	
	while(1) {
		uart_putchar('A');
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
	cli();
	//start the PWM timer 		
	TCNT0 = 0; //Initialize timer count to 0
	TCCR0B |= (1 << CS01); //prescaler = clk/8
	sei();
	set_servo(GRIPPER_SERVO,180);
	
	//todo: check limit switch and stop when hit instead of delay
	_delay_ms(5000);

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
	
	//todo: check limit switch and stop when hit instead of delay
	_delay_ms(5000);
	
	cli();
	//stop the timer
	TCCR0B &= ~(1 << CS01);	
	sei();
}