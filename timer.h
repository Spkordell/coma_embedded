/*
 * timer.h
 *
 * Created: 12/28/2014 2:59:07 PM
 *  Author: Steve kordell
 */ 


#ifndef TIMER_H_
#define TIMER_H_

typedef enum {NORMAL, CTC} TimerMode;

void InitTimer(unsigned char number, TimerMode mode, unsigned short int comp);
void setCompValue(unsigned char number, unsigned short int comp);


#endif /* TIMER_H_ */