//- ------------------------------------------------------------------
//-    Copyright (c) James W. Grenning -- All Rights Reserved         
//-    For use by owners of Test-Driven Development for Embedded C,   
//-    and attendees of Renaissance Software Consulting, Co. training 
//-    classes.                                                       
//-                                                                   
//-    Available at http://pragprog.com/titles/jgade/                 
//-        ISBN 1-934356-62-X, ISBN13 978-1-934356-62-3               
//-                                                                   
//-    Authorized users may use this source code in your own          
//-    projects, however the source code may not be used to           
//-    create training material, courses, books, articles, and        
//-    the like. We make no guarantees that this source code is       
//-    fit for any purpose.                                           
//-                                                                   
//-    www.renaissancesoftware.net james@renaissancesoftware.net      
//- ------------------------------------------------------------------

extern "C"
{
#include "can_os.h"
#include "can_command.h"
}

#include "CppUTest/TestHarness.h"

canMessage_t message;
canCommand_t command;
uint8_t data[256];
extern CanTxMsg txMsg;
extern CanRxMsg rxMsg;

TEST_GROUP(CANOSDriver)
{
    void setup()
    {
		uint16_t i;
		
		for(i = 0; i < 256; i++)
		{
			data[i] = i;
		}
		memset(&rxMsg, 0, sizeof(CanRxMsg));
		memset(&message, 0, sizeof(canMessage_t));
		memset(&command, 0, sizeof(canCommand_t));
    }

    void teardown()
    {
		free(command.data);
    }
};

IGNORE_TEST(CANOSDriver, testConstructor)
{
	uint8_t ret;

	ret = can_message_construct(&message, 0x0a, 0x08, 8, data);	

	BYTES_EQUAL(1, ret);
	BYTES_EQUAL(1, message.header.bit.segment);
	BYTES_EQUAL(0x0508, message.identifier.bitField);
	BYTES_EQUAL(0x0a, message.identifier.bit.messageType);
	BYTES_EQUAL(0x08, message.identifier.bit.nodeID);

	BYTES_EQUAL(0x88, message.header.bitField);
	BYTES_EQUAL(0x00, message.segment.bitField);
	BYTES_EQUAL(8, message.length);
	BYTES_EQUAL(2, message.total_packet);
	BYTES_EQUAL(2, message.last_packet_length);
	BYTES_EQUAL(0, message.data[0]);
	BYTES_EQUAL(1, message.data[1]);
	BYTES_EQUAL(2, message.data[2]);	
}

TEST(CANOSDriver, testTransmitter1)
{
	uint8_t ret;
	
	ret = can_message_construct(&message, 0x0a, 0x08, 15, data);		
	ret = can_message_transmit(&message, 2);
	
	BYTES_EQUAL(1, ret);
	BYTES_EQUAL(5, txMsg.DLC);
	BYTES_EQUAL(CAN_ID_STD, txMsg.IDE);
	LONGS_EQUAL(0x0508, txMsg.StdId);
	BYTES_EQUAL(0x88, txMsg.Data[0]);
	BYTES_EQUAL(0x82, txMsg.Data[1]);	
	BYTES_EQUAL(12, txMsg.Data[2]);	
	BYTES_EQUAL(13, txMsg.Data[3]);	
	BYTES_EQUAL(14, txMsg.Data[4]);		
}

TEST(CANOSDriver, testTransmitter2)
{
	uint8_t ret;
	
	ret = can_message_construct(&message, 0x0a, 0x08, 15, data);		
	ret = can_message_transmit(&message, 1);
	
	BYTES_EQUAL(1, ret);
	BYTES_EQUAL(8, txMsg.DLC);
	BYTES_EQUAL(CAN_ID_STD, txMsg.IDE);
	LONGS_EQUAL(0x0508, txMsg.StdId);
	BYTES_EQUAL(0x88, txMsg.Data[0]);
	BYTES_EQUAL(0x41, txMsg.Data[1]);	
	BYTES_EQUAL(6, txMsg.Data[2]);	
	BYTES_EQUAL(7, txMsg.Data[3]);	
	BYTES_EQUAL(8, txMsg.Data[4]);		
	BYTES_EQUAL(9, txMsg.Data[5]);	
	BYTES_EQUAL(10, txMsg.Data[6]);	
	BYTES_EQUAL(11, txMsg.Data[7]);		
}

