/*
 * Delay.c
 *
 *  Created on: Jun 15, 2024
 *      Author: Administrator
 */

#include "delay.h"

#define AHB_INPUT  72  //�밴RCC�����õ�AHBʱ��Ƶ����д�������λMHz��

void delay_us(uint32_t uS){ //uS΢�뼶��ʱ���򣨲ο�ֵ������ʱ����72MHzʱ���ֵ233015��
	SysTick->LOAD=AHB_INPUT*uS;      //��װ������ֵ������Ƶ��72MHz��72��Ϊ1΢�룩
	SysTick->VAL=0x00;        //��ն�ʱ���ļ�����
	SysTick->CTRL=0x00000005;//ʱ��ԴHCLK���򿪶�ʱ��
	while(!(SysTick->CTRL&0x00010000)); //�ȴ�������0
	SysTick->CTRL=0x00000004;//�رն�ʱ��
}

void delay_ms(uint16_t ms){ //mS���뼶��ʱ���򣨲ο�ֵ������ʱ�������ֵ65535��
	while( ms-- != 0){
		delay_us(1000);	//����1000΢�����ʱ
	}
}

void delay_s(uint16_t s){ //S�뼶��ʱ���򣨲ο�ֵ������ʱ�������ֵ65535��
	while( s-- != 0){
		delay_ms(1000);	//����1000�������ʱ
	}
}



