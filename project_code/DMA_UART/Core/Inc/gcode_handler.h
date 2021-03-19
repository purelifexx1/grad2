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


void update_gcode_point(DUTY_Command_TypeDef *duty_cmd, SCARA_Gcode_Cor_TypeDef gcode_point);


#endif /* INC_GCODE_HANDLER_H_ */
