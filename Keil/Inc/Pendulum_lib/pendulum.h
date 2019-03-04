#ifndef PENDULUM_H
#define PENDULUM_H


/**
	Change this part to suite your project hardware	
**/
// motor - hall effect encoder 
#define M_CPR 20				
#define M_CPR2RAD (2.0*PI/M_CPR)
// pendulum - quadrature encoder
#define P_CPR (4*600)  	
#define P_CPR2RAD (2.0*PI/P_CPR)
#define P_OFFSET PI // encoder inital position offset

// maximal power supply
#define U_PWR  18  			
// angle where linear controller switchen in
#define P_SWITCH_ANGLE PI/10

// Control time constants
#define M_Ts 0.01 	// motor sampling time - oversamplig
#define P_Ts (0.05) // pendulum sampling time
#define M_WTf 0.05  // motor velocity filtering time constant

// Voltage and PWM constants
#define PWMmax 1000
#define M_Umax (PWMmax)
#define u2pwm_(u) (u*M_Umax/U_PWR)
#define sign_(a) (((a)<0) ? -1 : ((a)>0))


// SWING-UP parameters
// Physical system parameters  
// chnage once mechanical model changed
#define P_Mp   	0.236					// full pendulum mass
#define P_Mmw   0.170      		// motor+wheel mass
#define P_Lcm   0.110    			// center of mass distance
#define P_Jw   	162231e-9     // Wheel momentum of inertia
#define P_g  		9.81          // gravity acceleration
#define P_Jp   (P_Mp*pow(P_Lcm,2) + 1153811.05e-9)   // full pendulum momentum of inertia
#define E_target  (2*P_Mp*P_g*P_Lcm) // Potential energy of upright pendulum


// encoder parameters
#define CNT_MAX 50000.0
#define CNT_OVERFLOW 60000.0

#define PI 3.14159265359

// struct for geting motor variables
struct motor_s{
	float position;
	float velocity;
};
extern  struct motor_s motor;
struct pendulum_s{
	float position;
	float velocity;
	float E_tot; // current Ekin+Epot
	float E_tar; // Potential energy of upright pendulum
};
extern  struct pendulum_s pendulum;

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
		Swingup bang bang controller for the pendulum
*/
void pendulum_swingup();

/*
	Disable motor of the pendulum
*/
void pendulum_disable();


#endif /* MOTOR_H */