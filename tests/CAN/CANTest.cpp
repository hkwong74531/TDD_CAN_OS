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
}

#include "CppUTest/TestHarness.h"

canMessage_t message;
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
    }

    void teardown()
    {
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
