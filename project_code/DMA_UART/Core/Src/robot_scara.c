/*
 * robot_scara.c
 *
 *  Created on: Mar 10, 2020
 *      Author: Dang Nam
 */


#include "robot_scara.h"
#include "kinematic.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "robot_lowlayer.h"
#include "gcode_handler.h"

SCARA_TypeDef 				mySCARA = { SCARA_METHOD_SEMI_AUTO,
										SCARA_MODE_DUTY,
										SCARA_DUTY_STATE_READY,
										FALSE,
										FALSE};
DUTY_TypeDef				myDUTY;

SCARA_PositionTypeDef		positionPrevios;
SCARA_PositionTypeDef		positionCurrent;
SCARA_PositionTypeDef		positionNext;
SCARA_PositionTypeDef		positionTrue;
SCARA_PositionTypeDef		positionKeyInit;
//SCARA_Gcode_Cor_TypeDef		Gcode_Cor[125];

Trajectory_TargetTypeDef	joint_taget[4] = {  TRAJECTORY_J0, TRAJECTORY_J1,
												TRAJECTORY_J2, TRAJECTORY_J3};

void				scaraStartup(void) {
#ifdef SIMULATION
	scaraSetScanFlag();
#endif
	lowlayer_CPLD_Init();
	lowlayer_stepMotorInit();
	lowlayer_resetEncoder();
	//lowlayer_writePulse(0, 0, 0, 0);
}


