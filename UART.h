/*
 * UART.h
 *
 * Created: 12/27/2014 10:25:45 PM
 *  Author: Steve Kordell
 */ 


#ifndef UART_H_
#define UART_H_

#define BAUD 115200
#define BAUD_TOL 2 //set the baud rate tolerance to 2%

#define UART0_DATA_EMPTY_IRQ USART0_UDRE_vect //UART data buffer ready interrupt vector
#define UART0_RX_IRQ USART0_RX_vect //UART data received interrupt vector

void init_uart(void);
void uart_putchar(uint8_t data);
uint8_t uart_getchar(void);
uint8_t uart_char_waiting(void);
void uart_put_string(char* in_string);

#endif /* UART_H_ */