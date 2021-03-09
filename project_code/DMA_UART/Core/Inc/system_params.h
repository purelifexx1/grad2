/*
 * system_params.h
 *
 *  Created on: May 26, 2020
 *      Author: Dang Nam
 */

#ifndef INC_SYSTEM_PARAMS_H_
#define INC_SYSTEM_PARAMS_H_

/* Limit Joint*/
#define LIM_MIN_J0		(-85.0*PI/180.0)
#define LIM_MAX_J0		(85.0*PI/180.0)
#define LIM_MIN_J1		(-135.0*PI/180.0)
#define LIM_MAX_J1		(135.0*PI/180.0)
#define LIM_MIN_J2		(0.0)
#define LIM_MAX_J2		(100.0)
#define LIM_MIN_J3		(-170.0*PI/180)
#define LIM_MAX_J3		(170.0*PI/180.0)

/* Maximum Velocity System*/
#define V_DESIGN_J0		(0.9*LIM_PULSE_J0*2*PI)/(GEAR_J0*T_SAMPLING)
#define V_DESIGN_J1		(0.9*LIM_PULSE_J1*2*PI)/(GEAR_J1*T_SAMPLING)
#define V_DESIGN_J2		(0.9*LIM_PULSE_J2)/(GEAR_J2*T_SAMPLING)
#define V_DESIGN_J3		(0.9*LIM_PULSE_J3*2*PI)/(GEAR_J3*T_SAMPLING)

#define V_MOVE_MAX      750 //912.5mm/s maximum speed calculate from jacobi matrix
#define A_MOVE_MAX 		500

#define V_DESIGN_3D		V_DESIGN_J2
#define V_DESIGN_ROLL	V_DESIGN_J0


/* Maximum Accelerate System*/
#define A_DESIGN_3D		(V_DESIGN_3D/1.0)
#define A_DESIGN_ROLL	(V_DESIGN_ROLL/1.0)
#define A_DESIGN_J0		(V_DESIGN_J0/1.0)
#define A_DESIGN_J1		(V_DESIGN_J1/1.0)
#define A_DESIGN_J2		(V_DESIGN_J2/1.0)
#define A_DESIGN_J3		(V_DESIGN_J3/1.0)

/* Sampling Period*/
#define T_SAMPLING		(0.01f)


/* Motor Positive( Anti-Clockwise) Direction ---> Driver */
#define DIR_J0				(0x01)
#define DIR_J1				(0x01)
#define DIR_J2				(0x00)
#define DIR_J3				(0x01)

/* Encoder Increase Direction --- CPLD count independently */
#define DIR_ENCODER_0		(1.0f)
#define DIR_ENCODER_1		(1.0f)
#define DIR_ENCODER_2		(-1.0f)

/* Encoder Ratio */
#define ENCODER_J0				(20000.0f*4.0f) // per round ( 2*PI)
// baud rate
#define ENCODER_J1				(32000.0f*4.0f) // per round ( 2*PI)
// baud rate, ball screw T3
#define ENCODER_J2				(500.0f*4.0f/3.0f) // per milimeter.


/* Motor Ratio -- Driver */
// baud rate
#define GEAR_J0				(20000.0f) // per round ( 2*PI)
// baud rate
#define GEAR_J1				(32000.0f) // per round ( 2*PI)
// baud rate, ball screw T3
#define GEAR_J2				(300.0f/3.0f) // per milimeter.
// motor, micro step, big-gear, small-gear
#define GEAR_J3				(400.0f*16.0f*60.0f/19.0f) // per round ( 2*PI)


/* Limit pulse per 10 ms (pulse) */
#define LIM_PULSE_J0		(75u) // Calculate in file Excel
#define LIM_PULSE_J1		(120u)
#define LIM_PULSE_J2		(125u)
#define LIM_PULSE_J3		(101u)

/* Module DDA Address (8 bit data) */
#define ADDRESS_DDA_0 			(0x60000000)
#define ADDRESS_DDA_1 			(0x60000002)
#define ADDRESS_DDA_2 			(0x60000004)
#define ADDRESS_DDA_3 			(0x60000006)

/* Limit Switch Address (8 bit data) */
#define ADDRESS_LIMIT 			(0x60000020)

