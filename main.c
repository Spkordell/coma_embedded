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
	//InitTimer(1, NORMAL, 0);
	sei();
	
    while(1) {
		if (uart_char_waiting()) {
			uart_putchar(uart_getchar());
		}
    }
}

/*
unsigned long count = 0;
ISR(TIMER1_OVF_vect) {
	count++;
	char buffer[10];
	itoa(count,buffer,10);
	uart_put_string(buffer);
	uart_putchar('\n');
}*/

/*
ISR(TIMER2_COMPA_vect) {
}
*/

