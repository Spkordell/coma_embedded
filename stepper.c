/*
 * stepper.c
 *
 * Created: 12/27/2014 10:24:21 PM
 *  Author: Steve Kordell
 */ 

#include "main.h"

unsigned long stepperCounts[STEPPER_COUNT];
unsigned long stepperTargets[STEPPER_COUNT];


//TODO: handle fault conditions


void init_steppers(void) {
	DDRB |= (DATA | LATCH | CLOCK);		//Set shift control pins as outputs
	PORTB &= ~(DATA | LATCH | CLOCK);	//Set shift control pins low
	
	DDRA |= (STEPPER_RESET | STEPPER_SLEEP | STEPPER_ENABLE | SHIFT_ENABLE | SHIFT_CLEAR); 	//set pins as output
	PORTA |= (STEPPER_RESET | STEPPER_SLEEP | SHIFT_CLEAR); 								//set pins high
	PORTA &= ~(STEPPER_ENABLE | SHIFT_ENABLE);												//set pins low
	
	init_SPI();
	
	//initialize stepper counts and tagets to zero
	for (unsigned int i = 0; i < STEPPER_COUNT; i++) {
		stepperCounts[i] = 0;
		stepperTargets[i] = 0;
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

void send_step(void) {	
	
	while (stepperCounts[0] != stepperTargets[0]) {
		//clear shift register
		PORTA &= ~SHIFT_CLEAR;		//write low
		PORTA |= SHIFT_CLEAR; 		//write high
		//Toggle latch to copy data to the storage register
		PORTB |= LATCH;
		PORTB &= ~LATCH;
	
		//send step signal
		spi_send((unsigned char)0b00000000);
		spi_send((unsigned char)0b00000000);
		spi_send((unsigned char)0b00000001 | ((stepperCounts[0] > stepperTargets[0]) << 1) );
		//Toggle latch to copy data to the storage register
		PORTB |= LATCH;
		PORTB &= ~LATCH;
		
		stepperCounts[0] += stepperCounts[0] < stepperTargets[0] ? 1 : -1;
		
		_delay_ms(3);
	}
}

void set_step_target(unsigned int stepper, unsigned long target){
	stepperTargets[stepper] = target;
}