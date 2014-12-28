/*
 * FIFO.h
 *
 * Created: 12/28/2014 12:02:22 PM
 *  Author: Steven Kordell
 */ 


#ifndef FIFO_H_
#define FIFO_H_

#define FIFO_SIZE 2048

struct fifo {
	volatile int head;
	volatile int tail;
	char fifo[FIFO_SIZE];
};

typedef struct fifo FIFO;

char fifo_put(FIFO* fifo, char data);
char fifo_get(FIFO* which_fifo, char* data);
char fifo_is_empty(FIFO* fifo);

#endif /* FIFO_H_ */