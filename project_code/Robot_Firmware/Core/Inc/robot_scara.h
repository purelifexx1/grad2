/*
 * robot_scara.h
 *
 *  Created on: Mar 10, 2020
 *      Author: Dang Nam
 */

#ifndef INC_ROBOT_SCARA_H_
#define INC_ROBOT_SCARA_H_

#include "common_def.h"
#include "system_params.h"


/* FOR ROBOT */
typedef enum
{
	  SCARA_STATUS_OK					= 0x00U,
	  SCARA_STATUS_ERROR				= 0x01U,
	  SCARA_STATUS_ERROR_SPACE			= 0x02U,
	  SCARA_STATUS_ERROR_TASK			= 0x03U,
	  SCARA_STATUS_ERROR_JOINT			= 0x04U,
	  SCARA_STATUS_ERROR_TRAJECTORY		= 0x05U,
	  SCARA_STATUS_ERROR_PARA			= 0x06U,
	  SCARA_STATUS_ERROR_OVER_WORKSPACE = 0x07U,
	  SCARA_STATUS_ERROR_MODE_INIT 		= 0x08U,
	  SCARA_STATUS_ERROR_OVER_VELOC		= 0x09U,
	  SCARA_STATUS_ERROR_OVER_ACCEL		= 0x0AU,
	  SCARA_STATUS_ERROR_JOINT_NUM		= 0x0BU,
	  SCARA_STATUS_ERROR_COORDINATE		= 0x0CU,
	  NUM_OF_STATUS						= 0x0DU
}SCARA_StatusTypeDef;

typedef enum
{
	  SCARA_METHOD_MANUAL					= 0x00U,  /*!< Control by joy stick */
	  SCARA_METHOD_SEMI_AUTO				= 0x01U,  /*!< Control by single command: MOVJ, MOVL, MOVC   */
	  SCARA_METHOD_GCODE					= 0x02U,   /*!< Control by job file  */
	  SCARA_METHOD_TEST						= 0X03U,
	  SCARA_METHOD_PICK_AND_PLACE			= 0x04U
}SCARA_MethodTypeDef;

typedef enum
{
	SCARA_METHOD_IDLE,
	SCARA_METHOD_ACTIVE,
}SCARA_MethodChangeState;

typedef enum
{
	  SCARA_MODE_STOP					= 0x00U,  /*!< SCARA scan stop immediate  */
	  SCARA_MODE_SCAN					= 0x01U,  /*!< SCARA scan limit switch to determine absolute position*/
	  SCARA_MODE_DUTY					= 0x02U   /*!< SCARA work  */
}SCARA_ModeTypeDef;

typedef enum
{
	  SCARA_DUTY_STATE_READY			= 0x00U,  /*!< There is not duty yet */
	  SCARA_DUTY_STATE_INIT				= 0x01U,  /*!< Init duty   */
	  SCARA_DUTY_STATE_FLOW				= 0x02U,  /*!< FLow in duty */
	  SCARA_DUTY_STATE_FINISH			= 0x03U,  /*!< Fishish duty */
	  SCARA_DUTY_STATE_OPERATION		= 0X04U
}SCARA_DutyStateTypeDef;

typedef enum
{
	  SCARA_KEY_STATE_READY			= 0x00U,  /*!< There is not key yet */
	  SCARA_KEY_STATE_INIT			= 0x01U,  /*!< Init key   */
	  SCARA_KEY_STATE_FLOW			= 0x02U,  /*!< FLow in key */
	  SCARA_KEY_STATE_FINISH		= 0x03U  /*!< Fishish key */
}SCARA_KeyStateTypeDef;

typedef enum
{
	SCARA_PICK_STATE_DOWN,
	SCARA_PICK_STATE_ATTACH,
	SCARA_PICK_STATE_UP
}SCARA_PickStateTypeDef;

typedef enum
{
	SCARA_PLACE_STATE_DOWN,
	SCARA_PLACE_STATE_DETACH,
	SCARA_PLACE_STATE_UP
}SCARA_PlaceStateTypeDef;

typedef enum
{
	  SCARA_SCAN_STATE_INIT				= 0x00U,
	  SCARA_SCAN_STATE_HARD				= 0x01U,
	  SCARA_SCAN_STATE_SOFT				= 0x02U,
	  SCARA_SCAN_STATE_FINISH			= 0x03U
}SCARA_ScanStateTypeDef;

/* FOR DUTY */
typedef enum
{
	  DUTY_COORDINATES_ABS				= 0x00U,  /*!< Absolute position */
	  DUTY_COORDINATES_REL				= 0x01U  /*!< Relative position*/
}CoordinatesTypeDef;

