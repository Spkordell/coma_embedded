/*
 * timer.c
 *
 * Created: 12/28/2014 2:58:57 PM
 *  Author: Steve Kordell
 *	Contains the functions for initializing a timer in either NORMAL or CTC mode as well as changing the COMPA value for timers initiated in CTC mode.
 */ 

#include "main.h"

/*
 * Initializes any timer (0,1, or 2) in either NORMAL or CTC mode. In NORMAL mode the third comp parameter has no effect, in CTC mode the comp parameter sets the comparison A value.
 * No clock prescaling is used.
 */

void InitTimer(unsigned char number, TimerMode mode, unsigned short int comp) {
	switch(number) {
	case 0:
		//Initialize timer to 0
		TCNT0 = 0;
		//disable prescaler	
		TCCR0B &= ~((1<<2)|(1<<1));
		TCCR0B |= (1<<0);
		//enable Timer overflow interrupt:
		TIMSK0 |= (1<<0);	
		//Enable Timer interrupt flag		
		TIFR0 |= (1<<0);		
		break;
	case 1:
		//Initialize timer to 0
		TCNT1 = 0;
		//disable prescaler
		TCCR1B &= ~((1<<2)|(1<<1));
		TCCR1B |= (1<<0);
		//enable Timer overflow interrupt:
		TIMSK1 |= (1<<0);
		//Enable Timer interrupt flag
		TIFR1 |= (1<<0);	
		break;
	case 2:
		//Initialize timer to 0
		TCNT2 = 0;
		//disable prescaler
		TCCR2B &= ~((1<<2)|(1<<1));
		TCCR2B |= (1<<0);
		//enable Timer overflow interrupt:
		TIMSK2 |= (1<<0);
		//Enable Timer interrupt flag
		TIFR2 |= (1<<0);	
	break;
		case 3:
		//Initialize timer to 0
		TCNT3 = 0;
		//disable prescaler
		TCCR3B &= ~((1<<2)|(1<<1));
		TCCR3B |= (1<<0);
		//enable Timer overflow interrupt:
		TIMSK3 |= (1<<0);
		//Enable Timer interrupt flag
		TIFR3 |= (1<<0);
		break;
	}
	if (mode == CTC) {
		//Enable CTC mode
		switch(number) {
		case 0:
			//Set mode to CTC			
			TCCR0A &= ~(1<<0);
			TCCR0A |= (1<<1);
			TCCR0B &= ~(1<<3);
			//Set comparison value
			OCR0A = comp;
			//Enable timer interrupts for CTC mode
			TIMSK0 |= (1<<1);			
			//Clear timer compare match flag
			TIFR0 |= (1<<1);			
			break;
		case 1:
			//Set mode to CTC			
			TCCR1A &= ~((1<<1)|(1<<0));
			TCCR1B |= (1<<3);
			TCCR1B &= ~(1<<4);
			//Set comparison value
			OCR1A = comp;
			//Enable timer interrupts for CTC mode		
			TIMSK1 |= (1<<1);
			//Clear timer compare match flag
			TIFR1 |= (1<<1);
			break;
		case 2:
			//Set mode to CTC
			TCCR2A &= ~(1<<0);
			TCCR2A |= (1<<1);
			TCCR2B &= ~(1<<3);
			//Set comparison value
			OCR2A = comp;
			//Enable timer interrupts for CTC mode
			TIMSK2 |= (1<<1);	
			//Clear timer compare match flag
			TIFR2 |= (1<<1);			
			break;
		case 3:
			//Set mode to CTC
			TCCR3A &= ~((1<<1)|(1<<0));
			TCCR3B |= (1<<3);
			TCCR3B &= ~(1<<4);
			//Set comparison value
			OCR3A = comp;
			//Enable timer interrupts for CTC mode
			TIMSK3 |= (1<<1);
			//Clear timer compare match flag
			TIFR3 |= (1<<1);
			break;
		}
	}
}

/*
 * Sets a timers comparison A value. Only has an effect for timers in CTC mode.
 */
void setCompValue(unsigned char number, unsigned short int comp) {
	switch(number) {
	case 0:
		OCR0A = comp; //Set comparison value
		break;
	case 1:
		OCR1A = comp; //Set comparison value
		break;
	case 2:
		OCR2A = comp; //Set comparison value
		break;
	}
}
