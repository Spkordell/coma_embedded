/*
 * stepper.c
 *
 * Created: 12/27/2014 10:24:21 PM
 *  Author: Steven
 */ 

#include "main.h"

#define DATA (1<<PB5) //MOSI
#define LATCH (1<<PB4) //SS
#define CLOCK (1<<PB7) //SCK
#define SHIFT_ENABLE (1<<PA7)
#define SHIFT_CLEAR (1<<PA6)
#define STEPPER_RESET (1<<PA3)
#define STEPPER_SLEEP (1<<PA4)
#define STEPPER_ENABLE (1<<PA5)

//TODO: handle fault conditions

void init_steppers(void) {
	DDRB |= (DATA | LATCH | CLOCK);		//Set shift control pins as outputs
	PORTB &= ~(DATA | LATCH | CLOCK);	//Set shift control pins low
	
	DDRA |= (STEPPER_RESET | STEPPER_SLEEP | STEPPER_ENABLE | SHIFT_ENABLE | SHIFT_CLEAR); 	//set pins as output
	PORTA |= (STEPPER_RESET | STEPPER_SLEEP | SHIFT_CLEAR); 								//set pins high
	PORTA &= ~(STEPPER_ENABLE | SHIFT_ENABLE);												//set pins low
	
	init_SPI();
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
	//clear shift register
	PORTA &= ~SHIFT_CLEAR;		//write low
	PORTA |= SHIFT_CLEAR; 		//write high
	//Toggle latch to copy data to the storage register
	PORTB |= LATCH;
	PORTB &= ~LATCH;
	
	//send step signal
	spi_send((unsigned char)0b00000000);
	spi_send((unsigned char)0b00000000);
	spi_send((unsigned char)0b00000001);
	//Toggle latch to copy data to the storage register
	PORTB |= LATCH;
	PORTB &= ~LATCH;
	
	_delay_ms(2);	
}