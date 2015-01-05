#include "can_protocol.h"

static uint8_t canProtocolRcvFlag;
static canProtocolState_t canProtocolState;
static canCommand_t canProtocolReceived, canProtocolSend;
static uint16_t canProtocol_ctrlID, canProtocol_rdrID;

uint16_t can_protocol_getCtrlID(void)
{
	return canProtocol_ctrlID;
}

uint16_t can_protocol_getRdrID(void)
{
	return canProtocol_rdrID;
}

void can_protocol_init(void)
{
	can_command_init();
	canProtocolState = CAN_PROTOCOL_IDLE_STATE;
	canProtocolRcvFlag = 0;
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
	free(canProtocolSend.data);

	memcpy(&canProtocolSend, send, sizeof(canCommand_t));
}

uint8_t can_protocol_setReply(uint8_t command_type, uint16_t data_length, uint8_t* data)
{
	uint8_t ret = 0;

//	free(canProtocolSend.data);
	
	canProtocolSend.identifier = canProtocolReceived.identifier;
	canProtocolSend.message_type = canProtocolReceived.message_type;
	canProtocolSend.command = canProtocolReceived.command;
	canProtocolSend.message_id = canProtocol_ctrlID;
	
	canProtocolSend.command_type = command_type;
	canProtocolSend.data_length = data_length;

	if(data_length > 4)
	{
		canProtocolSend.data = realloc(canProtocolSend.data, data_length - 4);
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

uint8_t can_protocol_getRcvFlag(void)
{
	return canProtocolRcvFlag;
}

void can_protocol_getReceived(canCommand_t* received)
{
	memcpy(received, &canProtocolReceived, sizeof(canCommand_t));
	canProtocolRcvFlag = 0;
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
				canProtocolRcvFlag = 1;
				canProtocolState = CAN_PROTOCOL_RCV_COMMAND_STATE;
			}
			else if(canProtocolReceived.message_id == canProtocol_ctrlID - 1)
			{
				if(!memcmp(&canProtocolBuffer, &canProtocolReceived, sizeof(canCommand_t)))
				{
					canProtocolRcvFlag = 1;
					canProtocol_ctrlID--;
					canProtocolState = CAN_PROTOCOL_REPLY_LOST_STATE;
				}
			}
		}
		else if(event == CAN_PROTOCOL_SEND_EVENT)
		{
			canProtocolRcvFlag = 0;
			ret = can_command_transmit(canProtocolSend);
			if(can_command_isAllSent())
			{
				canProtocolState = CAN_PROTOCOL_SENT_COMMAND_STATE;
				can_command_clearAllSent();
			}
			else
			{
				canProtocolState = CAN_PROTOCOL_SENDING_COMMAND_STATE;
			}
		}
		break;
	case CAN_PROTOCOL_RCV_COMMAND_STATE:
	case CAN_PROTOCOL_REPLY_LOST_STATE:
		if(event == CAN_PROTOCOL_REPLY_EVENT)
		{
			canProtocolRcvFlag = 0;
			ret = can_command_transmit(canProtocolSend);
			if(can_command_isAllSent())
			{
				canProtocolState = CAN_PROTOCOL_IDLE_STATE;
				can_command_clearAllSent();
				can_command_clearAllReceived();
				canProtocol_ctrlID++;
			}
			else
			{
				canProtocolState = CAN_PROTOCOL_REPLYING_COMMAND_STATE;
			}
		}
		break;
	case CAN_PROTOCOL_SENDING_COMMAND_STATE:
		if(can_command_isAllSent())
		{
			canProtocolState = CAN_PROTOCOL_SENT_COMMAND_STATE;
			can_command_clearAllSent();			
		}		
		break;
	case CAN_PROTOCOL_SENT_COMMAND_STATE:
		if(can_command_isAllReceived())
		{
			ret = can_command_acquired(&canProtocolReceived);
			canProtocolRcvFlag = 1;
			if(canProtocolReceived.message_id == canProtocol_rdrID)
			{
				if(canProtocolReceived.message_type == canProtocolSend.message_type &&
				   canProtocolReceived.command == canProtocolSend.command)
				{
					canProtocolState = CAN_PROTOCOL_IDLE_STATE;
					canProtocol_rdrID++;
				}
			}
			can_command_clearAllReceived();
		}
		else if(event == CAN_PROTOCOL_TIMEOUT_EVENT)
		{
			ret = can_command_transmit(canProtocolSend);
			if(ret == 1)
			{
				canProtocolState = CAN_PROTOCOL_SENDING_COMMAND_STATE;
			}
		}
		break;
	case CAN_PROTOCOL_REPLYING_COMMAND_STATE:
		if(can_command_isAllSent())
		{
			canProtocolState = CAN_PROTOCOL_IDLE_STATE;
			can_command_clearAllSent();	
			can_command_clearAllReceived();	
			canProtocol_ctrlID++;			
		}		
		break;
	default:
		break;
	}
	
	return canProtocolState;
}