/* Compute duty corresponding to new command */
SCARA_StatusTypeDef	scaraInitDuty		(DUTY_Command_TypeDef command) {
	SCARA_StatusTypeDef status, status1, status2;

	/*----------- Space Task ------------*/
	if(DUTY_SPACE_TASK == command.space_type) {
		double total_s, angle_s;
		int8_t dir_angle;
		SCARA_PositionTypeDef	target_point;
		// Change Degree --> Radian
		command.target_point.roll = command.target_point.roll*PI/180.0;
		// Coordinate
		if( DUTY_COORDINATES_REL == command.coordinate_type) {
			target_point.x 		= positionCurrent.x + command.target_point.x;
			target_point.y 		= positionCurrent.y + command.target_point.y;
			target_point.z 		= positionCurrent.z + command.target_point.z;
			target_point.roll	= positionCurrent.roll + command.target_point.roll;
		} else if (DUTY_COORDINATES_ABS == command.coordinate_type) {
			target_point.x 		= command.target_point.x;
			target_point.y 		= command.target_point.y;
			target_point.z 		= command.target_point.z;
			target_point.roll	= command.target_point.roll;
		} else {
			return SCARA_STATUS_ERROR_COORDINATE;
		}

		angle_s = target_point.roll - positionCurrent.roll;
		dir_angle = 1;
		if ( angle_s < 0) {
			dir_angle = -1;
		}
//		if ( fabs(angle_s) > PI) {
//			dir_angle = -dir_angle;
//			angle_s = (2*PI - fabsf(angle_s))*dir_angle;
//		}
//
//		if(positionCurrent.roll + angle_s < -3.14 || positionCurrent.roll + angle_s > 3.14){
//			dir_angle = -dir_angle;
//			angle_s = (2*PI - fabs(angle_s))*dir_angle;
//		}

		myDUTY.space_type = DUTY_SPACE_TASK;// Change type
		myDUTY.task.roll_start = positionCurrent.roll;
		// 1-Path Planning
			// Straight Line
		if ( DUTY_PATH_LINE == command.path_type ) {
			myDUTY.task.path.path_type = DUTY_PATH_LINE;
			status = scaraInitLine(&(myDUTY.task.path.line), positionCurrent, target_point);
			total_s = myDUTY.task.path.line.total_s;
			// Circular
		} else if ( DUTY_PATH_CIRCLE == command.path_type ) {
			SCARA_PositionTypeDef	center_point;
			target_point.z = positionCurrent.z;

			center_point.x 		= positionCurrent.x + command.sub_point.x;
			center_point.y 		= positionCurrent.y + command.sub_point.y;
			center_point.z 		= positionCurrent.z;

			myDUTY.task.path.path_type = DUTY_PATH_CIRCLE;
			status = scaraInitCircle(&(myDUTY.task.path.circle),
										positionCurrent,
										target_point,
										center_point,
										command.arc_dir );
			total_s = myDUTY.task.path.circle.total_s;

//		}else if(DUTY_PATH_BEZIER_CURVE == command.path_type){
//			myDUTY.task.path.path_type = DUTY_PATH_BEZIER_CURVE;
//			myDUTY.task.path.line.z0 = command.target_point.z;
//		}
		}else {
			return SCARA_STATUS_ERROR_TASK;
		}

		if ( SCARA_STATUS_OK != status) {
			return status;
		}

		// 2-Trajectory Planning
			// LSPB
		if ( DUTY_TRAJECTORY_LSPB == command.trajec_type ) {

			if ( DUTY_MODE_INIT_QVT == command.modeInit_type) {
				//myDUTY.task.trajectory_3d.lspb.Tf = command.time_total;
				myDUTY.task.trajectory_3d.trajectory_type = DUTY_TRAJECTORY_LSPB;
				status1 = scaraInitLSPB1(&(myDUTY.task.trajectory_3d.lspb), TRAJECTORY_3D,
										 total_s, DUTY_MODE_INIT_QVT, command.v_factor, command.time_total);
				//myDUTY.task.trajectory_3d.lspb.Tf might change at this point,
				myDUTY.task.trajectory_roll.linear.dir = dir_angle;
				//myDUTY.task.trajectory_roll.linear.Tf = myDUTY.task.trajectory_3d.lspb.Tf;
				myDUTY.time_total = myDUTY.task.trajectory_3d.lspb.Tf;
				myDUTY.task.trajectory_roll.trajectory_type = DUTY_TRAJECTORY_LINEAR;
				status2 = scaraInitLinear(&(myDUTY.task.trajectory_roll.linear), TRAJECTORY_ROLL, angle_s*dir_angle, DUTY_MODE_INIT_QT, myDUTY.time_total);
			} else if (DUTY_MODE_INIT_QVA == command.modeInit_type) {
				//myDUTY.task.trajectory_3d.lspb.Tf = command.time_total;
				myDUTY.task.trajectory_3d.trajectory_type = DUTY_TRAJECTORY_LSPB;
				status1 = scaraInitLSPB1(&(myDUTY.task.trajectory_3d.lspb), TRAJECTORY_3D,
										 total_s, DUTY_MODE_INIT_QVA, command.v_factor, command.a_factor);
				//myDUTY.task.trajectory_3d.lspb.Tf might change at this point,
				myDUTY.task.trajectory_roll.linear.dir = dir_angle;
				//myDUTY.task.trajectory_roll.linear.Tf = myDUTY.task.trajectory_3d.lspb.Tf;
				myDUTY.time_total = myDUTY.task.trajectory_3d.lspb.Tf;
				myDUTY.task.trajectory_roll.trajectory_type = DUTY_TRAJECTORY_LINEAR;
				status2 = scaraInitLinear(&(myDUTY.task.trajectory_roll.linear), TRAJECTORY_ROLL, angle_s*dir_angle, DUTY_MODE_INIT_QT, myDUTY.time_total);

			} else {
				return SCARA_STATUS_ERROR_MODE_INIT ;
			}
			// SCURVE
		} else if 	( DUTY_TRAJECTORY_SCURVE == command.trajec_type ){

			if ( DUTY_MODE_INIT_QVT == command.modeInit_type) {
				//myDUTY.task.trajectory_3d.scurve.Tf = command.time_total;
				myDUTY.task.trajectory_3d.trajectory_type = DUTY_TRAJECTORY_SCURVE;
				status1 = scaraInitScurve1(&(myDUTY.task.trajectory_3d.scurve), TRAJECTORY_3D,
										 total_s, DUTY_MODE_INIT_QVT, command.v_factor, command.time_total);
				//myDUTY.task.trajectory_3d.lspb.Tf might change at this point,
				myDUTY.task.trajectory_roll.linear.dir = dir_angle;
				//myDUTY.task.trajectory_roll.linear.Tf = command.time_total;
				myDUTY.time_total = myDUTY.task.trajectory_3d.scurve.Tf;
				myDUTY.task.trajectory_roll.trajectory_type = DUTY_TRAJECTORY_LINEAR;
				status2 = scaraInitLinear(&(myDUTY.task.trajectory_roll.linear), TRAJECTORY_ROLL, angle_s*dir_angle, DUTY_MODE_INIT_QT, myDUTY.time_total);

			} else if (DUTY_MODE_INIT_QVA == command.modeInit_type) {
				//myDUTY.task.trajectory_3d.scurve.Tf = command.time_total;
				myDUTY.task.trajectory_3d.trajectory_type = DUTY_TRAJECTORY_SCURVE;
				status1 = scaraInitScurve1(&(myDUTY.task.trajectory_3d.scurve), TRAJECTORY_3D,
										 total_s, DUTY_MODE_INIT_QVA, command.v_factor, command.a_factor);
				//myDUTY.task.trajectory_3d.lspb.Tf might change at this point,
				myDUTY.task.trajectory_roll.linear.dir = dir_angle;
				//myDUTY.task.trajectory_roll.linear.Tf = command.time_total;
				myDUTY.time_total = myDUTY.task.trajectory_3d.scurve.Tf;
				myDUTY.task.trajectory_roll.trajectory_type = DUTY_TRAJECTORY_LINEAR;
				status2 = scaraInitLinear(&(myDUTY.task.trajectory_roll.linear), TRAJECTORY_ROLL, angle_s*dir_angle, DUTY_MODE_INIT_QT, myDUTY.time_total);
			}else {
				return SCARA_STATUS_ERROR_MODE_INIT;
			}
		}else if(DUTY_TRAJECTORY_LINEAR == command.trajec_type){
			//myDUTY.time_total = command.time_total;
			myDUTY.task.trajectory_roll.linear.dir = dir_angle;
			myDUTY.task.trajectory_3d.trajectory_type = DUTY_TRAJECTORY_LINEAR;
			myDUTY.task.trajectory_roll.trajectory_type = DUTY_TRAJECTORY_LINEAR;
			if(command.modeInit_type == DUTY_MODE_INIT_QT){
				status1 = scaraInitLinear(&(myDUTY.task.trajectory_3d.linear), TRAJECTORY_3D, fabs(total_s), DUTY_MODE_INIT_QT, command.time_total);
			}else if(command.modeInit_type == DUTY_MODE_INIT_QV){
				status1 = scaraInitLinear(&(myDUTY.task.trajectory_3d.linear), TRAJECTORY_3D, fabs(total_s), DUTY_MODE_INIT_QV, command.v_factor);
			}else {
				return SCARA_STATUS_ERROR_MODE_INIT;
			}
			myDUTY.time_total = myDUTY.task.trajectory_3d.linear.Tf;
			status2 = scaraInitLinear(&(myDUTY.task.trajectory_roll.linear), TRAJECTORY_ROLL, angle_s*dir_angle, DUTY_MODE_INIT_QT, myDUTY.time_total);
		}else if(DUTY_TRAJECTORY_GCODE_LSPB == command.trajec_type){
			status1 = SCARA_STATUS_OK;
			myDUTY.task.trajectory_roll.linear.dir = dir_angle;
			myDUTY.task.trajectory_3d.trajectory_type = DUTY_TRAJECTORY_GCODE_LSPB;
			myDUTY.task.trajectory_roll.trajectory_type = DUTY_TRAJECTORY_LINEAR;
			myDUTY.time_total = command.time_total;
			status2 = scaraInitLinear(&(myDUTY.task.trajectory_roll.linear), TRAJECTORY_ROLL, angle_s*dir_angle, DUTY_MODE_INIT_QT, command.time_total - last_T);
		}
//		else if(DUTY_TRAJECTORY_BEZIER_CURVE == command.trajec_type){
//			myDUTY.task.trajectory_roll.linear.dir = dir_angle;
//			myDUTY.task.trajectory_3d.trajectory_type = DUTY_TRAJECTORY_BEZIER_CURVE;
//			myDUTY.time_total = command.time_total;
//			status1 = SCARA_STATUS_OK;
//			status2 = scaraInitLinear(&(myDUTY.task.trajectory_roll.linear), TRAJECTORY_ROLL, angle_s*dir_angle, DUTY_MODE_INIT_QT, myDUTY.time_total);
//		}
		else {
			return SCARA_STATUS_ERROR_TRAJECTORY;
		}
		// Check Init Status
		if ( SCARA_STATUS_OK != status1) {
			return status1;
		}
		if ( SCARA_STATUS_OK != status2) {
			return status2;
		}


	
	} else if (DUTY_SPACE_JOINT == command.space_type) {
		/*----------- Space Joint ------------*/
		myDUTY.space_type = DUTY_SPACE_JOINT;// Change type
		myDUTY.joint.theta1_start	 = positionCurrent.Theta1;
		myDUTY.joint.theta2_start	 = positionCurrent.Theta2;
		myDUTY.joint.d3_start		 = positionCurrent.D3;
		myDUTY.joint.theta4_start 	 = positionCurrent.Theta4;

		SCARA_PositionTypeDef	target_point;
		// Change Degree --> Radian
		command.target_point.roll = command.target_point.roll*PI/180.0;
		// Coordinate
		if( DUTY_COORDINATES_REL == command.coordinate_type) {
			target_point.x 		= positionCurrent.x + command.target_point.x;
			target_point.y 		= positionCurrent.y + command.target_point.y;
			target_point.z 		= positionCurrent.z + command.target_point.z;
			target_point.roll	= positionCurrent.roll + command.target_point.roll;
		} else if (DUTY_COORDINATES_ABS == command.coordinate_type) {
			target_point.x 		= command.target_point.x;
			target_point.y 		= command.target_point.y;
			target_point.z 		= command.target_point.z;
			target_point.roll	= command.target_point.roll;
		} else {
			return SCARA_STATUS_ERROR_COORDINATE;
		}

		if( FALSE == kinematicInverse(&target_point, positionCurrent)) {
			return SCARA_STATUS_ERROR_OVER_WORKSPACE;// Exit with error
		}
		// Trajectory 4 profile
		double q[4];
		q[0] = target_point.Theta1 - positionCurrent.Theta1;
		q[1] = target_point.Theta2 - positionCurrent.Theta2;
		q[2] = target_point.D3 	   - positionCurrent.D3;
		q[3] = target_point.Theta4 - positionCurrent.Theta4;

		// LSPB
		if ( DUTY_TRAJECTORY_LSPB == command.trajec_type) {
			// Mode Init Time
			if ( DUTY_MODE_INIT_QVT == command.modeInit_type) {
				for ( uint8_t i = 0; i < 4; i++) {
					myDUTY.joint.trajectory[i].trajectory_type = DUTY_TRAJECTORY_LSPB;
					myDUTY.joint.trajectory[i].lspb.Tf = command.time_total;
					status1 = scaraInitLSPB1(&(myDUTY.joint.trajectory[i].lspb), joint_taget[i],
										q[i], DUTY_MODE_INIT_QVT, command.v_factor, command.time_total);
					if(status1 != SCARA_STATUS_OK){
						return status1;
					}
				}
				myDUTY.time_total = command.time_total;
			// Mode Init Acc
			} else if  ( DUTY_MODE_INIT_QVA == command.modeInit_type) {
				for ( uint8_t i = 0; i < 4; i++) {
					myDUTY.joint.trajectory[i].trajectory_type = DUTY_TRAJECTORY_LSPB;
					status1 = scaraInitLSPB1(&(myDUTY.joint.trajectory[i].lspb), joint_taget[i],
										q[i], DUTY_MODE_INIT_QVA, command.v_factor, command.a_factor);
				}
				myDUTY.time_total = 0;
				for ( uint8_t i = 0; i < 4; i++) {
					if ( myDUTY.joint.trajectory[i].lspb.Tf > myDUTY.time_total) {
						myDUTY.time_total = myDUTY.joint.trajectory[i].lspb.Tf;
					}
				}
				for ( uint8_t i = 0; i < 4; i++) {
					myDUTY.joint.trajectory[i].trajectory_type = DUTY_TRAJECTORY_LSPB;
					myDUTY.joint.trajectory[i].lspb.Tf = myDUTY.time_total;
					status1 = scaraInitLSPB1(&(myDUTY.joint.trajectory[i].lspb), joint_taget[i],
										q[i], DUTY_MODE_INIT_QVT, command.v_factor, command.a_factor);
				}

			} else {
				return SCARA_STATUS_ERROR_MODE_INIT;
			}
			// SCURVE
		} else if ( DUTY_TRAJECTORY_SCURVE == command.trajec_type) {
			// Mode Init Time
			if ( DUTY_MODE_INIT_QVT == command.modeInit_type) {
				for ( uint8_t i = 0; i < 4; i++) {
					myDUTY.joint.trajectory[i].trajectory_type = DUTY_TRAJECTORY_SCURVE;
					myDUTY.joint.trajectory[i].scurve.Tf = command.time_total;
					status1 = scaraInitScurve1(&(myDUTY.joint.trajectory[i].scurve), joint_taget[i],
										q[i], DUTY_MODE_INIT_QVT, command.v_factor, command.time_total);
					if(status1 != SCARA_STATUS_OK){
						return status1;
					}
				}
				myDUTY.time_total = command.time_total;
			// Mode Init Acc
			} else if  ( DUTY_MODE_INIT_QVA == command.modeInit_type) {
				for ( uint8_t i = 0; i < 4; i++) {
					myDUTY.joint.trajectory[i].trajectory_type = DUTY_TRAJECTORY_SCURVE;
					status1 = scaraInitScurve1(&(myDUTY.joint.trajectory[i].scurve), joint_taget[i],
										q[i], DUTY_MODE_INIT_QVA, command.v_factor, command.a_factor);
				}
				myDUTY.time_total = 0;
				for ( uint8_t i = 0; i < 4; i++) {
					if ( myDUTY.joint.trajectory[i].scurve.Tf > myDUTY.time_total) {
						myDUTY.time_total = myDUTY.joint.trajectory[i].scurve.Tf;
					}
				}
				for ( uint8_t i = 0; i < 4; i++) {
					myDUTY.joint.trajectory[i].trajectory_type = DUTY_TRAJECTORY_SCURVE;
					myDUTY.joint.trajectory[i].scurve.Tf = myDUTY.time_total;
					status1 = scaraInitScurve1(&(myDUTY.joint.trajectory[i].scurve), joint_taget[i],
										q[i], DUTY_MODE_INIT_QVT, command.v_factor, command.a_factor);
				}

			} else {
				return SCARA_STATUS_ERROR_MODE_INIT;
			}

			} else {
				return SCARA_STATUS_ERROR_TRAJECTORY;
			}

	} else {
		return SCARA_STATUS_ERROR_SPACE;
	}

	// Reset time and distance
	positionCurrent.t = 0;
	positionCurrent.total_time = myDUTY.time_total;
	positionNext.t = 0;
	positionNext.total_time = myDUTY.time_total;
	positionTrue.t = 0;
	positionTrue.total_time = myDUTY.time_total;

	return SCARA_STATUS_OK;
}

