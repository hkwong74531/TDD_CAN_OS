#include "can_command.h"

uint8_t can_message_to_command(canMessage_t message, canCommand_t* command)
{
	free(command->data);
	
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
		command->data = malloc(command->data_length - 4);
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

void can_command_construct(
	canCommand_t* canCommand, 
	uint8_t  identifier,
	uint8_t  message_type,
	uint8_t  command,
	uint8_t  command_type,
	uint16_t data_length,
	uint16_t message_id,
	uint8_t* data)
{
	canCommand->identifier = identifier;
	canCommand->message_type = message_type;
	canCommand->command = command;
	canCommand->command_type = command_type;
	canCommand->data_length = data_length;
	canCommand->message_id = message_id;
	canCommand->data = data;
}