typedef enum
{
	  DUTY_MODE_INIT_QVA				= 0x00U,  /*!< Consume A max, determine T max */
	  DUTY_MODE_INIT_QVT				= 0x01U,  /*!< Consume T max, determine A max   */
	  DUTY_MODE_INIT_QV				    = 0x02U,
	  DUTY_MODE_INIT_QT					= 0x03U
}ModeInitTypeDef;

typedef enum
{
	  DUTY_SPACE_TASK					= 0x00U,  /*!< Task space */
	  DUTY_SPACE_JOINT					= 0x01U  /*!< Joint space  */
}SpaceTypeDef;

typedef enum
{
	  DUTY_PATH_LINE					= 0x00U,  /*!< Path planning straight line in task space */
	  DUTY_PATH_CIRCLE					= 0x01U,  /*!< Path planning circle in task space */
	  DUTY_PATH_BEZIER_CURVE			= 0x02U
}PathTypeDef;

typedef enum
{
	  DUTY_JOINT_SINGLE					= 0x00U,  /*!< Robot rotate a single joint */
	  DUTY_JOINT_4DOF					= 0x01U  /*!< Robot  */
}JointTypeDef;

typedef enum
{
	  DUTY_TRAJECTORY_LSPB				= 0x00U,  /*!< Trajectory planning LSBP */
	  DUTY_TRAJECTORY_SCURVE			= 0x01U,  /*!< Trajectory planning S-curve */
	  DUTY_TRAJECTORY_LINEAR			= 0x02U,
	  DUTY_TRAJECTORY_GCODE_LSPB		= 0x03U,
	  DUTY_TRAJECTORY_BEZIER_CURVE		= 0x04U
}TrajectoryTypeDef;

typedef enum
{
	  TRAJECTORY_J0				= 0x00U,  /*!< Trajectory planning for rad */
	  TRAJECTORY_J1				= 0x01U,  /*!< Trajectory planning for rad */
	  TRAJECTORY_J2				= 0x02U,  /*!< Trajectory planning for rad */
	  TRAJECTORY_J3				= 0x03U,  /*!< Trajectory planning for rad */
	  TRAJECTORY_3D				= 0x04U,  /*!< Trajectory planning for mm  */
	  TRAJECTORY_ROLL			= 0x05U,  /*!< Trajectory planning for mm  */
}Trajectory_TargetTypeDef;

typedef enum
{
    SCARA_KEY_X_INC,
    SCARA_KEY_X_DEC,
    SCARA_KEY_Y_INC,
    SCARA_KEY_Y_DEC,
    SCARA_KEY_Z_INC,
    SCARA_KEY_Z_DEC,
    SCARA_KEY_ROLL_INC,
    SCARA_KEY_ROLL_DEC,
//    SCARA_KEY_VAR0_INC,
//    SCARA_KEY_VAR0_DEC,
//    SCARA_KEY_VAR1_INC,
//    SCARA_KEY_VAR1_DEC,
//    SCARA_KEY_VAR2_INC,
//    SCARA_KEY_VAR2_DEC,
//    SCARA_KEY_VAR3_INC,
//    SCARA_KEY_VAR3_DEC,// 16 key board
}SCARA_KeyTypeDef;

/*** Instance Form ***/
typedef struct
{
	double						q;
//	double						q_roll;
//	double						q_theta1;
//	double						q_theta2;
//	double						q_d3;
//	double						q_theta4;
	uint64_t					packet_time_stamp;
	double						t;
	double						total_time;
	double 						x;
	double 						y;
	double 						z;
	double						roll;
	double 						Theta1;//Rotate
	double 						Theta2;//Rotate
	double 						D3;//Transpose
	double 						Theta4;//Rotate
//	double						v_theta1;
//	double						v_theta2;
//	double						v_d3;
//	double						v_theta4;
	double						v_3d;
	double						v_roll;
}SCARA_PositionTypeDef;

typedef struct
{
	int8_t dir;
	double constant_v;
	double total_s;
	double Tf;
	uint32_t number_of_sample;
}Trajectory_Linear_TypeDef;

typedef struct
{
	int8_t			 dir;
	double			 s0;
	double 			 s1;
	double			 v0;
	double			 v1;
	double 			 v_design;
	double 			 a_design;
	double			 v_lim;
	double			 Ta;
	double			 Td;
	double			 Tf;
	uint32_t		 num_of_sampling;
	double			 total_s;
	double			 a_current;
	double			 v_current;
	double			 s_current;

}Trajectory_LSPB_TypeDef;


