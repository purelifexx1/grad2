/*
 * gcode_handler.c
 *
 *  Created on: Mar 13, 2021
 *      Author: Admin
 */

#include "gcode_handler.h"
extern int16_t 							pre_clutch_index;
extern Gcode_Packet_Command_TypeDef 	pre_height;
extern Gcode_Packet_Command_TypeDef 	Gcode_Mode;
extern SCARA_Gcode_Cor_TypeDef			Gcode_Cor[1000];
void update_gcode_point(DUTY_Command_TypeDef *duty_cmd, int32_t run_point)
{
//	if(Gcode_Cor[run_point].configure.type_define[0] == BEZIER_TYPE){
//		duty_cmd->target_point.x = (double)Gcode_Cor[run_point + 1].X * COR_INVERSE_SCALE + offset_x;
//		duty_cmd->target_point.y = (double)Gcode_Cor[run_point + 1].Y * COR_INVERSE_SCALE + offset_y;
//		if(Gcode_Cor[run_point].configure.type_define[1] == UP_Z){
//		  duty_cmd->target_point.z = up_z_height + offset_z;
//		}else{
//		  duty_cmd->target_point.z = down_z_height + offset_z;
//		}
//		duty_cmd->target_point.roll = roll_angle;
//		duty_cmd->space_type = DUTY_SPACE_TASK;
//		duty_cmd->coordinate_type = DUTY_COORDINATES_ABS;
//		duty_cmd->path_type = DUTY_PATH_BEZIER_CURVE;
//		duty_cmd->trajec_type = DUTY_TRAJECTORY_BEZIER_CURVE;
//		double feedrate = (double)Gcode_Cor[run_point].F * COR_INVERSE_SCALE;
//		double distance = (double)Gcode_Cor[run_point].I * COR_INVERSE_SCALE;
//		duty_cmd->time_total = distance/feedrate;
//		bezier_x_coeffs[0] = (double)Gcode_Cor[run_point-1].X * COR_INVERSE_SCALE + offset_x;
//		bezier_x_coeffs[1] = ((double)Gcode_Cor[run_point].X * COR_INVERSE_SCALE + offset_x)*2.0*bezier_wc;
//		bezier_x_coeffs[2] = duty_cmd->target_point.x;
//		bezier_y_coeffs[0] = (double)Gcode_Cor[run_point-1].Y * COR_INVERSE_SCALE + offset_y;
//		bezier_y_coeffs[1] = ((double)Gcode_Cor[run_point].Y * COR_INVERSE_SCALE + offset_y)*2.0*bezier_wc;
//		bezier_y_coeffs[2] = duty_cmd->target_point.y;
//	}else{
//
//	}
	duty_cmd->target_point.x = (double)Gcode_Cor[run_point].X * DATA_INVERSE_SCALE + offset_x;
	duty_cmd->target_point.y = (double)Gcode_Cor[run_point].Y * DATA_INVERSE_SCALE + offset_y;
	if(Gcode_Cor[run_point].configure.type_define[1] == UP_Z){
	  duty_cmd->target_point.z = up_z_height + offset_z;
	}else{
	  duty_cmd->target_point.z = down_z_height + offset_z;
	}
	duty_cmd->target_point.roll = roll_angle;
	duty_cmd->space_type = DUTY_SPACE_TASK;
	duty_cmd->coordinate_type = DUTY_COORDINATES_ABS;
	if(Gcode_Cor[run_point].configure.type_define[0] == LINEAR_TYPE){
	  duty_cmd->path_type = DUTY_PATH_LINE;
	}else if(Gcode_Cor[run_point].configure.type_define[0] == ARC_AW_TYPE){
	  duty_cmd->sub_point.x = (double)Gcode_Cor[run_point].I * DATA_INVERSE_SCALE;
	  duty_cmd->sub_point.y = (double)Gcode_Cor[run_point].J * DATA_INVERSE_SCALE;
	  duty_cmd->path_type = DUTY_PATH_CIRCLE;
	  duty_cmd->arc_dir = 1;
	}else if(Gcode_Cor[run_point].configure.type_define[0] == ARC_CW_TYPE){
	  duty_cmd->sub_point.x = (double)Gcode_Cor[run_point].I * DATA_INVERSE_SCALE;
	  duty_cmd->sub_point.y = (double)Gcode_Cor[run_point].J * DATA_INVERSE_SCALE;
	  duty_cmd->path_type = DUTY_PATH_CIRCLE;
	  duty_cmd->arc_dir = -1;
	}

	if(Gcode_Mode == GCODE_LINEAR){
		duty_cmd->v_factor = (double)Gcode_Cor[run_point].F * DATA_INVERSE_SCALE / V_MOVE_MAX;
		if(run_point == 1){
			duty_cmd->trajec_type = DUTY_TRAJECTORY_LSPB;
			duty_cmd->modeInit_type = DUTY_MODE_INIT_QVT;
			duty_cmd->time_total = 0;
		}else{
			duty_cmd->trajec_type = DUTY_TRAJECTORY_LINEAR;
			duty_cmd->modeInit_type = DUTY_MODE_INIT_QV;
		}
	}else if(Gcode_Mode == GCODE_SMOOTH_LSPB){
		if(Gcode_Cor[run_point].configure.type_define[1] == pre_height){
			if(Gcode_Cor[run_point].configure.clutch_index != pre_clutch_index){
				LSPB_calculation(gcode_clutch_configure[Gcode_Cor[run_point].configure.clutch_index]);
				pre_clutch_index = Gcode_Cor[run_point].configure.clutch_index;
			}
			duty_cmd->trajec_type = DUTY_TRAJECTORY_GCODE_LSPB;
			duty_cmd->time_total = (double)(Gcode_Cor[run_point].T*DATA_INVERSE_SCALE);
		}else{
			duty_cmd->trajec_type = DUTY_TRAJECTORY_LINEAR;
			duty_cmd->modeInit_type = DUTY_MODE_INIT_QV;
			duty_cmd->v_factor = (double)gcode_clutch_configure[Gcode_Cor[run_point].configure.clutch_index].Depth_Feed*DATA_INVERSE_SCALE/V_MOVE_MAX;
			pre_height = Gcode_Cor[run_point].configure.type_define[1];
		}
	}

}
void LSPB_calculation(SCARA_LSPB_Clutch_TypeDef configure)
{
	double accel = 0;
	double total_s = (double)configure.total_s*DATA_INVERSE_SCALE;
	double veloc   = (double)configure.veloc*DATA_INVERSE_SCALE;
	time_move = 1.2*total_s/veloc;
	time_acc = time_move - total_s/veloc;
	time_dec = time_move - time_acc;
	accel = veloc / time_acc;
	acc0 = 0.5*accel;
	constant[0] = veloc;
	constant[1] = -0.5*time_acc*veloc;
    deacc[0] 	= -0.5*accel;
    deacc[1]	= veloc + accel*time_dec;
    deacc[2]    = total_s - time_move*(2*veloc + 2*accel*time_dec - accel*time_move)/2;
    accumulate_s = 0;
    last_T = 0;
}
void accumulate_update(SCARA_Gcode_Cor_TypeDef gcode_point){
	scaraFlowGCODE(&accumulate_s, (double)gcode_point.T*DATA_INVERSE_SCALE);
	last_T = (double)gcode_point.T*DATA_INVERSE_SCALE;
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
