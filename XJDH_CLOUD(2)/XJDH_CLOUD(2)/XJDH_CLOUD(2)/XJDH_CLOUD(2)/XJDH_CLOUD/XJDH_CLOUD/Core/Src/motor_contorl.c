/*
 * motor.c
 *
 *  Created on: 2023骞�7鏈�17鏃�
 *      Author: Qi520503
 */
#include "motor_contorl.h"
#include "tim.h"
#include "board_init.h"
#include "math.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
#define PI                      3.141592654
#define EARTH_RADIUS            6378.137        //地球近似半径
#define TURN_LEFT 				1
#define TURN_RIGHT              2
#define GO_STRIGHT              3
#define GO_STOP                 4
#define ONE 1
#define TWO 2
#define THREE 3
#define FOUR 4

#define mode_zero 1
#define mode_normal 0
#define mode_stright 2
#define mode_rightleft 3
#define mode_upleft 4
#define mode_upright 5
#define mode_downleft 6
#define mode_downright 7
#define PWM_REG_MID 1000
#define PWM_REG_RANGE 333


 void Servo_Control(uint8_t mode_flag , uint16_t X , uint16_t Y)
 {
	float weight,left_pwm,right_pwm;
	float temp_k;
	float precision_l,precision_r,precision;
	float motor,motor_l,motor_r;
	//printf("X: %d   Y:%d\r\n",X,Y);
	if(mode_flag == mode_zero){
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, Duty_cycle_zero);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, Duty_cycle_zero);
		return ;
	}
	else if(mode_flag == mode_stright){
		precision =(Duty_cycle_max - Duty_cycle_min) / (4096 - 0);
		motor = ((4096-Y) * precision) + Duty_cycle_min ;
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, motor);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, motor);
		//printf("motor_l:%f\r\n",motor);
		//printf("motor_r:%f\r\n",motor);
		return;
	}
	else if(mode_flag == mode_rightleft){
		precision_l = (Duty_cycle_max - Duty_cycle_min) / (4096 - 0);
		precision_r = (Duty_cycle_max - Duty_cycle_min) / (4096 - 0);
		motor_l = Duty_cycle_max - ((4096-X) * precision_r);
		motor_r = ((4096-X) * precision_l) + Duty_cycle_min ;
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, motor_l);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, motor_r);
		//printf("motor_l:%f\r\n",motor_l);
		//printf("motor_r:%f\r\n",motor_r);
		return ;
	}
	else if(mode_flag == mode_upleft){
		temp_k = cmu_k((float)X,(float)Y);
		//printf("K:%f\r\n",temp_k);
		if(temp_k > 1)
			right_pwm=3000+((2048 - X)/2048.0)*1000 + 800;
		else
			right_pwm=3000+((2048 - Y)/2048.0)*1000 + 300;
		left_pwm=(uint16_t)(3000 + temp_k*1000) - 1200;
		if(left_pwm > Duty_cycle_max)left_pwm = Duty_cycle_max;
		if(left_pwm < Duty_cycle_min)left_pwm = Duty_cycle_min;
		if(right_pwm > Duty_cycle_max)right_pwm = Duty_cycle_max;
		if(right_pwm < Duty_cycle_min)right_pwm = Duty_cycle_min;
		//printf("motor_l:%f\r\n",left_pwm);
		//printf("motor_r:%f\r\n",right_pwm);
		if(left_pwm > right_pwm)return;
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, left_pwm);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, right_pwm);
		return;
	}
	else if(mode_flag == mode_upright){
		temp_k = cmu_k((float)X,(float)Y);
	    //printf("K:%f\r\n",temp_k);
		if(temp_k < (-1)){
			left_pwm = 3000+((X - 2048)/2048.0)*1000 + 1200;
		}
		else{
			left_pwm=3000+((2048 - Y)/2048.0)*1000 + 1200;
		}
		right_pwm=(uint16_t)(3000-temp_k*1000) - 1200;
		if(left_pwm > Duty_cycle_max)left_pwm = Duty_cycle_max;
		if(left_pwm < Duty_cycle_min)left_pwm = Duty_cycle_min;
		if(right_pwm > Duty_cycle_max)right_pwm = Duty_cycle_max;
		if(right_pwm < Duty_cycle_min)right_pwm = Duty_cycle_min;
		//printf("motor_l:%f\r\n",left_pwm);
		//printf("motor_r:%f\r\n",right_pwm);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, left_pwm);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, right_pwm);
		return;
	}
	else if(mode_flag == mode_downleft){
		temp_k = cmu_k((float)X,(float)Y);
		//printf("K:%f\r\n",temp_k);
		if(temp_k<(-1))
			left_pwm=3000-((2048-X)/2048.0)*1000 - 1200;
		else
			left_pwm=3000-((2048-Y)/2048.0)*1000 - 1200;
		right_pwm=(uint16_t)(3000+temp_k*1000) + 1200;
		if(left_pwm > Duty_cycle_max)left_pwm = Duty_cycle_max;
		if(left_pwm < Duty_cycle_min)left_pwm = Duty_cycle_min;
		if(right_pwm > Duty_cycle_max)right_pwm = Duty_cycle_max;
		if(right_pwm < Duty_cycle_min)right_pwm = Duty_cycle_min;
		//printf("motor_l:%f\r\n",left_pwm);
		//printf("motor_r:%f\r\n",right_pwm);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, left_pwm);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, right_pwm);
		return;
	}
	else if(mode_flag == mode_downright){
		temp_k = cmu_k((float)X,(float)Y);
		//printf("K:%f\r\n",temp_k);
		if(temp_k > 1.0){
			right_pwm = 3000-((2048-Y)/2048.0)*1000 - 1850;
		}
		else{
			right_pwm= 3000-((X-2048)/2048.0)*1000 - 100;
		}
		left_pwm=(uint16_t)(3000-temp_k*1000) + 1600;
		if(left_pwm > Duty_cycle_max)left_pwm = Duty_cycle_max;
		if(left_pwm < 1800)left_pwm = Duty_cycle_min;
		if(right_pwm > Duty_cycle_max)right_pwm = Duty_cycle_max;
		if(right_pwm < Duty_cycle_min)right_pwm = Duty_cycle_min;
		//printf("motor_l:%f\r\n",left_pwm);
		//printf("motor_r:%f\r\n",right_pwm);
		if(right_pwm > left_pwm)return;
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, left_pwm);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, right_pwm);
		return;
	}
 }

