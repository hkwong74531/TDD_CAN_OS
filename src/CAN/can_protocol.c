#include "can_protocol.h"

static canProtocolState_t canProtocolState;
static canCommand_t canProtocolReceived, canProtocolSend;
static uint16_t canProtocol_ctrlID, canProtocol_rdrID;//, canProtocol_commandType, canProtocol_dataLength;
//static uint8_t canProtocolData[160];
#if 0
static uint8_t can_protocol_reply(
	canCommand_t* canCommandOut, 
	canCommand_t canCommandIn,
	uint8_t command_type,
	uint16_t data_length,
	uint8_t* data)
{
	uint8_t ret = 0;

	free(canCommandOut->data);

	canCommandOut->identifier = canCommandIn.identifier;
	canCommandOut->message_type = canCommandIn.message_type;
	canCommandOut->command = canCommandIn.command;
	canCommandOut->message_id = canCommandIn.message_id;
	
	canCommandOut->command_type = command_type;
	canCommandOut->data_length = data_length;

	if(data_length > 4)
	{
		canCommandOut->data = malloc(data_length - 4);
		if(canCommandOut->data != NULL)
		{
			ret = 1;
			memcpy(canCommandOut->data, data, data_length - 4);
		}
	}
	else if(data_length == 4)
	{
		ret = 1;
	}
	return ret;
}
#endif
void can_protocol_init(void)
{
	canProtocolState = CAN_PROTOCOL_IDLE_STATE;
	canProtocol_ctrlID = 0;
	canProtocol_rdrID  = 0;
	memset(&canProtocolReceived, 0, sizeof(canCommand_t));
	memset(&canProtocolSend, 0, sizeof(canCommand_t));	
}

void can_protocol_deinit(void)
{
	free(canProtocolReceived.data);
	free(canProtocolSend.data);	
}

void can_protocol_setSend(canCommand_t* send)
{
	memcpy(&canProtocolSend, send, sizeof(canCommand_t));
}

uint8_t can_protocol_setReply(uint8_t command_type, uint16_t data_length, uint8_t* data)
{
	uint8_t ret = 0;

	if(canProtocolSend.data != NULL)
	{
		free(canProtocolSend.data);
	}

	canProtocolSend.identifier = canProtocolReceived.identifier;
	canProtocolSend.message_type = canProtocolReceived.message_type;
	canProtocolSend.command = canProtocolReceived.command;
	canProtocolSend.message_id = canProtocol_ctrlID;
	
	canProtocolSend.command_type = command_type;
	canProtocolSend.data_length = data_length;

	if(data_length > 4)
	{
		canProtocolSend.data = malloc(data_length - 4);
		if(canProtocolSend.data != NULL)
		{
			ret = 1;
			memcpy(canProtocolSend.data, data, data_length - 4);
		}
	}
	else if(data_length == 4)
	{
		ret = 1;
	}
	return ret;	
}

void can_protocol_getReceived(canCommand_t* received)
{
	memcpy(received, &canProtocolReceived, sizeof(canCommand_t));
}

canProtocolState_t can_protocol_state_process(canProtocolEvent_t event)
{
	can_command_state_process();
	switch(canProtocolState)
	{
	uint8_t ret;
	
	case CAN_PROTOCOL_IDLE_STATE:
		if(can_command_isAllReceived())
		{
			canCommand_t canProtocolBuffer;
			
			memcpy(&canProtocolBuffer, &canProtocolReceived, sizeof(canCommand_t));
			ret = can_command_acquired(&canProtocolReceived);
			if(canProtocolReceived.message_id == canProtocol_ctrlID)
			{
				canProtocolState = CAN_PROTOCOL_RCV_COMMAND_STATE;
			}
			else if(canProtocolReceived.message_id == canProtocol_ctrlID - 1)
			{
				if(!memcmp(&canProtocolBuffer, &canProtocolReceived, sizeof(canCommand_t)))
				{
					canProtocolState = CAN_PROTOCOL_REPLY_LOST_STATE;
				}
			}
		}
		break;
	case CAN_PROTOCOL_RCV_COMMAND_STATE:
	case CAN_PROTOCOL_REPLY_LOST_STATE:
		if(event == CAN_PROTOCOL_REPLY_EVENT)
		{
			ret = can_command_transmit(canProtocolSend);
			if(ret == 1)
			{
				canProtocolState = CAN_PROTOCOL_IDLE_STATE;
				canProtocol_ctrlID++;
			}
		}
		break;
	default:
		break;
	}
	
	return canProtocolState;
}
