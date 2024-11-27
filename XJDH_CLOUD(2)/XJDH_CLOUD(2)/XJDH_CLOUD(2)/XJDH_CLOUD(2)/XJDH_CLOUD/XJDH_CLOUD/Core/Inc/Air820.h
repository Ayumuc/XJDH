#ifndef AIR820__h
#define AIR820__h

#include "main.h"
#include "stdio.h"
#include "usart.h"
#include "delay.h"


extern uint8_t RECV_4G_BUFF[4096];
extern int Uart_4g_End;
extern int rc4g_flag;
int Air820_init(void);
uint8_t Air_820ug_rxdecode(void);
int Air820_uart_send(char* tx_buf, uint16_t len ,char* rx_buf);
int Mode_mqtt(int step);
int Mode_http(int step);
int Data_Update(void);
void board_init(void);
char* extract_json_data(const char *input);
void parse_json_and_update(const char *json_data);
void parse_cmd_args(const char *args_str);
void print_pid_and_waypoints(void);
void decode_4G(void);

#endif

