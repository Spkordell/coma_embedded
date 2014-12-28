/*
 * stepper.h
 *
 * Created: 12/27/2014 10:24:45 PM
 *  Author: Steven
 */ 


#ifndef STEPPER_H_
#define STEPPER_H_


void init_steppers(void);
void init_SPI(void);
void spi_send(unsigned char byte);


#endif /* STEPPER_H_ */