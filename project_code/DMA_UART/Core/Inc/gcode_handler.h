/*
 * gcode_handler.h
 *
 *  Created on: Mar 13, 2021
 *      Author: Admin
 */

#ifndef INC_GCODE_HANDLER_H_
#define INC_GCODE_HANDLER_H_
#include "main.h"
#include "robot_scara.h"
#include "system_params.h"
#include "command_respond.h"

#define OFFSET_X        200.0
#define OFFSET_Y        -75.0
#define OFFSET_Z		95.27
double offset_x;
double offset_y;
double offset_z;
double roll_angle;
// 170
// -95
// 133
// 90

//X: 120 maximum
//Y: 250 maximum

int16_t 						pre_clutch_index;
Gcode_Packet_Command_TypeDef 	pre_height;
Gcode_Packet_Command_TypeDef 	Gcode_Mode;
double 							acc0;
double 							constant[2];
double							deacc[3];
double							time_acc;
double 							time_dec;
double							time_move;
double 							last_T;
double 							accumulate_s;
double							bezier_x_coeffs[3];
double							bezier_y_coeffs[3];
double 							bezier_wc;
void update_gcode_point(DUTY_Command_TypeDef *duty_cmd, int32_t run_point);
void LSPB_calculation(SCARA_LSPB_Clutch_TypeDef configure);
void accumulate_update(SCARA_Gcode_Cor_TypeDef gcode_point);
//SCARA_StatusTypeDef	Gcode_Testing();


#endif /* INC_GCODE_HANDLER_H_ */
