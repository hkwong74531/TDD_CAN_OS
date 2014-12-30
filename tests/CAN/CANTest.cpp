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

extern canMessage_t can_command_message;
canCommand_t command;
uint8_t data[256];
extern CanTxMsg txMsg;
extern CanRxMsg rxMsg;

TEST_GROUP(CANCommandState)
{
    void setup()
    {
		uint16_t i;
		
		for(i = 0; i < 256; i++)
		{
			data[i] = i;
		}
		memset(&rxMsg, 0, sizeof(CanRxMsg));
		memset(&command, 0, sizeof(canCommand_t));
		
		can_command_init();
    }

    void teardown()
    {
		free(command.data);
    }
};

TEST(CANCommandState, testTransmitter0)
{
	uint8_t ret;

	can_command_state_process();
	BYTES_EQUAL(CAN_COMMAND_IDLE_STATE, can_command_getState());
	BYTES_EQUAL(0, can_command_getPacketCnt());
	
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
	
	ret = can_command_transmit(command);
	
	can_command_state_process();

	BYTES_EQUAL(1, ret);
	BYTES_EQUAL(0, can_command_isAllSent());
	BYTES_EQUAL(1, can_command_getPacketCnt());
	BYTES_EQUAL(CAN_COMMAND_SENDING_STATE, can_command_getState());

	can_command_state_process();

	BYTES_EQUAL(1, can_command_isAllSent());
	BYTES_EQUAL(0, can_command_getPacketCnt());
	BYTES_EQUAL(CAN_COMMAND_IDLE_STATE, can_command_getState());
}

TEST(CANCommandState, testReceiver0)
{
	uint8_t ret;

	can_command_state_process();
	BYTES_EQUAL(0, can_command_isAllReceived());	
	
	rxMsg.StdId = 0x0508;
	rxMsg.IDE = CAN_ID_STD;
	rxMsg.DLC = 7;
	rxMsg.Data[0] = 0x08;
	/* data as below */
	rxMsg.Data[1] = 0;
	rxMsg.Data[2] = 1;	
	rxMsg.Data[3] = 0;	
	rxMsg.Data[4] = 4;	
	rxMsg.Data[5] = 0;	
	rxMsg.Data[6] = 1;	
	
	can_command_setEvent(CAN_COMMAND_PACKET_RECEIVED_EVENT);
	can_command_state_process();
	
	BYTES_EQUAL(1, can_command_isAllReceived());
#if 0	
	LONGS_EQUAL(0x0508, can_command_message.identifier.bitField);
	BYTES_EQUAL(0x08, can_command_message.header.bitField);
	BYTES_EQUAL(5, can_command_message.length);
	BYTES_EQUAL(5, can_command_message.last_packet_length);
	BYTES_EQUAL(0, can_command_message.data[0]);	
	BYTES_EQUAL(1, can_command_message.data[1]);	
	BYTES_EQUAL(2, can_command_message.data[2]);		
	BYTES_EQUAL(3, can_command_message.data[3]);	
	BYTES_EQUAL(4, can_command_message.data[4]);	
	BYTES_EQUAL(0, can_command_message.data[5]);	
#endif	
	ret = can_command_acquired(&command);

	BYTES_EQUAL(1, ret);
	BYTES_EQUAL(0x08, command.identifier);
	BYTES_EQUAL(0x0a, command.message_type);
	BYTES_EQUAL(0, command.command);
	BYTES_EQUAL(1, command.command_type);
}

TEST(CANCommandState, testReceiver1)
{
	uint8_t ret;

	can_command_state_process();
	BYTES_EQUAL(0, can_command_isAllReceived());	
	
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
	
	can_command_setEvent(CAN_COMMAND_PACKET_RECEIVED_EVENT);
	can_command_state_process();
	
	BYTES_EQUAL(0, can_command_isAllReceived());
	BYTES_EQUAL(CAN_COMMAND_RECEIVING_STATE, can_command_getState());

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
	
	can_command_setEvent(CAN_COMMAND_PACKET_RECEIVED_EVENT);	
	can_command_state_process();
	
	BYTES_EQUAL(0, can_command_isAllReceived());
	BYTES_EQUAL(CAN_COMMAND_RECEIVING_STATE, can_command_getState());

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

	can_command_setEvent(CAN_COMMAND_PACKET_RECEIVED_EVENT);		
	can_command_state_process();
	
	BYTES_EQUAL(1, can_command_isAllReceived());
	BYTES_EQUAL(CAN_COMMAND_IDLE_STATE, can_command_getState());
#if 0
	LONGS_EQUAL(0x0509, can_command_message.identifier.bitField);
	BYTES_EQUAL(0x89, can_command_message.header.bitField);
	BYTES_EQUAL(15, can_command_message.length);
	BYTES_EQUAL(3, can_command_message.last_packet_length);
	BYTES_EQUAL(0, can_command_message.data[0]);	
	BYTES_EQUAL(1, can_command_message.data[1]);	
	BYTES_EQUAL(0, can_command_message.data[2]);		
	BYTES_EQUAL(13, can_command_message.data[3]);	
	BYTES_EQUAL(4, can_command_message.data[4]);	
	BYTES_EQUAL(5, can_command_message.data[5]);		
	BYTES_EQUAL(6, can_command_message.data[6]);
	BYTES_EQUAL(7, can_command_message.data[7]);
	BYTES_EQUAL(8, can_command_message.data[8]);
	BYTES_EQUAL(9, can_command_message.data[9]);
	BYTES_EQUAL(10, can_command_message.data[10]);	
	BYTES_EQUAL(11, can_command_message.data[11]);		
	BYTES_EQUAL(12, can_command_message.data[12]);	
	BYTES_EQUAL(13, can_command_message.data[13]);		
	BYTES_EQUAL(14, can_command_message.data[14]);			
	BYTES_EQUAL(0, can_command_message.data[15]);	
	BYTES_EQUAL(0, can_command_message.data[16]);		
	BYTES_EQUAL(0, can_command_message.data[17]);			
#endif	
	ret = can_command_acquired(&command);

	BYTES_EQUAL(1, ret);
	BYTES_EQUAL(0x09, command.identifier);
	BYTES_EQUAL(0x0a, command.message_type);
	BYTES_EQUAL(0, command.command);
	BYTES_EQUAL(1, command.command_type);
	BYTES_EQUAL(6, command.data[0]);
	BYTES_EQUAL(7, command.data[1]);
	BYTES_EQUAL(8, command.data[2]);
	BYTES_EQUAL(9, command.data[3]);
	BYTES_EQUAL(10, command.data[4]);	
	BYTES_EQUAL(11, command.data[5]);		
	BYTES_EQUAL(12, command.data[6]);	
	BYTES_EQUAL(13, command.data[7]);		
	BYTES_EQUAL(14, command.data[8]);		
}

#if 0
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

#endif