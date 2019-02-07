#ifndef PENDULUM_H
#define PENDULUM_H


/**
	Change this part to suite your project hardware	
**/

// motor - hall effect encoder 
#define M_CPR 20				
// pendulum - quadrature encoder
#define P_CPR (4*600)  	
// maximal power supply
#define U_PWR  18  			
// encoder inital position offset
#define P_OFFSET 1.48 //rad


#define PI 3.14159265359

#define CNT_MAX 30000.0
#define CNT_OVERFLOW 60000.0

#define PWMmax 1000
#define M_Umax 1000


#define M_CPR2RAD (2.0*PI/M_CPR)

#define P_CPR2RAD (2.0*PI/P_CPR)


#define u2pwm(u) (u*M_Umax/U_PWR)

#define M_Ts 0.05
#define P_Ts (M_Ts)

// struct for geting motor variables
struct actuator{
	float position;
	float velocity;
	float setPoint;
	float current;
};
extern  struct actuator motor, pendulum;

/**
	function setting pwm duty cycle to the motor1
	input:
		pwm - int - [-100, 100]
*/	
void motor_setPwm(int pwm);



/**
	Function iteratively calculating velocity and 
	position of the motor. It is intended to be called cyclicly 
	with the sample time defined in the header file "Ts"
	
	The function updates the structure motor and its properties
	motor.position and motor.velocity
*/
void motor_variables_iter();


/**
	Function iteratively calculating velocity and 
	position of the poendulum. It is intended to be called cyclicly 
	with the sample time defined in the header file "Ts"
	
	The function updates the structure pendulum nad its properties
	pendulum.position and pendulum.velocity
*/
void pendulum_variables_iter();

/**
	LQR stabilization controller

*/
void pendulum_LQR();


/**
	P stabilization controller
*/
void pendulum_P();

/*
	Disable motor of the pendulum
*/
void pendulum_disable();


#endif /* MOTOR_H */