/* Compute straight line path parameters */
SCARA_StatusTypeDef	scaraInitLine		(Path_Line_TypeDef *line,
										SCARA_PositionTypeDef start,
										SCARA_PositionTypeDef end) {
	// Check limit workspace
	if ( FALSE ==  kinematicInverse(&end, start)) {
		return SCARA_STATUS_ERROR_OVER_WORKSPACE;
	}
	// Init line params
	line->x1 = end.x;
	line->y1 = end.y;
	line->z1 = end.z;
	line->x0 = start.x;
	line->y0 = start.y;
	line->z0 = start.z;
	line->denta_x = end.x  -  start.x;
	line->denta_y = end.y  -  start.y;
	line->denta_z = end.z  -  start.z;
	line->total_s = sqrt((line->denta_x)*(line->denta_x)
			       +(line->denta_y)*(line->denta_y)
				   +(line->denta_z)*(line->denta_z));
	line->x_current = line->x0;
	line->y_current = line->y0;
	line->z_current = line->z0;

	return SCARA_STATUS_OK;
}

/* Compute circular path parameters */
SCARA_StatusTypeDef	scaraInitCircle		(Path_Circle_TypeDef *circle,
										SCARA_PositionTypeDef start,
										SCARA_PositionTypeDef end,
										SCARA_PositionTypeDef center,
										int32_t dir){
	// Check limit workspace
	if ( FALSE == kinematicInverse(&end, start) ) {
		return SCARA_STATUS_ERROR_OVER_WORKSPACE;
	}

//	if( 1 != dir && -1 != dir) {
//		return SCARA_STATUS_ERROR_PARA;// error direction param !!!
//	}
	if(dir != 1 && dir != -1){
		return SCARA_STATUS_ERROR_PARA;
	}

	double x_start, y_start, x_stop, y_stop;
	double r1, r2, angle_start, angle_stop, delta_angle;
	x_start = start.x - center.x;
	y_start = start.y - center.y;
	x_stop  = end.x  - center.x;
	y_stop  = end.y  - center.y;
	r1 = sqrt(x_start*x_start + y_start*y_start);
	r2 = sqrt(x_stop*x_stop + y_stop*y_stop);

	if( 1.0 < fabs(r1 - r2)) {
		return SCARA_STATUS_ERROR_PARA; //start & stop are not in a circle together !!
	}

//	if (r1 < 0.1 || r2 < 0.1) {
//		return SCARA_STATUS_ERROR_PARA; //start and center almost in the same phace, radius = 0 !!
//	}

	angle_start = atan2(y_start, x_start);
	angle_stop  = atan2(y_stop, x_stop);
	delta_angle = angle_stop - angle_start;

	if (delta_angle < 0 && dir > 0) {
		delta_angle = 2*PI - fabs(delta_angle); //this state mean that the arc radius is over 180
	}else if(delta_angle > 0 && dir < 0){
		delta_angle = 2*PI - fabs(delta_angle);
	}
	if(delta_angle*dir < 0){
		delta_angle = fabs(delta_angle);
	}

//	if ( 0 > dir) {
//		delta_angle = 2*PI - delta_angle;
//	}
	// Init circle params
	circle->dir = dir;
	circle->radius = r1;
	circle->angle_start = angle_start;
	circle->angle_stop  = angle_stop;
	circle->total_angle = delta_angle;
	circle->total_s = delta_angle*r1;
	circle->x0 = start.x;
	circle->y0 = start.y;
	circle->x1 = end.x;
	circle->y1 = end.y;
	circle->xi = center.x;
	circle->yi = center.y;
	circle->z0 = start.z;
	circle->z1 = start.z;
	circle->zi = start.z;
	circle->x_current = circle->x0;
	circle->y_current = circle->y0;
	circle->z_current = circle->z0;

	return SCARA_STATUS_OK;
}