typedef struct
{
	int8_t			 dir;
	double			 s0;
	double 			 s1;
	double			 v0;
	double			 v1;
	double 			 v_design;
	double 			 a_design;
	double			 v_lim;
	uint8_t			 num_of_phase;
	double			 j_max;
	double			 Td;
	double			 Ta;
	double			 Tf;
	uint32_t		 num_of_sampling;
	double			 total_s;
	double			 k_1;
	double			 k_2;
	double			 k_3;
	double			 k_4;
	double			 k_5;
	double			 k_6;
	double			 k_7;
	double			 k_8;
	double			 a_current;
	double			 v_current;
	double			 s_current;
}Trajectory_Scurve_TypeDef;


typedef struct
{
	double			 x0;
	double			 x1;
	double			 y0;
	double			 y1;
	double			 z0;
	double			 z1;
	double			 denta_x;
	double			 denta_y;
	double			 denta_z;
	double			 total_s;
	double			 delta_s;
	double			 x_current;
	double			 y_current;
	double			 z_current;
}Path_Line_TypeDef;


typedef struct
{
	int8_t			 dir;
	double			 radius;
	double			 angle_start;
	double			 angle_stop;
	double			 x0;
	double			 x1;
	double			 y0;
	double			 y1;
	double			 xi;
	double			 yi;
	double			 z0;
	double			 z1;
	double			 zi;
	double			 total_s;
	double			 total_angle;
	double			 x_current;
	double			 y_current;
	double			 z_current;
}Path_Circle_TypeDef;


typedef struct
{
	PathTypeDef 			path_type;
	Path_Line_TypeDef 		line;
	Path_Circle_TypeDef 	circle;
}Path_TypeDef;


typedef struct
{
	TrajectoryTypeDef 					trajectory_type;
	Trajectory_LSPB_TypeDef 			lspb;
	Trajectory_Scurve_TypeDef			scurve;
	Trajectory_Linear_TypeDef			linear;
}Trajectory_TypeDef;


typedef struct
{
	Path_TypeDef	 		path;
	Trajectory_TypeDef 		trajectory_3d;
	Trajectory_TypeDef 		trajectory_roll;
	double					roll_start;
}DUTY_Task_TypeDef;


typedef struct
{
	Trajectory_TypeDef		trajectory[4];
	double					theta1_start;
	double					theta2_start;
	double					d3_start;
	double					theta4_start;
}DUTY_Joint_TypeDef;


typedef struct
{
	SpaceTypeDef		space_type;
	DUTY_Task_TypeDef	task;
	DUTY_Joint_TypeDef	joint;
	double				time_total;
}DUTY_TypeDef;

typedef enum
{
    SCARA_TEST_MOTOR1_POS,
    SCARA_TEST_MOTOR1_NEG,
    SCARA_TEST_MOTOR2_POS,
    SCARA_TEST_MOTOR2_NEG,
    SCARA_TEST_MOTOR3_POS,
    SCARA_TEST_MOTOR3_NEG,
    SCARA_TEST_MOTOR4_POS,
    SCARA_TEST_MOTOR4_NEG,
	SCARA_TEST_MOTOR_STOP
}SCARA_TestMode;

typedef struct
{
	uint8_t						change_method;
	SCARA_MethodTypeDef			robot_method;
	SCARA_ModeTypeDef			robot_mode;
	SCARA_KeyTypeDef			keyboard;
	int32_t						key_speed;
	int32_t 					id_command;
	CoordinatesTypeDef			coordinate_type;
	SpaceTypeDef				space_type;
	PathTypeDef 				path_type;
	JointTypeDef				joint_type;
	TrajectoryTypeDef			trajec_type;
	ModeInitTypeDef				modeInit_type;
	int32_t						arc_dir;
	double						sub_para_double;
	double						time_total;
	SCARA_PositionTypeDef		target_point;
	SCARA_PositionTypeDef		sub_point;
	double						v_factor;
	double						a_factor;
	SCARA_TestMode				test_key;
}DUTY_Command_TypeDef;

typedef struct
{
	SCARA_MethodTypeDef			method;
	SCARA_ModeTypeDef 			mode;
	SCARA_DutyStateTypeDef 		duty_State;
	uint8_t						isScanLitmit;
	uint8_t						outputSet;
}SCARA_TypeDef;

typedef struct
{
	SCARA_PositionTypeDef 		object_position;
	SCARA_PositionTypeDef		slot_position;
	uint64_t					timer_value;
	ObjectType					object_type;
}SCARA_Pick_And_Place_Package;

typedef enum
{
	SCARA_MOVE_TO_TARGET         ,
	SCARA_MOVE_DOWN_ON_OBJECT	 ,
	SCARA_ATTACH		         ,
	SCARA_MOVE_UP_ON_OBJECT		 ,
	SCARA_MOVE_TO_SLOT           ,
	SCARA_MOVE_DOWN_ON_SLOT	     ,
	SCARA_RELEASE		         ,
	SCARA_MOVE_UP_ON_SLOT		 ,
}SCARA_Pick_And_Place_State;

