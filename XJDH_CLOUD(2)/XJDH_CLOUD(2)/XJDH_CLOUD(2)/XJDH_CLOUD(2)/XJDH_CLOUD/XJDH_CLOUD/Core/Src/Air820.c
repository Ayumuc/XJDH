#include "Air820.h"
#include "string.h"
#include "stdlib.h"
#include "board_init.h"
#include "cJSON.h"
#include "pid.h"
#include "Data_Collect.h"
#include "usart.h"

#define CGNSPWR 1
#define CGNSSEQ 2
#define CPIN 3
#define CGSN 4
#define CGATT 5
#define SAPBR_CON 6
#define SAPBR_APN 7
#define MCONFIG 8
#define MIPSTART 9
#define MCONNECT 10
#define SUB 11
#define PUB 12

typedef struct {
    float longitude;
    float latitude;
} Waypoint;

#define NUM_WAYPOINTS 6
Waypoint waypoints[NUM_WAYPOINTS];



int Air820_init(void){
	static int flag = 0;
	static int mqtinit_flag = 1;
	static int htpinit_flag = 1;
	static int http_flag = 0;
	flag++;
	if(board_config.Net_4g.Status == Outline && flag >= 500 && http_flag == 0){
		Mode_http(htpinit_flag);
		flag = 0;
		if(htpinit_flag <= 13){
			htpinit_flag++;
		}
		else{
			http_flag = 1;
		}
	}
	if(board_config.Net_4g.Status == Outline && flag >= 500 && http_flag == 1){
		Mode_mqtt(mqtinit_flag);
		flag = 0;
		if(mqtinit_flag <= 12){
			mqtinit_flag++;
		}
		else{
			board_config.Net_4g.Status = Online;
			HAL_UART_Transmit(&huart4,"GET_4GDATA\r\n",strlen("GET_4GDATA\r\n"),0xff);
			//HAL_TIM_Base_Start_IT(&htim2);
		}
	}
	else if(flag == 250 && board_config.Net_4g.Status == Online){
		gps_collect();
		adc_collect();
		//HAL_UART_Receive_IT(&huart4, (uint8_t *)rxu4_buffer, RXU4_BUFFER_SIZE);
	}
	else if(flag == 400 && board_config.Net_4g.Status == Online && board_config.Net_4g.Lock_Status == 0){
		Data_Update();
	}
	else if(flag == 400 && board_config.Net_4g.Status == Online && board_config.Net_4g.Lock_Status == 1){
		Data_Update();
	}
	else if(flag == 450 && board_config.Net_4g.Status == Online && board_config.Net_4g.Lock_Status == 1){
		Data_Update();
	}
	else if(flag >= 500 && board_config.Net_4g.Status == Online){
		flag = 0;
	}
	return 1;
}