void Servo_Control_Blue(uint8_t mode_flag , uint16_t X , uint16_t Y){
	float weight,left_pwm,right_pwm;
	float temp_k;
	float precision_l,precision_r,precision;
	float motor,motor_l,motor_r;
	switch(mode_flag){
	case mode_zero:
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, Duty_cycle_zero);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, Duty_cycle_zero);
		break;
	case mode_stright:
		precision =(Duty_cycle_max - Duty_cycle_min) / (4096 - 0);
		motor = ((4096-Y) * precision) + Duty_cycle_min ;
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, motor);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, motor);
		break;
	case mode_rightleft:
		precision_l = (Duty_cycle_max - Duty_cycle_min) / (4096 - 0);
		precision_r = (Duty_cycle_max - Duty_cycle_min) / (4096 - 0);
		motor_l = Duty_cycle_max - ((4096-X) * precision_r);
		motor_r = ((4096-X) * precision_l) + Duty_cycle_min ;
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, motor_l);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, motor_r);
		break;
	case mode_upleft:
		temp_k = cmu_k((float)X,(float)Y);
		//printf("K:%f\r\n",temp_k);
		if(temp_k > 1)
			right_pwm=3000+((2048 - X)/2048.0)*1000 + 800;
		else
			right_pwm=3000+((2048 - Y)/2048.0)*1000 + 300;
		left_pwm=(uint16_t)(3000 + temp_k*1000) - 1200;
		if(left_pwm > 4200)left_pwm = 4200;
		if(left_pwm < 1800)left_pwm = 1800;
		if(right_pwm > 4200)right_pwm = 4200;
		if(right_pwm < 1800)right_pwm = 1800;
		//printf("motor_l:%f\r\n",left_pwm);
		//printf("motor_r:%f\r\n",right_pwm);
		if(left_pwm > right_pwm)return;
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, left_pwm);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, right_pwm);
		break;
	case mode_upright:
		temp_k = cmu_k((float)X,(float)Y);
		//printf("K:%f\r\n",temp_k);
		if(temp_k < (-1)){
			left_pwm = 3000+((X - 2048)/2048.0)*1000 + 1200;
		}
		else{
			left_pwm=3000+((2048 - Y)/2048.0)*1000 + 1200;
		}
		right_pwm=(uint16_t)(3000-temp_k*1000) - 1200;
		if(left_pwm > 4200)left_pwm = 4200;
		if(left_pwm < 1800)left_pwm = 1800;
		if(right_pwm > 4200)right_pwm = 4200;
		if(right_pwm < 1800)right_pwm = 1800;
		//printf("motor_l:%f\r\n",left_pwm);
		//printf("motor_r:%f\r\n",right_pwm);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, left_pwm);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, right_pwm);
		break;
	case mode_downleft:
		temp_k = cmu_k((float)X,(float)Y);
		//printf("K:%f\r\n",temp_k);
		if(temp_k<(-1))
			left_pwm=3000-((2048-X)/2048.0)*1000 - 1200;
		else
			left_pwm=3000-((2048-Y)/2048.0)*1000 - 1200;
		right_pwm=(uint16_t)(3000+temp_k*1000) + 1200;
		if(left_pwm > 4200)left_pwm = 4200;
		if(left_pwm < 1800)left_pwm = 1800;
		if(right_pwm > 4200)right_pwm = 4200;
		if(right_pwm < 1800)right_pwm = 1800;
		//printf("motor_l:%f\r\n",left_pwm);
		//printf("motor_r:%f\r\n",right_pwm);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, left_pwm);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, right_pwm);
		break;
	case mode_downright:
		temp_k = cmu_k((float)X,(float)Y);
		//printf("K:%f\r\n",temp_k);
		if(temp_k > 1.0){
			right_pwm = 3000-((2048-Y)/2048.0)*1000 - 1850;
		}
		else{
			right_pwm= 3000-((X-2048)/2048.0)*1000 - 100;
		}
		left_pwm=(uint16_t)(3000-temp_k*1000) + 1600;
		if(left_pwm > 4200)left_pwm = 4200;
		if(left_pwm < 1800)left_pwm = 1800;
		if(right_pwm > 4200)right_pwm = 4200;
		if(right_pwm < 1800)right_pwm = 1800;
		//printf("motor_l:%f\r\n",left_pwm);
		//printf("motor_r:%f\r\n",right_pwm);
		if(right_pwm > left_pwm)return;
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, left_pwm);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, right_pwm);
		break;
	default:break;
	}
	return ;
}


