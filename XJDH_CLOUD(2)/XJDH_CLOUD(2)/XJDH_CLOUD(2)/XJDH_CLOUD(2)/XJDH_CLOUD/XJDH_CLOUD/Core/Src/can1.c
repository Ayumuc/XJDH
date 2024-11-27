#include "can1.h" //搴撴枃浠跺０鏄�
#include "main.h"
#include "board_init.h"
uint16_t can_temp_cir(void);



CAN_TxHeaderTypeDef Tx_pHeader;
/*
 * @brief: CAN Send Message.
 * @param: "TxData[]" stored the message of ready to send, which length must between 0 and 8.
 * @param: "length" stored the number of the data (one data is 8 bit) of ready to send.
 * @retval: Tx_Error: send error; other: the mailbox which has been used, this parameter can be a CAN_TX_MAILBOX0,
 * 																						   CAN_TX_MAILBOX1,
 * 																						   CAN_TX_MAILBOX2.
 */
uint32_t CAN_TX_Message(uint8_t TxData[], uint8_t length)
{

	uint32_t TxMailboxNumber = 0x00000000U;    // 瀛樺偍鏈鍙戦�佹墍浣跨敤閭鐨勯偖绠卞彿

	Tx_pHeader.StdId = 0x200;    // 浠ユID鍙戦��
	Tx_pHeader.ExtId = 0x0000;    // 鎵╁睍ID锛堟澶勬棤鐢級
	Tx_pHeader.IDE = CAN_ID_STD;    // 鏍囧噯甯�
	Tx_pHeader.RTR = CAN_RTR_DATA;    // 鏁版嵁甯�
	Tx_pHeader.DLC = length;    // 鍙戦�佹暟鎹殑闀垮害
	Tx_pHeader.TransmitGlobalTime = DISABLE;

	if(HAL_CAN_AddTxMessage(&hcan1, &Tx_pHeader, TxData, &TxMailboxNumber) != HAL_OK)
	{
		return Tx_Error;
	}
	return TxMailboxNumber;
}


/*
 * @brief: CAN Receive Message.
 * @param: "RxData[]" will store the message which has been received, which length must between 0 and 8.
 * @retval: receive status.
 */
int16_t CAN_Receive_Msg(void)
{
	uint32_t i;
	uint8_t recv_data[8];
	CAN_RxHeaderTypeDef RxMessage;

	while(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) != 0) //循环等待接收
	{
		HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxMessage, recv_data);
		for(i=0;i<RxMessage.DLC;i++){
			CAN_RXBUF[i]=recv_data[i];
			printf("%02x",CAN_RXBUF[i]);
		}
		printf("\r\n");
		can_decode(RxMessage);
	}

	while(HAL_CAN_GetRxFifoFillLevel(&hcan2, CAN_RX_FIFO1) != 0){
		HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO1, &RxMessage, recv_data);
		for(i=0;i<RxMessage.DLC;i++){
			CAN_RXBUF[i]=recv_data[i];
			printf("%02x",CAN_RXBUF[i]);
		}
		printf("\r\n");
		can_decode(RxMessage);
	} //循环等待接收

	return RxMessage.DLC;
}


void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	//中断内容
	return;
}


int can_collect(void){
	CAN_Receive_Msg();
	return 1;
}

uint16_t can_decode(CAN_RxHeaderTypeDef RxMessage){
	if(RxMessage.StdId == 0x300){
		board_config.motor_status.L_motor_speed = (CAN_RXBUF[0] << 8) | CAN_RXBUF[1];
		board_config.motor_status.L_motor_curr = ((CAN_RXBUF[2] << 8) | CAN_RXBUF[3])/100;
		board_config.motor_status.L_motor_vol = CAN_RXBUF[4];
		can_temp_cir();
		board_config.motor_status.L_motor_status = CAN_RXBUF[6];
		board_config.motor_status.L_motor_fault =  CAN_RXBUF[7];
		printf("speed:%d\r\n",board_config.motor_status.L_motor_speed);
		printf("curr:%d\r\n",board_config.motor_status.L_motor_curr);
		printf("vol:%d\r\n",board_config.motor_status.L_motor_vol);
		printf("temp:%d\r\n",board_config.motor_status.L_motor_temp);
		printf("status:%d\r\n",board_config.motor_status.L_motor_status);
		printf("fault:%d\r\n",board_config.motor_status.L_motor_fault);
	}
	else if(RxMessage.StdId == 0x301){
		board_config.motor_status.R_motor_speed = (CAN_RXBUF[0] << 8) | CAN_RXBUF[1];
		board_config.motor_status.R_motor_curr = ((CAN_RXBUF[2] << 8) | CAN_RXBUF[3])/100;
		board_config.motor_status.R_motor_vol = CAN_RXBUF[4];
		can_temp_cir();
		board_config.motor_status.R_motor_status = CAN_RXBUF[6];
		board_config.motor_status.R_motor_fault =  CAN_RXBUF[7];
		printf("speed:%d\r\n",board_config.motor_status.R_motor_speed);
		printf("curr:%d\r\n",board_config.motor_status.R_motor_curr);
		printf("vol:%d\r\n",board_config.motor_status.R_motor_vol);
		printf("temp:%d\r\n",board_config.motor_status.R_motor_temp);
		printf("status:%d\r\n",board_config.motor_status.R_motor_status);
		printf("fault:%d\r\n",board_config.motor_status.R_motor_fault);
	}

	return 1;
}


float temp_before = 0.0;
uint16_t can_temp_cir(void){
	float cof = 0.8;
	static int count = 0;
	float temp_tem = 0.0;
	float temp_after = 0.0;
	if(count == 0){
		temp_tem = CAN_RXBUF[5];
		temp_before = temp_tem;
		count = 1;
		board_config.motor_status.L_motor_temp = temp_tem;
		return 0;
	}
	else if(count == 1){
		temp_after =  cof*CAN_RXBUF[5] +(1.000- cof)*temp_before;
		board_config.motor_status.R_motor_temp = temp_after;
		temp_before = temp_after;
		return 2;
	}
	return 1;
}





