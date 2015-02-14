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

#define GRIPPER_SERVO 0
#define WRIST_ROTATE_SERVO 1
#define WRIST_FLEX_SERVO 2


void init_servos();
void set_servo(int servo, double angle);
void servo_test(int servo);
void gripper_open();
void gripper_open();




#endif /* SERVO_H_ */