#define can_mode_zero 1
#define can_mode_upstright 2
#define can_mode_downstright 3
#define can_mode_left 4
#define can_mode_right 5
#define can_mode_leftup 6
#define can_mode_rightup 7
#define can_mode_leftdown 8
#define can_mode_rightdown 9
#define CAN_Duty_cycle_max 2000
#define CAN_Duty_cycle_min 0

#define	SET_BIT(x, bit)	(x |= (1 << bit))	/* 置位第bit位 */
#define	CLEAR_BIT(x, bit)	(x &= ~(1 << bit))	/* 清零第bit位 */

	 uint8_t TxData[8] = {0};
 void Servo_Control_CAN(uint8_t mode_flag , uint16_t X , uint16_t Y){
	 uint8_t length = 0x08;
	 uint8_t high,low;
	 float temp_k;
	 float weight,left_pwm,right_pwm;
	 float precision,precision_l,precision_r,motor,motor_l,motor_r;
	 if(mode_flag == mode_zero){
		for(int i = 0 ; i < 8 ; i++){
			TxData[i] = 0x00;
		}
		CAN_TX_Message(TxData, length);
		return ;
	 }
	 else if(mode_flag == can_mode_upstright){
		motor = CAN_Duty_cycle_max - ((4096 - CAN_Duty_cycle_min)*(4096-abs(Y-4096))/(4096 - 0));
		printf("motor:%f\r\n",motor);
		if(motor > 2000)motor = 2000;
		if(motor < 0)motor = 0;
		TxData[0] = ((int)motor >> 8) & 0xff;	 	//高8位
		TxData[1] = 	(int)motor & 0xff; 		//低8位
		TxData[2] = ((int)motor >> 8) & 0xff;	 	//高8位
		TxData[3] = 	(int)motor & 0xff; 		 //低8位
		CLEAR_BIT(TxData[0],3);
		CLEAR_BIT(TxData[2],3);
		CAN_TX_Message(TxData, length);
		return;
	 }
	 else if(mode_flag == can_mode_downstright){
		motor = CAN_Duty_cycle_max - ((4096 - CAN_Duty_cycle_min)*(4096-Y)/(4096 - 0));
		if(motor > 2000)motor = 2000;
		if(motor < 0)motor = 0;
		TxData[0] = ((int)motor >> 8) & 0xff;	 	//高8位
		TxData[1] = 	(int)motor & 0xff; 		//低8位
		TxData[2] = ((int)motor >> 8) & 0xff;	 	//高8位
		TxData[3] = 	(int)motor & 0xff; 		//低8位
		SET_BIT(TxData[0],3);
		SET_BIT(TxData[2],3);
		CAN_TX_Message(TxData, length);
		return;
	 }
	 else if(mode_flag == can_mode_left){
		motor_l = CAN_Duty_cycle_max - ((4096 - CAN_Duty_cycle_min)*(4096-abs(X-4096))/(4096 - 0));
		if(motor > 2000)motor_l = 2000;
		if(motor < 0)motor_l = 0;
		TxData[0] = ((int)motor_l >> 8) & 0xff;	 	//高8位
		TxData[1] = 	(int)motor_l & 0xff; 		//低8位
		TxData[2] = ((int)motor_l >> 8) & 0xff;	 	//高8位
		TxData[3] = 	(int)motor_l & 0xff; 		//低8位
		SET_BIT(TxData[0],3);
		CLEAR_BIT(TxData[2],3);
		CAN_TX_Message(TxData, length);
	 }
	 else if(mode_flag == can_mode_right){
		motor_l = CAN_Duty_cycle_max - ((4096 - CAN_Duty_cycle_min)*(4096-X)/(4096 - 0));
		if(motor > 2000)motor_l = 2000;
		if(motor < 0)motor_l = 0;
		TxData[0] = ((int)motor_l >> 8) & 0xff;	 	//高8位
		TxData[1] = 	(int)motor_l & 0xff; 		//低8位
		TxData[2] = ((int)motor_l >> 8) & 0xff;	 	//高8位
		TxData[3] = 	(int)motor_l & 0xff; 		//低8位
		printf("motor_l:%f\r\n",motor_l);
		printf("motor_r:%f\r\n",motor_l);
		CLEAR_BIT(TxData[0],3);
		SET_BIT(TxData[2],3);
		CAN_TX_Message(TxData, length);
	 }
	 else if(mode_flag == can_mode_leftup){
		right_pwm=CAN_Duty_cycle_max - ((CAN_Duty_cycle_max - CAN_Duty_cycle_min)*(4096-abs(X-4096))/(4096 - 0));
		left_pwm=((4096 - CAN_Duty_cycle_min)*(4096-abs(Y-4096))/(4096 - 0)) + CAN_Duty_cycle_min;
		if(left_pwm > 2000)left_pwm = 2000;
		if(left_pwm < 0)left_pwm = 0;
		if(right_pwm > 2000)right_pwm = 2000;
		if(right_pwm < 0)right_pwm = 0;
		TxData[0] = ((int)left_pwm >> 8) & 0xff;	 	//高8位
		TxData[1] = 	(int)left_pwm & 0xff; 		//低8位
		TxData[2] = ((int)right_pwm >> 8) & 0xff;	 	//高8位
		TxData[3] = 	(int)right_pwm & 0xff; 		//低8位
		SET_BIT(TxData[0],3);
		CLEAR_BIT(TxData[2],3);
		if(X > Y){
			left_pwm=((4096 - CAN_Duty_cycle_min)*(4096-abs(X-4096))/(4096 - 0)) + CAN_Duty_cycle_min;
			right_pwm=((CAN_Duty_cycle_max - CAN_Duty_cycle_min)*(4096-Y)/(4096 - 0)) + CAN_Duty_cycle_min ;
			if(left_pwm > 2000)left_pwm = 2000;
			if(left_pwm < 0)left_pwm = 0;
			if(right_pwm > 2000)right_pwm = 2000;
			if(right_pwm < 0)right_pwm = 0;
			TxData[0] = ((int)left_pwm >> 8) & 0xff;	 	//高8位
			TxData[1] = 	(int)left_pwm & 0xff; 		//低8位
			TxData[2] = ((int)right_pwm >> 8) & 0xff;	 	//高8位
			TxData[3] = 	(int)right_pwm & 0xff; 		//低8位
			CLEAR_BIT(TxData[0],3);
			CLEAR_BIT(TxData[2],3);
		}
		printf("motor_l:%f\r\n",left_pwm);
		printf("motor_r:%f\r\n",right_pwm);
		CAN_TX_Message(TxData, length);
		return;
	 }
	 else if(mode_flag == can_mode_rightup){
		temp_k = cmu_k((float)X,(float)Y);
		printf("X:%f\r\n",(float)X);
		printf("Y:%f\r\n",(float)Y);
		printf("k:%f\r\n",temp_k);
		if(temp_k > (-1)){
			right_pwm=((4096 - CAN_Duty_cycle_min)*(4096-abs(board_config.Rockers.Rocker_Y-4096))/(4096 - 0)) + CAN_Duty_cycle_min;
			left_pwm=CAN_Duty_cycle_max - ((CAN_Duty_cycle_max - CAN_Duty_cycle_min)*(4096-board_config.Rockers.Rocker_X)/(4096 - 0));
			if(left_pwm > 2000)left_pwm = 2000;
			if(left_pwm < 0)left_pwm = 0;
			if(right_pwm > 2000)right_pwm = 2000;
			if(right_pwm < 0)right_pwm = 0;
			TxData[0] = ((int)left_pwm >> 8) & 0xff;	 	//高8位
			TxData[1] = 	(int)left_pwm & 0xff; 		//低8位
			TxData[2] = ((int)right_pwm >> 8) & 0xff;	 	//高8位
			TxData[3] = 	(int)right_pwm & 0xff; 		//低8位
			CLEAR_BIT(TxData[0],3);
			SET_BIT(TxData[2],3);
		}
		else if(temp_k <= -1 && board_config.Rockers.Rocker_X != 2048){
			right_pwm=((4096 - CAN_Duty_cycle_min)*(4096-board_config.Rockers.Rocker_X)/(4096 - 0)) + CAN_Duty_cycle_min ;
			left_pwm=((4096 - CAN_Duty_cycle_min)*(4096-board_config.Rockers.Rocker_Y)/(4096 - 0)) + CAN_Duty_cycle_min;
			if(left_pwm > 2000)left_pwm = 2000;
			if(left_pwm < 0)left_pwm = 0;
			if(right_pwm > 2000)right_pwm = 2000;
			if(right_pwm < 0)right_pwm = 0;
			TxData[0] = ((int)left_pwm >> 8) & 0xff;	 	//高8位
			TxData[1] = 	(int)left_pwm & 0xff; 		//低8位
			TxData[2] = ((int)right_pwm >> 8) & 0xff;	 	//高8位
			TxData[3] = 	(int)right_pwm & 0xff; 		//低8位
			CLEAR_BIT(TxData[0],3);
			CLEAR_BIT(TxData[2],3);
		}
//		printf("motor_l:%f\r\n",left_pwm);
//		printf("motor_r:%f\r\n",right_pwm);
		CAN_TX_Message(TxData, length);
		return;
	 }
	 else if(mode_flag == can_mode_leftdown){
		temp_k = cmu_k((float)board_config.Rockers.Rocker_X,(float)board_config.Rockers.Rocker_Y);
		printf("X:%f\r\n",(float)board_config.Rockers.Rocker_X);
		printf("Y:%f\r\n",(float)board_config.Rockers.Rocker_Y);
		printf("k:%f\r\n",temp_k);
		if(temp_k > (-1)){
			right_pwm=((4096 - CAN_Duty_cycle_min)*(4096-board_config.Rockers.Rocker_Y)/(4096 - 0)) + CAN_Duty_cycle_min;
			left_pwm=CAN_Duty_cycle_max - ((4096 - CAN_Duty_cycle_min)*(4096-abs(board_config.Rockers.Rocker_X-4096))/(4096 - 0)) ;
			if(left_pwm > 2000)left_pwm = 2000;
			if(left_pwm < 0)left_pwm = 0;
			if(right_pwm > 2000)right_pwm = 2000;
			if(right_pwm < 0)right_pwm = 0;
			TxData[0] = ((int)left_pwm >> 8) & 0xff;	 	//高8位
			TxData[1] = 	(int)left_pwm & 0xff; 		//低8位
			TxData[2] = ((int)right_pwm >> 8) & 0xff;	 	//高8位
			TxData[3] = 	(int)right_pwm & 0xff; 		//低8位
			SET_BIT(TxData[0],3);
			CLEAR_BIT(TxData[2],3);
		}
		else if(temp_k <= (-1) && board_config.Rockers.Rocker_X != 2048){
			right_pwm=((4096 - CAN_Duty_cycle_min)*(4096-abs(board_config.Rockers.Rocker_X-4096))/(4096 - 0)) + CAN_Duty_cycle_min;
			left_pwm=((4096 - CAN_Duty_cycle_min)*(4096-abs(board_config.Rockers.Rocker_Y-4096))/(4096 - 0)) + CAN_Duty_cycle_min  ;
			if(left_pwm > 2000)left_pwm = 2000;
			if(left_pwm < 0)left_pwm = 0;
			if(right_pwm > 2000)right_pwm = 2000;
			if(right_pwm < 0)right_pwm = 0;
			TxData[0] = ((int)left_pwm >> 8) & 0xff;	 	//高8位
			TxData[1] = 	(int)left_pwm & 0xff; 		//低8位
			TxData[2] = ((int)right_pwm >> 8) & 0xff;	 	//高8位
			TxData[3] = 	(int)right_pwm & 0xff; 		//低8位
			SET_BIT(TxData[0],3);
			SET_BIT(TxData[2],3);
		}
		CAN_TX_Message(TxData, length);
		return ;
	 }
	 else if(mode_flag == can_mode_rightdown){
		temp_k = cmu_k((float)board_config.Rockers.Rocker_X,(float)board_config.Rockers.Rocker_Y);
		printf("X:%f\r\n",(float)board_config.Rockers.Rocker_X);
		printf("Y:%f\r\n",(float)board_config.Rockers.Rocker_Y);
		printf("k:%f\r\n",temp_k);
		if(temp_k < 1){
			right_pwm=CAN_Duty_cycle_max - ((4096 - CAN_Duty_cycle_min)*(4096-board_config.Rockers.Rocker_X)/(4096 - 0)) ;
			left_pwm=((4096 - CAN_Duty_cycle_min)*(4096-board_config.Rockers.Rocker_Y)/(4096 - 0)) + CAN_Duty_cycle_min;
			if(left_pwm > 2000)left_pwm = 2000;
			if(left_pwm < 0)left_pwm = 0;
			if(right_pwm > 2000)right_pwm = 2000;
			if(right_pwm < 0)right_pwm = 0;
			TxData[0] = ((int)left_pwm >> 8) & 0xff;	 	//高8位
			TxData[1] = 	(int)left_pwm & 0xff; 		//低8位
			TxData[2] = ((int)right_pwm >> 8) & 0xff;	 	//高8位
			TxData[3] = 	(int)right_pwm & 0xff; 		//低8位
			CLEAR_BIT(TxData[0],3);
			SET_BIT(TxData[2],3);
		}
		else if(temp_k >= 1 && board_config.Rockers.Rocker_X != 2048){
			right_pwm=((4096 - CAN_Duty_cycle_min)*(4096-abs(board_config.Rockers.Rocker_Y-4096))/(4096 - 0)) + CAN_Duty_cycle_min;
			left_pwm=((4096 - CAN_Duty_cycle_min)*(4096-board_config.Rockers.Rocker_X)/(4096 - 0)) + CAN_Duty_cycle_min  ;
			if(left_pwm > 2000)left_pwm = 2000;
			if(left_pwm < 0)left_pwm = 0;
			if(right_pwm > 2000)right_pwm = 2000;
			if(right_pwm < 0)right_pwm = 0;
			TxData[0] = ((int)left_pwm >> 8) & 0xff;	 	//高8位
			TxData[1] = 	(int)left_pwm & 0xff; 		//低8位
			TxData[2] = ((int)right_pwm >> 8) & 0xff;	 	//高8位
			TxData[3] = 	(int)right_pwm & 0xff; 		//低8位
			SET_BIT(TxData[0],3);
			SET_BIT(TxData[2],3);
		}
		CAN_TX_Message(TxData, length);
		return ;
	 }
 }

 float cmu_k(float x,float y){
	 if(x == 2048){
		 return 0;
	 }
     return ((y-2048)/(x-2048));
 }

 uint8_t Auto_Sail(void){
	float now_angle = 0.0;//必须校准陀螺仪方向
	float Target_angle = 0.0;
	float board_angle = GO_STOP;
	float Target_lon = 22.304161;
	float Target_lat = 113.484184;
	float distant = 0.0;
	while(1){
		Target_angle = get_angle(Target_lon, Target_lat, atof(board_config.gps.lon), atof(board_config.gps.lat));
		board_angle = motor_contorl(Target_angle,board_config.motor_status.now_angle);
		distant = get_distance(Target_lon, Target_lat, atof(board_config.gps.lon), atof(board_config.gps.lat));
		if(board_angle == TURN_LEFT){
				Servo_Control(3,0,2048);
		}
		else if(board_angle == TURN_RIGHT){
				Servo_Control(3,4096,2048);
		}
		else if(board_angle == GO_STRIGHT){
				if(distant > 0.5){
					Servo_Control(2,2048,4096);
				}
				else if(distant <= 0.5 && distant > 0.0){
					Servo_Control(2,2048,(4096-(1.0-distant)*2048));
				}
		}
		printf("now_angle:%f\r\n",now_angle);
	}
	return 0;
 }




 double get_distance(double lat1, double lng1, double lat2, double lng2)
 {
 	double radLat1 = lat1 * PI / 180.0;   //角度1˚ = π / 180
 	double radLat2 = lat2 * PI / 180.0;   //角度1˚ = π / 180
 	double a = radLat1 - radLat2;//纬度之差
 	double b = lng1 * PI / 180.0 - lng2* PI / 180.0;  //经度之差
 	double dst = 2 * asin((sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2))));
 	dst = dst * EARTH_RADIUS;
 	dst = round(dst * 10000) / 10000;
 	return dst;
 }

 //计算角度
 float get_angle(double lat1, double lng1, double lat2, double lng2)
 {
 	double x = lat1 - lat2;//t d
 	double y = lng1 - lng2;//z y
 	float angle=-1;
 	if (y == 0 && x > 0) angle = 0;
 	if (y == 0 && x < 0) angle = 180;
 	if(x ==0 && y > 0) angle = 90;
 	if(x == 0 && y < 0) angle = 270;
 	if (angle == -1)
 	{
 		double dislat = get_distance(lat1, lng2, lat2, lng2);
 		double dislng = get_distance(lat2, lng1, lat2, lng2);
 		if (x > 0 && y > 0) angle = atan2(dislng, dislat) / PI * 180;
 		if (x < 0 && y > 0) angle = atan2(dislat, dislng) / PI * 180+90;
 		if (x < 0 && y < 0) angle = atan2(dislng, dislat) / PI * 180 + 180;
 		if (x > 0 && y < 0) angle = atan2(dislat, dislng) / PI * 180 + 270;
 	}
 	return angle;
 }

 int motor_contorl(int target_angle , int now_angle){
 	int FLAG_TARGET = flag_get(target_angle);
 	int FLAG_NOW = 	flag_get(now_angle);
 	if((target_angle - now_angle) > 0 && (target_angle - now_angle) < 180){
 		return TURN_RIGHT;
 	}
 	else if((target_angle - now_angle) == 0){
 		return GO_STRIGHT;
 	}
 	else{
 		return TURN_LEFT;
 	}
 	return 1;
 }

 int flag_get(int angle){
 	if(angle < 90){
 		return ONE;
 	}
 	else if(angle >= 90 && angle < 180){
 		return TWO;
 	}
 	else if(angle >= 180 && angle < 270){
 		return THREE;
 	}
 	else if(angle >= 270 && angle < 360){
 		return FOUR;
 	}
 	return 0;
 }

