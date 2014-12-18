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
	uint8_t dlc;
	uint8_t data[8];

	if(message->length > CAN_MAX_DATA_LENGTH)
	{
		return 0;
	}
	else if(message->length > CAN_PACKET_SIZE + 1 && (*message).header.bit.segment == 0)	// wrong format
	{
		return 0;
	}
	else if(packet >= message->total_packet)
	{
		return 0;
	}
	else if(message->length <= CAN_PACKET_SIZE + 1 && packet > 0)
	{
		return 0;
	}
	
	(*message).segment.bit.packetCnt = packet;

	if(message->length <= CAN_PACKET_SIZE + 1)		// single & first packet
	{
		dlc = (*message).last_packet_length + 1;
		data[0] = (*message).header.bitField;
		memcpy(&(data[1]), message->data, (*message).last_packet_length);
	}
	else if(packet == 0)	// first packet
	{
		dlc = CAN_PACKET_SIZE + 2;
		(*message).segment.bit.segmentType = CAN_FIRST_PACKET;
		data[0] = (*message).header.bitField;
		data[1] = (*message).segment.bitField;		
		memcpy(&(data[2]), message->data, CAN_PACKET_SIZE);
	}
	else if(packet == message->total_packet - 1)	// last packet
	{
		dlc = (*message).last_packet_length + 2;
		(*message).segment.bit.segmentType = CAN_LAST_PACKET;		
		data[0] = (*message).header.bitField;
		data[1] = (*message).segment.bitField;
		memcpy(&(data[2]), message->data + packet*CAN_PACKET_SIZE, (*message).last_packet_length);
	}
	else	// general packet
	{
		dlc = CAN_PACKET_SIZE + 2;
		(*message).segment.bit.segmentType = CAN_GENERAL_PACKET;		
		data[0] = (*message).header.bitField;
		data[1] = (*message).segment.bitField;
		memcpy(&(data[2]), message->data + packet*CAN_PACKET_SIZE, CAN_PACKET_SIZE);
	}
	
	CAN_SetTxMsg((*message).identifier.bitField, CAN_ID_STD, dlc, data);
	
	return 1;
}

uint8_t can_message_receive(canMessage_t* message)
{
	uint16_t id;
	uint32_t id_type;
	uint8_t dlc;
	uint8_t data[8];

	CAN_GetRxMsg((uint32_t *)&id, &id_type, &dlc, data);
	
	if(id_type == CAN_ID_STD)
	{
		if((id & 0x3F) == (data[0] & 0x3F))	// node ID correct format
		{
			if(!(data[0] & 0x80))	// single packet
			{
				(*message).identifier.bitField = id;
				(*message).header.bitField = data[0];
				message->length = dlc - 1;
				message->total_packet = 1;
				message->last_packet_length = dlc - 1;
				memcpy(message->data, data + 1, dlc - 1);
				
				return CAN_RCV_SINGLE_PACKET;
			}
			else if(data[1] == 0 && dlc == CAN_PACKET_SIZE + 2)	// 1st packet
			{
				(*message).identifier.bitField = id;
				(*message).header.bitField = data[0];
				(*message).segment.bitField = data[1];
				message->length = CAN_PACKET_SIZE;
				message->total_packet = 1;				
				message->last_packet_length = CAN_PACKET_SIZE;
				memcpy(message->data, data + 2, CAN_PACKET_SIZE);
				
				return CAN_RCV_FIRST_PACKET;
			}
			else if((data[1] & 0xC0) == 0x40 && dlc == CAN_PACKET_SIZE + 2)	// general packet
			{
				if(id == (*message).identifier.bitField)	// should be same data set_new_handler
				{
					if((data[1] & 0x3F) == message->total_packet)	// correct packet sequence
					{
						uint8_t packetCnt;
					
						packetCnt = (data[1] & 0x3F);
						(message->total_packet)++;
						message->segment.bitField = data[1];
						message->length += CAN_PACKET_SIZE;
						message->last_packet_length = CAN_PACKET_SIZE;
						memcpy((message->data)+packetCnt*CAN_PACKET_SIZE, data + 2, CAN_PACKET_SIZE);
						
						return CAN_RCV_GENERAL_PACKET;
					}
				}
			}
			else if((data[1] & 0xC0) == 0x80)	// last packet
			{
				if(id == (*message).identifier.bitField)	// should be same data set_new_handler
				{
					if((data[1] & 0x3F) == message->total_packet)	// correct packet sequence
					{
						uint8_t packetCnt;
					
						packetCnt = (data[1] & 0x3F);
						(message->total_packet)++;
						message->segment.bitField = data[1];
						message->length += (dlc - 2);
						message->last_packet_length = dlc - 2;
						memcpy((message->data)+packetCnt*CAN_PACKET_SIZE, data + 2, dlc - 2);
						
						return CAN_RCV_LAST_PACKET;
					}
				}
			}
		}
	}
	
	return CAN_RCV_ERROR_PACKET;
}
/**************************END OF FILE************************************/

