#include "u_RockerKey.h"
#include "adc.h"
#include "tim.h"
#include "motor_contorl.h"
#include "board_init.h"
#include "math.h"
#include "main.h"
#include "usart.h"

#define POWER_MODE_PWM 1
#define POWER_MODE_CAN 2

xyzVal_struct xyzVal;
uint16_t g_iAdcx[2];

/*函数名称:获取xyz轴的值*/
/*形参:无              */
/*返回值:无            */
void Get_xyzVal(void)
{
  uint16_t ADC1_Value;
  uint16_t ADC2_Value;
  HAL_ADC_Start(&hadc1);
  HAL_ADC_Start(&hadc2);
  int power_mode = POWER_MODE_PWM;
  HAL_ADC_PollForConversion(&hadc1, 50);   //绛夊緟杞崲瀹屾垚锛�50涓烘渶澶х瓑寰呮椂闂达紝鍗曚綅涓簃s
  if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
  {
   ADC1_Value = HAL_ADC_GetValue(&hadc1);   //鑾峰彇AD鍊�
   //printf("ADC1: %d   ",ADC1_Value);
  }
  HAL_ADC_PollForConversion(&hadc2, 50);   //绛夊緟杞崲瀹屾垚锛�50涓烘渶澶х瓑寰呮椂闂达紝鍗曚綅涓簃s
  if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc2), HAL_ADC_STATE_REG_EOC))
  {
   ADC2_Value = HAL_ADC_GetValue(&hadc2);   //鑾峰彇AD鍊�
   //printf("ADC2: %d \r\n",ADC2_Value);
  }
  if(power_mode == POWER_MODE_PWM){
	  if(ADC2_Value >= 2077 && ADC2_Value <= 2079){
		  ADC2_Value = 2078;
	  }
	 // ADC2_Value = abs(ADC2_Value - 4096) ;
  }
  board_config.Rockers.Rocker_X = ADC1_Value;
  board_config.Rockers.Rocker_Y = ADC2_Value;
  g_iAdcx[0] = ADC2_Value;
  g_iAdcx[1] = ADC1_Value;
 // printf("Y = %d\r\nX = %d\r\n", g_iAdcx[0], g_iAdcx[1]);
  if(g_iAdcx[0]<1000 && g_iAdcx[0]>0)          //y0
    xyzVal.Y = Yup;
  else if(g_iAdcx[0]<1800 && g_iAdcx[0]>1000)  //y1
    xyzVal.Y = Ysup;
  else if(g_iAdcx[0]<2200 && g_iAdcx[0]>1800)  //y2
    xyzVal.Y = Ysp;
  else if(g_iAdcx[0]<3000 && g_iAdcx[0]>2200)  //y3
    xyzVal.Y = Ysdn;
  else if(g_iAdcx[0]<4200 && g_iAdcx[0]>3000)  //y4
    xyzVal.Y = Ydn;


  if(g_iAdcx[1]<1000 && g_iAdcx[1]>0)          //x0
    xyzVal.X = Xlf;
  else if(g_iAdcx[1]<1800 && g_iAdcx[1]>1000)  //x1
    xyzVal.X = Xslf;
  else if(g_iAdcx[1]<2200 && g_iAdcx[1]>1800)  //x2
    xyzVal.X = Xsp;
  else if(g_iAdcx[1]<3000 && g_iAdcx[1]>2200)  //x3
    xyzVal.X = Xsri;
  else if(g_iAdcx[1]<4200 && g_iAdcx[1]>3000)  //x4
    xyzVal.X = Xri;

//  PrintfDebug("xyzVal.X = %d\r\nxyzVal.Y = %d\r\nxyzVal.Z = %d\r\n", xyzVal.X, xyzVal.Y, xyzVal.Z);
}

