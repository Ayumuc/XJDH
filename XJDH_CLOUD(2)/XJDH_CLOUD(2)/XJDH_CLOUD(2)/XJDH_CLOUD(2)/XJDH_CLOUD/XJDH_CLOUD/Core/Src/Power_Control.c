#include "main.h"
#include "Power_Control.h"
#include "u_RockerKey.h"


uint8_t Power_Contorl_BLUE(void){
	Status_get_and_Powerset_Blue(Get_KeyState_Blue());
	return 1;
}

uint8_t Power_Contorl_AD(void){
	Status_get_and_Powerset(Get_KeyState());
	return 1;
}

uint8_t Power_Contorl_NET(void){
	return 1;
}
