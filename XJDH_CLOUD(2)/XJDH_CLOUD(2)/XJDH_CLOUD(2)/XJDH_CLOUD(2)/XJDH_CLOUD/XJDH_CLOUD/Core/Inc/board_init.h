/*
 * board_init.h
 *
 *  Created on: Jul 9, 2024
 *      Author: Administrator
 */

#ifndef INC_BOARD_INIT_H_
#define INC_BOARD_INIT_H_

#define Duty_cycle_max 4200.0
#define Duty_cycle_zero 3000.0
#define Duty_cycle_min 1800.0
#define Online 1
#define Outline 0

/*DeviceImei = "2401234567",//imei号,10位字符
UseState=0,//使用状态,int 0未使用 1使用中
LockState = 0,//锁状态,int 0关锁 1开锁
Enabled = 0,//使能,int 0表示不能使用 1表示可以使用
MasterTemperature = 0,//float,主电池温度
MasterVoltage = 0,//float,主电池电压
MasterCurrent = 0, //float,主电池电流
SlaveTemperature = 0,//float,备电池温度
SlaveVoltage = 0,//float,备电池电压
SlaveCurrent = 0, //float,备电池电流
//摇杆
RockerPointX = 0.55,//double,摇杆X位置
RockerPointY = 0.33,//double,摇杆Y位置
Gear = 0,//摇杆档位int 0静止,1前进，2后退
Direction =0,//方向int 0静止,1左转，2右转
AcceleratorValue = 0,//int摇杆当前值
AcceleratorMax = 0,//int摇杆最大值
AcceleratorMin = 0,//int摇杆最小值
//左电机
MotorState1 = 0,//int电机状态 0表示正常 >0表示异常
MotorPower1 = 0,//float电机功率数据
MotorVoltage1 = 0,//float电机电压
MotorSpeed1 = 0,//float电机转速
MotorTemperature1 = 0,//float电机温度
//右电机
MotorState2 = 0,//int电机状态 0表示正常 >0表示异常
MotorPower2 = 0,//float电机功率数据
MotorVoltage2 = 0,//float电机电压
MotorSpeed2 = 0,//float电机转速
MotorTemperature2 = 0,//float电机温度
//GPS
GPSLongitude = 0,//double,经度
GPSLatitude = 0,//double,纬度
GPSSpeed = 0,//float,GSP速度
NoOfSatellites = 1,//int,接收卫星数量
Distance = 0,//float,距离
GPSState = 0,//int,GPS状态 0表示正常 >0表示异常
//保留位
ReserveInt1 = 0,//int,int保留位1
ReserveInt2 = 0,//int,int保留位2
ReserveStr1 = "",//string,string保留位1
ReserveStr2 = "",//string,string保留位2*/

typedef struct borad_gps{
	uint8_t lat[30];
	uint8_t lon[30];
	uint8_t Speed[30];

	double Target_gpslat[6];
	double Target_gpslon[6];
	uint8_t Target_gps[6][50];
	double lat_deg;
	double lon_deg;
	double Goal_Lat;
	double Goal_Lon;
	int gps_num;
	int now_number;
}Board_Gps;

typedef struct imu{
	float Ax;
	float Ay;
	float Az;
	float AT;
	float Wx;
	float Wy;
	float Wz;
	float Roll;
	double Pitch;
	float Yaw;
	float Imu_P;
	float Imu_H;
}imu_data;

typedef struct net{
	uint8_t Imei[30];
	uint8_t Lock_Status;
	uint8_t Use_Status;
	uint8_t Status;
	uint8_t Enable;
	int ReserveInt1;
}Net;

typedef struct rocker{
	uint16_t Rocker_X;
	uint16_t Rocker_Y;
}Rocker;

typedef struct motor{
	unsigned int MotorSpeed_L;  // 电机当前速度数值，从编码器中获取
	unsigned int MotorSpeed_R;  // 电机当前速度数值，从编码器中获取
	int MotorOutput_L;		  // 电机输出
	int MotorOutput_R;		  // 电机输出
	float Angle_L;
	float Angle_R;
	int Gear;
	int Direction;//方向int 0静止,1左转，2右转
	int AcceleratorValue;//int摇杆当前值
	int AcceleratorMax0;//int摇杆最大值
	int AcceleratorMin0;//int摇杆最小值
	uint16_t L_motor_speed;
	uint16_t L_motor_curr;
	uint8_t L_motor_vol;
	uint8_t L_motor_temp;
	uint8_t L_motor_status;
	uint8_t L_motor_fault;
	uint16_t R_motor_speed;
	uint16_t R_motor_curr;
	uint8_t R_motor_vol;
	uint8_t R_motor_temp;
	uint8_t R_motor_status;
	uint8_t R_motor_fault;
	float now_angle;
}Motor;

typedef struct battery{
	float Bat_V;
	float Bat_A;
	float MasterVoltage;
}BAT;

typedef struct board{
	Board_Gps gps;
	imu_data Imu_data;
	Net Net_4g;
	Motor motor_status;
	Rocker Rockers;
	BAT Battery;
}Board;

extern Board board_config;

#endif /* INC_BOARD_INIT_H_ */
