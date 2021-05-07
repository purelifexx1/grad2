/*
 * command_decode.c
 *
 *  Created on: Mar 7, 2020
 *      Author: Dang Nam
 */

#include "command_respond.h"
#include "robot_scara.h"
#include <stdio.h>
#include <string.h>
extern const char *DETAIL_STATUS[NUM_OF_STATUS];
extern int8_t 	test_value_array[4] = {5, 5, 5, 10};
extern double conveyor_speed = 30.0;
extern double PUT_DOWN_TIME_ON_SLOT 	= 0.3f;
extern double PUT_DOWN_TIME_ON_OBJECT	= 0.3f;
extern double PICK_UP_TIME_ON_OBJECT 	= 0.3f;
extern double PICK_UP_TIME_ON_SLOT	    = 0.3f;
extern double MOVE_TIME 				= 1.2f;
extern double ATTACH_TIME 			    = 0.01f;
extern double DETACH_TIME 			    = 0.01f;
extern double UP_HEIGHT 				= 131.0f;
extern double DOWN_HEIGHT_ON_OBJECT 	= 126.5f;
extern double DOWN_HEIGHT_ON_SLOT 	    = 126.5f;
extern SCARA_LSPB_Clutch_TypeDef  gcode_clutch_configure[200];
Position_DataType position_type;
SCARA_Gcode_Cor_TypeDef	Gcode_Cor[1000];
uint16_t point_counter = 0, current_clutch_index = 0;
uint8_t step_status;
Robot_CommandTypedef pnp_move_option;
Robot_CommandTypedef 	packetRead	(uint8_t *message, int32_t length, int32_t *id_command, DUTY_Command_TypeDef *duty_cmd) {
	Transfer_Protocol protocol_id = message[0];
    duty_cmd->id_command = message[1];
	int32_t temp_pointer = 2;
    switch(protocol_id) 
    {
        // reserved for Gcode file transfer
        case FILE_TRANSMISION:
        {
        	temp_pointer = 1;
        	uint16_t timeout_value = 0;
        	do {
        		Gcode_Packet_Command_TypeDef move_type 	= message[temp_pointer] & 0x0f;
				switch (move_type){
				case FIRST_PACKET:{
					Gcode_Mode = message[temp_pointer++] >> 4 & 0x0f;
//					if(Gcode_Mode == GCODE_LINEAR){
//						bezier_wc = (double)B2I(temp_pointer)*COR_INVERSE_SCALE; temp_pointer+=4;
//					}
					down_z_height = (double)B2I(temp_pointer)*COR_INVERSE_SCALE; temp_pointer+=4;
					up_z_height   = (double)B2I(temp_pointer)*COR_INVERSE_SCALE; temp_pointer+=4;
					total_num_of_point = B2I(temp_pointer);						 temp_pointer+=4;
					point_counter = 0;
					current_clutch_index = 0;
					pre_clutch_index = -1;
					pre_height = UP_Z;
				}
				break;
				case CLUTCH_HEADER_TYPE:{
					temp_pointer++;
					gcode_clutch_configure[current_clutch_index].total_s     = B2I(temp_pointer); temp_pointer+=4;
					gcode_clutch_configure[current_clutch_index].veloc 	     = B2I(temp_pointer); temp_pointer+=4;
					gcode_clutch_configure[current_clutch_index].Depth_Feed  = B2I(temp_pointer); temp_pointer+=4;
					current_clutch_index++;
				}
				break;
//				case BEZIER_TYPE:
				case LINEAR_TYPE:{
					Gcode_Cor[point_counter].configure.type_define[0] = move_type;
					Gcode_Cor[point_counter].configure.type_define[1] = message[temp_pointer++] >> 4 & 0x0f;
//					if(move_type == BEZIER_TYPE){
//						Gcode_Cor[point_counter].I = B2I(temp_pointer);	temp_pointer+=4;
//					}
					Gcode_Cor[point_counter].X = B2I(temp_pointer);	temp_pointer+=4;
					Gcode_Cor[point_counter].Y = B2I(temp_pointer);	temp_pointer+=4;
					if(Gcode_Mode == GCODE_LINEAR){
						Gcode_Cor[point_counter].F = B2I(temp_pointer);	temp_pointer+=4;
					}else if(Gcode_Mode == GCODE_SMOOTH_LSPB){
						Gcode_Cor[point_counter].configure.clutch_index = current_clutch_index - 1;
						Gcode_Cor[point_counter].T = B2I(temp_pointer);	temp_pointer+=4;
					}
					point_counter++;
				}
				break;
				case ARC_CW_TYPE:
				case ARC_AW_TYPE:{
					Gcode_Cor[point_counter].configure.type_define[0] = move_type;
					Gcode_Cor[point_counter].configure.type_define[1] = message[temp_pointer++] >> 4 & 0x0f;
					Gcode_Cor[point_counter].X = B2I(temp_pointer);	temp_pointer+=4;
					Gcode_Cor[point_counter].Y = B2I(temp_pointer);	temp_pointer+=4;
					if(Gcode_Mode == GCODE_LINEAR){
						Gcode_Cor[point_counter].F = B2I(temp_pointer);	temp_pointer+=4;
					}else if(Gcode_Mode == GCODE_SMOOTH_LSPB){
						Gcode_Cor[point_counter].configure.clutch_index = current_clutch_index - 1;
						Gcode_Cor[point_counter].T = B2I(temp_pointer);	temp_pointer+=4;
					}
					Gcode_Cor[point_counter].I = B2I(temp_pointer);	temp_pointer+=4;
					Gcode_Cor[point_counter].J = B2I(temp_pointer);	temp_pointer+=4;
					point_counter++;
				}
				break;
				}
				if(timeout_value++ == 0xffff) break;
        	}while(temp_pointer < length);
        	return CMD_GCODE;

        }
        break;

        // command control from pc
        case COMMAND_TRANSMISION:
        {
        	Robot_CommandTypedef command_id = message[1];
            switch(command_id)
            {
                // Stop now
                case CMD_STOPNOW:
                {
                    duty_cmd->robot_mode = SCARA_MODE_STOP;
                    duty_cmd->robot_method = SCARA_METHOD_SEMI_AUTO;
                    duty_cmd->change_method = FALSE;
                    return CMD_STOPNOW;
                }
                break;

                // Scan limit
                case CMD_SCAN_LIMIT:
                {
                    duty_cmd->robot_mode = SCARA_MODE_SCAN;
                    duty_cmd->robot_method = SCARA_METHOD_SEMI_AUTO;
                    duty_cmd->change_method = FALSE;
                    return CMD_SCAN_LIMIT;
                }
                break;

                // Move home
                case CMD_MOVE_HOME:
                {
                    if (length == 10){ // 2 int32_t number + 2 define byte
						temp_pointer = -2;
						duty_cmd->v_factor = B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						duty_cmd->a_factor = B2I(temp_pointer)*COR_INVERSE_SCALE;
                    }else{
                        return CMD_ERROR;
                    }
                    duty_cmd->target_point.x = 250;
                    duty_cmd->target_point.y = -200;
                    duty_cmd->target_point.z = 120;
                    duty_cmd->target_point.roll = 0;

                    duty_cmd->modeInit_type = DUTY_MODE_INIT_QVA;
                    duty_cmd->space_type = DUTY_SPACE_JOINT;
                    duty_cmd->joint_type = DUTY_JOINT_4DOF;
                    duty_cmd->robot_mode = SCARA_MODE_DUTY;
                    duty_cmd->robot_method = SCARA_METHOD_SEMI_AUTO;
                    duty_cmd->change_method = FALSE;
                    return CMD_MOVE_HOME;
                }
                break;

                case CMD_MOVE_JOINT:
                // Move line
                case CMD_MOVE_LINE:
                {
                    if (length == 29){ // 6 int32_t number + 3 byte number + 2 define byte
						//scaraSetScanFlag();
						uint8_t mode_init;
						temp_pointer = -2;

						duty_cmd->target_point.x = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						duty_cmd->target_point.y = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						duty_cmd->target_point.z = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						duty_cmd->target_point.roll = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						duty_cmd->v_factor = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						mode_init = message[temp_pointer+=4];

						if(mode_init == DUTY_MODE_INIT_QVA){
							duty_cmd->modeInit_type = DUTY_MODE_INIT_QVA;
							duty_cmd->a_factor = (double)B2I(temp_pointer+=1)*COR_INVERSE_SCALE;
						}else if(mode_init == DUTY_MODE_INIT_QVT){
							duty_cmd->modeInit_type = DUTY_MODE_INIT_QVT;
							duty_cmd->time_total = (double)B2I(temp_pointer+=1)*COR_INVERSE_SCALE;
						}else if(mode_init == DUTY_MODE_INIT_QT){
							duty_cmd->modeInit_type = DUTY_MODE_INIT_QT;
							duty_cmd->time_total = (double)B2I(temp_pointer+=1)*COR_INVERSE_SCALE;
						}else if(mode_init == DUTY_MODE_INIT_QV){
							duty_cmd->modeInit_type = DUTY_MODE_INIT_QV;
							duty_cmd->v_factor = (double)B2I(temp_pointer+=1)*COR_INVERSE_SCALE;
						}else{
							return CMD_ERROR;
						}
						duty_cmd->coordinate_type = message[temp_pointer+=4];
						duty_cmd->trajec_type = message[temp_pointer+=1];
						if(command_id == CMD_MOVE_LINE){
							duty_cmd->path_type = DUTY_PATH_LINE;
							duty_cmd->space_type = DUTY_SPACE_TASK;
						}else if(command_id == CMD_MOVE_JOINT){
							duty_cmd->joint_type = DUTY_JOINT_4DOF;
							duty_cmd->space_type = DUTY_SPACE_JOINT;
						}else{
							return CMD_ERROR;
						}

					}else{
						return CMD_ERROR;
					}
					duty_cmd->robot_mode = SCARA_MODE_DUTY;
					duty_cmd->robot_method = SCARA_METHOD_SEMI_AUTO;
					duty_cmd->change_method = FALSE;
					return command_id;
                }
				break;


				// Move circle
                case CMD_MOVE_CIRCLE:{
                	if(length == 34){ // 7 int32_t number + 4 byte number + 2 define byte
                		uint8_t mode_init, arc_type;
                		temp_pointer = -2;
                		duty_cmd->target_point.x      = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
                		duty_cmd->target_point.y      = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
                		duty_cmd->sub_point.x 	      = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
                		duty_cmd->sub_point.y         = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
                		duty_cmd->target_point.roll   = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
                		duty_cmd->v_factor			  = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
                		arc_type = message[temp_pointer+=4];
                		if(arc_type == ARC_AW_TYPE){
                			duty_cmd->arc_dir = 1;
                		}else if(arc_type == ARC_CW_TYPE){
                			duty_cmd->arc_dir = -1;
                		}else{
                			return CMD_ERROR;
                		}
                		mode_init = message[temp_pointer+=1];
						if(mode_init == DUTY_MODE_INIT_QVA){
							duty_cmd->modeInit_type = DUTY_MODE_INIT_QVA;
							duty_cmd->a_factor = (double)B2I(temp_pointer+=1)*COR_INVERSE_SCALE;
						}else if(mode_init == DUTY_MODE_INIT_QVT){
							duty_cmd->modeInit_type = DUTY_MODE_INIT_QVT;
							duty_cmd->time_total = (double)B2I(temp_pointer+=1)*COR_INVERSE_SCALE;
						}else if(mode_init == DUTY_MODE_INIT_QT){
							duty_cmd->modeInit_type = DUTY_MODE_INIT_QT;
							duty_cmd->time_total = (double)B2I(temp_pointer+=1)*COR_INVERSE_SCALE;
						}else if(mode_init == DUTY_MODE_INIT_QV){
							duty_cmd->modeInit_type = DUTY_MODE_INIT_QV;
							duty_cmd->v_factor = (double)B2I(temp_pointer+=1)*COR_INVERSE_SCALE;
						}else{
							return CMD_ERROR;
						}
						duty_cmd->coordinate_type = message[temp_pointer+=4];
						duty_cmd->trajec_type = message[temp_pointer+=1];
						duty_cmd->path_type = DUTY_PATH_CIRCLE;
						duty_cmd->space_type = DUTY_SPACE_TASK;
                	}else{
                		return CMD_ERROR;
                	}
                	duty_cmd->robot_mode = SCARA_MODE_DUTY;
					duty_cmd->robot_method = SCARA_METHOD_SEMI_AUTO;
					duty_cmd->change_method = FALSE;
					return CMD_MOVE_CIRCLE;
                }
                break;

				// Rotate Single
				case CMD_ROTATE_SINGLE:
				{
					return CMD_ROTATE_SINGLE;
				}
				break;

				// Set output
				case CMD_OUTPUT:
				{
					if(length == 3){ // 1 byte output value + 2 byte define
						temp_pointer = 2;
						duty_cmd->arc_dir = message[temp_pointer];
						return CMD_OUTPUT;
					}else{
						return CMD_ERROR;
					}					
				}
				break;

				// Read position
				case CMD_READ_POSITION:
				{
					if(length == 3){ // 1 byte categorize read type + 2 byte define
						temp_pointer = 2;
						position_type = message[temp_pointer];
						return CMD_READ_POSITION;
					}else{
						return CMD_ERROR;
					}

				}
				break;			

				// Setting
				case CMD_TEST_METHOD_SETTING:
				{
					if (length == 6){ // 4 byte configure + 2 byte define
						temp_pointer = 2;
						test_value_array[0] = message[temp_pointer++];
						test_value_array[1] = message[temp_pointer++];
						test_value_array[2] = message[temp_pointer++];
						test_value_array[3] = message[temp_pointer++];
						return CMD_TEST_METHOD_SETTING;
					}else{
						return CMD_ERROR;
					}
				}
				break;

				// Change method
				case CMD_METHOD_CHANGE:
				{
					if(length == 3){ // 1 byte configure + 2 byte define
						uint8_t method;
						temp_pointer = 2;
						method = message[temp_pointer];
						if (SCARA_METHOD_MANUAL == method) {
							duty_cmd->robot_method = SCARA_METHOD_MANUAL;
						} else if (SCARA_METHOD_SEMI_AUTO == method) {
							duty_cmd->robot_method = SCARA_METHOD_SEMI_AUTO;
						} else if (SCARA_METHOD_GCODE == method) {
							duty_cmd->robot_method = SCARA_METHOD_GCODE;
						}else if(SCARA_METHOD_TEST == method){
							duty_cmd->robot_method = SCARA_METHOD_TEST;
						}else if(SCARA_METHOD_PICK_AND_PLACE == method){
							duty_cmd->robot_method = SCARA_METHOD_PICK_AND_PLACE;
						}else {
							return CMD_ERROR;
						}
						duty_cmd->change_method = TRUE;
						return CMD_METHOD_CHANGE;
					}else{
						return CMD_ERROR;
					}
				}
				break;

				// Set manual key
				case CMD_KEYBOARD:
				{
					if(length == 3){ // 1 byte output value + 2 byte define
						int temp_pointer = 2;
						duty_cmd->keyboard = (SCARA_KeyTypeDef)message[temp_pointer];
						duty_cmd->robot_method = SCARA_METHOD_MANUAL;
						duty_cmd->change_method = FALSE;
						return CMD_KEYBOARD;
					}else{
						return CMD_ERROR;
					}					
				}
				break;

				// Set manual key speed
				case CMD_KEY_SPEED:
				{
					if(length == 3){ // 1 byte key value + 2 byte define
						int temp_pointer = 2;
						int32_t speed = (int32_t)message[temp_pointer];
						// check limit
						if ((speed < SHIFT_SPEED_MIN) || (speed > SHIFT_SPEED_MAX)) {
							return CMD_ERROR;
						}
						duty_cmd->key_speed = speed;
						current_key_speed1 = speed;
						return CMD_KEY_SPEED;
					}else{
						return CMD_ERROR;
					}
				}
				break;

				// Set test mode
				case CMD_MOTOR_TEST:
				{
					if(length == 3){ // 1byte key mode + 2 byte define
						int temp_pointer = 2;
						uint8_t test_key = message[temp_pointer];
						if(test_key < 0 || test_key > 8){
							return CMD_ERROR;
						}
						duty_cmd->test_key = test_key;
						duty_cmd->robot_method = SCARA_METHOD_TEST;	
						duty_cmd->change_method = FALSE;
						return CMD_MOTOR_TEST;
					}else{
						return CMD_ERROR;
					}
				}
				break;

				case CMD_OBJECT_DETECTED:
				{
					if (length == 15){ // 3 int32_t number + 1 byte object type + 2 define byte
						temp_pointer = -2;
						duty_cmd->target_point.x = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						duty_cmd->target_point.y = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						//duty_cmd->target_point.z = (double)(*(int32_t*)(&message[temp_pointer+=4]))*COR_INVERSE_SCALE;
						duty_cmd->target_point.roll = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						duty_cmd->target_point.object_type = message[temp_pointer+=4];
						duty_cmd->target_point.packet_time_stamp = GET_MICROS;

					}else{
						return CMD_ERROR;
					}
					duty_cmd->robot_method = SCARA_METHOD_PICK_AND_PLACE;
					duty_cmd->change_method = FALSE;
					return CMD_OBJECT_DETECTED;
				}
				break;

				case CMD_SETUP_CONVEYOR_SPEED:
				{
					if (length == 47) { // 11 int32_t number + 1 byte move type + 2 define byte
						temp_pointer = -2;
						conveyor_speed           = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						PUT_DOWN_TIME_ON_SLOT 	 = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						PUT_DOWN_TIME_ON_OBJECT	 = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						PICK_UP_TIME_ON_OBJECT 	 = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						PICK_UP_TIME_ON_SLOT	 = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						MOVE_TIME 				 = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						ATTACH_TIME 			 = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						DETACH_TIME 			 = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						UP_HEIGHT 				 = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						DOWN_HEIGHT_ON_OBJECT 	 = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						DOWN_HEIGHT_ON_SLOT 	 = (double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						pnp_move_option 		 = message[temp_pointer+=4];
						return CMD_SETUP_CONVEYOR_SPEED;
					}else{
						return CMD_ERROR;
					}
				}
				break;
				case CMD_GCODE_CONFIGURE:
				{
					if(length == 18) { // 4 int32_t number + 2 define byte
						temp_pointer = -2;
						offset_x = 	(double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						offset_y = 	(double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						offset_z = 	(double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						roll_angle =(double)B2I(temp_pointer+=4)*COR_INVERSE_SCALE;
						return CMD_GCODE_CONFIGURE;
					}else{
						return CMD_ERROR;
					}
				}
				break;
				case CMD_GCODE_RUN:
				{
					duty_cmd->robot_method = SCARA_METHOD_GCODE;
					duty_cmd->change_method = FALSE;
					return CMD_GCODE_RUN;
				}
				break;
				case CMD_GCODE_STOP:
				{
					duty_cmd->robot_method = SCARA_METHOD_GCODE;
					duty_cmd->change_method = FALSE;
					return CMD_GCODE_STOP;
				}
				break;
				case CMD_GCODE_PAUSE:
				{
					duty_cmd->robot_method = SCARA_METHOD_GCODE;
					duty_cmd->change_method = FALSE;
					return CMD_GCODE_PAUSE;
				}
				break;
				case CMD_GCODE_RESUME:
				{
					duty_cmd->robot_method = SCARA_METHOD_GCODE;
					duty_cmd->change_method = FALSE;
					return CMD_GCODE_RESUME;
				}
				break;
				case CMD_STEP_ON_OFF:
				{
					if(length == 3){ //1 byte status + 2 define byte
						temp_pointer = 2;
						if(message[temp_pointer] == 1){
							step_status = 1;
							HAL_GPIO_WritePin(STEP_ENABLE_GPIO_Port, STEP_ENABLE_Pin, GPIO_PIN_RESET);
						}else if(message[temp_pointer] == 0){
							step_status = 0;
							HAL_GPIO_WritePin(STEP_ENABLE_GPIO_Port, STEP_ENABLE_Pin, GPIO_PIN_SET);
						}
					}else{
						return CMD_ERROR;
					}
					return CMD_STEP_ON_OFF;
				}
				break;
				//Unknow command id
				default:
				{
					return CMD_ERROR;
				}
            }
        }
        break;

        //unknow protocol id
        default:
        {
            return PROTOCOL_ERROR;
        }
    }
}

Robot_RespondTypedef	commandReply	(Robot_CommandTypedef cmd_type,
										DUTY_Command_TypeDef duty_cmd,
										uint8_t *detail, int32_t *detail_length) {
	Robot_RespondTypedef ret;

	switch(cmd_type) {
	case CMD_STOPNOW:
	case CMD_SCAN_LIMIT:
	case CMD_MOVE_HOME:
	case CMD_MOVE_LINE:
	case CMD_MOTOR_TEST:
	case CMD_MOVE_CIRCLE:
	case CMD_MOVE_JOINT:
	case CMD_OBJECT_DETECTED:
	case CMD_ROTATE_SINGLE:
		ret = RPD_DUTY;
		break;
	case CMD_OUTPUT:
		{
			if (1 == duty_cmd.arc_dir) {
				scaraSetOutput(1);
				// strcpy( (char *)detail, "Output ON");
				// detail_length += 9;
				detail[(*detail_length)++] = OUTPUT_ON;
			} else if (0 == duty_cmd.arc_dir) {
				scaraSetOutput(0);
				detail[(*detail_length)++] = OUTPUT_OFF;
			} else {
				// strcpy( (char *)detail, "Wrong Value");
				// detail_length += 11;
				detail[(*detail_length)++] = WRONG_OUTPUT_VALUE ;
				return RPD_ERROR;
			}
			ret = RPD_OK;
		}
		break;
	case CMD_READ_STATUS:
		{
			SCARA_ModeTypeDef		current_mode;
			SCARA_DutyStateTypeDef 	current_state;
			current_mode	 = scaraGetMode();
			current_state	 = scaraGetDutyState();
			if ( SCARA_MODE_DUTY == current_mode && SCARA_DUTY_STATE_READY == current_state) {
				ret = RPD_IDLE;
			} else {
				ret = RPD_BUSY;
			}
		}
		break;
	case CMD_READ_POSITION:
		{
			SCARA_PositionTypeDef position;
			if(position_type == READ_CONTINUOUS_ENABLE){
				detail[(*detail_length)++] = POSREAD_CONTINUOUS_ENABLE;
//				lowlayer_readTruePosition(&position);
//				kinematicForward(&position);
//				if(position_type == REAL_POSITION_DATA_PLUS_UPDATE){
//					scaraUpdatePosition(&position);
//				}
				ret = RPD_OK;
				continuous_update = 1;
			}else if(position_type == READ_CONTINUOUS_DISABLE){
				detail[(*detail_length)++] = POSREAD_CONTINUOUS_DISABLE;
//				scaraGetPosition(&position);
				continuous_update = 0;
				ret = RPD_OK;
			}else if(position_type == POSITION_UPDATE){
				detail[(*detail_length)++] = UPDATE_REAL_POS;
				SCARA_PositionTypeDef position;
				lowlayer_readTruePosition(&position);
				kinematicForward(&position);
				scaraUpdatePosition(&position);
				ret = RPD_OK;
			}else if(position_type == READ_REAL_DATA){
				SCARA_PositionTypeDef position;
				lowlayer_readTruePosition(&position);
				kinematicForward(&position);
				*detail_length = scaraPosition_packaging(detail, position);
				ret =  RPD_POSITION;
			}else{
				detail[(*detail_length)++] = WRONG_READ_POSITION_TYPE;
				return RPD_ERROR;
			}
//			*detail_length = scaraPosition_packaging(detail, position);
//			ret =  RPD_POSITION;
//			ret =  RPD_OK;
		}
		break;
	case CMD_TEST_METHOD_SETTING:
		detail[(*detail_length)++] = TEST_VALUE_SETTING;
		ret = RPD_OK;
		break;
	case CMD_SETUP_CONVEYOR_SPEED:
		ret = RPD_OK;
		break;
	case CMD_METHOD_CHANGE:
		ret = RPD_DUTY;
		break;
	case CMD_GCODE:{
		if(point_counter == total_num_of_point){
			detail[(*detail_length)++] = GCODE_TRANSFER_FINISH;
			ret = RPD_OK;
		}else{
			ret = RPD_TRANSFER;
		}
	}
	break;
	case CMD_GCODE_CONFIGURE:
	{
		detail[(*detail_length)++] = GCODE_OFFSET_CONFIGURE;
		ret = RPD_OK;
	}
	break;
	case CMD_GCODE_STOP:
	case CMD_GCODE_PAUSE:
	case CMD_JOB_PUSH_MOVE_LINE:
	case CMD_JOB_PUSH_MOVE_JOINT:
	case CMD_GCODE_RESUME:
	case CMD_GCODE_RUN:
		ret = RPD_DUTY;
		break;
	case CMD_KEYBOARD:
		ret = RPD_DUTY;
		break;
	case CMD_KEY_SPEED:{
		detail[(*detail_length)++] = MANUAL_SPEED;
		detail[(*detail_length)++] = (uint8_t)(duty_cmd.key_speed);
		ret = RPD_OK;
	}
		break;
	case CMD_ERROR:{
		detail[(*detail_length)++] = CHECK_PARAMETER;
		ret = RPD_ERROR;
	}
		break;
	case CMD_STEP_ON_OFF:{
		if(step_status == 1){
			detail[(*detail_length)++] = STEP_ON;
			ret = RPD_OK;
		}else if(step_status == 0){
			detail[(*detail_length)++] = STEP_OFF;
			ret = RPD_OK;
		}else{
			ret = RPD_ERROR;
		}
	}
	break;
	default:
		detail[(*detail_length)++] = UNKNOW_COMMAND;
		ret = RPD_ERROR;
	}
	return ret;
}


int32_t				commandRespond1	(Robot_RespondTypedef rpd,
										int32_t id_command,
										char *detail,
										int32_t detail_length,
										char *respond) {
	int32_t out_length = 0;
	respond[out_length++] = 0x28;
	respond[out_length++] = 0;
	respond[out_length++] = RESPONSE_TRANSMISION;
	respond[out_length++] = rpd;
	respond[out_length++] = id_command;
	if(detail_length != 0){
		memcpy(&respond[out_length], detail, detail_length);
		out_length += detail_length;
	}else{
		respond[out_length++] = NONE;
	}
	respond[out_length++] = 0x7d;
	respond[out_length++] = 0x29;
	respond[1] = out_length - 2;
	return out_length;
}

