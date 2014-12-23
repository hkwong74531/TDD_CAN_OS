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

typedef enum{
	CAN_COMMAND_RESET_STATE,
	CAN_COMMAND_IDLE_STATE,
	CAN_COMMAND_SENDING_STATE,
	CAN_COMMAND_RECEIVING_STATE,
	CAN_COMMAND_DUMMY_STATE,
} canCommandState_t;

typedef enum{
	CAN_COMMAND_INIT_EVENT,
	CAN_COMMAND_SEND_EVENT,
	CAN_COMMAND_PACKET_RECEIVED_EVENT,
	CAN_COMMAND_DUMMY_EVENT,
} canCommandEvent_t;

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

void can_command_init(void);
uint8_t can_command_transmit(canCommand_t);
uint8_t can_command_acquired(canCommand_t*);
uint8_t can_command_isAllSent(void);
uint8_t can_command_isAllReceived(void);
uint16_t can_command_getPacketCnt(void);
canCommandState_t can_command_getState(void);
void can_command_setEvent(canCommandEvent_t);
void can_command_state_process(void);

#endif