/*
 * main.c
 *
 * Created: 12/27/2014 10:22:59 PM
 *  Author: Steve Kordell
 */ 

#include "main.h"

mode_t mode;
volatile long millis = 0; //number of milliseconds which have passed

int main(void) {	
	cli();
	init_endstops();
	init_steppers();
	init_servos();
	init_uart();
	sei();	
		
	//test_endstops();		
	//home_steppers();
			
	//get mode selection from host computer
	uart_putchar('R');				//send ready signal to host computer
		
	//servo_test(WRIST_ROTATE_SERVO);
	//servo_test(WRIST_FLEX_SERVO);
	//servo_test(GRIPPER_SERVO);

	while(!uart_char_waiting());    //wait for mode selection
	if (uart_getchar() == 'T') {
		mode = TELEOP;
	} else {
		mode = PATH;
		cli();
		InitTimer(3, CTC, 18432); //interrupt every 1 ms (clock speed is 18432000) // alternatively, could use timer 0 with prescaler = 256 and max count = 74
		sei();
	}
	
    while(1) {
		parseInput();
    }
}

void parseInput(void) {	
	//Format: timestamp:stepper number(0..11):target count\r	
	char buffer[PARSE_BUFFER_SIZE];
	char* p = buffer;
	long timeStamp = 0;
	char stepper = 0;
	long target = 0;
	unsigned int at = 0;
	char instuctionToAdd = 1;
	unsigned long teleop_instruction[STEPPER_COUNT];
	unsigned long servo_instruction[3]; //0: rotate, 1: flex, 2: gripper (1 = open, 0 = close)
	
	//extract the timestamp, stepper, and target
	while (1) {
		/*
		while (is_fault()) {
			//could also do this using external GPIO interrupt INT0 instead of polling
			uart_putchar('F');
			_delay_us(2);
		}*/
		if(uart_char_waiting()) {
			*p = uart_getchar();
			
			#ifdef UART_ECHO
				uart_putchar(*p);
			#endif
			
			if (*p == 'R') {		
				cli();
				millis = 0; //reset millis count
				TCNT1 = 0;  //set timer value to 0
				sei();
				instuctionToAdd = 0;
				break;
			}
			if (*p == 'H') {
				uart_putchar('R'); //signal that we are ready for the next command
				home_steppers();
				instuctionToAdd = 0;
				break;
			}
			if (*p == 'T') {
				instuctionToAdd = 0;
				mode = TELEOP;
				break;
			}
			if (*p == 'P') {
				instuctionToAdd = 0;
				mode = PATH;
				break;
			}
						
			if (mode == TELEOP) {
				instuctionToAdd = 0;
				//uart_putchar(*p);
				if (*p == ':' || *p == ';' || *p == '\r') {
					*p = '\0';
					target = atol(buffer);
					if (at < 12) {
						teleop_instruction[at++] = target;
					} else {
						servo_instruction[at-12] = target;
						at++;
					}
					p = buffer;
					if (at >= 15) {
						at = 0;
						uart_putchar('R'); //signal that we are ready for the next command	
						//send_servo_instruction(servo_instruction); //TODO: add this back on once the limit switches for the end effector are attached. Without the limits, the program will freeze opening/closing the gripper
						send_teleop_step(teleop_instruction);
					}
				} else {
					p++;
				}
			} else {
				if (*p == ':' || *p == '\r') {
					*p = '\0';
					switch (at) {
						case 0:
							timeStamp = atol(buffer);
							p = buffer;
							break;
						case 1:
							stepper = atoi(buffer);
							p = buffer;
							break;
						case 2:
							target = atol(buffer);
							break;			
					}
					if (at == 2) {
						break;
					} else {
						at++;
					}
				} else {
					p++;
				}
			}
		}
	}
	if (instuctionToAdd) {
		add_stepper_instruction(timeStamp, stepper, target);
		#ifdef UART_ECHO
			char buff1[30];
			char buff2[30];
			char buff3[30];
			ltoa(timeStamp,buff1,10);
			ltoa(stepper,buff2,10);
			ltoa(target,buff3,10);
			uart_put_string(buff1);
			uart_putchar('\r');
			uart_put_string(buff2);
			uart_putchar('\r');
			uart_put_string(buff3);
			uart_putchar('\r');
		#endif
	}
}

#if 0
ISR(TIMER3_COMPA_vect) {
	//called every millisecond	
	//cli(); //temporary, for testing, todo
	
	

	unsigned int instructionIndex;
	long nextInstructionTimestamp = getNextInstructionTimestamp(&instructionIndex);
	

	if (++millis >= nextInstructionTimestamp && nextInstructionTimestamp != 0) {		
		send_step_instruction(instructionIndex);  //todo: this can take a long time, probably want to make a pending instruction buffer.
		uart_put_string("Sent instruction\r");
		/*
		char buff[30];
		ltoa(nextInstructionTimestamp,buff,10);
		uart_put_string(buff);
		uart_putchar('\r');
		*/
	}
	//char asciiMillis[30];
	//ltoa(millis, asciiMillis, 10);
	//uart_put_string(asciiMillis);
	//uart_putchar('\r');
	
	//_delay_us(500);
	
	//sei(); //temporary, for testing, todo
}
#endif
