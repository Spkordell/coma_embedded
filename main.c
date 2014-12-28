/*
 * main.c
 *
 * Created: 12/27/2014 10:22:59 PM
 *  Author: Steve Kordell
 */ 

#include "main.h"

int main(void) {	
	cli();
	init_steppers();
	init_uart();
	sei();
	
    while(1) {
		if (uart_char_waiting()) {
			uart_putchar(uart_getchar());
		}
    }
}