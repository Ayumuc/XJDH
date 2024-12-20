/*
 * Data_Collect.c
 *
 *  Created on: Jun 27, 2024
 *      Author: Administrator
 */

#include "Data_Collect.h"
#include "wt1_imu.h"
#include "main.h"
#include "can1.h"
#include "adc.h"
#include "Power_Control.h"
#include "gpio.h"
#include "Air820.h"
#include "board_init.h"
#include "tim.h"
#include "pid.h"
#include "usart.h"
#include "cJSON.h"

typedef struct {
    double pitch;
    double gps_lat;
    double gps_lon;
} PositionData;

int parse_position_data(const char *input, PositionData *data);

int Data_Collect(void){
	//通过can采集电调数据
	//can_collect();
	//通过串口采集陀螺仪数据
	//imu_collect();
	//通过adc采集电池数据
	//adc_collect();
	//通过rs485读取上位机数据
	gps_collect();
	//读取惯导数据
	//Inertial_Navigation_Collect();
	//读取镭神数据
	Laser_Navigation_Collect();
	return 1;
}

int KEY = 0;
int KEY_FLAG = 0;
int KEY_COUNT = 2;
int Key_Switch(void){
  KEY = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14);
  if(KEY == 0){
	KEY_FLAG = 1;
  }
  if(KEY_FLAG == 1 && KEY == 1){
	KEY_FLAG = 2;
	KEY_COUNT++;
  }
  //printf("%d\r\n",KEY_COUNT);
  if(KEY_COUNT%5 == 0){
 	  KEY_COUNT = 2;
  }
  if(KEY_COUNT%4 == 0){
  	  //printf("Auto\r\n");
   	  auto_flag = 1;
    }
  else if(KEY_COUNT%2 == 0){
	  Status_get_and_Powerset(Get_KeyState());
	  auto_flag = 0;
	  //printf("Rocker\r\n");
  }
  else if(KEY_COUNT%3 == 0){
	  Status_get_and_Powerset_Blue(Get_KeyState_Blue());
	  auto_flag = 0;
	  //printf("Blue\r\n");
  }
  return KEY_COUNT;
}

#define REF_VOLTAGE     1224UL        // 基准电压千倍
#define VOLTAGE         3.3        // 电压千倍
#define ADC_DIP(X)      (X)           // ADC分辨率

#define MAX_Voltage  54.2
#define MIN_Voltage  40.0

uint8_t adc_collect(void){
	float Voltage_value;
	int ADC_value = 0;
	HAL_ADC_Start(&hadc3);
    HAL_ADC_PollForConversion(&hadc3, 100);
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc3), HAL_ADC_STATE_REG_EOC))
    {
		 ADC_value = HAL_ADC_GetValue(&hadc3);
		 Voltage_value = (((ADC_value*VOLTAGE)/4096)*600/20) + 1.1;
		 board_config.Battery.MasterVoltage = Voltage_value;
		 if(Voltage_value < 40){
			 board_config.Net_4g.ReserveInt1 = 1;
		 }
		 else{
			 board_config.Net_4g.ReserveInt1 = ((Voltage_value - 40)/(MAX_Voltage-MIN_Voltage))*100;
		 }

#if PRINTF
		 printf("adc_v: %d   V:%f  \r\n",ADC_value,Voltage_value);
#endif
    }
	return 1;
}

uint16_t gps_collect(void){
	if (rxu4_data_ready == 1) {
	  // 处理接收到的数据
	  gps_decode();
	  // 清空接收缓冲区
	  memset(rxu4_buffer, 0, sizeof(rxu4_buffer));
	  rxu4_data_ready = 0;
	  // 重新启动DMA接收
	  HAL_UART_Receive_DMA(&huart4, (uint8_t *)rxu4_buffer, RXU4_BUFFER_SIZE);
    }
	return 1;
}

