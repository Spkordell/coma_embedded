/*
 * servo.h
 *
 * Created: 2/14/2015 8:01:26 AM
 *  Author: Steven Kordell
 */ 


#ifndef SERVO_H_
#define SERVO_H_

#define GRIPPER_SERVO_PIN (1 << PB3)
#define WRIST_ROTATE_SERVO_PIN (1 << PD4)
#define WRIST_FLEX_SERVO_PIN (1 << PD5)

void init_servos();
void set_servo(int servo, double angle);
void servo_test(int servo);

#endif /* SERVO_H_ */