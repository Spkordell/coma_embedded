/*
 * main.h
 *
 * Created: 12/27/2014 10:36:23 PM
 *  Author: Steven
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 18432000UL //clock speed: 18.432 MHz

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "stepper.h"
#include "FIFO.h"
#include "UART.h"

#include <util/setbaud.h>

int main(void);

#endif /* MAIN_H_ */