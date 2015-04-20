/*
 * stepper.c
 *
 * Created: 12/27/2014 10:24:21 PM
 *  Author: Steve Kordell
 */ 

#include "main.h"

unsigned long currentStepperCounts[STEPPER_COUNT];
volatile unsigned long pathInstructionBuffer[PATH_INSTRUCTION_BUFFER_SIZE][STEPPER_COUNT+1]; //instructionBuffer[n][0] = timestamp, instructionBuffer[n][1..12] = targer stepper count
//unsigned long teleopInstructionBuffer[TELEOP_INSTRUCTION_BUFFER_SIZE][STEPPER_COUNT];
//unsigned long* teleopInstructionBufferPtr[TELEOP_INSTRUCTION_BUFFER_SIZE];
//MULTI_DIM_FIFO teleopInstructionFifo;


void init_endstops(void) {	
	//Set end stops as inputs		
	DDRC &= ~(ENDSTOP0 | ENDSTOP1 | ENDSTOP2 | ENDSTOP3 | ENDSTOP4 | ENDSTOP5 | ENDSTOP6 | ENDSTOP7);
	DDRA &= ~(ENDSTOP8 | ENDSTOP9 | ENDSTOP10);
	DDRB &= ~(ENDSTOP11);
		
	//enable pull up resistor
	PORTC |= (ENDSTOP0 | ENDSTOP1 | ENDSTOP2 | ENDSTOP3 | ENDSTOP4 | ENDSTOP5 | ENDSTOP6 | ENDSTOP7);
	PORTA |= (ENDSTOP8 | ENDSTOP9 | ENDSTOP10);
	PORTB |= (ENDSTOP11);	
}

void init_steppers(void) {
	DDRB |= (DATA | LATCH | CLOCK);		//Set shift control pins as outputs
	PORTB &= ~(DATA | LATCH | CLOCK);	//Set shift control pins low
	
	DDRA |= (STEPPER_RESET | STEPPER_SLEEP | STEPPER_ENABLE | SHIFT_ENABLE | SHIFT_CLEAR); 	//set pins as output
	PORTA |= (STEPPER_RESET | STEPPER_SLEEP | SHIFT_CLEAR); 								//set pins high
	PORTA &= ~(STEPPER_ENABLE | SHIFT_ENABLE);												//set pins low
	
	DDRD &= ~(STEPPER_FAULT); //set fault pin as input
	
	init_SPI();
	
	//initialize stepper counts and path instruction buffer to 0
	for (unsigned int i = 0; i < STEPPER_COUNT; i++) {
		currentStepperCounts[i] = 0;
	}
	for (unsigned int i = 0; i < PATH_INSTRUCTION_BUFFER_SIZE; i++) {
		for (unsigned int j = 0; j < STEPPER_COUNT+1; j++) {
			pathInstructionBuffer[i][j] = 0;
		}
	}
	/*
	teleopInstructionFifo.head = 0;
	teleopInstructionFifo.tail = 0;
	teleopInstructionFifo.size = TELEOP_INSTRUCTION_BUFFER_SIZE;
	teleopInstructionFifo.fifo = teleopInstructionBufferPtr;	
	*/
}


void test_endstops(void) {
	while(1) {
		if(!(PINC & ENDSTOP0)) {
			uart_putchar('3');
			uart_putchar('\n');
		}
		if(!(PINC & ENDSTOP1)) {
			uart_putchar('4');
			uart_putchar('\n');
		}
		if(!(PINC & ENDSTOP2)) {
			uart_putchar('7');
			uart_putchar('\n');
		}
		if(!(PINC & ENDSTOP3)) {
			uart_putchar('8');
			uart_putchar('\n');
		}
		if(!(PINC & ENDSTOP4)) {
			uart_putchar('1');
			uart_putchar('1');
			uart_putchar('\n');
		}
		if(!(PINC & ENDSTOP5)) {
			uart_putchar('1');
			uart_putchar('2');
			uart_putchar('\n');
		}
		if(!(PINC & ENDSTOP6)) {
			uart_putchar('1');
			uart_putchar('\n');
		}
		if(!(PINC & ENDSTOP7)) {
			uart_putchar('2');
			uart_putchar('\n');
		}
		if(!(PINA & ENDSTOP8)) {
			uart_putchar('5');
			uart_putchar('\n');
		}
		if(!(PINA & ENDSTOP9)) {
			uart_putchar('6');
			uart_putchar('\n');
		}
		if(!(PINA & ENDSTOP10)) {
			uart_putchar('9');			
			uart_putchar('\n');
		}
		if(!(PINB & ENDSTOP11)) {
			uart_putchar('1');
			uart_putchar('0');
			uart_putchar('\n');
		}
		_delay_ms(100);
	}
}

