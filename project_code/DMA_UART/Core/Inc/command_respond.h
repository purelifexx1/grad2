/*
 * command_decode.h
 *
 *  Created on: Mar 7, 2020
 *      Author: Dang Nam
 */

#ifndef INC_COMMAND_RESPOND_H_
#define INC_COMMAND_RESPOND_H_

#include  "common_def.h"
#include  "robot_scara.h"
#include  "robot_lowlayer.h"
#include  "kinematic.h"
#include  "gcode_handler.h"

#define B2I(temp_pointer) 		(*(int32_t*)(&message[temp_pointer]))
#define LINEAR_PACKET_LENGTH	13 // 12 byte for x y feedrate, 1 byte define
#define CIRCLE_PACKET_LENGTH	21 // 20 byte for x y feedrate i j, 1 byte define
#define FIRST_PACKET_LENGTH		13 // 12 byte for MinZ MaxZ Total_num_of_point, 1 byte define
double up_z_height, down_z_height;
int32_t current_key_speed1;
int32_t total_num_of_point;
int8_t  test_value_array[4];
SCARA_LSPB_Clutch_TypeDef  gcode_clutch_configure[200];
double PUT_DOWN_TIME_ON_SLOT;
double PUT_DOWN_TIME_ON_OBJECT;
double PICK_UP_TIME_ON_OBJECT;
double PICK_UP_TIME_ON_SLOT;
double MOVE_TIME;
double ATTACH_TIME;
double	DETACH_TIME;
double UP_HEIGHT;
double DOWN_HEIGHT_ON_OBJECT;
double DOWN_HEIGHT_ON_SLOT;
double conveyor_speed;
typedef enum
{
	CMD_STOPNOW = 0x00,
	CMD_SCAN_LIMIT,
	CMD_MOVE_HOME,
	CMD_MOVE_LINE,
	CMD_MOVE_CIRCLE,
	CMD_MOVE_JOINT,
	CMD_ROTATE_SINGLE,
	CMD_OUTPUT,
	CMD_READ_STATUS,
	CMD_READ_POSITION,
	CMD_TEST_METHOD_SETTING,
	CMD_METHOD_CHANGE,
    CMD_MOTOR_TEST,

    CMD_GCODE_STOP,
    CMD_GCODE_PAUSE,
    CMD_JOB_PUSH_MOVE_LINE,
    CMD_JOB_PUSH_MOVE_JOINT,
    CMD_GCODE_CONFIGURE,
    CMD_GCODE_RESUME,
    CMD_GCODE_RUN,// 7 job

    CMD_KEYBOARD,// 2 key board
	CMD_KEY_SPEED,

	CMD_ERROR,
	PROTOCOL_ERROR,

	CMD_OBJECT_DETECTED,
	CMD_SETUP_CONVEYOR_SPEED,
	CMD_GCODE,
	NUM_OF_COMMAND
}Robot_CommandTypedef;

typedef enum
{
    RPD_IDLE				= 0x00,
    RPD_BUSY				= 0x01,
	RPD_POSITION			= 0x02,
    RPD_START				= 0x03,
    RPD_RUNNING				= 0x04,
	RPD_DONE				= 0x05,
    RPD_STOP				= 0x06,
    RPD_ERROR				= 0x07,
	RPD_OK 					= 0x08,
	RPD_DUTY				= 0x09,
	RPD_TRANSFER			= 0x0A,
	RDP_GCODE_PROCESS		= 0x0B,
    NUM_OF_RESPOND			= 0x0C
}Robot_RespondTypedef;

typedef enum
{
	FILE_TRANSMISION,
	COMMAND_TRANSMISION,
	RESPONSE_TRANSMISION
}Transfer_Protocol;

typedef enum
{
	ACCEPT_COMMAND,
    STUPID_CODE,
    WRONG_SPACE_TYPE,
    WRONG_TASK_TYPE,
    WRONG_JOINT_TYPE,
    WRONG_TRAJECTORY_TYPE,
    WRONG_PARAMETER,
    OVER_WORKSPACE,
    WRONG_MODE_INIT,
    OVER_VELOCITY,
    OVER_ACCELERATE,
    WRONG_JOINT_NUM,
    WRONG_COORDINATE,
    OUTPUT_ON,
    OUTPUT_OFF,
    WRONG_OUTPUT_VALUE,
	WRONG_READ_POSITION_TYPE,
    TEST_VALUE_SETTING,
    RELATIVE,
    LSPB,
    S_CURVE,
    CHECK_PARAMETER,
    MANUAL_SPEED,
    UNKNOW_COMMAND,
    MANUAL_METHOD   ,
    SEMI_AUTO_METHOD,
    GCODE_METHOD     ,
    TEST_METHOD     ,
	PICK_AND_PLACE_METHOD,
	OBJECT_DETECTED 	,
	GCODE_TRANSFER_FINISH,
	GCODE_OFFSET_CONFIGURE,
    STOP_NOW        ,
    START_SCAN      ,
    BUSY            ,
    NOT_SCAN        ,
    INCORRECT_METHOD,
	
	NONE
}Response_ID;
/*** Instance Form ***/

typedef enum
{
	REAL_POSITION_DATA,
	REAL_POSITION_DATA_PLUS_UPDATE,
	ESTIMATE_POSITION_DATA
}Position_DataType;


/*---Function Prototype---*/
Robot_CommandTypedef 	packetRead		(uint8_t *message, int32_t length,
										int32_t *id_command,
										DUTY_Command_TypeDef *duty_cmd);

Robot_RespondTypedef	commandReply	(Robot_CommandTypedef cmd_type,
										DUTY_Command_TypeDef duty_cmd,
										uint8_t *detail, int32_t *detail_length);

int32_t 				commandRespond1 (Robot_RespondTypedef rpd,
										int32_t id_command,
										char *detail,
										int32_t detail_length,
										char *respond);

#endif /* INC_COMMAND_RESPOND_H_ */
