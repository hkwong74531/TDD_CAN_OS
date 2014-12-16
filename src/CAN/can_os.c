#include "can_os.h" 

uint8_t can_message_construct(canMessage_t* message, uint8_t type, uint16_t id, uint8_t length, uint8_t* data)
{
	if(id > CAN_ID_RANGE)
	{
		return 0;
	}
	else if(length > CAN_MAX_DATA_LENGTH)
	{
		return 0;
	}
	else if(type > MESSAGE_TYPE_RANGE)
	{
		return 0;
	}
	
	(*message).length = length;
	
  	if(length > CAN_PACKET_SIZE + 1)
  	{
  		(*message).header.bit.segment = 1;
		(*message).total_packet = length/CAN_PACKET_SIZE + ((length%CAN_PACKET_SIZE)?1:0);
		(*message).last_packet_length = (length%CAN_PACKET_SIZE)?(length%CAN_PACKET_SIZE):6;
  	}
  	else
  	{ 
  		(*message).header.bit.segment = 0;
		(*message).total_packet = 1;
		(*message).last_packet_length = length;
  	}

	(*message).identifier.bit.nodeID = id;
	(*message).identifier.bit.messageType = type;
//	(*message).identifier.bitField = (type << 7)|id;
	(*message).header.bit.nodeID = id;
	(*message).segment.bit.packetCnt = 0;
	(*message).segment.bit.segmentType = CAN_FIRST_PACKET;
	
	memcpy(message->data, data, length);
	
	return 1;
}

uint8_t can_message_transmit(canMessage_t* message, uint8_t packet)
{
	CanTxMsg txMsg;

	uint8_t dlc;
	uint8_t total_packet = 1;
	uint8_t data[8];

	if(message->length > CAN_MAX_DATA_LENGTH)
	{
		return 0;
	}
	else if(message->length > CAN_PACKET_SIZE + 1 && (*message).header.bit.segment == 0)	// wrong format
	{
		return 0;
	}
	
	if(message->length <= CAN_PACKET_SIZE + 1 && packet > 0)
	{
		return 0;
	}
	
	if(packet == total_packet)
	{
		dlc = (*message).last_packet_length + 2;
	}
	else
	{
		dlc = CAN_PACKET_SIZE + 2;
	}
	
	data[0] = (*message).header.bitField;
	data[1] = (*message).segment.bitField;
	memcpy(&(data[2]), message->data, dlc - 2);
	CAN_SetTxMsg(&txMsg, (*message).identifier.bitField, CAN_ID_STD, dlc, data);
	
	return 1;
}
/**************************END OF FILE************************************/