void home_steppers(void) {
	unsigned char spi_buffer;
		
		
	//home the bottom link
	
	while ((PINC & ENDSTOP6) || (PINC & ENDSTOP7) || (PINA & ENDSTOP8) || (PINA & ENDSTOP9) || (PINA & ENDSTOP10) || (PINB & ENDSTOP11)) { //loop until all endstops are pressed (endstops are active low)
		//clear shift register
		PORTA &= ~SHIFT_CLEAR;		//write low
		PORTA |= SHIFT_CLEAR; 		//write high
		//Toggle latch to copy data to the storage register
		PORTB |= LATCH;
		PORTB &= ~LATCH;
					
		SPDR = ((unsigned char) //compute and transmit first instruction 
		( (1 << 7) | ((PINB & ENDSTOP11) ? 1 << 6 : 0)
		| (0 << 5) | ((PINA & ENDSTOP10) ? 1 << 4 : 0)
		| (1 << 3) | ((PINA & ENDSTOP9) ? 1 << 2 : 0)
		| (0 << 1) | ((PINA & ENDSTOP8) ? 1 << 0 : 0)));
		spi_buffer = ((unsigned char) // while waiting for first transfer to finish, compute second instruction
		(  1 << 7) | ((PINC & ENDSTOP7) ? 1 << 6 : 0)
		| (0 << 5) | ((PINC & ENDSTOP6) ? 1 << 4 : 0));
		while(!(SPSR & (1<<SPIF)));  //Wait for first SPI transfer to finish
		SPDR = spi_buffer; //send second instruction
		spi_buffer = ((unsigned char)0); // compute third instruction
		while(!(SPSR & (1<<SPIF))); //Wait for second SPI transfer to finish
		SPDR = spi_buffer; //send third instruction
		while(!(SPSR & (1<<SPIF))); //Wait for third SPI transfer to finish
					
		//Toggle latch to copy data to the storage register
		PORTB |= LATCH;
		PORTB &= ~LATCH;
										
		_delay_ms(HOMING_STEPPER_DELAY);			
	}
	
	//home the top link
	while ((PINC & ENDSTOP0) || (PINC & ENDSTOP1) || (PINC & ENDSTOP2) || (PINC & ENDSTOP3) || (PINC & ENDSTOP4) || (PINC & ENDSTOP5)) { //loop until all endstops are pressed (endstops are active low)
				//clear shift register
				PORTA &= ~SHIFT_CLEAR;		//write low
				PORTA |= SHIFT_CLEAR; 		//write high
				//Toggle latch to copy data to the storage register
				PORTB |= LATCH;
				PORTB &= ~LATCH;
				
				SPDR = ((unsigned char) 0); /*transmit first instruction */
				spi_buffer = ((unsigned char) /*while waiting for first transfer to finish, compute second instruction*/
				( (1 << 3) | ((PINC & ENDSTOP5) ? 1 << 2 : 0)
				| (0 << 1) | ((PINC & ENDSTOP4) ? 1 << 0 : 0)));
				while(!(SPSR & (1<<SPIF)));  //Wait for first SPI transfer to finish
				SPDR = spi_buffer; /*send second instruction*/
				spi_buffer = ((unsigned char) /*while waiting for second transfer to finish, compute third instruction*/
				( (1 << 7) | ((PINC & ENDSTOP3) ? 1 << 6: 0)
				| (0 << 5) | ((PINC & ENDSTOP2) ? 1 << 4 : 0)
				| (1 << 3) | ((PINC & ENDSTOP1) ? 1 << 2 : 0)
				| (0 << 1) | ((PINC & ENDSTOP0) ? 1 << 0 : 0)));
				while(!(SPSR & (1<<SPIF))); //Wait for second SPI transfer to finish
				SPDR = spi_buffer; //send third instruction
				while(!(SPSR & (1<<SPIF))); //Wait for third SPI transfer to finish
				
				//Toggle latch to copy data to the storage register
				PORTB |= LATCH;
				PORTB &= ~LATCH;
				
				_delay_ms(HOMING_STEPPER_DELAY);
	}
	
	//set counts to 0
	for (unsigned int i = 0; i < STEPPER_COUNT; i++) {
		currentStepperCounts[i] = 0;
	}
}

void init_SPI(void){
	//Setup SPI
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPI2X); //Start SPI as Master, speed = Fosc/2
}

void spi_send(unsigned char byte){
	SPDR = byte; //Shift in some data
	while(!(SPSR & (1<<SPIF))); //Wait for SPI process to finish
}

char is_fault() {
	return (~PIND & STEPPER_FAULT);	
}