void Get_xyzVal_Blue(void){

  //HAL_ADC_Start_DMA(&hadc1, (uint32_t*)g_iAdcx, sizeof(g_iAdcx) / sizeof(g_iAdcx[0]));
  HAL_UART_Receive_DMA(&huart1,BLUE_BUF,sizeof(BLUE_BUF));
  if(BLUE_BUF[0] != 0x55){
	  board_config.Rockers.Rocker_X = 2048;
	  board_config.Rockers.Rocker_Y = 2048;
	  return;
  }
  board_config.Rockers.Rocker_X = (BLUE_BUF[2] << 8 | BLUE_BUF[3]);
  board_config.Rockers.Rocker_Y = (BLUE_BUF[4] << 8 | BLUE_BUF[5]);
  board_config.Rockers.Rocker_Y = abs(board_config.Rockers.Rocker_Y - 4096) ;
  g_iAdcx[0] = board_config.Rockers.Rocker_Y;
  g_iAdcx[1] = board_config.Rockers.Rocker_X;
  printf("X = %d\r\n Y = %d\r\n", g_iAdcx[0], g_iAdcx[1]);
  if(g_iAdcx[0]<1000 && g_iAdcx[0]>0)          //y0
    xyzVal.Y = Yup;
  else if(g_iAdcx[0]<1800 && g_iAdcx[0]>1000)  //y1
    xyzVal.Y = Ysup;
  else if(g_iAdcx[0]<2200 && g_iAdcx[0]>1800)  //y2
    xyzVal.Y = Ysp;
  else if(g_iAdcx[0]<3000 && g_iAdcx[0]>2200)  //y3
    xyzVal.Y = Ysdn;
  else if(g_iAdcx[0]<4200 && g_iAdcx[0]>3000)  //y4
    xyzVal.Y = Ydn;


  if(g_iAdcx[1]<1000 && g_iAdcx[1]>0)          //x0
    xyzVal.X = Xlf;
  else if(g_iAdcx[1]<1800 && g_iAdcx[1]>1000)  //x1
    xyzVal.X = Xslf;
  else if(g_iAdcx[1]<2200 && g_iAdcx[1]>1800)  //x2
    xyzVal.X = Xsp;
  else if(g_iAdcx[1]<3000 && g_iAdcx[1]>2200)  //x3
    xyzVal.X = Xsri;
  else if(g_iAdcx[1]<4200 && g_iAdcx[1]>3000)  //x4
    xyzVal.X = Xri;

//  PrintfDebug("xyzVal.X = %d\r\nxyzVal.Y = %d\r\nxyzVal.Z = %d\r\n", xyzVal.X, xyzVal.Y, xyzVal.Z);
}