TEST(CANOSDriver, testTransmitter3)
{
	uint8_t ret;
	
	ret = can_message_construct(&message, 0x0a, 0x08, 15, data);		
	ret = can_message_transmit(&message, 0);
	
	BYTES_EQUAL(1, ret);
	BYTES_EQUAL(8, txMsg.DLC);
	BYTES_EQUAL(CAN_ID_STD, txMsg.IDE);
	LONGS_EQUAL(0x0508, txMsg.StdId);
	BYTES_EQUAL(0x88, txMsg.Data[0]);
	BYTES_EQUAL(0x00, txMsg.Data[1]);	
	BYTES_EQUAL(0, txMsg.Data[2]);	
	BYTES_EQUAL(1, txMsg.Data[3]);	
	BYTES_EQUAL(2, txMsg.Data[4]);		
	BYTES_EQUAL(3, txMsg.Data[5]);	
	BYTES_EQUAL(4, txMsg.Data[6]);	
	BYTES_EQUAL(5, txMsg.Data[7]);		
}

TEST(CANOSDriver, testTransmitter0)
{
	uint8_t ret;
	
	ret = can_message_construct(&message, 0x0a, 0x08, 5, data);		
	ret = can_message_transmit(&message, 0);
	
	BYTES_EQUAL(1, ret);
	BYTES_EQUAL(6, txMsg.DLC);
	BYTES_EQUAL(CAN_ID_STD, txMsg.IDE);
	LONGS_EQUAL(0x0508, txMsg.StdId);
	BYTES_EQUAL(0x08, txMsg.Data[0]);
	BYTES_EQUAL(0, txMsg.Data[1]);	
	BYTES_EQUAL(1, txMsg.Data[2]);	
	BYTES_EQUAL(2, txMsg.Data[3]);		
	BYTES_EQUAL(3, txMsg.Data[4]);	
	BYTES_EQUAL(4, txMsg.Data[5]);	
}

TEST(CANOSDriver, testReceiver0)
{
	uint8_t ret;

	rxMsg.StdId = 0x0508;
	rxMsg.IDE = CAN_ID_STD;
	rxMsg.DLC = 6;
	rxMsg.Data[0] = 0x08;
	/* data as below */
	rxMsg.Data[1] = 0;
	rxMsg.Data[2] = 1;	
	rxMsg.Data[3] = 2;	
	rxMsg.Data[4] = 3;	
	rxMsg.Data[5] = 4;	
	
	ret = can_message_receive(&message);
	
	BYTES_EQUAL(CAN_RCV_SINGLE_PACKET, ret);
	LONGS_EQUAL(0x0508, message.identifier.bitField);
	BYTES_EQUAL(0x08, message.header.bitField);
	BYTES_EQUAL(5, message.length);
	BYTES_EQUAL(5, message.last_packet_length);
	BYTES_EQUAL(0, message.data[0]);	
	BYTES_EQUAL(1, message.data[1]);	
	BYTES_EQUAL(2, message.data[2]);		
	BYTES_EQUAL(3, message.data[3]);	
	BYTES_EQUAL(4, message.data[4]);	
	BYTES_EQUAL(0, message.data[5]);		
}