int Mode_mqtt(int step){
	switch(step){
	case CGNSPWR:
		Air820_uart_send("AT+CGNSPWR=1\r\n",sizeof("AT+CGNSPWR=1\r\n"),"OK");HAL_Delay(3);
		Air820_uart_send("AT+CGNSAID=31,1,1,1\r\n",sizeof("AT+CGNSAID=31,1,1,1\r\n"),"OK");break;
	case CGNSSEQ:
		Air820_uart_send("AT+CGNSPWR=1\r\n",sizeof("AT+CGNSPWR=1\r\n"),"OK");HAL_Delay(3);
		Air820_uart_send("AT+CGNSAID=31,1,1,1\r\n",sizeof("AT+CGNSAID=31,1,1,1\r\n"),"OK");HAL_Delay(3);
		Air820_uart_send("AT+CGNSSEQ=\"RMC\"\r\n",sizeof("AT+CGNSSEQ=\"RMC\"\r\n"),"OK");break;
	case CPIN:
		Air820_uart_send("AT+CPIN?\r\n",sizeof("AT+CPIN?\r\n"),"OK");break;
	case CGSN:
		Air820_uart_send("AT+CGSN\r\n",sizeof("AT+CGSN\r\n"),"OK");break;
	case CGATT:
		Air820_uart_send("AT+CGATT?\r\n",sizeof("AT+CGATT?\r\n"),"OK");break;
	case SAPBR_CON:
		Air820_uart_send("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n",sizeof("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n"),"OK");break;
	case SAPBR_APN:
		Air820_uart_send("AT+SAPBR=3,1,\"APN\",\"\"\r\n",sizeof("AT+SAPBR=3,1,\"APN\",\"\"\r\n"),"OK");break;
	case MCONFIG:
		char CONFIG_BUF[200] = "";
		sprintf(CONFIG_BUF,"AT+MCONFIG=\"%s\",\"test2\",\"test2\"\r\n",board_config.Net_4g.Imei);
		HAL_UART_Transmit(&huart3,CONFIG_BUF,strlen(CONFIG_BUF),0xff);break;
	case MIPSTART:
		Air820_uart_send("AT+MIPSTART=\"47.113.196.79\",\"1883\"\r\n",sizeof("AT+MIPSTART=\"47.113.196.79\",\"1883\"\r\n"),"OK");break;
	case MCONNECT:
		Air820_uart_send("AT+MCONNECT=1,60\r\n",sizeof("AT+MCONNECT=1,60\r\n"),"OK");break;
	case SUB:
		char MSUB_BUF[200] = "";
		sprintf(MSUB_BUF,"AT+MSUB=\"/device/%s\",0\r\n",board_config.Net_4g.Imei);
		HAL_UART_Transmit(&huart3,MSUB_BUF,strlen(MSUB_BUF),0xff);break;
	case PUB:
		char mqtt_firstencode[4096] = {0};
		sprintf(mqtt_firstencode,"AT+MPUB=\"/admin/%s\",0,0,\"{\\22DeviceImei\\22:\\22%s\\22,\
																			   \\22UseState\\22:1,\
																			   \\22LockState\\22:%d,\
																			   \\22Enabled\\22:1,\
																			   \\22MasterTemperature\\22:0,\
																			   \\22MasterVoltage\\22:%f,\
																			   \\22MasterCurrent\\22:0,\
																			   \\22SlaveTemperature\\22:0,\
																			   \\22SlaveVoltage\\22:0,\
																			   \\22SlaveCurrent\\22:0,\
																			   \\22RockerPointX\\22:\\22%d\\22,\
																			   \\22RockerPointY\\22:\\22%d\\22,\
																			   \\22Gear\\22:0,\
																			   \\22Direction\\22:0,\
																			   \\22AcceleratorValue\\22:0,\
																			   \\22AcceleratorMax\\22:4096,\
																			   \\22AcceleratorMin\\22:0,\
																			   \\22MotorState1\\22:0,\
																			   \\22MotorPower1\\22:0,\
																			   \\22MotorVoltage1\\22:0,\
																			   \\22MotorSpeed1\\22:0,\
																			   \\22MotorTemperature1\\22:0,\
																			   \\22MotorState2\\22:0,\
																			   \\22MotorPower2\\22:0,\
																			   \\22MotorVoltage2\\22:0,\
																			   \\22MotorSpeed2\\22:0,\
																			   \\22MotorTemperature2\\22:0,\
																			   \\22GPSLongitude\\22:%f,\
																			   \\22GPSLatitude\\22:%f,\
																			   \\22GPSSpeed\\22:0,\
																			   \\22NoOfSatellites\\22:0,\
																			   \\22Distance\\22:0,\
																			   \\22GPSState\\22:0,\
																			   \\22DrivingState\\22:1,\
																			   \\22DrivingMode\\22:0,\
																			   \\22AutomaticReturn\\22:1,\
																			   \\22DrivingSpeed\\22:1.53,\
																			   \\22DrivingAngle\\22:%f,\
																			   \\22ReserveStr1\\22:\\220\\22,\
																			   \\22ReserveInt1\\22:%d}\"\r\n",board_config.Net_4g.Imei,board_config.Net_4g.Imei,board_config.Net_4g.Lock_Status,board_config.Battery.MasterVoltage,\
																			   	   	   	   	   	   	   	  board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y,board_config.gps.lon_deg,board_config.gps.lat_deg,board_config.Imu_data.Pitch,board_config.Net_4g.ReserveInt1);
		HAL_UART_Transmit(&huart3,mqtt_firstencode,strlen(mqtt_firstencode),0xff);
		break;
	default:break;
	}
//	Air820_uart_send("AT+SAPBR=1,1\r\n",sizeof("AT+SAPBR=1,1\r\n"),"OK");
//	HAL_Delay(3000);
	return 1;

}

