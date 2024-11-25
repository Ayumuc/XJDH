#ifndef CAN_CAN1_H_
#define CAN_CAN1_H_

#include "main.h"
#include "stdio.h"
#include "can.h"

#define Tx_Error 0
#define Rx_Error 0
#define Rx_OK 1

extern uint8_t CAN_RXBUF[8];

uint32_t CAN_TX_Message(uint8_t TxData[], uint8_t length);
//uint32_t CAN_RX_Message(uint8_t RxData[]);
int16_t CAN_Receive_Msg(void);
uint16_t can_decode(CAN_RxHeaderTypeDef RxMessage);
#endif /* CAN_CAN1_H_ */