TEST(CANOSDriver, testReceiver1)
{
	uint8_t ret;
//	uint16_t data_length, message_id;

	rxMsg.StdId = 0x0509;
	rxMsg.IDE = CAN_ID_STD;
	rxMsg.DLC = 8;
	rxMsg.Data[0] = 0x89;
	/* data as below */
	rxMsg.Data[1] = 0x00;
	rxMsg.Data[2] = 0;	
	rxMsg.Data[3] = 1;	
	rxMsg.Data[4] = 0;	
	rxMsg.Data[5] = 13;	
	rxMsg.Data[6] = 4;	
	rxMsg.Data[7] = 5;	
	
	ret = can_message_receive(&message);
	
	BYTES_EQUAL(CAN_RCV_FIRST_PACKET, ret);
	LONGS_EQUAL(0x0509, message.identifier.bitField);
	BYTES_EQUAL(0x89, message.header.bitField);
	BYTES_EQUAL(6, message.length);
	BYTES_EQUAL(6, message.last_packet_length);
	BYTES_EQUAL(0, message.data[0]);	
	BYTES_EQUAL(1, message.data[1]);	
	BYTES_EQUAL(0, message.data[2]);		
	BYTES_EQUAL(13, message.data[3]);	
	BYTES_EQUAL(4, message.data[4]);	
	BYTES_EQUAL(5, message.data[5]);		
	BYTES_EQUAL(0, message.data[6]);

	rxMsg.StdId = 0x0509;
	rxMsg.IDE = CAN_ID_STD;
	rxMsg.DLC = 8;
	rxMsg.Data[0] = 0x89;
	/* data as below */
	rxMsg.Data[1] = 0x41;
	rxMsg.Data[2] = 6;	
	rxMsg.Data[3] = 7;	
	rxMsg.Data[4] = 8;	
	rxMsg.Data[5] = 9;	
	rxMsg.Data[6] = 10;	
	rxMsg.Data[7] = 11;	
	
	ret = can_message_receive(&message);
	BYTES_EQUAL(CAN_RCV_GENERAL_PACKET, ret);
	LONGS_EQUAL(0x0509, message.identifier.bitField);
	BYTES_EQUAL(0x89, message.header.bitField);
	BYTES_EQUAL(12, message.length);
	BYTES_EQUAL(6, message.last_packet_length);
	BYTES_EQUAL(0, message.data[0]);	
	BYTES_EQUAL(1, message.data[1]);	
	BYTES_EQUAL(0, message.data[2]);		
	BYTES_EQUAL(13, message.data[3]);	
	BYTES_EQUAL(4, message.data[4]);	
	BYTES_EQUAL(5, message.data[5]);		
	BYTES_EQUAL(6, message.data[6]);
	BYTES_EQUAL(7, message.data[7]);
	BYTES_EQUAL(8, message.data[8]);
	BYTES_EQUAL(9, message.data[9]);
	BYTES_EQUAL(10, message.data[10]);	
	BYTES_EQUAL(11, message.data[11]);		

	rxMsg.StdId = 0x0509;
	rxMsg.IDE = CAN_ID_STD;
	rxMsg.DLC = 5;
	rxMsg.Data[0] = 0x89;
	/* data as below */
	rxMsg.Data[1] = 0x82;
	rxMsg.Data[2] = 12;	
	rxMsg.Data[3] = 13;	
	rxMsg.Data[4] = 14;	
	rxMsg.Data[5] = 0;	
	rxMsg.Data[6] = 0;	
	rxMsg.Data[7] = 0;	
	
	ret = can_message_receive(&message);
	BYTES_EQUAL(CAN_RCV_LAST_PACKET, ret);
	LONGS_EQUAL(0x0509, message.identifier.bitField);
	BYTES_EQUAL(0x89, message.header.bitField);
	BYTES_EQUAL(15, message.length);
	BYTES_EQUAL(3, message.last_packet_length);
	BYTES_EQUAL(0, message.data[0]);	
	BYTES_EQUAL(1, message.data[1]);	
	BYTES_EQUAL(0, message.data[2]);		
	BYTES_EQUAL(13, message.data[3]);	
	BYTES_EQUAL(4, message.data[4]);	
	BYTES_EQUAL(5, message.data[5]);		
	BYTES_EQUAL(6, message.data[6]);
	BYTES_EQUAL(7, message.data[7]);
	BYTES_EQUAL(8, message.data[8]);
	BYTES_EQUAL(9, message.data[9]);
	BYTES_EQUAL(10, message.data[10]);	
	BYTES_EQUAL(11, message.data[11]);		
	BYTES_EQUAL(12, message.data[12]);	
	BYTES_EQUAL(13, message.data[13]);		
	BYTES_EQUAL(14, message.data[14]);			
	BYTES_EQUAL(0, message.data[15]);	
	BYTES_EQUAL(0, message.data[16]);		
	BYTES_EQUAL(0, message.data[17]);			

//	data_length = (message.data[2] << 8) | message.data[3];
//	message_id = (message.data[4] << 8) | message.data[5];
//	can_command_receive(&command, message.identifier.bit.nodeID, message.identifier.bit.messageType, message.data[0], message.data[1], data_length, message_id, &(message.data[6]));
	ret = can_message_to_command(message, &command);

	BYTES_EQUAL(1, ret);
	BYTES_EQUAL(0x09, command.identifier);
	BYTES_EQUAL(0x0a, command.message_type);
	BYTES_EQUAL(0, command.command);
	BYTES_EQUAL(1, command.command_type);
	BYTES_EQUAL(6, command.data[0]);
}