int Mode_http(int step){
	char htp_sendbuf[100] = {0};
	//检测sim卡没问题
	switch(step){
	case 1:Air820_uart_send("AT+CGSN\r\n",sizeof("AT+CGSN\r\n"),"OK");HAL_Delay(5);Air820_uart_send("AT+CGSN\r\n",sizeof("AT+CGSN\r\n"),"OK");break;
	case 2:Air820_uart_send("AT+CGATT?\r\n",sizeof("AT+CGATT?\r\n"),"OK");break;
	case 3:Air820_uart_send("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n",sizeof("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n"),"OK");break;
	case 4:Air820_uart_send("AT+SAPBR=3,1,\"APN\",\"\"\r\n",sizeof("AT+SAPBR=3,1,\"APN\",\"\"\r\n"),"OK");break;
	case 5:Air820_uart_send("AT+SAPBR=1,1\r\n",sizeof("AT+SAPBR=1,1\r\n"),"OK");break;
	case 6:Air820_uart_send("AT+SAPBR=2,1\r\n",sizeof("AT+SAPBR=2,1\r\n"),"OK");break;
	case 7:Air820_uart_send("AT+HTTPINIT\r\n",sizeof("AT+HTTPINIT\r\n"),"OK");break;
	case 8:Air820_uart_send("AT+HTTPPARA=\"CID\",1\r\n",sizeof("AT+HTTPPARA=\"CID\",1\r\n"),"OK");break;
	case 9:Air820_uart_send("AT+HTTPPARA=\"URL\",\"http://47.113.196.79:81/front/device/info\"\r\n",sizeof("AT+HTTPPARA=\"URL\",\"http://47.113.196.79:81/front/device/info\"\r\n"),"OK");break;
	case 10:Air820_uart_send("AT+HTTPACTION=0\r\n",sizeof("AT+HTTPACTION=0\r\n"),"OK");break;
	case 11:Air820_uart_send("AT+HTTPDATA=32,100000\r\n",sizeof("AT+HTTPDATA=32,100000\r\n"),"");break;
	HAL_Delay(5);
	sprintf(htp_sendbuf,"{\"DeviceImei\":\"%s\"}",board_config.Net_4g.Imei);
	case 12:Air820_uart_send(htp_sendbuf,strlen(htp_sendbuf),"OK");break;
	case 13:Air820_uart_send("AT+HTTPTERM\r\n",sizeof("AT+HTTPTERM\r\n"),"OK");break;
	default:break;
	}
	return 1;
}


int Air820_uart_send(char* tx_buf, uint16_t len ,char* rx_buf){
	HAL_UART_Transmit(&huart3, (uint8_t*)tx_buf, len,1000);
	while(1){
		if(Uart_4g_End == 1){
			if(strstr((char*)RECV_4G_BUFF,"CGNSPWR") != NULL){
				break;
			}
			if(strstr((char*)RECV_4G_BUFF,rx_buf) != NULL && Uart_4g_End == 1){
				Uart_4g_End = 0;
				//HAL_IWDG_Refresh(&hiwdg);
				//bzero(RECV_4G_BUFF,sizeof(RECV_4G_BUFF));
				break;
			}
			else if(strstr((char*)RECV_4G_BUFF,rx_buf) == NULL && Uart_4g_End == 1){
				//HAL_UART_Transmit(&huart3, (uint8_t*)tx_buf, len,1000);
				//HAL_IWDG_Refresh(&hiwdg);
				HAL_Delay(1000);
				break;
			}
		}
		HAL_Delay(20);
	}
	return 1;
}
uint8_t gps_sendb_buf[50];
uint8_t gps_send_endbuf[50];
uint8_t Air_820ug_rxdecode(void){
	if (rc4g_flag == 1) {
	  printf("%s\r\n",RECV_4G_BUFF);
	  // 处理接收到的数据
      decode_4G();
	  // 清空接收缓冲区
	  memset(RECV_4G_BUFF, 0, sizeof(RECV_4G_BUFF));
	  rc4g_flag = 0;
	  // 重新启动DMA接收
	  HAL_UART_Receive_DMA(&huart3,RECV_4G_BUFF,sizeof(RECV_4G_BUFF));
	}
	return 1;
}

