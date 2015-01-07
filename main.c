/*
 * main.c
 *
 * Created: 12/27/2014 10:22:59 PM
 *  Author: Steve Kordell
 */ 

#include "main.h"

volatile long millis = 0; //number of milliseconds which have passed

int main(void) {	
	cli();
	init_steppers();
	init_uart();
	InitTimer(1, CTC, 18432); //interrupt every 1 ms (clock speed is 18432000) // alternatively, could use timer 0 with prescaler = 256 and max count = 74
	sei();	
    while(1) {	
		/*		
		if (uart_char_waiting()) {
			uart_putchar(uart_getchar());
			//send_step_instruction();
		}*/
		parseInput();
		//send_step_instruction();
    }
}

void parseInput(void) {
	
	//todo: add commands for homing and syncing timer
	
	//Format: timestamp:stepper number(0..11):target count\r	
	char buffer[30];
	char* p = buffer;
	long timeStamp = 0;
	char stepper = 0;
	long target = 0;
	char at = 0;
	char instuctionToAdd = 1;
	
	//extract the timestamp, stepper, and target
	while (1) {
		if(uart_char_waiting()) {
			*p = uart_getchar();
			uart_putchar(*p);
			
			if (*p == 'R') {		
				cli();
				millis = 0; //reset millis count
				TCNT1 = 0;  //set timer value to 0
				sei();
				instuctionToAdd = 0;
				break;
			}
			if (*p == 'H') {
				//todo: perform homing routine
				instuctionToAdd = 0;
				break;
			}
			
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
	if (instuctionToAdd) {
		add_stepper_instruction(timeStamp, stepper, target);
	}
}

ISR(TIMER1_COMPA_vect) {
	//called every millisecond
	long nextInstructionTimestamp = getNextInstructionTimestamp();
	if (++millis >= nextInstructionTimestamp && nextInstructionTimestamp != 0) {
		send_step_instruction(nextInstructionTimestamp);
	}
	char asciiMillis[30];
	ltoa(millis, asciiMillis, 10);
	uart_put_string(asciiMillis);
}
