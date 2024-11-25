/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#define        UART1_BUF_SIZE       (512)
extern     volatile uint16_t  uart1_rx_len;
extern     volatile uint8_t   uart1_recv_end_flag;
extern              uint8_t   uart1_rx_buffer[UART1_BUF_SIZE];
#define        UART2_BUF_SIZE       (512)
extern     volatile uint16_t  uart2_rx_len;
extern     volatile uint8_t   uart2_recv_end_flag;
extern              uint8_t   uart2_rx_buffer[UART2_BUF_SIZE];
#define        UART3_BUF_SIZE       (512)
extern     volatile uint16_t  uart3_rx_len;
extern     volatile uint8_t   uart3_recv_end_flag;
extern              uint8_t   uart3_rx_buffer[UART3_BUF_SIZE];
#define        UART4_BUF_SIZE       (512)
extern     volatile uint16_t  uart4_rx_len;
extern     volatile uint8_t   uart4_recv_end_flag;
extern              uint8_t   uart4_rx_buffer[UART4_BUF_SIZE];
#define        UART5_BUF_SIZE       (512)
extern     volatile uint16_t  uart5_rx_len;
extern     volatile uint8_t   uart5_recv_end_flag;
extern              uint8_t   uart5_rx_buffer[UART5_BUF_SIZE];
/* USER CODE END Includes */

extern UART_HandleTypeDef huart4;

extern UART_HandleTypeDef huart5;

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_UART4_Init(void);
void MX_UART5_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