int Data_Update(void){
	//update到mqtt服务器
	char mqtt_firstencode[4096] = {0};
	memset(RECV_4G_BUFF,0,sizeof(RECV_4G_BUFF));
	sprintf(mqtt_firstencode,"AT+MPUB=\"/admin/%s\",0,0,\"{\\22DeviceImei\\22:\\22%s\\22,\
																		   \\22UseState\\22:1,\
																		   \\22LockState\\22:%d,\
																		   \\22Enabled\\22:1,\
																		   \\22MasterTemperature\\22:0,\
																		   \\22MasterVoltage\\22:%f,\
																		   \\22MasterCurrent\\22:0,\
																		   \\22SlaveTemperature\\22:0,\
																		   \\22SlaveVoltage\\22:0,\
																		   \\22SlaveCurrent\\22:0,\
																		   \\22RockerPointX\\22:\\22%d\\22,\
																		   \\22RockerPointY\\22:\\22%d\\22,\
																		   \\22Gear\\22:0,\
																		   \\22Direction\\22:0,\
																		   \\22AcceleratorValue\\22:0,\
																		   \\22AcceleratorMax\\22:4096,\
																		   \\22AcceleratorMin\\22:0,\
																		   \\22MotorState1\\22:0,\
																		   \\22MotorPower1\\22:0,\
																		   \\22MotorVoltage1\\22:0,\
																		   \\22MotorSpeed1\\22:0,\
																		   \\22MotorTemperature1\\22:0,\
																		   \\22MotorState2\\22:0,\
																		   \\22MotorPower2\\22:0,\
																		   \\22MotorVoltage2\\22:0,\
																		   \\22MotorSpeed2\\22:0,\
																		   \\22MotorTemperature2\\22:0,\
																		   \\22GPSLongitude\\22:%f,\
																		   \\22GPSLatitude\\22:%f,\
																		   \\22GPSSpeed\\22:0,\
																		   \\22NoOfSatellites\\22:0,\
																		   \\22Distance\\22:0,\
																		   \\22GPSState\\22:0,\
																		   \\22DrivingState\\22:1,\
																		   \\22DrivingMode\\22:0,\
																		   \\22AutomaticReturn\\22:1,\
																		   \\22DrivingSpeed\\22:1.53,\
																		   \\22DrivingAngle\\22:%f,\
																		   \\22ReserveStr1\\22:\\220\\22,\
																		   \\22ReserveInt1\\22:%d}\"\r\n",board_config.Net_4g.Imei,board_config.Net_4g.Imei,board_config.Net_4g.Lock_Status,board_config.Battery.MasterVoltage,\
																		   	   	   	   	   	   	   	  board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y,board_config.gps.lon_deg,board_config.gps.lat_deg,board_config.Imu_data.Pitch,board_config.Net_4g.ReserveInt1);
	HAL_UART_Transmit(&huart3,mqtt_firstencode,strlen(mqtt_firstencode),0xff);
	return 1;
}

void board_init(void){
  strcpy(board_config.Net_4g.Imei,"0");
  board_config.Net_4g.Use_Status = 0;
  board_config.Net_4g.Lock_Status = 0;
  board_config.Net_4g.Enable = 0;
  board_config.Battery.MasterVoltage = 0 ;
  board_config.Rockers.Rocker_X = 0;
  board_config.Rockers.Rocker_Y=0;
  board_config.motor_status.Gear=0;
  board_config.motor_status.Direction=0;
  board_config.motor_status.L_motor_status = 0;
  board_config.motor_status.L_motor_curr=0;
  board_config.motor_status.R_motor_curr = 0;
  board_config.motor_status.L_motor_vol=0;
  board_config.motor_status.L_motor_speed=0;
  board_config.motor_status.L_motor_temp=0;
  board_config.motor_status.R_motor_status=0;
  board_config.motor_status.R_motor_vol=0;
  board_config.motor_status.R_motor_speed=0;
  board_config.motor_status.R_motor_speed=0;
  strcpy(board_config.gps.lon,"0");
  strcpy(board_config.gps.lat,"0");
  return;
}

