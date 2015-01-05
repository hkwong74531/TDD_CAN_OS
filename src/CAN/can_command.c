#include "can_command.h"

static canCommandState_t canCommandState = CAN_COMMAND_IDLE_STATE;
static canCommandEvent_t canCommandEvent = CAN_COMMAND_DUMMY_EVENT;
static canMessage_t can_command_message;
static uint8_t can_command_allSent = 0;
static uint8_t can_command_allReceived = 0;
static uint16_t can_command_packetCnt = 0;

static void can_command_idle_state(canCommandEvent_t* canCommandEvent);
static void can_command_sending_state(canCommandEvent_t* canCommandEvent);
static void can_command_receiving_state(canCommandEvent_t* canCommandEvent);

static void can_command_idle_enter(void);
static void can_command_sending_enter(void);
static void can_command_receiving_enter(void);

void (*can_command_state_function[CAN_COMMAND_DUMMY_STATE])(canCommandEvent_t* event) =
{
	NULL,
	can_command_idle_state,
	can_command_sending_state,
	can_command_receiving_state,
};

static void can_command_idle_enter(void)
{
	canCommandState = CAN_COMMAND_IDLE_STATE;
	can_command_packetCnt = 0;
}

static void can_command_idle_state(canCommandEvent_t* canCommandEvent)
{
	uint8_t ret;
	canCommandEvent_t event;

	event = *canCommandEvent;
	*canCommandEvent = CAN_COMMAND_DUMMY_EVENT;
	switch(event)
	{
	case CAN_COMMAND_SEND_EVENT:
		can_command_allSent = 0;
		ret = can_message_transmit(&can_command_message, 0);
		if(ret == 1)
		{
			can_command_packetCnt++;
			if(can_command_packetCnt < can_command_message.total_packet)
			{
				can_command_sending_enter();
			}
			else
			{
				can_command_allSent = 1;
			}
		}
		break;
	case CAN_COMMAND_PACKET_RECEIVED_EVENT:
		memset(&can_command_message, 0, sizeof(canMessage_t));	
		ret = can_message_receive(&can_command_message);
		if(ret == CAN_RCV_SINGLE_PACKET)
		{
			can_command_allReceived = 1;
		}
		else if(ret != CAN_RCV_ERROR_PACKET)
		{
			can_command_receiving_enter();
		}
		break;
	case CAN_COMMAND_INIT_EVENT:
		break;
	case CAN_COMMAND_DUMMY_EVENT:
	default:
		break;
	}
}

static void can_command_sending_enter(void)
{
	canCommandState = CAN_COMMAND_SENDING_STATE;
	can_command_allSent = 0;
}

static void can_command_sending_state(canCommandEvent_t* canCommandEvent)
{
	uint8_t ret;

	canCommandEvent_t event;

	event = *canCommandEvent;
	*canCommandEvent = CAN_COMMAND_DUMMY_EVENT;
	switch(event)
	{
	case CAN_COMMAND_INIT_EVENT:
		can_command_idle_enter();
		break;
	case CAN_COMMAND_SEND_EVENT:
		ret = can_message_transmit(&can_command_message, can_command_packetCnt);
		if(ret == 1)
		{
			can_command_packetCnt++;
			if(can_command_packetCnt == can_command_message.total_packet)
			{
				can_command_allSent = 1;
				can_command_idle_enter();
			}
		}
		break;
	case CAN_COMMAND_DUMMY_EVENT:
		if(can_command_packetCnt < can_command_message.total_packet)
		{
			ret = can_message_transmit(&can_command_message, can_command_packetCnt);
			if(ret == 1)
			{
				can_command_packetCnt++;
				if(can_command_packetCnt == can_command_message.total_packet)
				{
					can_command_allSent = 1;
					can_command_idle_enter();
				}
			}
		}
		break;
	case CAN_COMMAND_PACKET_RECEIVED_EVENT:		
	default:
		break;
	}
}

static void can_command_receiving_enter(void)
{
	canCommandState = CAN_COMMAND_RECEIVING_STATE;
	can_command_allReceived = 0;
}