/*函数名称:获取摇杆的状态 */
/*形参:无                */
/*返回值:keystate        */
/*说明:keystate为E_RKEYState的变量，返回按键状态值*/
E_RKEYState Get_KeyState(void)
{
  E_RKEYState keystate;
  Get_xyzVal();
//  PrintfDebug("%d %d\r\n", xyzVal.X, xyzVal.Y);                 //摇杆状态编码
  if((xyzVal.Y == Ysp) && (xyzVal.X == Xsp)) //x2 y2 z0 0
    keystate = KEYSTOP;
  else if((xyzVal.Y == Yup) && (xyzVal.X == Xsp))                 //x2 y0 z0 1
    keystate = KEYUP;
  else if((xyzVal.Y == Ysup) && (xyzVal.X == Xsp))                //x2 y1 z0 2
    keystate = KEYSUP;
  else if((xyzVal.Y == Ydn) && (xyzVal.X == Xsp))                 //x2 y4 z0 3
    keystate = KEYDOWN;
  else if((xyzVal.Y == Ysdn) && (xyzVal.X == Xsp))                //x2 y3 z0 4
    keystate = KEYSDOWN;
  else if((xyzVal.Y == Ysp) && (xyzVal.X == Xlf))                 //x0 y2 z0 5
    keystate = KEYLEFT;
  else if((xyzVal.Y == Ysp) && (xyzVal.X == Xslf))                //x1 y2 z0 6
    keystate = KEYSLEFT;
  else if((xyzVal.Y == Ysp) && (xyzVal.X == Xri))                 //x4 y2 z0 7
    keystate = KEYRIGHT;
  else if((xyzVal.Y == Ysp) &&  (xyzVal.X == Xsri))                //x3 y2 z0 8
    keystate = KEYSRIGHT;
  else if(((xyzVal.Y==Yup)&&(xyzVal.X==Xlf))||((xyzVal.Y==Yup)&&(xyzVal.X==Xslf))||((xyzVal.Y==Ysup)&&(xyzVal.X==Xlf)))                 //x01 y01 z0 10
    keystate = KEYUPLF;
  else if((xyzVal.Y == Ysup) && (xyzVal.X == Xslf))               //x1 y1 z0 11
    keystate = KEYSUPLF;
  else if(((xyzVal.Y==Yup)&&(xyzVal.X==Xri))||((xyzVal.Y==Yup)&&(xyzVal.X==Xsri))||((xyzVal.Y==Ysup)&&(xyzVal.X==Xri)))                 //x34 y01 z0 12
    keystate = KEYUPRI;
  else if((xyzVal.Y == Ysup) && (xyzVal.X == Xsri))               //x3 y1 z0 13
    keystate = KEYSUPRI;
  else if(((xyzVal.Y==Ydn)&&(xyzVal.X==Xlf))||((xyzVal.Y==Ydn)&&(xyzVal.X==Xslf))||((xyzVal.Y==Ysdn)&&(xyzVal.X==Xlf)))                 //x01 y34 z0 14
    keystate = KEYDOWNLF;
  else if((xyzVal.Y == Ysdn) && (xyzVal.X == Xslf))               //x1 y3 z0 15
    keystate = KEYSDOWNLF;
  else if(((xyzVal.Y==Ydn)&&(xyzVal.X==Xri))||((xyzVal.Y==Ydn)&&(xyzVal.X==Xsri))||((xyzVal.Y==Ysdn)&&(xyzVal.X==Xri)))                 //x34 y34 z0 16
    keystate = KEYDOWNRI;
  else if((xyzVal.Y == Ysdn) && (xyzVal.X == Xsri))               //x3 y3 z0 17
    keystate = KEYSDOWNRI;
  else
    keystate = KEYState_MAX;
//  PrintfDebug("keystate = %d\r\n", keystate);
  return keystate;
}

