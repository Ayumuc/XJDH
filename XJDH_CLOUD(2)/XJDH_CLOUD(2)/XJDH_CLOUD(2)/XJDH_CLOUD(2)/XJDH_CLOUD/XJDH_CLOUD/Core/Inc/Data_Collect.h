/*
 * Data_Collect.h
 *
 *  Created on: Jun 27, 2024
 *      Author: Administrator
 */

#ifndef INC_DATA_COLLECT_H_
#define INC_DATA_COLLECT_H_

#include "main.h"

int Data_Collect(void);
uint8_t adc_collect(void);
int Key_Switch(void);
uint16_t gps_collect(void);
void gps_decode(void);
uint16_t Inertial_Navigation_Collect(void);
int16_t bytesToShort(uint8_t highByte, uint8_t lowByte);
int32_t bytesToLong(uint8_t byte3, uint8_t byte2, uint8_t byte1, uint8_t byte0);
uint16_t Laser_Navigation_Collect(void);
void ParseLaserJSON(const char *jsonString);


#define RXU4_BUFFER_SIZE 256
extern uint8_t rxu4_buffer[RXU4_BUFFER_SIZE];
extern uint8_t rxu4_data_ready;
extern uint8_t Auto_Start_Flag;
extern uint8_t rxu2_data_ready;

#define KEY_AUTO 1
#define KEY_ROCKER 2
#define KEY_BLUE 3
#define KEY_NET 4
#define AUTO_START 1
#define AUTO_STOP 2



#endif /* INC_DATA_COLLECT_H_ */
