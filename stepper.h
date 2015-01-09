/*
 * stepper.h
 *
 * Created: 12/27/2014 10:24:45 PM
 *  Author: Steve Kordell
 */ 


#ifndef STEPPER_H_
#define STEPPER_H_

#define DATA (1<<PB5) //MOSI
#define LATCH (1<<PB4) //SS
#define CLOCK (1<<PB7) //SCK
#define SHIFT_ENABLE (1<<PA7)
#define SHIFT_CLEAR (1<<PA6)
#define STEPPER_RESET (1<<PA3)
#define STEPPER_SLEEP (1<<PA4)
#define STEPPER_ENABLE (1<<PA5)

#define STEPPER_COUNT 12
#define INSTRUCTION_BUFFER_SIZE 128

void init_steppers(void);
void init_SPI(void);
void spi_send(unsigned char byte);
void send_step_instruction(int instruction);
void add_stepper_instruction(unsigned long timeStamp, unsigned char stepper, unsigned long target);
long getNextInstructionTimestamp(unsigned int* instructionIndex);

#endif /* STEPPER_H_ */