SCARA_StatusTypeDef scaraInitLinear(Trajectory_Linear_TypeDef *linear, Trajectory_TargetTypeDef target, double total_s,
									ModeInitTypeDef modeinit, double additional_factor)
{

	if(target == TRAJECTORY_3D){

	}else if(target == TRAJECTORY_ROLL){

	}else{
		return SCARA_STATUS_ERROR_PARA;
	}

	if(modeinit == DUTY_MODE_INIT_QT){
		linear->constant_v = total_s / additional_factor;
		linear->Tf = additional_factor;
		//linear->number_of_sample = ceilf(additional_factor / T_SAMPLING); // ceiling
	}else if(modeinit == DUTY_MODE_INIT_QV){
		linear->constant_v = additional_factor * V_MOVE_MAX;
		linear->Tf = total_s / linear->constant_v;
	}else{
		return SCARA_STATUS_ERROR_PARA;
	}

	return SCARA_STATUS_OK;
}
SCARA_StatusTypeDef	scaraInitLSPB1		(Trajectory_LSPB_TypeDef *lspb,
										Trajectory_TargetTypeDef target,
										double total_s,
										ModeInitTypeDef modeinit,
										double v_factor,
										double additional_factor){
	double v_design, a_design, v_lim, q0, q1, v0, v1, ta, td, tf;
	uint32_t no_sample;
	if(total_s < 0){
		total_s = -total_s;
		lspb->dir= -1;
	}else{
		lspb->dir= 1;
	}
	if(target == TRAJECTORY_3D){
		if(modeinit == DUTY_MODE_INIT_QVT){
			v_design = V_MOVE_MAX*v_factor;
			tf = additional_factor;
		}else if(modeinit == DUTY_MODE_INIT_QVA){
			a_design = A_MOVE_MAX*additional_factor;
			v_design = V_MOVE_MAX*v_factor;
		}
	}else if(target == TRAJECTORY_J0){
		if(modeinit == DUTY_MODE_INIT_QVT){
			v_design = V_DESIGN_J0*v_factor;
			tf = additional_factor;
		}else if(modeinit == DUTY_MODE_INIT_QVA){

		}
	}else if(target == TRAJECTORY_J1){
		if(modeinit == DUTY_MODE_INIT_QVT){
			v_design = V_DESIGN_J1*v_factor;
			tf = additional_factor;
		}else if(modeinit == DUTY_MODE_INIT_QVA){

		}
	}else if(target == TRAJECTORY_J2){
		if(modeinit == DUTY_MODE_INIT_QVT){
			v_design = V_DESIGN_J2*v_factor;
			tf = additional_factor;
		}else if(modeinit == DUTY_MODE_INIT_QVA){

		}
	}else if(target == TRAJECTORY_J3){
		if(modeinit == DUTY_MODE_INIT_QVT){
			v_design = V_DESIGN_J3*v_factor;
			tf = additional_factor;
		}else if(modeinit == DUTY_MODE_INIT_QVA){

		}
	}else{
		return SCARA_STATUS_ERROR_PARA;
	}
	if(modeinit == DUTY_MODE_INIT_QVT){
		if(tf < 0.1 && v_design > 3){ //check if time valid or not
			tf = 1.5*total_s/v_design;
			//return SCARA_STATUS_ERROR_OVER_VELOC;
		}else if(v_design < 0.1 && tf > 0.15){ //check if velocity valid or not
			v_design = 1.5*total_s/tf;
		}else if(v_design < 0.1 && tf < 0.1){
			return SCARA_STATUS_ERROR_OVER_VELOC;
		}
		else{
			if(tf < 0.001){
				return SCARA_STATUS_ERROR_OVER_VELOC;
			}else{
				v_lim = total_s/tf;
			}
			if(v_design < 1.1*v_lim){ //the accelaration part is too large
				return SCARA_STATUS_ERROR_OVER_ACCEL;
			}else if(v_design > 2*v_lim){
				v_design = 2*v_lim;
			}else{

			}
		}
		ta = tf - total_s/v_design;
		a_design = v_design/ta;
		td = tf - ta;

	}else if(modeinit == DUTY_MODE_INIT_QVA){
		//check qva condition
		if(total_s*a_design > v_design*v_design){ //condition for ta smaller than td
//			if(total_s/v_design < 0.3){ //td too small
//				//recalculate v_design
//				v_design = total_s/0.3;
//			}
//			if(v_design/a_design < 0.1){ //ta is too small
//				//recalculate a_design
//				a_design = v_design/0.1;
//			}
			ta = v_design/a_design;
			td = total_s/v_design;
			tf = ta + td;

		}else{
			tf = 2*total_s/v_design;
			ta = tf/2;
			td = tf/2;
			a_design = v_design/ta;
		}
	}
	no_sample = ceilf(tf / T_SAMPLING); // ceiling
	 // Init lspb params

	 lspb->s0 = 0;
	 lspb->s1 = total_s;
	 lspb->Ta = ta;
	 lspb->Td = td;
	 lspb->Tf = tf;
	 lspb->a_design = a_design;
	 lspb->v_design = v_design;
	 lspb->v_lim = v_design;
	 lspb->v0 = 0;
	 lspb->v1 = 0;
	 lspb->num_of_sampling = no_sample;
	 lspb->total_s = lspb->s1 - lspb->s0;

	 return SCARA_STATUS_OK;
}
SCARA_StatusTypeDef			scaraInitScurve1	(Trajectory_Scurve_TypeDef *scurve,
												Trajectory_TargetTypeDef target,
												double total_s,
												ModeInitTypeDef modeinit,
												double v_factor,
												double additional_factor)
{
	double v_design, a_design, v_lim, q0, q1, v0, v1, ta, td, tf;
	uint32_t no_sample;
	if(total_s < 0){
		total_s = -total_s;
		scurve->dir= -1;
	}else{
		scurve->dir= 1;
	}
	if(target == TRAJECTORY_3D){
		if(modeinit == DUTY_MODE_INIT_QVT){
			v_design = V_MOVE_MAX*v_factor;
			tf = additional_factor;
		}else if(modeinit == DUTY_MODE_INIT_QVA){
			a_design = A_MOVE_MAX*additional_factor;
			v_design = V_MOVE_MAX*v_factor;
		}
	}else if(target == TRAJECTORY_J0){
		if(modeinit == DUTY_MODE_INIT_QVT){
			v_design = V_DESIGN_J0*v_factor;
			tf = additional_factor;
		}else if(modeinit == DUTY_MODE_INIT_QVA){
	
		}
	}else if(target == TRAJECTORY_J1){
		if(modeinit == DUTY_MODE_INIT_QVT){
			v_design = V_DESIGN_J1*v_factor;
			tf = additional_factor;
		}else if(modeinit == DUTY_MODE_INIT_QVA){
	
		}
	}else if(target == TRAJECTORY_J2){
		if(modeinit == DUTY_MODE_INIT_QVT){
			v_design = V_DESIGN_J2*v_factor;
			tf = additional_factor;
		}else if(modeinit == DUTY_MODE_INIT_QVA){
	
		}
	}else if(target == TRAJECTORY_J3){
		if(modeinit == DUTY_MODE_INIT_QVT){
			v_design = V_DESIGN_J3*v_factor;
			tf = additional_factor;
		}else if(modeinit == DUTY_MODE_INIT_QVA){
	
		}
	}else{
		return SCARA_STATUS_ERROR_PARA;
	}
	if(modeinit == DUTY_MODE_INIT_QVT){
		if(tf < 0.1 && v_design > 7.5){ //check if time valid or not
			tf = 1.5*total_s/v_design;
		}else if(v_design < 0.1 && tf > 0.15){ //check if velocity valid or not
			v_design = 1.5*total_s/tf;
		}else if(tf < 0.1 && v_design < 0.1){
			return SCARA_STATUS_ERROR_OVER_VELOC;
		}else{
			v_lim = total_s/tf;
			if(v_design < 1.1*v_lim){ // the accelaration part is too large
				return SCARA_STATUS_ERROR_OVER_ACCEL;
			}else if(v_design > 2*v_lim){
				v_design = 2*v_lim;
			}else{
	
			}
		}
		a_design = 2*v_design*v_design/(v_design*tf - total_s);
		ta = v_design/a_design;		
		td = tf - ta;
	
	}else if(modeinit == DUTY_MODE_INIT_QVA){
		//check qva condition
		if(total_s*a_design > 2*v_design*v_design){ //condition for ta smaller than tc
//			if(total_s/v_design < 0.3){ //tc too small
//				//recalculate v_design
//				v_design = total_s/0.3;
//			}
//			if(v_design/a_design < 0.1){ //ta is too small
//				//recalculate a_design
//				a_design = v_design/0.1;
//			}
			ta = v_design/a_design;
			td = total_s/v_design + ta;
			tf = ta + td;
//			if(v_design/a_design < 1){ //if ta is still too small
//				//recalculate a_design
//				a_design = v_design;
//				ta = 1;
//				td = total_s/v_design;
//				tf = ta + td;
//			}
	
		}else{
			tf = 2*total_s/v_design;
			ta = tf/4;
			td = 3*tf/4;
			a_design = v_design/ta;
		}
	}
	//no_sample = ceilf(tf / T_SAMPLING); // ceiling
	 // Init lspb params
	
	scurve->Ta = ta;
	scurve->Tf = tf;
	scurve->Td = td;
	scurve->a_design = a_design;
	scurve->v_design = v_design;
	scurve->v_lim = v_lim;
	scurve->total_s = total_s;
	scurve->a_current = 0;
	scurve->v_current = 0;
	scurve->s_current = 0;
	//define duplicate value when calculating scurve flow
	scurve->k_1        = v_design/(6*ta*ta);
	scurve->k_2		   = 2*ta*ta*ta;
	scurve->k_3        = 6*ta;
	scurve->k_4        = 6*ta*ta;
	return SCARA_STATUS_OK;
}
SCARA_StatusTypeDef	scaraFlowLSPB1	(Trajectory_LSPB_TypeDef *lspb, double time){
	double tf, td, ta;

	tf = lspb->Tf;
	td = lspb->Td;
	ta = lspb->Ta;
	// Accelerate
	if ( 0.0f <= time && time <= ta) {
		lspb->a_current		=	lspb->a_design;
		lspb->v_current		=	lspb->v0 + lspb->a_design*time;
		lspb->s_current		=	lspb->s0 + lspb->v0*time + 0.5*lspb->a_design*time*time;
	// Constant velocity
	} else if (ta < time && time <= td) {
		lspb->a_current		=	0;
		lspb->v_current		=	lspb->v_lim;
		lspb->s_current		=	lspb->s0 + lspb->v0*ta*0.5 + lspb->v_design*(time - ta*0.5);
	// Decelerate
	} else if (td < time && time <= tf) {
		lspb->a_current		=	-lspb->a_design;
		lspb->v_current		=	lspb->v_design + lspb->a_design*(td - time);
		lspb->s_current		=	lspb->s1 - lspb->v0*(tf - time)
								- (lspb->v_lim - lspb->v0)*(tf - time)*(tf - time)/(2*ta);
	} else {
		lspb->a_current 	=	0;
		lspb->v_current		=	0;
		lspb->s_current		=	lspb->total_s;
	}

	return SCARA_STATUS_OK;
}
SCARA_StatusTypeDef	scaraFlowGCODE(double *s, double time)
{
	if ( 0.0f <= time && time <= time_acc) {
		*s = acc0*time*time;
	// Constant velocity
	} else if (time_acc < time && time <= time_dec) {
		*s = constant[0]*time + constant[1];
	// Decelerate
	} else if (time_dec < time && time <= time_move) {
		*s = deacc[0]*time*time + deacc[1]*time + deacc[2];
	} else {
		*s = deacc[0]*time*time + deacc[1]*time + deacc[2];
	}
	return SCARA_STATUS_OK;
}
SCARA_StatusTypeDef	scaraFLowScurve1(Trajectory_Scurve_TypeDef *scurve, double t)
{
	double tf, td, ta, vc;
	tf = scurve->Tf;
	ta = scurve->Ta;
	td = scurve->Td;
	vc = scurve->v_design;
	if ( 0.0f <= t && t <= ta) {
//		lspb->a_current		=	(time*vc)/ta*ta;
//		lspb->v_current		=	(time*time*vc)/(2*ta*ta);
		scurve->s_current	=	t*t*t*scurve->k_1;
	// Constant velocity
	} else if (ta < t && t <= 2*ta) {
//		scurve->a_current   =	-(vc*(time - 2*ta))/(ta*ta);
//		scurve->v_current   =	vc/2 - (vc*(time - ta)*(time - 3*ta))/(2*ta*ta);
		scurve->s_current   =	-(t*t*t - t*t*scurve->k_3 + t*scurve->k_4 - scurve->k_2)*scurve->k_1;
	// Decelerate
	} else if (2*ta < t && t <= td - ta) {
//		scurve->a_current   =	0;
//		scurve->v_current   =	v_design;
		scurve->s_current   =	vc*(t - ta);
	} else if(td-ta < t && t <= td){
//		scurve->a_current 	=	-(vc*(t - td + ta))/ta^2;
//		scurve->v_current   =	vc/2 - (vc*(t + ta - tf)*(t + 3*ta - tf))/(2*ta*ta);
		scurve->s_current   =	scurve->total_s - (ta*vc)/3 - vc*(t - tf) - (t-tf)*(t-tf)*(t + scurve->k_3 - tf)*scurve->k_1;
	} else if(td < t && t <= tf){
//		scurve->a_current 	=	-(vc*(ta - t + td))/(ta*ta);
//		scurve->v_current   =	(vc*(t - tf)*(t-tf))/(2*ta*ta);
		scurve->s_current   =	scurve->total_s + (t-tf)*(t-tf)*(t-tf)*scurve->k_1;
	}

	return SCARA_STATUS_OK;
}