// 航点结构体
typedef struct {
    float longitude;
    float latitude;
} Waypoint;

typedef struct {
	float Min_in;
	float Max_in;
	int gps_num;
}Head_in;

// 航点数组
#define NUM_WAYPOINTS 6
Waypoint waypointss[NUM_WAYPOINTS];
Head_in head_in;
void gps_decode(void){
	printf("%s\r\n",rxu4_buffer);
	if(strstr(rxu4_buffer,"Kp")!=NULL && strstr(rxu4_buffer,"Ki")!=NULL && strstr(rxu4_buffer,"Kd")!=NULL && strstr(rxu4_buffer,"Lon1")!=NULL){
		printf("\r\n");
		char *token = strtok((char *)rxu4_buffer, ",");
		while (token != NULL) {
			if (strstr(token, "Kp=")) {
				sscanf(token, "Kp=%f", &heading_pid.Kp);
			} else if (strstr(token, "Ki=")) {
				sscanf(token, "Ki=%f", &heading_pid.Ki);
			} else if (strstr(token, "Kd=")) {
				sscanf(token, "Kd=%f", &heading_pid.Kd);
			}else if (strstr(token, "Min=")) {
				sscanf(token, "Min=%f", &head_in.Min_in);
			}else if (strstr(token, "Max=")) {
				sscanf(token, "Max=%f", &head_in.Max_in);
			}else if (strstr(token, "Num=")) {
				sscanf(token, "Num=%d", &head_in.gps_num);
			}else if (strstr(token, "Lon")) {
				int index;
				float value;
				if (sscanf(token, "Lon%d=%f", &index, &value) == 2 && index > 0 && index <= NUM_WAYPOINTS) {
					waypointss[index - 1].longitude = value;
				}
			} else if (strstr(token, "Lat")) {
				int index;
				float value;
				if (sscanf(token, "Lat%d=%f", &index, &value) == 2 && index > 0 && index <= NUM_WAYPOINTS) {
					waypointss[index - 1].latitude = value;
				}
			}
			token = strtok(NULL, ",");
		}
		Auto_Start_Flag = AUTO_START;
		//PID_Init(&heading_pid, heading_pid.Kp, heading_pid.Ki, heading_pid.Kd, 0.0f, head_in.Min_in, head_in.Max_in);
		board_config.gps.gps_num = head_in.gps_num;
		printf("P:%f I:%f D:%f Min:%f Max:%f Num:%d\r\n",heading_pid.Kp, heading_pid.Ki, heading_pid.Kd,head_in.Min_in, head_in.Max_in,board_config.gps.gps_num);
		for(int i = 0 ; i < 6 ; i++){
			board_config.gps.Target_gpslat[i] = waypointss[i].latitude;
			board_config.gps.Target_gpslon[i] = waypointss[i].longitude;
//			sprintf(board_config.gps.Target_gpslat[i],"%f",waypointss[i].latitude);
//			sprintf(board_config.gps.Target_gpslon[i],"%f",waypointss[i].longitude);
			printf("lat:%f   lon:%f\r\n",board_config.gps.Target_gpslat[i],board_config.gps.Target_gpslon[i]);
		}
		printf("\r\n");
		uint8_t Laser_Buf[512] = {0};
		sprintf(Laser_Buf,"{\"Goal1\":[{\"Lat\":%.7f,\"Lng\":%.7f}],\"Goal2\":[{\"Lat\":%.7f,\"Lng\":%.7f}],\"Goal3\":[{\"Lat\":%.7f,\"Lng\":%.7f}],\"Goal4\":[{\"Lat\":%.7f,\"Lng\":%.7f}],\"Goal5\":[{\"Lat\":%.7f,\"Lng\":%.7f}],\"Goal6\":[{\"Lat\":%.7f,\"Lng\":%.7f}]}",\
				board_config.gps.Target_gpslat[0],board_config.gps.Target_gpslon[0],board_config.gps.Target_gpslat[1],board_config.gps.Target_gpslon[1],board_config.gps.Target_gpslat[2],board_config.gps.Target_gpslat[2],\
				board_config.gps.Target_gpslat[3],board_config.gps.Target_gpslon[3],board_config.gps.Target_gpslat[4],board_config.gps.Target_gpslon[4],board_config.gps.Target_gpslat[5],board_config.gps.Target_gpslon[5]);
		HAL_UART_Transmit(&huart2,Laser_Buf,strlen(Laser_Buf),0xff);
	}
	else if(strstr(rxu4_buffer,"Pitch")!=NULL && strstr(rxu4_buffer,"Gps_Lat")!=NULL && strstr(rxu4_buffer,"Gps_Lon")!=NULL){
		PositionData data;
		if (parse_position_data(rxu4_buffer, &data) == 0) {
			// 输出解析后的数据
			printf("Pitch: %.6f", data.pitch);
			printf("  GPS Latitude: %.6f", data.gps_lat);
			printf("  GPS Longitude: %.6f\r\n", data.gps_lon);
			board_config.Imu_data.Pitch = data.pitch;
			board_config.gps.lat_deg = data.gps_lat;
			board_config.gps.lon_deg = data.gps_lon;
		} else {
			printf("Failed to parse the input data.\n");
		}
	}
	else if(strstr(rxu4_buffer,"AutomaticReturn_Start")!=NULL){
		double gps_temp = 0.0;
		for(int i = 0 ; i < (board_config.gps.gps_num / 2) ; i++){
			gps_temp = board_config.gps.Target_gpslat[board_config.gps.gps_num - 1 - i];
			board_config.gps.Target_gpslat[board_config.gps.gps_num - 1 - i] = board_config.gps.Target_gpslat[i];
			board_config.gps.Target_gpslat[i] = gps_temp;
		}
		for(int i = 0 ; i < 6 ; i++){
			printf("lat:%f   lon:%f\r\n",board_config.gps.Target_gpslat[i],board_config.gps.Target_gpslon[i]);
		}
		uint8_t Laser_Buf[512] = {0};
		sprintf(Laser_Buf,"{\"Goal1\":[{\"Lat\":%.7f,\"Lng\":%.7f}],\"Goal2\":[{\"Lat\":%.7f,\"Lng\":%.7f}],\"Goal3\":[{\"Lat\":%.7f,\"Lng\":%.7f}],\"Goal4\":[{\"Lat\":%.7f,\"Lng\":%.7f}],\"Goal5\":[{\"Lat\":%.7f,\"Lng\":%.7f}],\"Goal6\":[{\"Lat\":%.7f,\"Lng\":%.7f}]}",\
				board_config.gps.Target_gpslat[0],board_config.gps.Target_gpslon[0],board_config.gps.Target_gpslat[1],board_config.gps.Target_gpslon[1],board_config.gps.Target_gpslat[2],board_config.gps.Target_gpslat[2],\
				board_config.gps.Target_gpslat[3],board_config.gps.Target_gpslon[3],board_config.gps.Target_gpslat[4],board_config.gps.Target_gpslon[4],board_config.gps.Target_gpslat[5],board_config.gps.Target_gpslon[5]);
		HAL_UART_Transmit(&huart2,Laser_Buf,strlen(Laser_Buf),0xff);
		Auto_Start_Flag = AUTO_START;
		Auto_Status = Auto_Return;
	}
	else if(strstr(rxu4_buffer,"Stop_Auto")!=NULL){
		board_config.gps.now_number = 0.0;
		Auto_Start_Flag = AUTO_STOP;
	}
	return ;
}

