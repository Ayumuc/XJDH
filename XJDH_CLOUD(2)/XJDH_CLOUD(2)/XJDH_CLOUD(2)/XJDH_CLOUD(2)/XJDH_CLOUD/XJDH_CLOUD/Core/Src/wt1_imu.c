#include "main.h"
#include "wt1_imu.h"
#include "usart.h"
#include "board_init.h"




int imu_collect(void){
	HAL_UART_Receive_DMA(&huart3,imu_buf,sizeof(imu_buf));
//	printf("\r\n");
//	for(int j = 0 ; j < sizeof(imu_buf) ; j++){
//		printf(" %02x",imu_buf[j]);
//	}
//	printf("\r\n");
	imu_decode();
	return 1;
}

/*55 51 00 00 00 00 00 00 b6 01 5d 55 52 00 00 00 00 00 00 c9 89 f9 55 53 4e 05 36 ff 00 00 06 c1 f7*/

int imu_decode(void){
	double data = 0.0;
	//printf("\r\n");
	for(int i = 0 ; i < 92; i++){
		if(imu_buf[i] == 0x55 && imu_buf[i+1] == 0x51){
			data = (float)(imu_buf[i+3] << 8 | imu_buf[i+2])/32768*16;
			board_config.Imu_data.Ax = data;
#if PRINTF
			printf(" AX:%f",data);
#endif
			data = (float)(imu_buf[i+5] << 8 | imu_buf[i+4])/32768*16;
			board_config.Imu_data.Ay = data;
#if PRINTF
			printf(" AY:%f",data);
#endif
			data = (float)(imu_buf[i+7] << 8 | imu_buf[i+6])/32768*16;
			board_config.Imu_data.Az = data;
#if PRINTF
			printf(" AZ:%f",data);
#endif
			data = (float)(imu_buf[i+9] << 8 | imu_buf[i+8])/100;
			board_config.Imu_data.Ay = data;
#if PRINTF
			printf(" T:%f",data);
#endif
		}
		else if(imu_buf[i] == 0x55 && imu_buf[i+1] == 0x52){
			data = (float)(imu_buf[i+3] << 8 | imu_buf[i+2])/32768*2000;
			board_config.Imu_data.Wx = data;
#if PRINTF
			printf(" WX:%f",data);
#endif
			data = (float)(imu_buf[i+5] << 8 | imu_buf[i+4])/32768*2000;
			board_config.Imu_data.Wy = data;
#if PRINTF
			printf(" WY:%f",data);
#endif
			data = (float)(imu_buf[i+7] << 8 | imu_buf[i+6])/32768*2000;
			board_config.Imu_data.Wz = data;
#if PRINTF
			printf(" WZ:%f",data);
#endif
			data = (float)(imu_buf[i+9] << 8 | imu_buf[i+8])/100;
#if PRINTF
			printf(" V:%f",data);
#endif
		}
		else if(imu_buf[i] == 0x55 && imu_buf[i+1] == 0x53){
			data = (float)(imu_buf[i+3] << 8 | imu_buf[i+2])/32768*180;
			board_config.Imu_data.Roll = data;
#if PRINTF
			printf(" ROLL:%f",data);
#endif
			data = (float)(imu_buf[i+5] << 8 | imu_buf[i+4])/32768*180;
			board_config.Imu_data.Yaw = data;
#if PRINTF
			printf(" YAW:%f",data);
#endif
			static float last_pitch = 0.0;
			static int pitch_flag = 0;
			pitch_flag++;
			data = (double)(imu_buf[i+7] << 8 | imu_buf[i+6])/32768*180;
			if(data == 0.0){
				return 1;
			}
			if(data > 0.263671 && data < 0.263673){
				return 1;
			}
			last_pitch = data;
			if(pitch_flag < 2000){
				board_config.Imu_data.Pitch = data;
			}else if(abs(last_pitch - board_config.Imu_data.Pitch) < 24 || abs(last_pitch - board_config.Imu_data.Pitch) > 340){
				pitch_flag = 2001;
				board_config.Imu_data.Pitch = data;
			}
#if PRINTF
			printf(" PITCH:%lf\r\n",board_config.Imu_data.Pitch);
#endif
			data = (float)(imu_buf[i+9] << 8 | imu_buf[i+8]);
#if PRINTF
			printf(" BAN:%f",data);
#endif
		}
//		else if(buf[i] == 0x55 && buf[i+1] == 0x54){
//
//		}
	}
//	printf("\r\n");
	return 1;
}