typedef struct
{
	double posx;
	double posy;
	double roll;
}SCARA_Slot_TypeDef;
typedef struct
{
	uint16_t 						clutch_index; //
	Gcode_Packet_Command_TypeDef    type_define[2]; //0 for move type, 1 for height
}Gcode_Cor_Properties_TypeDef;
typedef struct
{
	int32_t                         X;
	int32_t                         Y;
	int32_t                         F;
	int32_t                         I;
	int32_t                         J;
	int32_t							T;
	Gcode_Cor_Properties_TypeDef    configure;
}SCARA_Gcode_Cor_TypeDef;
typedef struct
{
	int32_t 						total_s;
	int32_t							veloc;
	int32_t 						Depth_Feed; //if needed
}SCARA_LSPB_Clutch_TypeDef;

/* Function prototype */
void						scaraStartup		(void);
SCARA_StatusTypeDef			scaraInitDuty		(DUTY_Command_TypeDef command);

SCARA_StatusTypeDef			scaraInitLine		(Path_Line_TypeDef *line,
												SCARA_PositionTypeDef start,
												SCARA_PositionTypeDef end);

SCARA_StatusTypeDef			scaraInitCircle		(Path_Circle_TypeDef *circle,
												SCARA_PositionTypeDef start,
												SCARA_PositionTypeDef end,
												SCARA_PositionTypeDef center,
												int32_t dir);

SCARA_StatusTypeDef	scaraInitLSPB		(Trajectory_LSPB_TypeDef *lspb,
										Trajectory_TargetTypeDef target,
										double total_s,
										ModeInitTypeDef modeinit,
										double v_factor,
										double additional_factor);
SCARA_StatusTypeDef			scaraInitScurve	(Trajectory_Scurve_TypeDef *scurve,
												Trajectory_TargetTypeDef target,
												double total_s,
												ModeInitTypeDef modeinit,
												double v_factor,
												double additional_factor);


SCARA_StatusTypeDef			scaraFlowDuty		(double time,
												SCARA_PositionTypeDef *pos_Next ,
												SCARA_PositionTypeDef pos_Current);


SCARA_StatusTypeDef			scaraFlowLine		(Path_Line_TypeDef *line, double s);
SCARA_StatusTypeDef			scaraFlowCircle		(Path_Circle_TypeDef *circle, double s);
SCARA_StatusTypeDef			scaraFlowLSPB		(Trajectory_LSPB_TypeDef *lspb, double time);
SCARA_StatusTypeDef			scaraFlowBezierCurve(Path_Line_TypeDef *line, double s);
SCARA_StatusTypeDef			scaraFLowScurve		(Trajectory_Scurve_TypeDef *scurve, double t);
SCARA_StatusTypeDef			scaraFlowGCODE		(double *s, double time);

SCARA_StatusTypeDef			scaraCheckWorkSpace4(double theta1, double theta2, double d3, double theta4);
SCARA_StatusTypeDef			scaraCheckWorkSpace1(Trajectory_TargetTypeDef target, double value);
SCARA_StatusTypeDef			scaraTestDuty(void);

void						scaraSetScanFlag	(void);
void						scaraSetOutput		(uint8_t level);
void						scaraSetDutyState	(SCARA_DutyStateTypeDef state);
void						scaraSetMode		(SCARA_ModeTypeDef mode);
void						scaraSetMethod		(SCARA_MethodTypeDef method);

int32_t 					scaraPosition_packaging(uint8_t *data_packet, SCARA_PositionTypeDef position);
double						PNPcalMovDistance(double startx, double starty, double objectx, double objecty, double Vr, double Vc);
void 						Append_Coordinate_Value(int32_t append_value, uint8_t* append_position);
void						scaraGetPosition	(SCARA_PositionTypeDef *pos);
void						scaraUpdatePosition (SCARA_PositionTypeDef *pos);
SCARA_ModeTypeDef			scaraGetMode		(void);
SCARA_MethodTypeDef			scaraGetMethod		(void);
SCARA_DutyStateTypeDef		scaraGetDutyState	(void);
uint8_t						scaraIsScanLimit	(void);
uint8_t						scaraIsFinish		(double run_time);


SCARA_StatusTypeDef  		scaraKeyInit(SCARA_KeyTypeDef key, int32_t speed);
SCARA_StatusTypeDef scaraInitLinear(Trajectory_Linear_TypeDef *lspb, Trajectory_TargetTypeDef target, double total_s,
									ModeInitTypeDef modeinit, double additional_factor);
SCARA_StatusTypeDef scaraTestFlowDuty(double time, SCARA_PositionTypeDef *pos_next, SCARA_PositionTypeDef pos_current);									
#endif /* INC_ROBOT_SCARA_H_ */
