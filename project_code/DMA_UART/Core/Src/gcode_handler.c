/*
 * gcode_handler.c
 *
 *  Created on: Mar 13, 2021
 *      Author: Admin
 */

#include "gcode_handler.h"

void update_gcode_point(DUTY_Command_TypeDef *duty_cmd, SCARA_Gcode_Cor_TypeDef gcode_point, int32_t run_point)
{
	duty_cmd->target_point.x = (double)gcode_point.X * COR_INVERSE_SCALE + offset_x;
    duty_cmd->target_point.y = (double)gcode_point.Y * COR_INVERSE_SCALE + offset_y;
    if(gcode_point.type_define[1] == UP_Z){
  	  duty_cmd->target_point.z = up_z_height + offset_z;
    }else{
  	  duty_cmd->target_point.z = down_z_height + offset_z;
    }
    duty_cmd->target_point.roll = roll_angle;
    duty_cmd->v_factor = (double)gcode_point.F * COR_INVERSE_SCALE / V_MOVE_MAX;
    if(run_point == 1 || run_point == total_num_of_point){
    	duty_cmd->trajec_type = DUTY_TRAJECTORY_LSPB;
    	duty_cmd->modeInit_type = DUTY_MODE_INIT_QVT;
    	duty_cmd->time_total = 0;
    }else{
    	duty_cmd->trajec_type = DUTY_TRAJECTORY_LINEAR;
    	duty_cmd->modeInit_type = DUTY_MODE_INIT_QV;
    }
    duty_cmd->coordinate_type = DUTY_COORDINATES_ABS;
    if(gcode_point.type_define[0] == LINEAR_TYPE){
  	  duty_cmd->path_type = DUTY_PATH_LINE;
    }else if(gcode_point.type_define[0] == ARC_AW_TYPE){
      duty_cmd->sub_point.x = (double)gcode_point.I * COR_INVERSE_SCALE;
      duty_cmd->sub_point.y = (double)gcode_point.J * COR_INVERSE_SCALE;
      duty_cmd->path_type = DUTY_PATH_CIRCLE;
      duty_cmd->arc_dir = 1;
    }else if(gcode_point.type_define[0] == ARC_CW_TYPE){
      duty_cmd->sub_point.x = (double)gcode_point.I * COR_INVERSE_SCALE;
      duty_cmd->sub_point.y = (double)gcode_point.J * COR_INVERSE_SCALE;
      duty_cmd->path_type = DUTY_PATH_CIRCLE;
      duty_cmd->arc_dir = -1;
    }
    duty_cmd->space_type = DUTY_SPACE_TASK;
}
//SCARA_StatusTypeDef	Gcode_Testing() {
//	int32_t sample_count;
//	double run_time = 0;
//	SCARA_PositionTypeDef test_Next, test_Current;
//	SCARA_StatusTypeDef status;
//
//	for (int32_t i = 1; i <= total_num_of_point; i++) {
//		update_gcode_point(&duty_cmd, Gcode_Cor[run_point], i);
//		status = scaraInitDuty(duty_cmd);
//		if(status == SCARA_STATUS_OK){
//			run_time = 0;
//			sample_count = ceil(myDUTY.time_total/T_SAMPLING);
//		}else{
//			return status;
//		}
//		run_time += T_SAMPLING;
//		status = scaraFlowDuty(run_time, &test_Next, test_Current);
//		if (status != SCARA_STATUS_OK) {
//			return status;
//		}
//		memcpy(&test_Next, &test_Current, sizeof(SCARA_PositionTypeDef));
//	}
//	return SCARA_STATUS_OK;
//}
