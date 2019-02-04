#ifndef MOTOR_H
#define MOTOR_H


#define PI 3.14159265359

#define CNT_MAX 30000.0
#define CNT_OVERFLOW 60000.0

// motor2 position P constants
#define M1_P 100.0
// motor1 velocity PI constants
#define M1_Kr 30.0
#define M1_Ti 0.1
#define M1_Ts 0.05


// motor2 position P constants
#define M2_P 100.0
// motor2 velocity PI constants
#define M2_Kr 10.0
#define M2_Ti 0.1
#define M2_Ts 0.050

#define PWMmax 1000

#define M1_Umax 1000
#define M2_Umax 1000

#define M1_RATIO (20.0) 
#define M1_CPR2RAD (2.0*PI/M1_RATIO)

#define M2_RATIO (20.0) 
#define M2_CPR2RAD (2.0*PI/M2_RATIO)

#define PEN_RATIO (4.0*600.0)
#define PEN_CPR2RAD (2.0*PI/PEN_RATIO)
#define PEN_OFFSET 1.48 //rad

#define U_MAX  18
#define u2pwm(u) (u*1000/U_MAX)

// struct for geting motor variables
struct motor{
	float position;
	float velocity;
	float setPoint;
	float current;
};
extern  struct motor motor1, motor2, pendulum;

/**
	function setting pwm duty cycle to the motor1
	input:
		pwm - int - [-100, 100]
*/	
void motor1_setPwm(int pwm);

/**
	function setting pwm duty cycle to the motor2
	input:
		pwm - int - [-100, 100]
*/
void motor2_setPwm(int pwm);



/**
	Function iteratively calculating velocity and 
	position of the motor2. It is intended to be called cyclicly 
	with the sample time defined in the header file "Ts"
	
	The function updates the structure motor1 nad its properties
	motor1.position and motor1.velocity
*/
void motor1_variables_iter();

/**
	Function iteratively calculating velocity and 
	position of the motor1. It is intended to be called cyclicly 
	with the sample time defined in the header file "Ts"
	
	The function updates the structure motor1 nad its properties
	motor2.position and motor2.velocity
*/
void motor2_variables_iter();

/**
	Function iteratively calculating velocity and 
	position of the motor1. It is intended to be called cyclicly 
	with the sample time defined in the header file "Ts"
	
	The function updates the structure motor1 nad its properties
	pendulum.position and pendulum.velocity
*/
void pendulum_variables_iter();

/**
	LQR stabilization

*/
void pendulum_LQR();
/*
	Disable motor of the pendulum
*/
void pendulum_disable();
/**
	Motor1 PI velocity regulator
	PI controller constants are defined in the "motor.h"
*/
void motor1_velocityPI(float vel_SP);

/**
	Motor2 P position regulator
	P controller constants are defined in the "motor.h"
*/
void motor2_positionP(float pos_SP);

/**
	Motor3 P position regulator
	P controller constants are defined in the "motor.h"
*/
void motor1_positionP(float pos_SP);


#endif /* MOTOR_H */