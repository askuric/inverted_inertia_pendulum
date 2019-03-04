#include "pendulum.h"
#include "stm32f4xx_hal.h"
#include <math.h>

extern TIM_HandleTypeDef htim2; 
extern TIM_HandleTypeDef htim3; 

// motor structure definition
struct motor_s motor;
struct pendulum_s pendulum;

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
	static int cnt_1,real_count,d_cnt_1;
	static float dt; // sample time
	dt = (HAL_GetTick() - dt)*1e-3; // elapsed time calculation
	
	int cnt = TIM3->CNT;
	float d_cnt = cnt - cnt_1;
	
	// from [0,6000] to [-inf,inf]
	if (fabs(d_cnt) > CNT_MAX){
		if(cnt > cnt_1){
			d_cnt = -(CNT_OVERFLOW - cnt + cnt_1);
		}else if(cnt < cnt_1){
			d_cnt = CNT_OVERFLOW - cnt_1 + cnt;
		}
	}		
	
	// filtering d_pos Low pass
	d_cnt = (dt / (dt + M_WTf))*(d_cnt) + (1 - (dt / (dt + M_WTf)))*d_cnt_1;
	
	d_cnt_1 = d_cnt;
	real_count += d_cnt;
	cnt_1 = TIM3->CNT;
	motor.position = real_count*M_CPR2RAD; 
	motor.velocity = d_cnt/dt*M_CPR2RAD;
	
	dt = HAL_GetTick(); // saving timestap for next iteration
}

/**
	Function iteratively calculating velocity and 
	position of the motor1. It is intended to be called cyclicly 
	with the sample time defined in the header file "Ts"
	
	The function updates the structure motor1 nad its properties
	pendulum.position and pendulum.velocity
*/
void pendulum_variables_iter(){
	static int cnt_1,real_count,d_cnt_1;
	
	static float dt; // sample time
	dt = (HAL_GetTick() - dt)*1e-3; // elapsed time calculation
	
	int cnt = TIM4->CNT;
	int d_cnt = cnt - cnt_1;
	
	// from [0,6000] to [-inf,inf]
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
	// bound angle [-PI,PI]
	if((real_count*P_CPR2RAD - P_OFFSET) > 0){
		pendulum.position = fmod(real_count*P_CPR2RAD - P_OFFSET +PI,2*PI)-PI;
	}else{
		pendulum.position = fmod(real_count*P_CPR2RAD - P_OFFSET -PI,2*PI)+PI;
	}
	pendulum.velocity = d_cnt/dt*P_CPR2RAD;
	dt = HAL_GetTick();// saving timestap for next iteration
}

/**
	LQR stabilization
*/
void pendulum_LQR(){
	
	/** LQR Matlab generated controller
	* Q = diag([1,1,1]) R = [10]
	*/
	float Kr[3] = {330.1146,35.1388,0.19739};
	
	// P equation
	//float uk = 60000*pendulum.position; 
	// LQR equation
	float uk = Kr[0]*pendulum.position+Kr[1]*pendulum.velocity+Kr[2]*motor.velocity;
	
	uk = u2pwm_(uk);
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

/**
		Swing-up bang bang controller for the pendulum
*/
void pendulum_swingup(){
	// check if swingup needed
	if(fabs(pendulum.position) >= P_SWITCH_ANGLE){
		// simple hysteresys - not deterministic
		motor_setPwm(-sign_(pendulum.velocity)*M_Umax);
		
		
		float accel_sign;
		float E_kin,E_pot,E_tar;
		
		// kinetic energy
		E_kin = 0.5*P_Jp*pow(pendulum.velocity,2);
		// potential energy
		E_pot = P_Mp *P_g*P_Lcm*(1+cos(pendulum.position)); 
		// target energy
		pendulum.E_tar = E_target;
		pendulum.E_tot = E_kin+E_pot;
		
		// Bang bang controller
		if((1.0*pendulum.E_tar) > pendulum.E_tot){ 
			// if total energy too small accelerate
			accel_sign = 1;
		}else if((1.1*pendulum.E_tar) > pendulum.E_tot){ 
			// if in the area of target energy do nothing
			accel_sign = 0;
		}else{ 
			// if total energy too high decelerate
			accel_sign = -1;
		}
		
		motor_setPwm(-sign_(pendulum.velocity)*accel_sign*M_Umax);
		
	}
}

/*
	Disable motor of the pendulum
*/
void pendulum_disable(){
	
	motor_setPwm(0);
			
}
	

