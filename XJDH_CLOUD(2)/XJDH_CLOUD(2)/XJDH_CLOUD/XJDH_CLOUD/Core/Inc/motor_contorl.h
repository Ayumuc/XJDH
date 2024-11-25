/*
 * motor_contorl.h
 *
 *  Created on: Jul 5, 2024
 *      Author: Administrator
 */

#ifndef INC_MOTOR_CONTORL_H_
#define INC_MOTOR_CONTORL_H_

#include "main.h"
void Servo_Control(uint8_t mode_flag , uint16_t X , uint16_t Y);
void Servo_Control_CAN(uint8_t mode_flag , uint16_t X , uint16_t Y);
float cmu_k(float x,float y);
uint8_t Auto_Sail(void);
int flag_get(int angle);
int motor_contorl(int target_angle , int now_angle);
float get_angle(double lat1, double lng1, double lat2, double lng2);
double get_distance(double lat1, double lng1, double lat2, double lng2);
void Servo_Control_Blue(uint8_t mode_flag , uint16_t X , uint16_t Y);

#endif /* INC_MOTOR_CONTORL_H_ */