SCARA_StatusTypeDef scaraTestFlowDuty(double time, SCARA_PositionTypeDef *pos_Next, SCARA_PositionTypeDef pos_Current)
{
	double current_s;
	SCARA_StatusTypeDef status1;
	SCARA_PositionTypeDef positionCompute;
	current_s = time*myDUTY.task.trajectory_3d.lspb.v_design;
	status1 = scaraFlowLine(&(myDUTY.task.path.line), current_s);
	positionCompute.x = myDUTY.task.path.line.x_current;
	positionCompute.y = myDUTY.task.path.line.y_current;
	positionCompute.z = myDUTY.task.path.line.z_current;
	positionCompute.roll = myDUTY.task.roll_start + myDUTY.task.trajectory_roll.lspb.v_design*time;
	if ( FALSE == kinematicInverse(&positionCompute, pos_Current)) {
		return SCARA_STATUS_ERROR_OVER_WORKSPACE;
	} else {
		memcpy(pos_Next, &positionCompute, sizeof(SCARA_PositionTypeDef));
	}
	return SCARA_STATUS_OK;
}

/* Compute new x, y , z, theta1, theta2 , d3, theta4 corresponding to time */
SCARA_StatusTypeDef	scaraFlowDuty		(double time,
										SCARA_PositionTypeDef *pos_Next ,
										SCARA_PositionTypeDef pos_Current) {
	SCARA_StatusTypeDef status1, status2, status3, status4;
	SCARA_PositionTypeDef	positionCompute;
	// Update time
	positionCompute.t = time;
	/*---- Task space ----*/
	if ( DUTY_SPACE_TASK == myDUTY.space_type) {
		double s, angle, x, y, z, v, v_angle;
		int8_t	dir_roll;
		//---Trajectory flowing
			// LSPB
		if( DUTY_TRAJECTORY_LSPB == myDUTY.task.trajectory_3d.trajectory_type) {

			status1 = scaraFlowLSPB1(&(myDUTY.task.trajectory_3d.lspb), time);
			v = myDUTY.task.trajectory_3d.lspb.v_current;
			s = myDUTY.task.trajectory_3d.lspb.s_current;
			angle = myDUTY.task.trajectory_roll.linear.constant_v*time;
			dir_roll = myDUTY.task.trajectory_roll.linear.dir;
			status2 = SCARA_STATUS_OK;
			// SCURVE
		} else if ( DUTY_TRAJECTORY_SCURVE == myDUTY.task.trajectory_3d.trajectory_type) {

			status1 = scaraFLowScurve1(&(myDUTY.task.trajectory_3d.scurve), time);
			//v = myDUTY.task.trajectory_3d.lspb.v_current;
			s = myDUTY.task.trajectory_3d.scurve.s_current;
			angle = myDUTY.task.trajectory_roll.linear.constant_v*time;
			dir_roll = myDUTY.task.trajectory_roll.linear.dir;
			status2 = SCARA_STATUS_OK;
		}else if( DUTY_TRAJECTORY_LINEAR == myDUTY.task.trajectory_3d.trajectory_type){
			s = time*myDUTY.task.trajectory_3d.linear.constant_v;
			angle = myDUTY.task.trajectory_roll.linear.constant_v*time;
			dir_roll = myDUTY.task.trajectory_roll.linear.dir;
			status1 = SCARA_STATUS_OK;
			status2 = SCARA_STATUS_OK;
		}else if(DUTY_TRAJECTORY_GCODE_LSPB == myDUTY.task.trajectory_3d.trajectory_type){
			scaraFlowGCODE(&s, time);
			s -= accumulate_s;
			//LOG_REPORT1("s watcher:", s, time);
			angle = myDUTY.task.trajectory_roll.linear.constant_v*(time - last_T);
			dir_roll = myDUTY.task.trajectory_roll.linear.dir;
			status1 = SCARA_STATUS_OK;
			status2 = SCARA_STATUS_OK;

		}
//		else if(DUTY_TRAJECTORY_BEZIER_CURVE == myDUTY.task.trajectory_3d.trajectory_type){
//			s = time/myDUTY.time_total;
//			angle = myDUTY.task.trajectory_roll.linear.constant_v*time;
//			dir_roll = myDUTY.task.trajectory_roll.linear.dir;
//			status1 = SCARA_STATUS_OK;
//			status2 = SCARA_STATUS_OK;
//		}
		else{
			return SCARA_STATUS_ERROR_TRAJECTORY;
		}

		if ( SCARA_STATUS_OK != status1) {
			return status1;
		}
		if ( SCARA_STATUS_OK != status2) {
			return status2;
		}

		//---Path flowing
			// Straight line
		if( DUTY_PATH_LINE == myDUTY.task.path.path_type) {
			status1 = scaraFlowLine(&(myDUTY.task.path.line), s);
			x = myDUTY.task.path.line.x_current;
			y = myDUTY.task.path.line.y_current;
			z = myDUTY.task.path.line.z_current;
			// Circular
		} else if ( DUTY_PATH_CIRCLE == myDUTY.task.path.path_type) {
			status1 = scaraFlowCircle(&(myDUTY.task.path.circle), s);
			x = myDUTY.task.path.circle.x_current;
			y = myDUTY.task.path.circle.y_current;
			z = myDUTY.task.path.circle.z_current;
		}
//		else if(DUTY_PATH_BEZIER_CURVE == myDUTY.task.path.path_type){
//			status1 = scaraFlowBezierCurve(&(myDUTY.task.path.line), s);
//			x = myDUTY.task.path.line.x_current;
//			y = myDUTY.task.path.line.y_current;
//			z = myDUTY.task.path.line.z0;
//		}
		else {
			return SCARA_STATUS_ERROR_TASK;
		}

		positionCompute.x 		= x;
		positionCompute.y		= y;
		positionCompute.z 		= z;
		positionCompute.roll 	= myDUTY.task.roll_start + angle*dir_roll;

		positionCompute.q		= s;
//		positionCompute.q_roll  = angle;

//		positionCompute.v_3d    = v;
//		positionCompute.v_roll  = v_angle;

//		positionCompute.total_time = myDUTY.time_total;
//		positionCompute.t		= time;
		if ( FALSE == kinematicInverse(&positionCompute, pos_Current)) {
			return SCARA_STATUS_ERROR_OVER_WORKSPACE;
		} else {
			memcpy(pos_Next, &positionCompute, sizeof(SCARA_PositionTypeDef));
		}

	/*---- Joint space -----*/
	} else if (DUTY_SPACE_JOINT == myDUTY.space_type) {
		double s0, s1, s2, s3;
		double v0, v1, v2, v3;
		int8_t dir0, dir1, dir2, dir3;
		// Trajectory flowing
			// LSPB
		if( DUTY_TRAJECTORY_LSPB == myDUTY.joint.trajectory[0].trajectory_type) {
			status1 = scaraFlowLSPB1(&(myDUTY.joint.trajectory[0].lspb), time);
			status2 = scaraFlowLSPB1(&(myDUTY.joint.trajectory[1].lspb), time);
			status3 = scaraFlowLSPB1(&(myDUTY.joint.trajectory[2].lspb), time);
			status4 = scaraFlowLSPB1(&(myDUTY.joint.trajectory[3].lspb), time);

			dir0 = myDUTY.joint.trajectory[0].lspb.dir;
			dir1 = myDUTY.joint.trajectory[1].lspb.dir;
			dir2 = myDUTY.joint.trajectory[2].lspb.dir;
			dir3 = myDUTY.joint.trajectory[3].lspb.dir;

			s0 = myDUTY.joint.trajectory[0].lspb.s_current;
			s1 = myDUTY.joint.trajectory[1].lspb.s_current;
			s2 = myDUTY.joint.trajectory[2].lspb.s_current;
			s3 = myDUTY.joint.trajectory[3].lspb.s_current;

			v0 = myDUTY.joint.trajectory[0].lspb.v_current;
			v1 = myDUTY.joint.trajectory[1].lspb.v_current;
			v2 = myDUTY.joint.trajectory[2].lspb.v_current;
			v3 = myDUTY.joint.trajectory[3].lspb.v_current;

			// SCURVE
		} else if ( DUTY_TRAJECTORY_SCURVE == myDUTY.joint.trajectory[0].trajectory_type) {
			status1 = scaraFLowScurve1(&(myDUTY.joint.trajectory[0].scurve), time);
			status2 = scaraFLowScurve1(&(myDUTY.joint.trajectory[1].scurve), time);
			status3 = scaraFLowScurve1(&(myDUTY.joint.trajectory[2].scurve), time);
			status4 = scaraFLowScurve1(&(myDUTY.joint.trajectory[3].scurve), time);
			dir0 = myDUTY.joint.trajectory[0].scurve.dir;
			dir1 = myDUTY.joint.trajectory[1].scurve.dir;
			dir2 = myDUTY.joint.trajectory[2].scurve.dir;
			dir3 = myDUTY.joint.trajectory[3].scurve.dir;
			s0 = myDUTY.joint.trajectory[0].scurve.s_current;
			s1 = myDUTY.joint.trajectory[1].scurve.s_current;
			s2 = myDUTY.joint.trajectory[2].scurve.s_current;
			s3 = myDUTY.joint.trajectory[3].scurve.s_current;
//
//			v0 = myDUTY.joint.trajectory[0].scurve.v_current;
//			v1 = myDUTY.joint.trajectory[1].scurve.v_current;
//			v2 = myDUTY.joint.trajectory[2].scurve.v_current;
//			v3 = myDUTY.joint.trajectory[3].scurve.v_current;

		} else {
			return SCARA_STATUS_ERROR_TRAJECTORY;
		}
		// Check init status
		if ( SCARA_STATUS_OK != status1) {
			return status1;
		}
		if ( SCARA_STATUS_OK != status2) {
			return status2;
		}
		if ( SCARA_STATUS_OK != status3) {
			return status3;
		}
		if ( SCARA_STATUS_OK != status4) {
			return status4;
		}

		positionCompute.Theta1 	= myDUTY.joint.theta1_start + s0*dir0;
		positionCompute.Theta2 	= myDUTY.joint.theta2_start + s1*dir1;
		positionCompute.D3 		= myDUTY.joint.d3_start 	+ s2*dir2;
		positionCompute.Theta4 	= myDUTY.joint.theta4_start + s3*dir3;

		positionCompute.v_theta1 	= v0;
		positionCompute.v_theta2 	= v1;
		positionCompute.v_d3 		= v2;
		positionCompute.v_theta4 	= v3;

//		positionCompute.q_theta1 = s0;
//		positionCompute.q_theta2 = s1;
//		positionCompute.q_d3	 = s2;
//		positionCompute.q_theta4 = s3;

		positionCompute.total_time = myDUTY.time_total;
		positionCompute.t		= time;
		// Check workspace
		if( SCARA_STATUS_OK != scaraCheckWorkSpace4(positionCompute.Theta1,
							 	 	 	  positionCompute.Theta2,
										  positionCompute.D3,
										  positionCompute.Theta4)) {
			return SCARA_STATUS_ERROR_OVER_WORKSPACE;
		} else {
			memcpy(pos_Next, &positionCompute, sizeof(SCARA_PositionTypeDef));
		}
		kinematicForward(pos_Next);

	} else {
		return SCARA_STATUS_ERROR_SPACE;
	}

	return SCARA_STATUS_OK;
}

