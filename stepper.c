/*
 * stepper.c
 *
 * Created: 12/27/2014 10:24:21 PM
 *  Author: Steve Kordell
 */ 

#include "main.h"

unsigned long currentStepperCounts[STEPPER_COUNT];
volatile unsigned long instructionBuffer[INSTRUCTION_BUFFER_SIZE][STEPPER_COUNT+1]; //instructionBuffer[n][0] = timestamp, instructionBuffer[n][1..12] = targer stepper count


//TODO: handle fault conditions


void init_steppers(void) {
	DDRB |= (DATA | LATCH | CLOCK);		//Set shift control pins as outputs
	PORTB &= ~(DATA | LATCH | CLOCK);	//Set shift control pins low
	
	DDRA |= (STEPPER_RESET | STEPPER_SLEEP | STEPPER_ENABLE | SHIFT_ENABLE | SHIFT_CLEAR); 	//set pins as output
	PORTA |= (STEPPER_RESET | STEPPER_SLEEP | SHIFT_CLEAR); 								//set pins high
	PORTA &= ~(STEPPER_ENABLE | SHIFT_ENABLE);												//set pins low
	
	init_SPI();
	
	//initialize stepper counts and instruction buffer to 0
	for (unsigned int i = 0; i < STEPPER_COUNT; i++) {
		currentStepperCounts[i] = 0;
	}
	for (unsigned int i = 0; i < INSTRUCTION_BUFFER_SIZE; i++) {
		for (unsigned int j = 0; j < STEPPER_COUNT+1; j++) {
			instructionBuffer[i][j] = 0;
		}
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

void send_step_instruction(int instruction) {
	unsigned char spi_buffer;
	
	long stepperTargets[STEPPER_COUNT];
	for (unsigned int i = 0; i < STEPPER_COUNT; i++) {
		if (instructionBuffer[instruction][i+1] != 0) {
			stepperTargets[i] = instructionBuffer[instruction][i+1];
		} else {
			stepperTargets[i] = currentStepperCounts[i];
		}
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
			((currentStepperCounts[11] > stepperTargets[11]) << 7) | ((currentStepperCounts[11] != stepperTargets[11]) << 6)
			| ((currentStepperCounts[10] > stepperTargets[10]) << 5) | ((currentStepperCounts[10] != stepperTargets[10]) << 4)
			| ((currentStepperCounts[9] > stepperTargets[9]) << 3) | ((currentStepperCounts[9] != stepperTargets[9]) << 2)
			| ((currentStepperCounts[8] > stepperTargets[8]) << 1) | ((currentStepperCounts[8] != stepperTargets[8]) << 0));		
			spi_buffer = ((unsigned char) /*while waiting for first transfer to finish, computer second instruction*/
			((currentStepperCounts[7] > stepperTargets[7]) << 7) | ((currentStepperCounts[7] != stepperTargets[7]) << 6)
			| ((currentStepperCounts[6] > stepperTargets[6]) << 5) | ((currentStepperCounts[6] != stepperTargets[6]) << 4)
			| ((currentStepperCounts[5] > stepperTargets[5]) << 3) | ((currentStepperCounts[5] != stepperTargets[5]) << 2)
			| ((currentStepperCounts[4] > stepperTargets[4]) << 1) | ((currentStepperCounts[4] != stepperTargets[4]) << 0));
			while(!(SPSR & (1<<SPIF)));  //Wait for first SPI transfer to finish
			SPDR = spi_buffer; /*send second instruction*/
			spi_buffer = ((unsigned char) /*while waiting for second transfer to finish, computer third instruction*/		
			   ((currentStepperCounts[3] > stepperTargets[3]) << 7) | ((currentStepperCounts[3] != stepperTargets[3]) << 6)
			 | ((currentStepperCounts[2] > stepperTargets[2]) << 5) | ((currentStepperCounts[2] != stepperTargets[2]) << 4)
			 | ((currentStepperCounts[1] > stepperTargets[1]) << 3) | ((currentStepperCounts[1] != stepperTargets[1]) << 2)			
			 | ((currentStepperCounts[0] > stepperTargets[0]) << 1) | ((currentStepperCounts[0] != stepperTargets[0]) << 0));
			while(!(SPSR & (1<<SPIF))); //Wait for second SPI transfer to finish
			SPDR = spi_buffer; //send third instruction
			while(!(SPSR & (1<<SPIF))); //Wait for third SPI transfer to finish
			
			//Toggle latch to copy data to the storage register
			PORTB |= LATCH;
			PORTB &= ~LATCH;
		
			for (unsigned int i = stepper; i < STEPPER_COUNT; i++) {
				currentStepperCounts[i] += (currentStepperCounts[i] != stepperTargets[i]) * (currentStepperCounts[i] < stepperTargets[i] ? 1 : -1);
			}
		
			_delay_ms(3); //todo: implement delay using timer
			
		}
	}
}


void add_stepper_instruction(unsigned long timeStamp, unsigned char stepper, unsigned long target) {
	//find index of timestamp or create one if it doesn't exist in buffer
	
	//todo: handle buffer full condition (can probably just repeat everything and wait until an empty slot is found)
	
	char timeFound = 0;
	for (unsigned int i = 0; i < INSTRUCTION_BUFFER_SIZE; i++) {
		if (instructionBuffer[i][0] == timeStamp) {
			instructionBuffer[i][stepper+1] = target;
			timeFound = 1;
			break;
		}
	}
	if (!timeFound) {
		for (unsigned int i = 0; i < INSTRUCTION_BUFFER_SIZE; i++) {
			if (instructionBuffer[i][0] == 0) {
				instructionBuffer[i][0] = timeStamp;
				instructionBuffer[i][stepper+1] = target;
				break;
			}
		}
	}
}

long getNextInstructionTimestamp() {
	long nextInstructionTimeStamp = LONG_MAX;
	for (unsigned int i = 0; i < INSTRUCTION_BUFFER_SIZE; i++) {
		if (instructionBuffer[i][0] < nextInstructionTimeStamp) {
			nextInstructionTimeStamp = instructionBuffer[i][0];
		}
	}
	if (nextInstructionTimeStamp == LONG_MAX) {
		return 0; //signifies that the instruction buffer is empty
	}
	return nextInstructionTimeStamp;
}