/*
 * servo.h
 *
 * Created: 2/14/2015 8:01:26 AM
 *  Author: Steven Kordell
 */ 


#ifndef SERVO_H_
#define SERVO_H_

#define GRIPPER_SERVO_PIN (1 << PB3) //OC0A //note: this servo is continuous rotation with limit switches on each endpoint
#define WRIST_ROTATE_SERVO_PIN (1 << PD4) // OC1B
#define WRIST_FLEX_SERVO_PIN (1 << PD5) //OC1A
#define GRIPPER_ENDSTOP_OPEN_PIN (1 << PB1)
#define GRIPPER_ENDSTOP_CLOSE_PIN (1 << PB2)

#define GRIPPER_SERVO 0
#define WRIST_ROTATE_SERVO 1
#define WRIST_FLEX_SERVO 2

void init_servos();
void set_servo(int servo, double angle);
void servo_test(int servo);
void send_servo_instruction(unsigned long* servoTargets);
void stop_timer0();
void restart_timer0();
void gripper_open();
void gripper_close();

#endif /* SERVO_H_ */