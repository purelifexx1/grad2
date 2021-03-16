/*
 * gcode_handler.c
 *
 *  Created on: Mar 13, 2021
 *      Author: Admin
 */

#include "gcode_handler.h"

void update_gcode_point(DUTY_Command_TypeDef *duty_cmd, SCARA_Gcode_Cor_TypeDef gcode_point)
{
	duty_cmd->target_point.x = (double)gcode_point.X * COR_INVERSE_SCALE + OFFSET_X;
    duty_cmd->target_point.y = (double)gcode_point.Y * COR_INVERSE_SCALE + OFFSET_Y;
    if(gcode_point.type_define[1] == UP_Z){
  	  duty_cmd->target_point.z = up_z_height + OFFSET_Z;
    }else{
  	  duty_cmd->target_point.z = down_z_height + OFFSET_Z;
    }
    duty_cmd->target_point.roll = 0;
    duty_cmd->v_factor = (double)gcode_point.F * COR_INVERSE_SCALE / V_MOVE_MAX;
    duty_cmd->trajec_type = DUTY_TRAJECTORY_LINEAR;
    duty_cmd->coordinate_type = DUTY_COORDINATES_ABS;
    duty_cmd->modeInit_type = DUTY_MODE_INIT_QV;
    if(gcode_point.type_define[0] == LINEAR_TYPE){
  	  duty_cmd->path_type = DUTY_PATH_LINE;
    }else if(gcode_point.type_define[0] == ARC_AW_TYPE){

    }else if(gcode_point.type_define[0] == ARC_CW_TYPE){

    }
    duty_cmd->path_type = DUTY_PATH_LINE;
    duty_cmd->space_type = DUTY_SPACE_TASK;
}
