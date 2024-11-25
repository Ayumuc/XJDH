/*
 * wt1_imu.h
 *
 *  Created on: Jun 24, 2024
 *      Author: Administrator
 */

#ifndef SRC_WT1_IMU_H_
#define SRC_WT1_IMU_H_

#include "main.h"



int imu_collect(void);
int imu_decode(void);


extern uint8_t imu_buf[256];

#endif /* SRC_WT1_IMU_H_ */