E_RKEYState Get_KeyState_Blue(void)
{
  E_RKEYState keystate;
  Get_xyzVal_Blue();
//  PrintfDebug("%d %d\r\n", xyzVal.X, xyzVal.Y);                 //摇杆状态编码
  if((xyzVal.Y == Ysp) && (xyzVal.X == Xsp)) //x2 y2 z0 0
    keystate = KEYSTOP;
  else if((xyzVal.Y == Yup) && (xyzVal.X == Xsp))                 //x2 y0 z0 1
    keystate = KEYUP;
  else if((xyzVal.Y == Ysup) && (xyzVal.X == Xsp))                //x2 y1 z0 2
    keystate = KEYSUP;
  else if((xyzVal.Y == Ydn) && (xyzVal.X == Xsp))                 //x2 y4 z0 3
    keystate = KEYDOWN;
  else if((xyzVal.Y == Ysdn) && (xyzVal.X == Xsp))                //x2 y3 z0 4
    keystate = KEYSDOWN;
  else if((xyzVal.Y == Ysp) && (xyzVal.X == Xlf))                 //x0 y2 z0 5
    keystate = KEYLEFT;
  else if((xyzVal.Y == Ysp) && (xyzVal.X == Xslf))                //x1 y2 z0 6
    keystate = KEYSLEFT;
  else if((xyzVal.Y == Ysp) && (xyzVal.X == Xri))                 //x4 y2 z0 7
    keystate = KEYRIGHT;
  else if((xyzVal.Y == Ysp) &&  (xyzVal.X == Xsri))                //x3 y2 z0 8
    keystate = KEYSRIGHT;
  else if(((xyzVal.Y==Yup)&&(xyzVal.X==Xlf))||((xyzVal.Y==Yup)&&(xyzVal.X==Xslf))||((xyzVal.Y==Ysup)&&(xyzVal.X==Xlf)))                 //x01 y01 z0 10
    keystate = KEYUPLF;
  else if((xyzVal.Y == Ysup) && (xyzVal.X == Xslf))               //x1 y1 z0 11
    keystate = KEYSUPLF;
  else if(((xyzVal.Y==Yup)&&(xyzVal.X==Xri))||((xyzVal.Y==Yup)&&(xyzVal.X==Xsri))||((xyzVal.Y==Ysup)&&(xyzVal.X==Xri)))                 //x34 y01 z0 12
    keystate = KEYUPRI;
  else if((xyzVal.Y == Ysup) && (xyzVal.X == Xsri))               //x3 y1 z0 13
    keystate = KEYSUPRI;
  else if(((xyzVal.Y==Ydn)&&(xyzVal.X==Xlf))||((xyzVal.Y==Ydn)&&(xyzVal.X==Xslf))||((xyzVal.Y==Ysdn)&&(xyzVal.X==Xlf)))                 //x01 y34 z0 14
    keystate = KEYDOWNLF;
  else if((xyzVal.Y == Ysdn) && (xyzVal.X == Xslf))               //x1 y3 z0 15
    keystate = KEYSDOWNLF;
  else if(((xyzVal.Y==Ydn)&&(xyzVal.X==Xri))||((xyzVal.Y==Ydn)&&(xyzVal.X==Xsri))||((xyzVal.Y==Ysdn)&&(xyzVal.X==Xri)))                 //x34 y34 z0 16
    keystate = KEYDOWNRI;
  else if((xyzVal.Y == Ysdn) && (xyzVal.X == Xsri))               //x3 y3 z0 17
    keystate = KEYSDOWNRI;
  else
    keystate = KEYState_MAX;
//  PrintfDebug("keystate = %d\r\n", keystate);
  return keystate;
}