/*
void buffer_teleop_instruction(unsigned long* instruction) {
	static int index = 0;
	for(unsigned int i = 0; i < STEPPER_COUNT; i++) {
		teleopInstructionBuffer[index][i] = instruction[i];
	}
	multi_dim_fifo_put(&teleopInstructionFifo, teleopInstructionBuffer[index]);
	index++;
	if (index >= TELEOP_INSTRUCTION_BUFFER_SIZE) {
		index = 0;
	}
}*/

void send_step_instruction(int instruction) {
	unsigned char spi_buffer;
	
	unsigned long stepperTargets[STEPPER_COUNT];
	for (unsigned int i = 0; i < STEPPER_COUNT; i++) {
		if (pathInstructionBuffer[instruction][i+1] != 0) {
			stepperTargets[i] = pathInstructionBuffer[instruction][i+1];
		} else {
			stepperTargets[i] = currentStepperCounts[i];
		}
	}
	
	//remove instruction from buffer
	for (unsigned int i = 0; i < STEPPER_COUNT+1; i++) {
		pathInstructionBuffer[instruction][i] = 0;
	}
	
	for (unsigned int stepper = 0; stepper < STEPPER_COUNT; stepper++) {
		while (currentStepperCounts[stepper] != stepperTargets[stepper]) {
			//clear shift register
			PORTA &= ~SHIFT_CLEAR;		//write low
			PORTA |= SHIFT_CLEAR; 		//write high
			//Toggle latch to copy data to the storage register
			PORTB |= LATCH;
			PORTB &= ~LATCH;
	
			SPDR = ((unsigned char) /*compute and transmit first instruction */
			( ( currentStepperCounts[11] > stepperTargets[11]) << 7) | ((currentStepperCounts[11] != stepperTargets[11]) << 6)
			| ((currentStepperCounts[10] < stepperTargets[10]) << 5) | ((currentStepperCounts[10] != stepperTargets[10]) << 4)
			| ((currentStepperCounts[9] > stepperTargets[9]) << 3) | ((currentStepperCounts[9] != stepperTargets[9]) << 2)
			| ((currentStepperCounts[8] < stepperTargets[8]) << 1) | ((currentStepperCounts[8] != stepperTargets[8]) << 0));		
			spi_buffer = ((unsigned char) /*while waiting for first transfer to finish, compute second instruction*/
			( ( currentStepperCounts[7] > stepperTargets[7]) << 7) | ((currentStepperCounts[7] != stepperTargets[7]) << 6)
			| ((currentStepperCounts[6] < stepperTargets[6]) << 5) | ((currentStepperCounts[6] != stepperTargets[6]) << 4)
			| ((currentStepperCounts[5] > stepperTargets[5]) << 3) | ((currentStepperCounts[5] != stepperTargets[5]) << 2)
			| ((currentStepperCounts[4] < stepperTargets[4]) << 1) | ((currentStepperCounts[4] != stepperTargets[4]) << 0));
			while(!(SPSR & (1<<SPIF)));  //Wait for first SPI transfer to finish
			SPDR = spi_buffer; /*send second instruction*/
			spi_buffer = ((unsigned char) /*while waiting for second transfer to finish, compute third instruction*/		
			   ((currentStepperCounts[3] > stepperTargets[3]) << 7) | ((currentStepperCounts[3] != stepperTargets[3]) << 6)
			 | ((currentStepperCounts[2] < stepperTargets[2]) << 5) | ((currentStepperCounts[2] != stepperTargets[2]) << 4)
			 | ((currentStepperCounts[1] > stepperTargets[1]) << 3) | ((currentStepperCounts[1] != stepperTargets[1]) << 2)			
			 | ((currentStepperCounts[0] < stepperTargets[0]) << 1) | ((currentStepperCounts[0] != stepperTargets[0]) << 0));
			while(!(SPSR & (1<<SPIF))); //Wait for second SPI transfer to finish
			SPDR = spi_buffer; //send third instruction
			while(!(SPSR & (1<<SPIF))); //Wait for third SPI transfer to finish
			
			//Toggle latch to copy data to the storage register
			PORTB |= LATCH;
			PORTB &= ~LATCH;
		
			for (unsigned int i = stepper; i < STEPPER_COUNT; i++) {
				currentStepperCounts[i] += (currentStepperCounts[i] != stepperTargets[i]) * (currentStepperCounts[i] < stepperTargets[i] ? 1 : -1);
			}
		
			_delay_ms(STEPPER_DELAY); //todo: implement delay using timer
			
		}
	}
}

//todo: lots of common code between sent_teleop_step and send_step_instruction. Can simplify