/* Compute new x, y ,z corresponding to s */
SCARA_StatusTypeDef	scaraFlowLine		(Path_Line_TypeDef *line, double s) {
	// Avoid div with 0
	if ( line->total_s > 0.01) {
		line->x_current	 = line->x0 + line->denta_x*s/line->total_s;
		line->y_current	 = line->y0 + line->denta_y*s/line->total_s;
		line->z_current	 = line->z0 + line->denta_z*s/line->total_s;
	} else {
		line->x_current = line->x1;
		line->y_current = line->y1;
		line->z_current = line->z1;
	}

	return SCARA_STATUS_OK;
}
SCARA_StatusTypeDef	scaraFlowBezierCurve	(Path_Line_TypeDef *line, double s) {
	double a,b,c;
	a = (1-s)*(1-s);
	b = s*(1-s);
	c = s*s;
	line->x_current = (a*bezier_x_coeffs[0] + b*bezier_x_coeffs[1] + c*bezier_x_coeffs[2])/(a + 2*bezier_wc*b+c);
	line->y_current = (a*bezier_y_coeffs[0] + b*bezier_y_coeffs[1] + c*bezier_y_coeffs[2])/(a + 2*bezier_wc*b+c);
	return SCARA_STATUS_OK;
}

/* Compute new x, y ,z corresponding to s */
SCARA_StatusTypeDef	scaraFlowCircle		(Path_Circle_TypeDef *circle, double s) {
	double angle;
	angle = s/(circle->radius);
	circle->x_current = circle->xi + circle->radius*cos(circle->angle_start + circle->dir*angle);
	circle->y_current = circle->yi + circle->radius*sin(circle->angle_start + circle->dir*angle);
	circle->z_current = circle->zi; // XY plane

	return SCARA_STATUS_OK;
}

