#ifndef __PID_H__
#define __PID_H__

#include "main.h"
#include "board_init.h"

typedef struct {
    float Kp;          // 比例增益
    float Ki;          // 积分增益
    float Kd;          // 微分增益
    float setpoint;    // 设定点
    float integral;    // 积分项
    float previous_error;  // 上一次误差
    float last_input; // 上一次的输入
    float min_output;  // 输出最小限制
    float max_output;  // 输出最大限制
} PIDController;

typedef struct {
	float min_in;
	float max_in;
}PID_IN;

#define Auto_Normal 0
#define Auto_Return 1

extern PIDController heading_pid;
extern PID_IN heading_in;

extern float dt;
extern int auto_flag;
extern int Auto_Status;

void PID_Init(PIDController *pid, float Kp, float Ki, float Kd, float setpoint, float min_output, float max_output);
float PID_Update(PIDController *pid, float measured_value, float dt);
float get_heading_from_imu(void);
float calculate_bearing(double lat1, double lon1, double lat2, double lon2);
float convert_angle_0_to_360_to_180_to_neg180(float angle);
void reset_PID(PIDController *pid);
void Update_Data(double lon1 , double lat1 , double lon2 , double lat2 , float yaw , float left_pwm , float right_pwm , float dis ,float error,float target);



#endif /* __PID_H___H_ */