#define MID_VALUE 2048;
#define DEAD_ZONE 30  // 死区，用于过滤小的噪声
#define DUTY_CYCLE_MID 3000
#define DUTY_CYCLE_MIN 1800
#define DUTY_CYCLE_MAX 4200
 void Servo_Control_New(uint16_t X, uint16_t Y) {
//	 float duty_cycle_left, duty_cycle_right;
//	     int16_t x_deviation, y_deviation;
//
//	     // 计算偏差
//	     x_deviation = X - MID_VALUE;
//	     y_deviation = Y - MID_VALUE;
//
//	     // 死区处理
//	     if (abs(x_deviation) < DEAD_ZONE) x_deviation = 0;
//	     if (abs(y_deviation) < DEAD_ZONE) y_deviation = 0;
//
//	     // 计算基本速度
//	     float base_speed = DUTY_CYCLE_MID;
//
//	     if (x_deviation == 0 && y_deviation == 0) {
//	         // 摇杆在中间位置，停止所有动作
//	         duty_cycle_left = DUTY_CYCLE_MID;
//	         duty_cycle_right = DUTY_CYCLE_MID;
//	     } else {
//	         // 计算速度偏差
//	         float speed_deviation = (float)y_deviation / MID_VALUE * (DUTY_CYCLE_MAX - DUTY_CYCLE_MID);
//
//	         if (x_deviation >= 0 && y_deviation >= 0) {  // 第一象限
//	             duty_cycle_left = base_speed + speed_deviation - (float)x_deviation / MID_VALUE * (DUTY_CYCLE_MAX - DUTY_CYCLE_MID);
//	             duty_cycle_right = base_speed + speed_deviation + (float)x_deviation / MID_VALUE * (DUTY_CYCLE_MAX - DUTY_CYCLE_MID);
//	         } else if (x_deviation < 0 && y_deviation >= 0) {  // 第二象限
//	             duty_cycle_left = base_speed + speed_deviation + (float)x_deviation / MID_VALUE * (DUTY_CYCLE_MAX - DUTY_CYCLE_MID);
//	             duty_cycle_right = base_speed + speed_deviation - (float)x_deviation / MID_VALUE * (DUTY_CYCLE_MAX - DUTY_CYCLE_MID);
//	         } else if (x_deviation < 0 && y_deviation < 0) {  // 第三象限
//	             duty_cycle_left = base_speed - speed_deviation - (float)x_deviation / MID_VALUE * (DUTY_CYCLE_MAX - DUTY_CYCLE_MID);
//	             duty_cycle_right = base_speed - speed_deviation + (float)x_deviation / MID_VALUE * (DUTY_CYCLE_MAX - DUTY_CYCLE_MID);
//	         } else if (x_deviation >= 0 && y_deviation < 0) {  // 第四象限
//	             duty_cycle_left = base_speed - speed_deviation + (float)x_deviation / MID_VALUE * (DUTY_CYCLE_MAX - DUTY_CYCLE_MID);
//	             duty_cycle_right = base_speed - speed_deviation - (float)x_deviation / MID_VALUE * (DUTY_CYCLE_MAX - DUTY_CYCLE_MID);
//	         }
//
//	         // 限制 PWM 信号范围
//	         if (duty_cycle_left > DUTY_CYCLE_MAX) duty_cycle_left = DUTY_CYCLE_MAX;
//	         if (duty_cycle_left < DUTY_CYCLE_MIN) duty_cycle_left = DUTY_CYCLE_MIN;
//	         if (duty_cycle_right > DUTY_CYCLE_MAX) duty_cycle_right = DUTY_CYCLE_MAX;
//	         if (duty_cycle_right < DUTY_CYCLE_MIN) duty_cycle_right = DUTY_CYCLE_MIN;
//	     }
//
//	     // 将浮点数转换为整数
//	     uint32_t pwm_left = (uint32_t)duty_cycle_left;
//	     uint32_t pwm_right = (uint32_t)duty_cycle_right;
//
//	     // 设置 PWM 信号
//	     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm_left);
//	     __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, pwm_right);
	 return;
 }