static void can_command_receiving_state(canCommandEvent_t* canCommandEvent)
{
	uint8_t ret;
	canCommandEvent_t event;

	event = *canCommandEvent;
	*canCommandEvent = CAN_COMMAND_DUMMY_EVENT;
	switch(event)
	{
	case CAN_COMMAND_INIT_EVENT:
		can_command_idle_enter();
		break;
	case CAN_COMMAND_PACKET_RECEIVED_EVENT:
		ret = can_message_receive(&can_command_message);
		if(ret == CAN_RCV_LAST_PACKET)
		{
			can_command_allReceived = 1;
			can_command_idle_enter();
		}
		break;
	case CAN_COMMAND_SEND_EVENT:
	case CAN_COMMAND_DUMMY_EVENT:	
	default:
		break;
	}
}

void can_command_init(void)
{
	canCommandState = CAN_COMMAND_IDLE_STATE;
	canCommandEvent = CAN_COMMAND_DUMMY_EVENT;
	memset(&can_command_message, 0, sizeof(canMessage_t));
	can_command_allSent = 0;
	can_command_allReceived = 0;
	can_command_packetCnt = 0;
}

uint8_t can_message_to_command(canMessage_t message, canCommand_t* command)
{
	command->identifier = message.identifier.bit.nodeID;
	command->message_type = message.identifier.bit.messageType;
	command->command = message.data[0];
	command->command_type = message.data[1];
	command->data_length = (message.data[2] << 8) | message.data[3];
	if(command->data_length != message.length - 2)
	{
		return 0;
	}
	command->message_id = (message.data[4] << 8) | message.data[5];
	if(command->data_length > CAN_MAX_DATA_LENGTH)
	{
		return 0;
	}
	else
	{
		command->data = realloc(command->data, command->data_length - 4);
	}
	memcpy(command->data, &(message.data[6]), command->data_length - 4);
	return 1;
}

uint8_t can_command_to_message(canCommand_t command, canMessage_t* message)
{
	if(command.data_length < 4)
	{
		return 0;
	}
	else if(command.data == NULL && command.data_length > 4)
	{
		return 0;
	}
	(*message).identifier.bit.nodeID = command.identifier;
	(*message).identifier.bit.messageType = command.message_type;
	(*message).header.bit.nodeID = command.identifier;
	(*message).segment.bit.packetCnt = 0;
	(*message).segment.bit.segmentType = 0;
	(*message).data[0] = command.command;
	(*message).data[1] = command.command_type;
	(*message).data[2] = (command.data_length) >> 8;
	(*message).data[3] = (command.data_length) & 0x00FF;
	(*message).data[4] = (command.message_id) >> 8;
	(*message).data[5] = (command.message_id) & 0x00FF;
	memcpy(message->data + 6, command.data, command.data_length - 4);
	message->length = command.data_length + 2;
	if(message->length > CAN_PACKET_SIZE + 1)
	{
		(*message).header.bit.segment = 1;
		(*message).total_packet = (message->length)/CAN_PACKET_SIZE + (((message->length)%CAN_PACKET_SIZE)?1:0);
		(*message).last_packet_length = ((message->length)%CAN_PACKET_SIZE)?((message->length)%CAN_PACKET_SIZE):6;		
	}
	else
	{
		(*message).header.bit.segment = 0;
		(*message).total_packet = 1;
		(*message).last_packet_length = message->length;
	}
	return 1;
}

uint8_t can_command_transmit(canCommand_t command)
{
	uint8_t ret;
	
	ret = can_command_to_message(command, &can_command_message);
	if(ret == 1)
	{
		canCommandEvent = CAN_COMMAND_SEND_EVENT;
		return 1;
	}
	return 0;
}

uint8_t can_command_acquired(canCommand_t* command)
{
	uint8_t ret;
	
	ret = can_message_to_command(can_command_message, command);
	
	return ret;
}

uint8_t can_command_isAllSent(void)
{
	return can_command_allSent;
}

uint8_t can_command_isAllReceived(void)
{
	return can_command_allReceived;
}

void can_command_clearAllSent(void)
{
	can_command_allSent = 0;
}

void can_command_clearAllReceived(void)
{
	can_command_allReceived = 0;
}

uint16_t can_command_getPacketCnt(void)
{
	return can_command_packetCnt;
}

canCommandState_t can_command_getState(void)
{
	return canCommandState;
}

void can_command_setEvent(canCommandEvent_t event)
{
	canCommandEvent = event;
}

void can_command_state_process(void)
{
	(*can_command_state_function[canCommandState])(&canCommandEvent);
}
