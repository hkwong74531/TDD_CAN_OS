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

TEST_GROUP(CANOSDriver)
{
    void setup()
    {
		data[0] = 0xAA;
		data[1] = 0xBB;
		data[2] = 0xCC;
    }

    void teardown()
    {
    }
};

TEST(CANOSDriver, testConstructor)
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
	BYTES_EQUAL(0xAA, message.data[0]);
	BYTES_EQUAL(0xBB, message.data[1]);
	BYTES_EQUAL(0xCC, message.data[2]);	
}



