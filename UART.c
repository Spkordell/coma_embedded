/*
 * UART.c
 *
 * Created: 12/27/2014 10:25:20 PM
 *  Author: Steven
 */ 

#include "main.h"

volatile FIFO in_buffer;
volatile FIFO out_buffer;
char in_fifo[UART_IN_FIFO_SIZE];
char out_fifo[UART_OUT_FIFO_SIZE];

/**
 * \brief UART data register empty interrupt handler
 *
 * This handler is called each time the UART data register is available for
 * sending data.
 */
ISR(UART0_DATA_EMPTY_IRQ) {
	char data;
	//if there is data to send, fetch it and send it
	if (fifo_get(&out_buffer, &data)) {
		UDR0 = data;
	} else {
		// no more data to send, turn off data ready interrupt
		UCSR0B &= ~(1 << UDRIE0);
	}
}

/**
 * \brief Data RX interrupt handler
 *
 * This is the handler for UART receive data
 */
ISR(UART0_RX_IRQ) {
	fifo_put(&in_buffer, UDR0);
}

/**
 * \brief Initialize the UART with correct baud rate settings
 *
 * This function will initialize the UART baud rate registers with the correct
 * values using the AVR libc setbaud utility. In addition set the UART to
 * 8-bit, 1 stop and no parity.
 */
void init_uart(void) {
#if defined UBRR0H
	// get the values from the setbaud tool
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
#else
#error "Device is not supported by the driver"
#endif

#if USE_2X
	UCSR0A |= (1 << U2X0);
#endif
	// enable RX and TX and set interrupts on rx complete
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

	// 8-bit, 1 stop bit, no parity, asynchronous UART
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) | (0 << USBS0) |
			(0 << UPM01) | (0 << UPM00) | (0 << UMSEL01) |
			(0 << UMSEL00);

	// initialize the in and out buffer for the UART
	in_buffer.head = 0;
	in_buffer.tail = 0;
	in_buffer.size = UART_IN_FIFO_SIZE;
	in_buffer.fifo = in_fifo;
	out_buffer.head = 0;
	out_buffer.tail = 0;
	in_buffer.size = UART_OUT_FIFO_SIZE;
	out_buffer.fifo = out_fifo;
}

/**
 * \brief Function for putting a char in the UART buffer
 *
 * \param data the data to add to the UART buffer and send
 *
 */
void uart_putchar(uint8_t data) {
	cli();
	if (fifo_is_empty(&out_buffer)) {
		// First data in buffer, enable data ready interrupt
		UCSR0B |=  (1 << UDRIE0);
	}
	// Put data in buffer
	fifo_put(&out_buffer, data);
	sei();
}

/**
 * \brief Function for getting a char from the UART receive buffer
 *
 * \retval Next data byte in receive buffer
 */
uint8_t uart_getchar(void) {
	char data;
	fifo_get(&in_buffer, &data);
	return data;
}

/**
 * \brief Function to check if we have a char waiting in the UART receive buffer
 *
 * \retval true if data is waiting
 * \retval false if no data is waiting
 */
uint8_t uart_char_waiting(void) {
	return !fifo_is_empty(&in_buffer);
}

void uart_put_string(char* in_string) {
	char* p = in_string;
	while (*p) {
		uart_putchar(*p++);
	}
}