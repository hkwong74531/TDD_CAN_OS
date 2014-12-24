#ifndef D_CANProtocol_H
#define D_CANProtocol_H

#define	_CPPUTEST_

#include <stdlib.h>
#include <stdint.h>
#ifdef _CPPUTEST_
#include <memory.h>
#else
#include <string.h>
#endif

#include "can_command.h"

typedef enum
{
	CAN_PROTOCOL_IDLE_STATE,
	CAN_PROTOCOL_RCV_COMMAND_STATE,
	CAN_PROTOCOL_REPLY_LOST_STATE,
} canProtocolState_t;

typedef enum
{
	CAN_PROTOCOL_REPLY_EVENT,
	CAN_PROTOCOL_DUMMY_EVENT,
} canProtocolEvent_t;

void can_protocol_init(void);
void can_protocol_deinit(void);
void can_protocol_setSend(canCommand_t*);
uint8_t can_protocol_setReply(uint8_t, uint16_t, uint8_t*);
void can_protocol_getReceived(canCommand_t*);
canProtocolState_t can_protocol_state_process(canProtocolEvent_t);

#endif