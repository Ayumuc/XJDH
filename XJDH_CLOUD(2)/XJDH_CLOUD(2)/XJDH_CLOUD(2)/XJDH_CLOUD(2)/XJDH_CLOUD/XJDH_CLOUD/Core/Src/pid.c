#include "stm32f4xx_hal.h"  // 根据你的STM32系列选择正确的头文件
#include "pid.h"
#include "tim.h"
#include "usart.h"
#include "main.h"
#include "gps_Tobd09.h"
#include "motor_contorl.h"
#include "Data_Collect.h"

#define AUTO 4


// 初始化PID控制器
void PID_Init(PIDController *pid, float Kp, float Ki, float Kd, float setpoint, float min_output, float max_output) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->setpoint = setpoint;
    pid->integral = 0.0f;
    pid->previous_error = 0.0f;
    pid->min_output = min_output;
    pid->max_output = max_output;
}

// 更新PID控制器
float PID_Update(PIDController *pid, float measured_value, float dt) {
    float error = pid->setpoint - measured_value;

    // 计算比例、积分和微分项
    float proportional = pid->Kp * error;
    pid->integral += pid->Ki * error * dt;
    float derivative = pid->Kd * (error - pid->previous_error) / dt;

    // 计算总输出
    float output = proportional + pid->integral + derivative;

    // 防止输出超出范围
    if (output > pid->max_output) {
        output = pid->max_output;
    } else if (output < pid->min_output) {
        output = pid->min_output;
    }

    // 积分抗饱和
    if (output == pid->max_output || output == pid->min_output) {
        // 如果输出已经达到最大或最小值，停止积分项的累加
        pid->integral -= pid->Ki * error * dt;  // 取消本次积分累加
    }

    // 更新上一次误差
    pid->previous_error = error;

    return output;
}

void reset_PID(PIDController *pid) {
    pid->integral = 0.0f; // 重置积分器
    pid->previous_error = 0.0f; // 重置之前的误差
    pid->last_input = 0.0f; // 重置最后一次输入
}


#define PI 3.14159265358979323846
#define TO_RAD (PI / 180.0)  // 度转弧度
#define TO_DEG (180.0 / PI)  // 弧度转度
#define MID_PWM 3000

// 计算目标方位角（假设已知当前位置和目标位置）
float calculate_bearing(double lat1, double lon1, double lat2, double lon2) {
	double phi1 = lat1 * TO_RAD;
	double phi2 = lat2 * TO_RAD;
	double lambda1 = lon1 * TO_RAD;
	double lambda2 = lon2 * TO_RAD;

	// 使用球面三角公式计算方位角
	double y = sin(lambda2 - lambda1) * cos(phi2);
	double x = cos(phi1) * sin(phi2) -
			   sin(phi1) * cos(phi2) * cos(lambda2 - lambda1);

	// 计算方位角并将其转换为度数
	double bearing = atan2(y, x) * TO_DEG;

	// 调整方位角在0到360度之间
	if (bearing < 0.0) {
		bearing += 360.0;
	}

	return bearing;
}



// 目标经纬度
const double target_latitude = 22.304116;
const double target_longitude = 113.484167;

// 当前经纬度
double current_latitude = 22.303000;
double current_longitude = 113.482644;

#define MAX_DISTANCE 0.1 // 假设最大有效距离是0.1米，根据实际情况调整
#define PWM_MIN 3000
#define PWM_MAX 3600

/*113.4891718°，纬度22.3088105°*/

