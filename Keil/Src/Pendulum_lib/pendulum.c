#include "pendulum.h"
#include "stm32f4xx_hal.h"
#include <math.h>

extern TIM_HandleTypeDef htim2; 
extern TIM_HandleTypeDef htim3; 

// motor structure definition
struct actuator motor, pendulum;

/**
	function setting pwm duty cycle to the motor2
	input:
		pwm - int - [-100, 100]
*/
void motor_setPwm(int pwm){
	
	if(abs(pwm) > PWMmax){
		pwm = pwm > 0 ? PWMmax : -PWMmax;
	}
	
	if(pwm == 0){
		// disable motor
		HAL_GPIO_WritePin(GPIOA, M2_EN_Pin, GPIO_PIN_RESET);
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3, 0);
		return;
	}else{
		// enable motor
		HAL_GPIO_WritePin(GPIOA,M2_EN_Pin,GPIO_PIN_SET);
		if(pwm > 0){
			HAL_GPIO_WritePin(GPIOC,M2_CCW_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB,M2_CW_Pin,GPIO_PIN_SET);
		}else if(pwm < 0){
			HAL_GPIO_WritePin(GPIOB,M2_CW_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC,M2_CCW_Pin,GPIO_PIN_SET);
		}
		// set the pwm
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3, abs(pwm));
	}
}




/**
	Function iteratively calculating velocity and 
	position of the motor1. It is intended to be called cyclicly 
	with the sample time defined in the header file "Ts"
	
	The function updates the structure motor nad its properties
	motor.position and motor.velocity
*/
void motor_variables_iter(){
	static int cnt_1;
	static int real_count;
	static int d_cnt_1;
	int cnt = TIM3->CNT;
	float d_cnt = cnt - cnt_1;
	
	if (fabs(d_cnt) > CNT_MAX){
		if(cnt > cnt_1){
			d_cnt = -(CNT_OVERFLOW - cnt + cnt_1);
		}else if(cnt < cnt_1){
			d_cnt = CNT_OVERFLOW - cnt_1 + cnt;
		}
	}		
	d_cnt_1 = d_cnt;
	real_count += d_cnt;
	cnt_1 = TIM3->CNT;
	motor.position = real_count*M_CPR2RAD;
	motor.velocity = d_cnt/M_Ts*M_CPR2RAD;
}

/**
	Function iteratively calculating velocity and 
	position of the motor1. It is intended to be called cyclicly 
	with the sample time defined in the header file "Ts"
	
	The function updates the structure motor1 nad its properties
	pendulum.position and pendulum.velocity
*/
void pendulum_variables_iter(){
	static int cnt_1;
	static int real_count;
	static int d_cnt_1;
	int cnt = TIM4->CNT;
	int d_cnt = cnt - cnt_1;
	
	if (abs(d_cnt) > CNT_MAX){
		if(cnt > cnt_1){
			d_cnt = -(CNT_OVERFLOW - cnt + cnt_1);
		}else if(cnt < cnt_1){
			d_cnt = CNT_OVERFLOW - cnt_1 + cnt;
		}
	}		
	d_cnt_1 = d_cnt;
	real_count += d_cnt;
	cnt_1 = TIM4->CNT;
	pendulum.position = real_count*P_CPR2RAD - P_OFFSET;
	pendulum.velocity = d_cnt/P_Ts*P_CPR2RAD;
}

/**
	LQR stabilization
*/
void pendulum_LQR(){
	
	// LQR constants based on a dynamical model
	float Kr[3] = {440.8258,56.9627,0.21268};
	
	// P equation
	//float uk = 60000*pendulum.position; 
	// LQR equation
	float uk = Kr[0]*pendulum.position+Kr[1]*pendulum.velocity+Kr[2]*motor.velocity;
	
	uk = u2pwm(uk);
	// anti-windup
	if(abs((int)uk) > M_Umax) 	uk = uk > 0 ? M_Umax : -M_Umax;
	
	// set pwm to motor
	motor_setPwm(uk);
			
}

/**
	P stabilization controller
*/
void pendulum_P(){
	
	// P equation
	float uk = 60000*pendulum.position; 
	
	// anti-windup
	if(abs((int)uk) > M_Umax) 	uk = uk > 0 ? M_Umax : -M_Umax;
	
	// set pwm to motor
	motor_setPwm(uk);
			
}

/*
	Disable motor of the pendulum
*/
void pendulum_disable(){
	
	motor_setPwm(0);
			
}
	