/* Compute new s corresponding to time */
SCARA_StatusTypeDef	scaraFlowLSPB		(Trajectory_LSPB_TypeDef *lspb, double time) {
	double tf, td, ta;

	tf = lspb->Tf;
	td = lspb->Td;
	ta = lspb->Ta;

	// Accelerate
	if ( 0.0f <= time && time <= ta) {
		lspb->a_current		=	lspb->a_design;
		lspb->v_current		=	lspb->v0 + lspb->a_design*time;
		lspb->s_current		=	lspb->s0 + lspb->v0*time + 0.5*lspb->a_design*time*time;
	// Constant velocity
	} else if (ta <= time && time <= (tf - td)) {
		lspb->a_current		=	0;
		lspb->v_current		=	lspb->v_lim;
		lspb->s_current		=	lspb->s0 + lspb->v0*ta/2 + lspb->v_lim*(time - ta/2);
	// Decelerate
	} else if ((tf - td) <= time && time <= tf) {
		lspb->a_current		=	-lspb->a_design;
		lspb->v_current		=	lspb->v1 + lspb->a_design*(tf - time);
		lspb->s_current		=	lspb->s1 - lspb->v1*(tf - time)
								- (lspb->v_lim - lspb->v1)*(tf - time)*(tf - time)/(2*td);
	} else {
		lspb->a_current 	=	0;
		lspb->v_current		=	0;
		lspb->s_current		=	lspb->total_s;
	}

	return SCARA_STATUS_OK;
}


/* Check limit 4 joint variable */
SCARA_StatusTypeDef	scaraCheckWorkSpace4 (double theta1, double theta2, double d3, double theta4) {
	// check theta 1
	if ( theta1 < LIM_MIN_J0-0.1 || theta1 > LIM_MAX_J0+0.1) {
		return SCARA_STATUS_ERROR_OVER_WORKSPACE;
	}
	// check theta 2
	if ( theta2 < LIM_MIN_J1-0.1 || theta2 > LIM_MAX_J1+0.1) {
		return SCARA_STATUS_ERROR_OVER_WORKSPACE;
	}
	// check d 3
	if ( d3 < LIM_MIN_J2 || d3 > LIM_MAX_J2) {
		return SCARA_STATUS_ERROR_OVER_WORKSPACE;
	}
	// check theta 4
	if ( theta4 < LIM_MIN_J3-0.1 || theta4 > LIM_MAX_J3+0.1) {
		return SCARA_STATUS_ERROR_OVER_WORKSPACE;
	}
	return SCARA_STATUS_OK;
}

/* Check limit 1 joint variable */
SCARA_StatusTypeDef	scaraCheckWorkSpace1 (Trajectory_TargetTypeDef target, double value) {
	if ( TRAJECTORY_J0 == target) {
		if ( (LIM_MIN_J0 <= value) && ( value <= LIM_MAX_J0)) {
			return SCARA_STATUS_OK;
		} else {
			return SCARA_STATUS_ERROR_OVER_WORKSPACE;
		}
	} else if (TRAJECTORY_J1 == target) {
		if ( (LIM_MIN_J1 <= value) && ( value <= LIM_MAX_J1)) {
			return SCARA_STATUS_OK;
		} else {
			return SCARA_STATUS_ERROR_OVER_WORKSPACE;
		}
	} else if (TRAJECTORY_J2 == target) {
		if ( (LIM_MIN_J2 <= value) && ( value <= LIM_MAX_J2)) {
			return SCARA_STATUS_OK;
		} else {
			return SCARA_STATUS_ERROR_OVER_WORKSPACE;
		}
	} else if (TRAJECTORY_J3 == target) {
		if ( (LIM_MIN_J3 <= value) && ( value <= LIM_MAX_J3)) {
			return SCARA_STATUS_OK;
		} else {
			return SCARA_STATUS_ERROR_OVER_WORKSPACE;
		}
	} else {
		return SCARA_STATUS_ERROR;
	}
}
SCARA_StatusTypeDef	PNPcalMovtime(SCARA_PositionTypeDef start_point, SCARA_PositionTypeDef object_point, double K)
{
	double deltax = object_point.x - start_point.x;
	double deltay = object_point.y - start_point.y;
	double distance = sqrt(deltax*deltax + deltay*deltay);
	double alpha = atan2(deltax, deltay);
	double beta = asin(K*sin(alpha));
	return SCARA_STATUS_OK;
}

/* Check all of point in travel */
SCARA_StatusTypeDef	scaraTestDuty(void) {
	int32_t sample_count;
	double run_time = 0;
	SCARA_PositionTypeDef test_Next, test_Current;
	SCARA_StatusTypeDef status;
	memcpy(&test_Current, &positionCurrent, sizeof(SCARA_PositionTypeDef));
	sample_count = ceil(myDUTY.time_total/T_SAMPLING);
	for (int32_t i = 1; i < sample_count; i++) {
		run_time += T_SAMPLING;
		status = scaraFlowDuty(run_time, &test_Next, test_Current);
		if (status != SCARA_STATUS_OK) {
			return status;
		}
		memcpy(&test_Next, &test_Current, sizeof(SCARA_PositionTypeDef));
	}
	return SCARA_STATUS_OK;
}

void				scaraSetScanFlag(void) {
	mySCARA.isScanLitmit = TRUE;
}