/*{"curPt":[{"Alt":91.4,"Lat":221280068.0,"Lng":1133170163.0,"Yaw":-1488.64744}],"goalPt":[{"Alt":91.4,"Lat":221280068.0,"Lng":1133170163.0,"Yaw":-1488.6474609375}]}*/

int parse_position_data(const char *input, PositionData *data) {
    // 定义变量用于存储解析结果
    char pitch_str[32] = {0};
    char lat_str[32] = {0};
    char lon_str[32] = {0};

    // 使用sscanf函数解析输入字符串
    if (sscanf(input, "Pitch=%31[^,],Gps_Lat=%31[^,],Gps_Lon=%31[^\n]",
               pitch_str, lat_str, lon_str) != 3) {
        // 如果解析失败，返回错误码
        return -1;
    }

    // 将字符串转换为double类型
    data->pitch = atof(pitch_str);
    data->gps_lat = atof(lat_str);
    data->gps_lon = atof(lon_str);

    return 0; // 解析成功
}

/*"Pitch:230.123456,Gps_Lat:113.123456,Gps_Lon::22.123456"*/

int pid_flag = 0;
uint16_t Inertial_Navigation_Collect(void){
	HAL_UART_Receive_DMA(&huart3,imu_buf,sizeof(imu_buf));
//	printf("\r\n");
//	printf("%02x",imu_buf[0]);
//	for(int i = 1 ; i < sizeof(imu_buf) ; i++){
//		printf(" %02x",imu_buf[i]);
//	}
//	printf("\r\n");
	for(int i = 0 ; i < (sizeof(imu_buf) - 30) ; i++){
		if(imu_buf[i] == 0xBD && imu_buf[i+1] == 0xDB && imu_buf[i+2] == 0x0B){
//			printf("\r\n");
//			for(int j = i ; j < i+30 ; j++){
//				printf(" %02x",imu_buf[j]);
//			}
//			printf("\r\n");
//			if(imu_buf[i] == 0x00){
//				return 1;
//			}
		    // 解析航向角
		    int16_t heading = bytesToShort(imu_buf[i+8], imu_buf[i+7] << 8);
		    double headicng_angle = (heading * 360.0) / 32768.0;

		    // 纬度
			int32_t latitude = bytesToLong(imu_buf[i+26], imu_buf[i+25], imu_buf[i+24], imu_buf[i+23]);
			double latitude_deg = (double)latitude * 1.0e-7;

			// 经度
			int32_t longitude = bytesToLong(imu_buf[i+30], imu_buf[i+29], imu_buf[i+28], imu_buf[i+27]);
			double longitude_deg = (double)longitude * 1.0e-7;

			//天向速度


			if(latitude_deg < 20.0){
				return 1;
			}
			else if(latitude_deg > 25){
				return 1;
			}
			if(longitude_deg < 110.0){
				return 1;
			}
			else if(longitude_deg > 120){
				return 1;
			}
//
//			printf("\r\n");
//
//			printf("heading_deg:%.7f latitude_deg:%.7f longitude_deg:%.7f\r\n",headicng_angle,latitude_deg,longitude_deg);
//
//			printf("\r\n");

			board_config.Imu_data.Pitch = -headicng_angle;
			board_config.gps.lat_deg = latitude_deg;
			board_config.gps.lon_deg = longitude_deg;

			pid_flag++;
			if(pid_flag >= 200){
				char PitchMessage[256];
				sprintf(PitchMessage,"Pitch=%f,Gps_Lat=%f,Gps_Lon=%f",headicng_angle,latitude_deg,longitude_deg);
				HAL_UART_Transmit(&huart4,PitchMessage,strlen(PitchMessage),0xff);
				pid_flag = 0;
			}

			return 1;
		}
	}
	return 1;
}