char* extract_json_data(const char *input) {
    const char *start = strstr(input, "{");
    if (start == NULL) {
        return NULL;
    }
    const char *end = strchr(start, '}');
    if (end == NULL) {
        return NULL;
    }
    size_t length = end - start + 1;
    char *json_data = (char *)malloc(length + 1);
    if (json_data == NULL) {
        return NULL;
    }
    strncpy(json_data, start, length);
    json_data[length] = '\0';
    return json_data;
}

void parse_json_and_update(const char *json_data) {
    cJSON *root = cJSON_Parse(json_data);
    if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return;
    }

    // 获取CmdArgs字段
    cJSON *cmd_args = cJSON_GetObjectItemCaseSensitive(root, "CmdArgs");
    if (cJSON_IsString(cmd_args) && cmd_args->valuestring != NULL) {
        const char *args_str = cmd_args->valuestring;
        parse_cmd_args(args_str);
    }

    cJSON_Delete(root);
    return;
}


int num_waypoints;
void parse_cmd_args(const char *args_str) {
	for(int i = 0 ; i < 6 ; i++){
		waypoints[i].longitude = 0.0;
		waypoints[i].latitude = 0.0;
	}
    char *token = strtok((char *)args_str, ";");
    while (token != NULL) {
        if (strstr(token, "PStr=")) {
            sscanf(token, "PStr=%f", &heading_pid.Kp);
        } else if (strstr(token, "IStr=")) {
            sscanf(token, "IStr=%f", &heading_pid.Ki);
        } else if (strstr(token, "DStr=")) {
            sscanf(token, "DStr=%f", &heading_pid.Kd);
        } else if (strstr(token, "MinStr=")){
        	sscanf(token, "MinStr=%f", &heading_in.min_in);
        }else if (strstr(token, "MaxStr=")){
			sscanf(token, "MaxStr=%f", &heading_in.max_in);
		}
        else if (strstr(token, "Point")) {
            int point_index;
            float longitude, latitude;
            num_waypoints++;
            if (sscanf(token, "Point%d=longitude%d:%f_latitude%d:%f", &point_index, &point_index, &longitude, &point_index, &latitude) == 5) {
                if (point_index >= 0 && point_index < NUM_WAYPOINTS) {
                    waypoints[point_index].longitude = longitude;
                    waypoints[point_index].latitude = latitude;
                }
            }
        }
        token = strtok(NULL, ";");
    }
    return;
}

void print_pid_and_waypoints(void) {
    printf("PID Gains: Kp = %.2f, Ki = %.2f, Kd = %.2f  Min = %.2f  Max = %.2f Num = %d\r\n", heading_pid.Kp, heading_pid.Ki, heading_pid.Kd, heading_in.min_in, heading_in.max_in,num_waypoints);
    for (int i = 0; i < NUM_WAYPOINTS; i++) {
        printf("Waypoint %d: Longitude = %.6f, Latitude = %.6f\r\n", i, waypoints[i].longitude, waypoints[i].latitude);
        //sprintf(board_config.gps.Target_gps[i],"lon%d=%f,lat%d=%f",i,waypoints[i].longitude,i,waypoints[i].latitude);
    }
    char point_buf[500] = {0};
    sprintf(point_buf,"Kp=%f,Ki=%f,Kd=%f,Min=%f,Max=%f,Num=%d,Lon1=%f,Lat1=%f,Lon2=%f,Lat2=%f,Lon3=%f,Lat3=%f,Lon4=%f,Lat4=%f,Lon5=%f,Lat5=%f,Lon6=%f,Lat6=%f",\
    		heading_pid.Kp,heading_pid.Ki,heading_pid.Kd,heading_in.min_in,heading_in.max_in,num_waypoints,waypoints[0].longitude, waypoints[0].latitude,\
			waypoints[1].longitude, waypoints[1].latitude,waypoints[2].longitude, waypoints[2].latitude,waypoints[3].longitude, waypoints[3].latitude,\
			waypoints[4].longitude, waypoints[4].latitude,waypoints[5].longitude, waypoints[5].latitude);
    printf("lenght:%d\r\n",strlen(point_buf));
    HAL_UART_Transmit(&huart4,point_buf,strlen(point_buf),0xff);
    HAL_Delay(50);
    HAL_UART_Transmit(&huart4,point_buf,strlen(point_buf),0xff);
    num_waypoints = 0;
    return ;
}