/* Encoder Registers Address (16 bit data) */
// Encoder 32 bit counter (mode x4)
#define ADDRESS_ENC0_LOW		(0x60000040)
#define ADDRESS_ENC0_HIGH		(0x60000042)
#define ADDRESS_ENC1_LOW		(0x60000044)
#define ADDRESS_ENC1_HIGH		(0x60000046)
#define ADDRESS_ENC2_LOW		(0x60000048)
#define ADDRESS_ENC2_HIGH		(0x6000004A)

/* Limit Capture Registers Address (16 bit data) */
#define ADDRESS_CAP0_LOW		(0x60000060)
#define ADDRESS_CAP0_HIGH		(0x60000062)
#define ADDRESS_CAP1_LOW		(0x60000064)
#define ADDRESS_CAP1_HIGH		(0x60000066)
#define ADDRESS_CAP2_LOW		(0x60000068)
#define ADDRESS_CAP2_HIGH		(0x6000006A)

/* Hard Limit Absolute Position (Calibration) (rad) */
#define HARD_LIM0_NEG		(-1.621454508f)
#define HARD_LIM1_POS		(2.448773299f)
#define HARD_LIM2_NEG		(-2.236f)
#define HARD_LIM3_POS		(3.024928301f)


/* Basic keyboard shift unit*/
#define SHIFT_VAR0		(0.052359878f)
#define SHIFT_VAR1		(0.052359878f)
#define SHIFT_VAR2		(3.0f)
#define SHIFT_VAR3		(0.052359878f)
#define SHIFT_3D		(3.0f)
#define SHIFT_ROLL		(0.052359878f)

/* Shift period */
#define SHIFT_PERIOD	(1.2f)
#define SHIFT_T_UP		(0.264f)
#define SHIFT_KEY_TIME	0.05

/* Shift speed limit */
#define SHIFT_SPEED_MIN	(1)
#define SHIFT_SPEED_MAX	(14)
#define SHIFT_KEY_S		5

/* Kinematic Parameters (mm) */
#define		d1				(211.0f)
#define 	a1				(197.0f)
#define		a2				(160.0f)
#define		d4				(77.674f)
#define		a4				(32.36f)

/* Coordinate value scale number for transmission */
#define COR_FOWARD_SCALE 1000000.0f
#define COR_INVERSE_SCALE 0.000001f

/* Auto pick and place operation parameter definition */
#define PUT_DOWN_TIME_ON_SLOT 	0.6f
#define PUT_DOWN_TIME_ON_OBJECT	0.6f
#define PICK_UP_TIME_ON_OBJECT 	0.6f
#define PICK_UP_TIME_ON_SLOT	0.6f
#define MOVE_TIME 				1.2f
#define ATTACH_TIME 			0.01f
#define	DETACH_TIME 			0.01f
#define	TOTAL_TIME 				(PUT_DOWN_TIME + PICK_UP_TIME + MOVE_TIME + ATTACH_TIME + DETACH_TIME)
#define	TIMER_SCALE 			0.000297619f // 25e3/84e6
#define UP_HEIGHT 				131.0f
#define DOWN_HEIGHT_ON_OBJECT 	126.5f
#define DOWN_HEIGHT_ON_SLOT 	126.5f
//#define CONVEYOR_SPEED 30.0f //mm/s

typedef enum{
	VIETNAM_FLAG,
	JAPAN_FLAG,
	ENGLAND_FLAG,
	SWITZERLAND_FLAG,
	SWEDEN_FLAG,
	GERMANY_FLAG,
	NUM_OF_OBJECT
}ObjectType;

typedef enum
{
    FIRST_PACKET,                 //these contains in 4 bit low
    LINEAR_TYPE,   //G00, G01     //these contains in 4 bit low
    ARC_CW_TYPE,   //G02          //these contains in 4 bit low
    ARC_AW_TYPE,   //G03          //these contains in 4 bit low
    UP_Z,                         //these contains in 4 bit high
    DOWN_Z                        //these contains in 4 bit high
}Gcode_Packet_Command_TypeDef;

#endif /* INC_SYSTEM_PARAMS_H_ */