/*函数名称:测试摇杆功能   */
/*形参:E_RKEYState state */
/*返回值:无              */
#define NoState 1
#define HaveState 0
#define Gostright 2
#define Gorightorleft 3
#define GoUpleft 4
#define GoUpright 5
#define GoDownleft 6
#define GoDownright 7
#define Mode_PWM 1
#define Mode_CAN 2
void Status_get_and_Powerset(E_RKEYState state)
{
  int status = Mode_PWM;
  if(status == Mode_PWM){
	switch(state)
	{
	  case KEYSTOP://printf("No State\r\n");
	  Servo_Control(NoState,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 0;board_config.motor_status.Direction = 0;return;
		break;
	  case KEYUP://printf("Key Up\r\n");
	  Servo_Control(Gostright,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 1;board_config.motor_status.Direction = 0;return;
		break;
	  case KEYSUP://printf("Key Slowly Up\r\n");
	  Servo_Control(Gostright,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 1;board_config.motor_status.Direction = 0;return;
		break;
	  case KEYDOWN://printf("Key Down\r\n");
	  Servo_Control(Gostright,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 2;board_config.motor_status.Direction = 0;return;
		break;
	  case KEYSDOWN://printf("Key Slowly Down\r\n");
	  Servo_Control(Gostright,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 2;board_config.motor_status.Direction = 0;return;
		break;
	  case KEYLEFT://printf("Key Left\r\n");
	  Servo_Control(Gorightorleft,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 0;board_config.motor_status.Direction = 1;return;
		break;
	  case KEYSLEFT://printf("Key Slowly Left\r\n");
	  Servo_Control(Gorightorleft,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 0;board_config.motor_status.Direction = 1;return;
		break;
	  case KEYRIGHT://printf("Key Right\r\n");
	  Servo_Control(Gorightorleft,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 0;board_config.motor_status.Direction = 2;return;
		break;
	  case KEYSRIGHT://printf("Key Slowly Right\r\n");
	  Servo_Control(Gorightorleft,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 0;board_config.motor_status.Direction = 1;return;
		break;
	  case KEYPRESS:///printf("Press Key\r\n");
		break;

	  case KEYUPLF://printf("Key Up Left\r\n");
	  Servo_Control(GoUpleft,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 1;board_config.motor_status.Direction = 1;return;
		break;
	  case KEYSUPLF://printf("Key Slowly Up Left\r\n");
	  Servo_Control(GoUpleft,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 1;board_config.motor_status.Direction = 1;return;
		break;
	  case KEYUPRI://printf("Key Up Right\r\n");
	  Servo_Control(GoUpright,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 1;board_config.motor_status.Direction = 2;return;
		break;
	  case KEYSUPRI://printf("Key Slowly Up Right\r\n");
	  Servo_Control(GoUpright,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 1;board_config.motor_status.Direction = 2;return;
		break;
	  case KEYDOWNLF://printf("Key Down Left\r\n");
	  Servo_Control(GoDownleft,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 2;board_config.motor_status.Direction = 1;return;
		break;
	  case KEYSDOWNLF://printf("Key Slowly Down Left\r\n");
	  Servo_Control(GoDownleft,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 2;board_config.motor_status.Direction = 1;return;
		break;
	  case KEYDOWNRI://printf("Key Down Right\r\n");
	  Servo_Control(GoDownright,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 2;board_config.motor_status.Direction = 2;return;
		break;
	  case KEYSDOWNRI://printf("Key Slowly Down Right\r\n");
	  Servo_Control(GoDownright,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 2;board_config.motor_status.Direction = 2;return;
		break;
	  case KEYState_MAX://printf("State\r\n");
		break;
	  default:
		break;
	}
  }
  else if(status == Mode_CAN){
	switch(state)
	{
	  case KEYSTOP://printf("No State\r\n");
		  Servo_Control_CAN(NoState,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);return;
		break;
	  case KEYUP://printf("Key Up\r\n");
		  Servo_Control_CAN(2,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);return;
		break;
	  case KEYSUP://printf("Key Slowly Up\r\n");
		  Servo_Control_CAN(2,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);return;
		break;
	  case KEYDOWN://printf("Key Down\r\n");
		  Servo_Control_CAN(3,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);return;
		break;
	  case KEYSDOWN://printf("Key Slowly Down\r\n");
		  Servo_Control_CAN(3,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);return;
		break;
	  case KEYLEFT://printf("Key Left\r\n");
		  Servo_Control_CAN(4,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);return;
		break;
	  case KEYSLEFT://printf("Key Slowly Left\r\n");
		  Servo_Control_CAN(4,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);return;
		break;
	  case KEYRIGHT://printf("Key Right\r\n");
		  Servo_Control_CAN(5,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);return;
		break;
	  case KEYSRIGHT://printf("Key Slowly Right\r\n");
		  Servo_Control_CAN(5,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);return;
		break;
	  case KEYPRESS:///printf("Press Key\r\n");
		break;

	  case KEYUPLF://printf("Key Up Left\r\n");
		  Servo_Control_CAN(6,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);return;
		break;
	  case KEYSUPLF://printf("Key Slowly Up Left\r\n");
		  Servo_Control_CAN(6,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);return;
		break;
	  case KEYUPRI://printf("Key Up Right\r\n");
		  Servo_Control_CAN(7,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);return;
		break;
	  case KEYSUPRI://printf("Key Slowly Up Right\r\n");
		  Servo_Control_CAN(7,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);return;
		break;
	  case KEYDOWNLF://printf("Key Down Left\r\n");
		  Servo_Control_CAN(8,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);return;
		break;
	  case KEYSDOWNLF://printf("Key Slowly Down Left\r\n");
		  Servo_Control_CAN(8,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);return;
		break;
	  case KEYDOWNRI://printf("Key Down Right\r\n");
		  Servo_Control_CAN(9,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);return;
		break;
	  case KEYSDOWNRI://printf("Key Slowly Down Right\r\n");
		  Servo_Control_CAN(9,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);return;
		break;
	  case KEYState_MAX://printf("State\r\n");
		break;
	  default:
		break;
	}
  }
  return;
}

void Status_get_and_Powerset_Blue(E_RKEYState state){
	switch(state)
	{
	  case KEYSTOP://printf("No State\r\n");
	  Servo_Control_Blue(NoState,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 0;board_config.motor_status.Direction = 0;return;
		break;
	  case KEYUP://printf("Key Up\r\n");
	  Servo_Control_Blue(Gostright,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 1;board_config.motor_status.Direction = 0;return;
		break;
	  case KEYSUP://printf("Key Slowly Up\r\n");
	  Servo_Control_Blue(Gostright,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 1;board_config.motor_status.Direction = 0;return;
		break;
	  case KEYDOWN://printf("Key Down\r\n");
	  Servo_Control_Blue(Gostright,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 2;board_config.motor_status.Direction = 0;return;
		break;
	  case KEYSDOWN://printf("Key Slowly Down\r\n");
	  Servo_Control_Blue(Gostright,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 2;board_config.motor_status.Direction = 0;return;
		break;
	  case KEYLEFT://printf("Key Left\r\n");
	  Servo_Control_Blue(Gorightorleft,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 0;board_config.motor_status.Direction = 1;return;
		break;
	  case KEYSLEFT://printf("Key Slowly Left\r\n");
	  Servo_Control_Blue(Gorightorleft,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 0;board_config.motor_status.Direction = 1;return;
		break;
	  case KEYRIGHT://printf("Key Right\r\n");
	  Servo_Control_Blue(Gorightorleft,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 0;board_config.motor_status.Direction = 2;return;
		break;
	  case KEYSRIGHT://printf("Key Slowly Right\r\n");
	  Servo_Control_Blue(Gorightorleft,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 0;board_config.motor_status.Direction = 1;return;
		break;
	  case KEYPRESS:///printf("Press Key\r\n");
		break;

	  case KEYUPLF://printf("Key Up Left\r\n");
	  Servo_Control_Blue(GoUpleft,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 1;board_config.motor_status.Direction = 1;return;
		break;
	  case KEYSUPLF://printf("Key Slowly Up Left\r\n");
	  Servo_Control_Blue(GoUpleft,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 1;board_config.motor_status.Direction = 1;return;
		break;
	  case KEYUPRI://printf("Key Up Right\r\n");
	  Servo_Control_Blue(GoUpright,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 1;board_config.motor_status.Direction = 2;return;
		break;
	  case KEYSUPRI://printf("Key Slowly Up Right\r\n");
	  Servo_Control_Blue(GoUpright,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 1;board_config.motor_status.Direction = 2;return;
		break;
	  case KEYDOWNLF://printf("Key Down Left\r\n");
	  Servo_Control_Blue(GoDownleft,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 2;board_config.motor_status.Direction = 1;return;
		break;
	  case KEYSDOWNLF://printf("Key Slowly Down Left\r\n");
	  Servo_Control_Blue(GoDownleft,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 2;board_config.motor_status.Direction = 1;return;
		break;
	  case KEYDOWNRI://printf("Key Down Right\r\n");
	  Servo_Control_Blue(GoDownright,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 2;board_config.motor_status.Direction = 2;return;
		break;
	  case KEYSDOWNRI://printf("Key Slowly Down Right\r\n");
	  Servo_Control_Blue(GoDownright,board_config.Rockers.Rocker_X,board_config.Rockers.Rocker_Y);board_config.motor_status.Gear = 2;board_config.motor_status.Direction = 2;return;
		break;
	  case KEYState_MAX://printf("State\r\n");
		break;
	  default:
		break;
	}
	return ;
}





