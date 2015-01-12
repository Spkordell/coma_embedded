/*
 * FIFO.h
 *
 * Created: 12/28/2014 12:02:22 PM
 *  Author: Steven Kordell
 */ 


#ifndef FIFO_H_
#define FIFO_H_

struct fifo {
	volatile int head;
	volatile int tail;
	int size;
	char* fifo;	
};
typedef struct fifo FIFO;

struct multi_dim_fifo {
	volatile int head;
	volatile int tail;
	int size;
	unsigned long** fifo;
};
typedef struct multi_dim_fifo MULTI_DIM_FIFO;

char fifo_put(FIFO* fifo, char data);
char fifo_get(FIFO* which_fifo, char* data);
char fifo_is_empty(FIFO* fifo);

char multi_dim_fifo_put(MULTI_DIM_FIFO* which_fifo, unsigned long* data);
char multi_dim_fifo_get(MULTI_DIM_FIFO* which_fifo, unsigned long** data);
char multi_dim_fifo_is_empty(MULTI_DIM_FIFO* which_fifo);

#endif /* FIFO_H_ */