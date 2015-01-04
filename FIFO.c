/*
 * FIFO.c
 *
 * Created: 12/28/2014 12:01:31 PM
 *  Author: Steven Kordell
 */ 


#include "main.h"

char fifo_put(FIFO* which_fifo, char data) {
	int new_tail = which_fifo->tail + 1;
	if (new_tail >= which_fifo->size) new_tail = 0; // wrap around
	if (which_fifo->head != new_tail) {	// if the FIFO is not full
		which_fifo->fifo[which_fifo->tail] = data;		// store data into the FIFO
		which_fifo->tail = new_tail;		// advance FIFO tail index
		return 1; // success
	}
	return 0;	// full
}

// get data from the FIFO
// returns 1 on success, 0 if FIFO was empty
char fifo_get(FIFO* which_fifo, char* data)
{
	if (which_fifo->head != which_fifo->tail) {	// if the FIFO is not empty
		*data = which_fifo->fifo[which_fifo->head];	// read data from the FIFO
		if(which_fifo->head+1 >= which_fifo->size) {
			which_fifo->head = 0;
		} else {
			which_fifo->head++;
		}
		return 1; // success
	}
	return 0;	// empty
}

char fifo_is_empty(FIFO* which_fifo) {
	return which_fifo->head == which_fifo->tail;
}