void send_teleop_step(unsigned long* stepperTargets) {
	unsigned char spi_buffer;	
	for (unsigned int stepper = 0; stepper < STEPPER_COUNT; stepper++) {
		while (currentStepperCounts[stepper] != stepperTargets[stepper]) {
			//clear shift register
			PORTA &= ~SHIFT_CLEAR;		//write low
			PORTA |= SHIFT_CLEAR; 		//write high
			//Toggle latch to copy data to the storage register
			PORTB |= LATCH;
			PORTB &= ~LATCH;
			
			SPDR = ((unsigned char) /*compute and transmit first instruction */
			( ( currentStepperCounts[11] > stepperTargets[11]) << 7) | ((currentStepperCounts[11] != stepperTargets[11]) << 6)
			| ((currentStepperCounts[10] < stepperTargets[10]) << 5) | ((currentStepperCounts[10] != stepperTargets[10]) << 4)
			| ((currentStepperCounts[9] > stepperTargets[9]) << 3) | ((currentStepperCounts[9] != stepperTargets[9]) << 2)
			| ((currentStepperCounts[8] < stepperTargets[8]) << 1) | ((currentStepperCounts[8] != stepperTargets[8]) << 0));
			spi_buffer = ((unsigned char) /*while waiting for first transfer to finish, compute second instruction*/
			( ( currentStepperCounts[7] > stepperTargets[7]) << 7) | ((currentStepperCounts[7] != stepperTargets[7]) << 6)
			| ((currentStepperCounts[6] < stepperTargets[6]) << 5) | ((currentStepperCounts[6] != stepperTargets[6]) << 4)
			| ((currentStepperCounts[5] > stepperTargets[5]) << 3) | ((currentStepperCounts[5] != stepperTargets[5]) << 2)
			| ((currentStepperCounts[4] < stepperTargets[4]) << 1) | ((currentStepperCounts[4] != stepperTargets[4]) << 0));
			while(!(SPSR & (1<<SPIF)));  //Wait for first SPI transfer to finish
			SPDR = spi_buffer; /*send second instruction*/
			spi_buffer = ((unsigned char) /*while waiting for second transfer to finish, compute third instruction*/
			( ( currentStepperCounts[3] > stepperTargets[3]) << 7) | ((currentStepperCounts[3] != stepperTargets[3]) << 6)
			| ((currentStepperCounts[2] < stepperTargets[2]) << 5) | ((currentStepperCounts[2] != stepperTargets[2]) << 4)
			| ((currentStepperCounts[1] > stepperTargets[1]) << 3) | ((currentStepperCounts[1] != stepperTargets[1]) << 2)
			| ((currentStepperCounts[0] < stepperTargets[0]) << 1) | ((currentStepperCounts[0] != stepperTargets[0]) << 0));
			while(!(SPSR & (1<<SPIF))); //Wait for second SPI transfer to finish
			SPDR = spi_buffer; //send third instruction
			while(!(SPSR & (1<<SPIF))); //Wait for third SPI transfer to finish
			
			//Toggle latch to copy data to the storage register
			PORTB |= LATCH;
			PORTB &= ~LATCH;
			
			for (unsigned int i = stepper; i < STEPPER_COUNT; i++) {
				currentStepperCounts[i] += (currentStepperCounts[i] != stepperTargets[i]) * (currentStepperCounts[i] < stepperTargets[i] ? 1 : -1);
			}
			
			_delay_ms(STEPPER_DELAY); //todo: implement delay using timer
			
		}
	}
}


void add_stepper_instruction(unsigned long timeStamp, unsigned char stepper, unsigned long target) {
	//find index of timestamp or create one if it doesn't exist in buffer
	
	//todo: handle buffer full condition (can probably just repeat everything and wait until an empty slot is found)
	
	char timeFound = 0;
	for (unsigned int i = 0; i < PATH_INSTRUCTION_BUFFER_SIZE; i++) {
		if (pathInstructionBuffer[i][0] == timeStamp) {
			pathInstructionBuffer[i][stepper+1] = target;
			timeFound = 1;
			break;
		}
	}
	if (!timeFound) {
		for (unsigned int i = 0; i < PATH_INSTRUCTION_BUFFER_SIZE; i++) {
			if (pathInstructionBuffer[i][0] == 0) {
				pathInstructionBuffer[i][0] = timeStamp;
				pathInstructionBuffer[i][stepper+1] = target;
				break;
			}
		}
	}
}

long getNextInstructionTimestamp(unsigned int* instructionIndex) {
	long nextInstructionTimeStamp = LONG_MAX;
	for (unsigned int i = 0; i < PATH_INSTRUCTION_BUFFER_SIZE; i++) {
		if (pathInstructionBuffer[i][0] != 0 && pathInstructionBuffer[i][0] < nextInstructionTimeStamp) {
			nextInstructionTimeStamp = pathInstructionBuffer[i][0];
			*instructionIndex = i;
		}
	}
	if (nextInstructionTimeStamp == LONG_MAX) {
		return 0; //signifies that the instruction buffer is empty
	}
	return nextInstructionTimeStamp;
}