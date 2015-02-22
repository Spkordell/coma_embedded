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
#define STEPPER_FAULT (1<<PD2)

#define ENDSTOP0 (1 << PC0)
#define ENDSTOP1 (1 << PC1)
#define ENDSTOP2 (1 << PC2)
#define ENDSTOP3 (1 << PC3)
#define ENDSTOP4 (1 << PC4)
#define ENDSTOP5 (1 << PC5)
#define ENDSTOP6 (1 << PC6)
#define ENDSTOP7 (1 << PC7)
#define ENDSTOP8 (1 << PA0)
#define ENDSTOP9 (1 << PA1)
#define ENDSTOP10 (1 << PA2)
#define ENDSTOP11 (1 << PB0)

#define STEPPER_COUNT 12
#define PATH_INSTRUCTION_BUFFER_SIZE 64
#define TELEOP_INSTRUCTION_BUFFER_SIZE 64

void init_endstops(void);
void init_steppers(void);
void test_endstops(void);
void home_steppers(void);
void init_SPI(void);
void spi_send(unsigned char byte);
char is_fault();
void send_step_instruction(int instruction);
void send_teleop_step(unsigned long* stepperTargets);
void add_stepper_instruction(unsigned long timeStamp, unsigned char stepper, unsigned long target);
long getNextInstructionTimestamp(unsigned int* instructionIndex);
//void buffer_teleop_instruction(unsigned long* instruction);

#endif /* STEPPER_H_ */