uint16_t Laser_Navigation_Collect(void){
	if (rxu2_data_ready == 1) {
		  //printf("recv:%s\r\n",imu_buf);
		  // 处理接收到的数据
		  ParseLaserJSON(imu_buf);
		  // 清空接收缓冲区
		  memset(imu_buf, 0, sizeof(imu_buf));

		  rxu2_data_ready = 0;
		  // 重新启动DMA接收
		  HAL_UART_Receive_DMA(&huart2, (uint8_t *)imu_buf, imu_buf);

		  HAL_UART_Receive_IT(&huart2, (uint8_t *)imu_buf, sizeof(imu_buf));
	    }
		return 1;

	return 1;
}


void ParseLaserJSON(const char *jsonString)
{
	 cJSON *root = cJSON_Parse(jsonString);
	if (root == NULL)
	{
		const char *error_ptr = cJSON_GetErrorPtr();
		if (error_ptr != NULL)
		{
			printf("Error before: %s\r\n", error_ptr);
		}
		return;
	}

	// 获取curPt数组
	cJSON *curPtArray = cJSON_GetObjectItemCaseSensitive(root, "curPt");
	if (cJSON_IsArray(curPtArray) && cJSON_GetArraySize(curPtArray) > 0)
	{
		cJSON *curPt = cJSON_GetArrayItem(curPtArray, 0);
		if (cJSON_IsObject(curPt))
		{
			board_config.gps.lat_deg = (cJSON_GetObjectItemCaseSensitive(curPt, "Lat")->valuedouble)/10000000;
			board_config.gps.lon_deg = (cJSON_GetObjectItemCaseSensitive(curPt, "Lng")->valuedouble)/10000000;;
			//double curAlt = (cJSON_GetObjectItemCaseSensitive(curPt, "Alt")->valuedouble)/100;;
			board_config.Imu_data.Pitch = -((cJSON_GetObjectItemCaseSensitive(curPt, "Yaw")->valuedouble)/100);

			printf("Current Point - Lat: %.6f, Lng: %.6f, Yaw: %.6f\n", board_config.gps.lat_deg , board_config.gps.lon_deg, board_config.Imu_data.Pitch);
		}
	}

	// 获取goalPt数组
	cJSON *goalPtArray = cJSON_GetObjectItemCaseSensitive(root, "goalPt");
	if (cJSON_IsArray(goalPtArray) && cJSON_GetArraySize(goalPtArray) > 0)
	{
		cJSON *goalPt = cJSON_GetArrayItem(goalPtArray, 0);
		if (cJSON_IsObject(goalPt))
		{
			board_config.gps.Goal_Lat = (cJSON_GetObjectItemCaseSensitive(goalPt, "Lat")->valuedouble)/10000000;
			board_config.gps.Goal_Lon = (cJSON_GetObjectItemCaseSensitive(goalPt, "Lng")->valuedouble)/10000000;
//			double goalAlt = (cJSON_GetObjectItemCaseSensitive(goalPt, "Alt")->valuedouble)/100;
//			double goalYaw = (cJSON_GetObjectItemCaseSensitive(goalPt, "Yaw")->valuedouble)/100;

			printf("       Goal Point - Lat: %.6f, Lng: %.6f\r\n", board_config.gps.Goal_Lat, board_config.gps.Goal_Lon);
		}
	}

	pid_flag++;
	if(pid_flag >= 15){
		char PitchMessage[256];
		sprintf(PitchMessage,"Pitch=%f,Gps_Lat=%f,Gps_Lon=%f",board_config.Imu_data.Pitch,board_config.gps.lat_deg,board_config.gps.lon_deg);
		HAL_UART_Transmit(&huart4,PitchMessage,strlen(PitchMessage),0xff);
		pid_flag = 0;
	}

	// 清理
	cJSON_Delete(root);
}


// 将两个字节转换为有符号短整型
int16_t bytesToShort(uint8_t highByte, uint8_t lowByte) {
    int16_t value = (highByte << 8) | lowByte;
    return value;
}

// 将四个字节转换为有符号长整型
int32_t bytesToLong(uint8_t byte3, uint8_t byte2, uint8_t byte1, uint8_t byte0) {
    int32_t value = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
    return value;
}