void				scaraSetOutput		(uint8_t level) {
	mySCARA.outputSet	 = level;
	lowlayer_setOutput(level);
}

void				scaraSetDutyState(SCARA_DutyStateTypeDef state) {
	mySCARA.duty_State = state;
}

void				scaraSetMode(SCARA_ModeTypeDef mode) {
	mySCARA.mode = mode;
}

void				scaraSetMethod(SCARA_MethodTypeDef method) {
	mySCARA.method = method;
}


void				scaraGetPosition	(SCARA_PositionTypeDef *pos) {
	memcpy(pos, &positionCurrent, sizeof(SCARA_PositionTypeDef));
}

void				scaraUpdatePosition (SCARA_PositionTypeDef *pos) {
	memcpy(&positionNext, pos, sizeof(SCARA_PositionTypeDef));
}


SCARA_ModeTypeDef	scaraGetMode(void) {
	return mySCARA.mode;
}

SCARA_MethodTypeDef	scaraGetMethod(void) {
	return mySCARA.method;
}

SCARA_DutyStateTypeDef	scaraGetDutyState(void) {
	return mySCARA.duty_State;
}

uint8_t					scaraIsScanLimit(void) {
	return mySCARA.isScanLitmit;
}

uint8_t					scaraIsFinish		(double run_time) {
	if(myDUTY.time_total < run_time) {
		return TRUE;
	} else {
		return FALSE;
	}
}


int32_t scaraPosition_packaging(uint8_t *data_packet, SCARA_PositionTypeDef position)
{
	int32_t cur_ptr = -4;
//	Append_Coordinate_Value((int32_t)(position.x*DATA_FOWARD_SCALE), &data_packet[cur_ptr+=4]);
//	Append_Coordinate_Value((int32_t)(position.y*DATA_FOWARD_SCALE), &data_packet[cur_ptr+=4]);
//	Append_Coordinate_Value((int32_t)(position.z*DATA_FOWARD_SCALE), &data_packet[cur_ptr+=4]);
//	Append_Coordinate_Value((int32_t)(position.roll*DATA_FOWARD_SCALE), &data_packet[cur_ptr+=4]);
	Append_Coordinate_Value((int32_t)(position.Theta1*DATA_FOWARD_SCALE), &data_packet[cur_ptr+=4]);
	Append_Coordinate_Value((int32_t)(position.Theta2*DATA_FOWARD_SCALE), &data_packet[cur_ptr+=4]);
	Append_Coordinate_Value((int32_t)(position.D3*DATA_FOWARD_SCALE), &data_packet[cur_ptr+=4]);
	Append_Coordinate_Value((int32_t)(position.Theta4*DATA_FOWARD_SCALE), &data_packet[cur_ptr+=4]);
	return cur_ptr + 4;
}

void Append_Coordinate_Value(int32_t append_value, uint8_t* append_position)
{
	append_position[0] = (uint8_t)(append_value & 0xff);
	append_position[1] = (uint8_t)(append_value >> 8 & 0xff);
	append_position[2] = (uint8_t)(append_value >> 16 & 0xff);
	append_position[3] = (uint8_t)(append_value >> 24 & 0xff);
}
SCARA_StatusTypeDef  scaraKeyInit1(SCARA_KeyTypeDef key, int32_t speed)
{
	DUTY_Command_TypeDef cmd;
	SCARA_StatusTypeDef status;
	cmd.coordinate_type = DUTY_COORDINATES_REL;
	cmd.trajec_type = DUTY_TRAJECTORY_LSPB;
	cmd.modeInit_type = DUTY_MODE_INIT_QVT;

	if(speed < SHIFT_SPEED_MIN || speed > SHIFT_SPEED_MAX){
		return SCARA_STATUS_ERROR_PARA;
	}
	switch(key){
		case SCARA_KEY_X_INC:{
			cmd.v_factor = 0;
			cmd.space_type = DUTY_SPACE_TASK;
			cmd.path_type = DUTY_PATH_LINE;
			cmd.target_point.x 		= speed*SHIFT_KEY_S;
			cmd.target_point.y 		= 0;
			cmd.target_point.z 		= 0;
			cmd.target_point.roll 	= 0;
			cmd.time_total 			= 0.8 + speed*SHIFT_KEY_TIME;
		}
		break;
		case SCARA_KEY_X_DEC:{
			cmd.v_factor = 0;
			cmd.space_type = DUTY_SPACE_TASK;
			cmd.path_type = DUTY_PATH_LINE;
			cmd.target_point.x 		= -speed*SHIFT_KEY_S;
			cmd.target_point.y 		= 0;
			cmd.target_point.z 		= 0;
			cmd.target_point.roll 	= 0;
			cmd.time_total 			= 0.8 + speed*SHIFT_KEY_TIME;
		}
		break;
		case SCARA_KEY_Y_INC:{
			cmd.v_factor = 0;
			cmd.space_type = DUTY_SPACE_TASK;
			cmd.path_type = DUTY_PATH_LINE;
			cmd.target_point.x 		= 0;
			cmd.target_point.y 		= speed*SHIFT_KEY_S;
			cmd.target_point.z 		= 0;
			cmd.target_point.roll 	= 0;
			cmd.time_total 			= 0.8 + speed*SHIFT_KEY_TIME;
		}
		break;

		case SCARA_KEY_Y_DEC:{
			cmd.v_factor = 0;
			cmd.space_type = DUTY_SPACE_TASK;
			cmd.path_type = DUTY_PATH_LINE;
			cmd.target_point.x 		= 0;
			cmd.target_point.y 		= -speed*SHIFT_KEY_S;
			cmd.target_point.z 		= 0;
			cmd.target_point.roll 	= 0;
			cmd.time_total 			= 0.8 + speed*SHIFT_KEY_TIME;
		}
		break;
		case SCARA_KEY_Z_INC:{
			cmd.v_factor = 0;
			cmd.space_type = DUTY_SPACE_TASK;
			cmd.path_type = DUTY_PATH_LINE;
			cmd.target_point.x 		= 0;
			cmd.target_point.y 		= 0;
			cmd.target_point.z 		= speed*SHIFT_KEY_S;
			cmd.target_point.roll 	= 0;
			cmd.time_total 			= 0.8 + speed*SHIFT_KEY_TIME;
		}
		break;
		case SCARA_KEY_Z_DEC:{
			cmd.v_factor = 0;
			cmd.space_type = DUTY_SPACE_TASK;
			cmd.path_type = DUTY_PATH_LINE;
			cmd.target_point.x 		= 0;
			cmd.target_point.y 		= 0;
			cmd.target_point.z 		= -speed*SHIFT_KEY_S;
			cmd.target_point.roll 	= 0;
			cmd.time_total 			= 0.8 + speed*SHIFT_KEY_TIME;
		}
		break;
		case SCARA_KEY_ROLL_INC:{
			cmd.v_factor = 0;
			cmd.space_type = DUTY_SPACE_TASK;
			cmd.path_type = DUTY_PATH_LINE;
			cmd.target_point.x 		= 0;
			cmd.target_point.y 		= 0;
			cmd.target_point.z 		= 0;
			cmd.target_point.roll 	= speed*SHIFT_KEY_S;
			cmd.time_total 			= 0.8 + speed*SHIFT_KEY_TIME;
		}
		break;
		case SCARA_KEY_ROLL_DEC:{
			cmd.v_factor = 0;
			cmd.space_type = DUTY_SPACE_TASK;
			cmd.path_type = DUTY_PATH_LINE;
			cmd.target_point.x 		= 0;
			cmd.target_point.y 		= 0;
			cmd.target_point.z 		= 0;
			cmd.target_point.roll 	= -speed*SHIFT_KEY_S;
			cmd.time_total 			= 0.8 + speed*SHIFT_KEY_TIME;
		}
		break;
	}

	status = scaraInitDuty(cmd);
	if (status == SCARA_STATUS_OK) {
		status = scaraTestDuty();
		if (status != SCARA_STATUS_OK) {
			return status;
		}

	} else {
		return status;
	}
}



