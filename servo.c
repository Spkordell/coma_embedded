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
	//set OC2A for gripper sevo to output
	//DDRD |= (GRIPPER_SERVO_PIN);
	//set OC1A/B for rotation/flex servos to output
	DDRD |= (WRIST_ROTATE_SERVO_PIN | WRIST_FLEX_SERVO_PIN);
			
	//Set end stops as inputs
	DDRB &= ~(GRIPPER_ENDSTOP_OPEN_PIN | GRIPPER_ENDSTOP_CLOSE_PIN);
	//enable endstop pull up resistors
	PORTB |= (GRIPPER_ENDSTOP_OPEN_PIN | GRIPPER_ENDSTOP_CLOSE_PIN);
	
	
	//initialize timer 0 in phase-correct PWM mode for the gripper
	//Initialize timer count to 0
	TCNT0 = 0;
	//configure timer 0 for phase correct PWM mode, prescaler = clk/1024
	TCCR0A = (1 << COM0A1) | (1 << WGM00); //enable non-inverting PWM output on pin OC0A (we're not using 0C0B) //phase-correct
	//TCCR0A = (1 << COM0A1) | (1 << WGM00) | (1 << WGM01); //enable non-inverting PWM output on pin OC0A (we're not using 0C0B) //fast
	TCCR0B = (1 << CS02) | (1 << CS00); //prescaler = clk/1024	
	OCR0A = 13;
	//stop the timer for the gripper servo because it's continuous rotation
	stop_timer0();
	
		
	//initialize timer 1 in phase-correct PWM mode for the wrist rotation/flex
	//initialize timer count to 0	
	TCNT1 = 0;
	//configure timer 1 for phase correct PWM mode, prescale = 8	
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11); //use ICR1 as top and non-inverting PWM output		
	TCCR1B = (1 << WGM13) | (1 << CS11); //phase-correct, prescale = 8
	TCCR1C = 0;
	//set top comparison value // period is 20 ms with a 18432000 hz clock and prescale = clk/8
	ICR1 = 23040; // ((0.02 * F_CPU)/8)/2 //division by 8 because of prescale, division by 2 because of phase correct mode (don't divide by 2 for fast mode)		
	//ICR1 = 3836;//((0.00333 * F_CPU)/8)/2  //300 hz
	//ICR1 = 3456;//((0.003 * F_CPU)/8)/2  //~333 hz
	//ICR1 = 12800;//((0.00111 * F_CPU)/8)/2 //90hz
	//ICR1 = 5760;//((0.005 * F_CPU)/8)/2 //200hz
	//ICR1 = 3491;//((0.00303 * F_CPU)/8)/2  //333 hz
	//set servos to 0 position by setting duty cycle to 5% (1 ms pulse) (max servo range occurs at 12.5% duty (2.5 ms pulse)
	OCR1A = ICR1*0.05;
	OCR1B = ICR1*0.05;
}

void set_servo(int servo, double angle){
	static char T0TOP = 180;
	switch (servo) {
		/*
		case GRIPPER_SERVO:
			//OCR0A = (uint8_t)(T0TOP * (0.075*(angle/180)+0.05));
			OCR0A = (uint8_t)(T0TOP * (0.075*(angle/180)+0.05)); //0.02 to 0.14
			break;
		case WRIST_FLEX_SERVO:
			//OCR1A = (uint16_t)(ICR1 * (0.075*(angle/180)+0.05));
			OCR1A = (uint16_t)(ICR1 * (0.09*(angle/180)+0.03)); //want 0.3 to 0.11
			break;
		case WRIST_ROTATE_SERVO:
			OCR1B = (uint16_t)(ICR1 * (0.075*(angle/180)+0.05));
			break;			
			*/
		case GRIPPER_SERVO:
			OCR1B = (uint16_t)(ICR1 * (0.075*(angle/180)+0.03));
			//OCR1B = (uint16_t)(ICR1 * (0.03*(angle/180)+0.06));		
			//OCR1B = (uint16_t)(ICR1 * (0.65*(angle/180)+0.15)); //closest
			//OCR1B = (uint16_t)(ICR1 * (0.50*(angle/180)+0.25));		
			//OCR1B = (uint16_t)(ICR1 * (0.1666663*(angle/180)+0.166667));
			break;
		case WRIST_FLEX_SERVO:
			//OCR1A = (uint16_t)(ICR1 * (0.075*(angle/180)+0.05));
			OCR1A = (uint16_t)(ICR1 * (0.075*(angle/180)+0.03));
			//OCR1A = (uint16_t)(ICR1 * (0.03*(angle/180)+0.06));			
			//OCR1A = (uint16_t)(ICR1 * (0.33*(angle/180)+0.33)); 
			//OCR1A = (uint16_t)(ICR1 * (0.65*(angle/180)+0.15)); //closest we've come   //455 - 2400us
			//OCR1A = (uint16_t)(ICR1 * (0.462*(angle/180)+0.264));  //800-2200 us pulse
			//OCR1A = (uint16_t)(ICR1 * (0.25*(angle/180)+0.50));		
			//probably want .75 for 180 
			//OCR1A = (uint16_t)(ICR1 * (0.1666663*(angle/180)+0.166667));			
			break;
		case WRIST_ROTATE_SERVO:
			OCR0A = (uint8_t)(T0TOP * (0.075*(angle/180)+0.05));			
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

void send_servo_instruction(unsigned long* servoTargets) {
	set_servo(WRIST_ROTATE_SERVO,servoTargets[0]);
	set_servo(WRIST_FLEX_SERVO,servoTargets[1]);
	if (servoTargets[2]) {
		gripper_open();
	} else {
		gripper_close();
	}
}

void gripper_open() {
	restart_timer0();
	set_servo(GRIPPER_SERVO,90);
	/*
	while (PINB & GRIPPER_ENDSTOP_OPEN_PIN) { //loop until open endstop is pressed (endstops are active low)
	}
	stop_timer0();
	*/
}

void gripper_close() {
	restart_timer0();
	set_servo(GRIPPER_SERVO,180);
	/*
	while (PINB & GRIPPER_ENDSTOP_CLOSE_PIN) { //loop until close endstop is pressed (endstops are active low)
	}	
	stop_timer0();
	*/
}