void PID_Motor_Contorl(void){
	int key;
	float current_heading_before = 0.0;
	float current_heading = 0.0;
	double ret[2];
	double ret_target[2];
	double ret_last[2];
	float heading_error = 0.0;
	float target_bearing = 0.0;
	float correction = 0.0;
	float left_pwm = 0.0;
	float right_pwm = 0.0;
	float board_distance = 0.0;
	float board_distance_last = 0.0;
	float return_heading = 0.0;
	int return_flag = 0;
	float target_speed = 0.0; // 目标速度
	float current_speed = 0.0; // 当前速度
	float speed_error = 0.0; // 速度误差
	float speed_correction = 0.0; // 速度校正值
	PID_Init(&heading_pid, 1.8f, 1.1f, 0.075f, 0.0f, -100.0f, 100.0f);
	 // 调整这些参数以适应你的系统
	while (1) {
		// 获取当前控制方式
		key = Key_Switch();
		Data_Collect();
		//Update_Data(ret[0],ret[1],ret_target[0],ret_target[1],current_heading,left_pwm,right_pwm,board_distance,heading_error,target_bearing);
		if(key == AUTO && Auto_Start_Flag == AUTO_START){
			// 读取当前航向角
			current_heading = get_heading_from_imu();
			//current_heading = convert_angle_0_to_360_to_180_to_neg180(current_heading_before)-5;

			// 计算目标方位角
			wgs84_to_bd09(board_config.gps.lon_deg, board_config.gps.lat_deg, ret);
			wgs84_to_bd09(board_config.gps.Target_gpslon[board_config.gps.now_number], board_config.gps.Target_gpslat[board_config.gps.now_number],ret_last);
			wgs84_to_bd09(board_config.gps.Goal_Lon, board_config.gps.Goal_Lat, ret_target);
			target_bearing = calculate_bearing(ret[1], ret[0],ret_target[1],ret_target[0]);

			// 计算误差
			heading_error = fmod(target_bearing - current_heading + 180.0f, 360.0f) - 180.0f;

			if(abs(heading_error) >= 25.0 && abs(heading_error) <= 45.0){
				//PID_Init(&heading_pid, 1.5f, 1.1f, 0.075f, 0.0f, -1200.0f, 1200.0f);
				if(heading_error < 0.0){
					left_pwm = 2500;  // 中间值3000，加上负修正
					right_pwm = 3600;  // 中间值3000，加上正修正
				}else if(heading_error >= 0.0){
					left_pwm = 3600;  // 中间值3000，加上负修正
					right_pwm = 2500;  // 中间值3000，加上正修正
				}
//					left_pwm = (uint32_t)(3000.0f - correction);  // 中间值3000，加上负修正
//					right_pwm = (uint32_t)(3000.0f + correction);  // 中间值3000，加上正修正
			}
			else if(abs(heading_error) > 45.0){
				if(heading_error < 0.0){
					left_pwm = 1800;  // 中间值3000，加上负修正
					right_pwm = 4200;  // 中间值3000，加上正修正
				}else if(heading_error >= 0.0){
					left_pwm = 4200;  // 中间值3000，加上负修正
					right_pwm = 1800;  // 中间值3000，加上正修正
				}
			}
			else{
				PID_Init(&heading_pid, 1.5f, 1.0f, 0.075f, 0.0f, -120.0f, 120.0f);
				correction = PID_Update(&heading_pid, heading_error, dt);
				left_pwm = (uint32_t)(3750.0f - correction);  // 中间值3000，加上负修正
				right_pwm = (uint32_t)(3750.0f + correction);  // 中间值3000，加上正修正
			}


			// 确保PWM值在有效范围内
			if (left_pwm > 4200) left_pwm = 4200;
			if (left_pwm < 1800) left_pwm = 1800;
			if (right_pwm > 4200) right_pwm = 4200;
			if (right_pwm < 1800) right_pwm = 1800;

			// 获取最后一个点的定位

			board_distance = get_distance(ret_target[1], ret_target[0], ret[1], ret[0]);

			board_distance_last = get_distance(ret_last[1], ret_last[0], ret[1], ret[0]);

			if (board_distance_last < 0.020  && board_distance_last > 0.015) {
				if(board_config.gps.now_number < board_config.gps.gps_num){
					board_config.gps.now_number++;
				}
				else{
					// 计算PWM值
					__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 2800);
					__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 2800);
				}
			}
			else if(board_distance_last <= 0.0015){
				Auto_Start_Flag = AUTO_STOP;
				board_config.gps.now_number = 0;
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 3000);
				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 3000);
			}
			else{
				if(ret[0] == ret_target[0] && ret[1] == ret_target[1]){
					left_pwm = 3000;  // 中间值3000，加上负修正
					right_pwm = 3000;  // 中间值3000，加上正修正
				}
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, left_pwm);  // 基础占空比加上修正
				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, right_pwm);  // 基础占空比加上修正
			}
			printf("current_heading:%f  target_bearing:%f  heading_error:%f\r\n",current_heading,target_bearing,heading_error);
			printf("%f,%f ------------>%f,%f\r\n",ret[0],ret[1],ret_target[0],ret_target[1]);
			printf("correction:%f  left_pwm:%f  right_pwm:%f\r\n",correction,left_pwm,right_pwm);
			printf("distance:%fKm\r\n",board_distance);
			printf("\r\n");
			// 等待下一个控制周期
			HAL_Delay((uint32_t)(dt * 1000));
		}
		else{
			board_config.gps.now_number = 0;
			reset_PID(&heading_pid);
		}
	}
}

float get_heading_from_imu(void) {
    // 实现串口通信以读取IMU的数据
    // 返回当前的航向角（单位：度）
    return board_config.Imu_data.Pitch;  // 示例返回值

}

float convert_angle_0_to_360_to_180_to_neg180(float angle) {
    // 确保角度在0到360度之间
    angle = fmod(angle, 360.0f);

    // 如果角度大于180度，减去360度
    if (angle > 180.0f) {
        angle -= 360.0f;
    }

    angle = -angle;

    return angle;
}
#define JS_BUFFER_SIZE 256
char jsonMessage[JS_BUFFER_SIZE];

void Update_Data(double lon1 , double lat1 , double lon2 , double lat2 , float yaw , float left_pwm , float right_pwm , float dis ,float error,float target){
	float yaws = convert_angle_0_to_360_to_180_to_neg180(board_config.Imu_data.Pitch);
	sprintf(jsonMessage,"{\"now_lon\":\"%f\",\"now_lat\":\"%f\",\"target_lon\":\"%f\",\"target_lat\":\"%f\",\"speed\":\"%s\",\"yaw\":\"%f\",\"left_pwm\":\"%f\",\"right_pwm\":\"%f\",\"distantce\":\"%f\",\"error\":\"%f\",\"target\":\"%f\"}\r\n",\
		lon1,lat1,lon2,lat2,board_config.gps.Speed,yaws,left_pwm,right_pwm,dis,error,target);
	HAL_UART_Transmit(&huart2,jsonMessage,strlen(jsonMessage),0xff);
	return ;
}

