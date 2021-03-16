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

#define OFFSET_X        250.0
#define OFFSET_Y        -150.0
#define OFFSET_Z		83.3

void update_gcode_point(DUTY_Command_TypeDef *duty_cmd, SCARA_Gcode_Cor_TypeDef gcode_point);


#endif /* INC_GCODE_HANDLER_H_ */
