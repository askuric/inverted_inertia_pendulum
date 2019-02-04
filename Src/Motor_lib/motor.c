#include "motor.h"
#include "stm32f4xx_hal.h"
#include <math.h>

extern TIM_HandleTypeDef htim2; 
extern TIM_HandleTypeDef htim3; 

// motor structure definition
struct motor motor1,motor2,pendulum;

/**
	function setting pwm duty cycle to the motor1
	input:
		pwm - int - [-100, 100]
*/
void motor1_setPwm(int pwm){
	
	if(abs(pwm) > PWMmax){
		pwm = pwm > 0 ? PWMmax : -PWMmax;
	}
	
	if(pwm == 0){
		// disable motor
		HAL_GPIO_WritePin(GPIOA, M1_EN_Pin, GPIO_PIN_RESET);
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1, 0);
		return;
	}else{
		// enable motor
		HAL_GPIO_WritePin(GPIOA,M1_EN_Pin,GPIO_PIN_SET);
		if(pwm < 0){
			HAL_GPIO_WritePin(GPIOA,M1_CCW_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA,M1_CW_Pin,GPIO_PIN_SET);
		}else if(pwm > 0){
			HAL_GPIO_WritePin(GPIOA,M1_CW_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA,M1_CCW_Pin,GPIO_PIN_SET);
		}
		// set the pwm
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1, abs(pwm));
	}
}

/**
	function setting pwm duty cycle to the motor2
	input:
		pwm - int - [-100, 100]
*/
void motor2_setPwm(int pwm){
	
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
	position of the motor2. It is intended to be called cyclicly 
	with the sample time defined in the header file "Ts"
	
	The function updates the structure motor1 nad its properties
	motor1.position and motor1.velocity
*/
void motor1_variables_iter(){
	static int cnt_1;
	static int real_count;
	static int d_cnt_1;
	int cnt = TIM3->CNT;
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
	cnt_1 = TIM3->CNT;
	motor1.position = real_count*M1_CPR2RAD;
	motor1.velocity = d_cnt*M1_CPR2RAD/M1_Ts;
}

/**
	Function iteratively calculating velocity and 
	position of the motor1. It is intended to be called cyclicly 
	with the sample time defined in the header file "Ts"
	
	The function updates the structure motor1 nad its properties
	motor2.position and motor2.velocity
*/
void motor2_variables_iter(){
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
	motor2.position = real_count*M2_CPR2RAD;
	motor2.velocity = d_cnt/M2_Ts*M2_CPR2RAD;
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
	pendulum.position = real_count*PEN_CPR2RAD - PEN_OFFSET;
	pendulum.velocity = d_cnt/M2_Ts*PEN_CPR2RAD;
}

/**
	LQR stabilization

*/
void pendulum_LQR(){
	
	float Kr[3] = {440.8258,56.9627,0.21268};
	
	// P equation
	//float uk = 60000*pendulum.position; 
	// LQR equation
	float uk = Kr[0]*pendulum.position+Kr[1]*pendulum.velocity+Kr[2]*motor2.velocity;
	
	uk = u2pwm(uk);
	// anti-windup
	if(abs((int)uk) > M2_Umax) 	uk = uk > 0 ? M2_Umax : -M2_Umax;
	
	// set pwm to motor
	motor2_setPwm(uk);
			
}

/*
	Disable motor of the pendulum
*/
void pendulum_disable(){
	
	motor2_setPwm(0);
			
}


/**
	Motor2 PI velocity regulator
	PI controller constants are defined in the "motor.h"
*/
void motor2_velocityPI(float vel_SP){
	static float ek_1, uk_1;	
	
	// controller error
	float ek = vel_SP - motor2.velocity;
	
	// PI equation
	float uk = uk_1 + M2_Kr*(M2_Ts/(2*M2_Ti)+1)*ek + M2_Kr*(M2_Ts/(2*M2_Ti)-1)*ek_1;
	
	// anti-windup
	if(abs((int)uk) > M2_Umax) 	uk = uk > 0 ? M2_Umax : -M2_Umax;
	
	// set pwm to motor
	motor2_setPwm((int)uk);
	
	uk_1 = uk;
	ek_1 = ek;
		
}

/**
	Motor2 P position regulator
	P controller constants are defined in the "motor.h"
*/
void motor2_positionP(float pos_SP){
	
	// controller error
	float ek = pos_SP - motor2.position;
	
	// P equation
	float uk = M2_P*ek;
	
	// anti-windup
	if(abs((int)uk) > M2_Umax) 	uk = uk > 0 ? M2_Umax : -M2_Umax;
	
	// set pwm to motor
	motor2_velocityPI(uk);
			
}

/**
	Motor3 PI velocity regulator
	PI controller constants are defined in the "motor.h"
*/
void motor1_velocityPI(float vel_SP){
	static float ek_1, uk_1;	
	
	// controller error
	float ek = vel_SP - motor1.velocity;
	
	// PI equation
	float uk = uk_1 + M1_Kr*(M1_Ts/(2*M1_Ti)+1)*ek + M1_Kr*(M1_Ts/(2*M1_Ti)-1)*ek_1;
	
	// anti-windup
	if(abs((int)uk) > M1_Umax) 	uk = uk > 0 ? M1_Umax : -M1_Umax;
	
	// set pwm to motor
	motor1_setPwm((int)uk);
	
	uk_1 = uk;
	ek_1 = ek;
		
}

/**
	Motor3 P position regulator
	P controller constants are defined in the "motor.h"
*/
void motor1_positionP(float pos_SP){
	
	// controller error
	float ek = pos_SP - motor1.position;
	
	// P equation
	float uk = M1_P*ek;
	
	// anti-windup
	if(abs((int)uk) > M1_Umax) 	uk = uk > 0 ? M1_Umax : -M1_Umax;
	
	//motor3_velocityPI(uk);
	motor1_velocityPI(uk);
}

