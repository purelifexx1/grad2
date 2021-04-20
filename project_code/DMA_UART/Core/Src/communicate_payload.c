/*
 * comunicate_payload.c
 *
 *  Created on: Mar 2, 2020
 *      Author: Dang Nam
 */


#include "communicate_payload.h"
#include "malloc.h"
#include "string.h"


/*
 * Function: packPayLoad
 * --------------------
 * transform data buffer to message, add control char.
 *
 *  *input_buff		: pointer of input buffer, source
 *  *output_buff	: pointer of output buffer, destination
 *  in_lenght		: lenght of input_buff
 *
 *  returns:		: lenght of output_buff
 *  				  -1 if error
 */
int32_t	packPayload		(uint8_t *input_buff, uint8_t *output_buff, int32_t in_lenght) {
	if (NULL == input_buff) {
		return -1;
	}

	if (NULL == output_buff) {
		return -1;
	}

	int32_t out_lenght = 0;

	out_lenght = in_lenght + 2;//lenght of start char and end char
//	for (uint16_t i = 0; i < in_lenght; i++) {
//		if ( (START_CHAR == input_buff[i]) ||	(END_CHAR == input_buff[i])	|| (ADD_CHAR == input_buff[i]) ) {
//			out_lenght++;
//		}
//	}// compute output buffer lenght

	uint8_t temp_buff[out_lenght];

	out_lenght = 0;
	temp_buff[out_lenght++] = START_CHAR;
	for (int32_t i = 0; i < in_lenght; i++) {
//		if ( (START_CHAR == input_buff[i]) ||	(END_CHAR == input_buff[i])	|| (ADD_CHAR == input_buff[i]) ) {
//
//			temp_buff[out_lenght++] = ADD_CHAR;
//			temp_buff[out_lenght++] = input_buff[i] ^ XOR_CHAR;
//		} else {
//			temp_buff[out_lenght++] = input_buff[i];
//		}
		temp_buff[out_lenght++] = input_buff[i];
	}
	temp_buff[out_lenght++] = END_CHAR;

	memmove(output_buff, temp_buff, out_lenght);

	return out_lenght;
}


/*
 * Function: unPackPayLoad
 * --------------------
 * Restore data buffer, remove control char.
 *
 *  *input_buff		: pointer of mes buffer
 *  in_lenght		: lenght of input_buff
 *
 *  returns:		: lenght of output_buff
 *  				  -1 if error
 */
int32_t	unPackPayload	(uint8_t *message_buff, int32_t in_length, uint8_t*data_packet) {

	int32_t data_length = in_length - 3;
	uint16_t packet_length = *(uint16_t*)(&message_buff[1]);
	//check packet length
	if(packet_length == data_length && backup_available == 0){
		// check minimum lenght
		if (in_length < MIN_MESSAGE_LENGHT) {
			return -1;
		}

		// check start char
		if(message_buff[0] != START_CHAR){
			return -1;
		}
		memcpy(data_packet, &message_buff[3], data_length - RECEIVE_END_LENGTH);
	}else if(packet_length != data_length && backup_available == 0){
		// check start char
		if(message_buff[0] != START_CHAR){
			return -1;
		}
		backup_available = 1;
		memcpy(data_packet, &message_buff[3], data_length);
		backup_length = data_length;
		return - 1;
	}else if(backup_available == 1){
		backup_available = 0;
		memcpy(&data_packet[backup_length], message_buff, in_length);
		data_length = backup_length + in_length;
	}else{
		return - 1;
	}


	return data_length - RECEIVE_END_LENGTH;
}