TEST(CANOSDriver, testCommand2Message0)
{
	uint8_t ret;

	command.identifier = 0x09;
	command.message_type = 0x0a;
	command.command = 0x00;
	command.command_type = 0x01;
	command.data_length = 7;
	command.message_id = 1;
	command.data = (uint8_t*)malloc(3);
	command.data[0] = 4;
	command.data[1] = 5;
	command.data[2] = 6;
	
	ret = can_command_to_message(command, &message);
	
	BYTES_EQUAL(1, ret);
	BYTES_EQUAL(0x0509, message.identifier.bitField);
	BYTES_EQUAL(0x89, message.header.bitField);
	BYTES_EQUAL(9, message.length);
	BYTES_EQUAL(2, message.total_packet);
	BYTES_EQUAL(3, message.last_packet_length);
	BYTES_EQUAL(0x00, message.data[0]);
	BYTES_EQUAL(0x01, message.data[1]);
	BYTES_EQUAL(0x00, message.data[2]);
	BYTES_EQUAL(0x07, message.data[3]);
	BYTES_EQUAL(0x00, message.data[4]);
	BYTES_EQUAL(0x01, message.data[5]);
	BYTES_EQUAL(4, message.data[6]);	
	BYTES_EQUAL(5, message.data[7]);	
	BYTES_EQUAL(6, message.data[8]);		
}

TEST(CANOSDriver, testCommand2Message1)
{
	uint8_t ret;

	command.identifier = 0x09;
	command.message_type = 0x0a;
	command.command = 0x00;
	command.command_type = 0x01;
	command.data_length = 4;
	command.message_id = 1;
	command.data = (uint8_t*)malloc(3);
	command.data[0] = 0x55;
	
	ret = can_command_to_message(command, &message);
	
	BYTES_EQUAL(1, ret);
	BYTES_EQUAL(0x0509, message.identifier.bitField);
	BYTES_EQUAL(0x09, message.header.bitField);
	BYTES_EQUAL(6, message.length);
	BYTES_EQUAL(1, message.total_packet);
	BYTES_EQUAL(6, message.last_packet_length);
	BYTES_EQUAL(0x00, message.data[0]);
	BYTES_EQUAL(0x01, message.data[1]);
	BYTES_EQUAL(0x00, message.data[2]);
	BYTES_EQUAL(0x04, message.data[3]);
	BYTES_EQUAL(0x00, message.data[4]);
	BYTES_EQUAL(0x01, message.data[5]);
	BYTES_EQUAL(0x55, message.data[6]);	
	BYTES_EQUAL(0, message.data[7]);	
	BYTES_EQUAL(0, message.data[8]);		
}