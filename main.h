/*
 * main.h
 *
 * Created: 12/27/2014 10:36:23 PM
 *  Author: Steven
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 18432000UL //clock speed: 18.432 MHz

#define PARSE_BUFFER_SIZE 30

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <limits.h>

#include "Stepper.h"
#include "FIFO.h"
#include "UART.h"
#include "Timer.h"

#include <util/setbaud.h>

//#define UART_ECHO

typedef enum {TELEOP, PATH} mode_t;

int main(void);
void parseInput(void);

#endif /* MAIN_H_ */