void decode_4G(void){
	char* tmpbuf = NULL;
	static char spr_buf[100] = {0};
	static Air_flag = 0;
	if(Air_flag == 0){
		Air_flag = 1;
		sprintf(spr_buf,"/device/%s",board_config.Net_4g.Imei);
	}
	if(strstr(RECV_4G_BUFF,"+CGNSINF") != NULL){
		const char *token;
		char seng_gpsbuf[60] = {0};
		int count = 0;
		// 使用 strtok 函数逐个分隔字段
		token = strtok((char *)RECV_4G_BUFF, ",");  // 第一次调用 strtok 需要传入整个字符串
		while (token != NULL) {
			count++;
			if (count == 4) {  // 纬度
				sprintf(board_config.gps.lat,"%s",token);
			} else if (count == 5) {  // 经度
				sprintf(board_config.gps.lon,"%s",token);
			} else if (count == 7) {  //速度
				sprintf(board_config.gps.Speed,"%s",token);
			}
			token = strtok(NULL, ",");  // 后续调用只需要传入 NULL
		}
		printf("lat:%s lon:%s speed:%s\r\n",board_config.gps.lat,board_config.gps.lon,board_config.gps.Speed);
		sprintf(seng_gpsbuf,"now_gps:%s,%s;speed:%s",board_config.gps.lat,board_config.gps.lon,board_config.gps.Speed);
		HAL_UART_Transmit(&huart5,seng_gpsbuf,strlen(seng_gpsbuf),0xff);
	}
	else if(strstr(RECV_4G_BUFF,"CGSN") != NULL){
		tmpbuf = strtok(RECV_4G_BUFF,"\r\n\r\n");
		tmpbuf = strtok(NULL,"\r\n\r\n");
		sprintf(board_config.Net_4g.Imei,"%s",tmpbuf);
		printf("Imei:%s\r\n",board_config.Net_4g.Imei);
	}
//	else if(strstr(RECV_4G_BUFF,spr_buf) != NULL && strstr(RECV_4G_BUFF,"CmdArgs") != NULL && strstr(RECV_4G_BUFF,"arg0=longitude0") == NULL){
//		tmpbuf = strstr(RECV_4G_BUFF,"CmdValue");
//		tmpbuf = strstr(tmpbuf,":");
//		int recv_cmd = (tmpbuf[1] - '0');
//		switch(recv_cmd){
//		case 1:HAL_GPIO_WritePin(GPIOB, RELAY3_Pin, GPIO_PIN_SET);board_config.Net_4g.Lock_Status = 1;board_config.Net_4g.Enable = 1;board_config.Net_4g.Use_Status = 1;break;//open key
//		case 2:HAL_GPIO_WritePin(GPIOB, RELAY3_Pin, GPIO_PIN_RESET);board_config.Net_4g.Lock_Status = 0;board_config.Net_4g.Enable = 0;board_config.Net_4g.Use_Status = 0;break;//close key
//		case 6:HAL_NVIC_SystemReset();break;//reset device
//		default:break;
//		}
//	}
	else if(strstr(RECV_4G_BUFF,"\"CmdValue\":9") != NULL){
		printf("Stop Auto\r\n");
		HAL_UART_Transmit(&huart4,"Stop_Auto",strlen("Stop_Auto"),0xff);
	}
	else if(strstr(RECV_4G_BUFF,"AutomaticReturn=1") != NULL){
		printf("AutomaticReturn Start!\r\n");
		HAL_UART_Transmit(&huart4,"AutomaticReturn_Start",strlen("AutomaticReturn_Start"),0xff);
	}
	else if(strstr(RECV_4G_BUFF,"Point0=longitude0") != NULL){
		char *json_data = extract_json_data(RECV_4G_BUFF);
		if(json_data != NULL){
			parse_json_and_update(json_data);
		}
		print_pid_and_waypoints();
	}
}






