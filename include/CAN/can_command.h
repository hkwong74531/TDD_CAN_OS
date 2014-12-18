#ifndef D_CANCommand_H
#define D_CANCommand_H

#define	_CPPUTEST_

#include <stdlib.h>
#include <stdint.h>
#ifdef _CPPUTEST_
#include <memory.h>
#else
#include <string.h>
#endif

#include "can_os.h"

typedef struct{
	uint8_t  identifier;
	uint8_t  message_type;
	uint8_t  command;
	uint8_t  command_type;
	uint16_t data_length;
	uint16_t message_id;
	uint8_t* data;
} canCommand_t;

uint8_t can_message_to_command(canMessage_t, canCommand_t*);
uint8_t can_command_to_message(canCommand_t, canMessage_t*);
void can_command_construct(canCommand_t*, uint8_t, uint8_t, uint8_t, uint8_t, uint16_t, uint16_t, uint8_t*);

#endif