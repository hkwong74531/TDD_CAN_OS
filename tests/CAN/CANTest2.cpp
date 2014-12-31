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
#include "can_protocol.h"
}

#include "CppUTest/TestHarness.h"
extern canCommand_t command;
extern canMessage_t can_command_message;
extern CanTxMsg txMsg;
extern CanRxMsg rxMsg;

TEST_GROUP(CANProtocolLayer)
{
	void can_protocol_receive_with_resend(void)
	{
		canProtocolState_t state;
		canProtocolEvent_t event = CAN_PROTOCOL_DUMMY_EVENT;	
	
		rxMsg.StdId = 0x0508;
		rxMsg.IDE = CAN_ID_STD;
		rxMsg.DLC = 7;
		rxMsg.Data[0] = 0x08;
		/* data as below */
		rxMsg.Data[1] = 0;	// command
		rxMsg.Data[2] = 1;	// command_type
		rxMsg.Data[3] = 0;	
		rxMsg.Data[4] = 4;	// data_length
		rxMsg.Data[5] = (uint8_t)((can_protocol_getCtrlID() & 0xFF00) >> 8);	
		rxMsg.Data[6] = (uint8_t)(can_protocol_getCtrlID() & 0x00FF);	
		can_command_setEvent(CAN_COMMAND_PACKET_RECEIVED_EVENT);
	
		state = can_protocol_state_process(event);	
		BYTES_EQUAL(CAN_PROTOCOL_RCV_COMMAND_STATE, state);	
	
		can_protocol_setReply(0x03, 4, NULL);
		event = CAN_PROTOCOL_REPLY_EVENT;
		state = can_protocol_state_process(event);
		state = can_protocol_state_process(event);	
	
		BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);	

		rxMsg.StdId = 0x0508;
		rxMsg.IDE = CAN_ID_STD;
		rxMsg.DLC = 7;
		rxMsg.Data[0] = 0x08;
		/* data as below */
		rxMsg.Data[1] = 0;	// command
		rxMsg.Data[2] = 1;	// command_type
		rxMsg.Data[3] = 0;	
		rxMsg.Data[4] = 4;	// data_length
		rxMsg.Data[5] = (uint8_t)(((can_protocol_getCtrlID()-1) & 0xFF00) >> 8);	
		rxMsg.Data[6] = (uint8_t)((can_protocol_getCtrlID()-1) & 0x00FF);	
		can_command_setEvent(CAN_COMMAND_PACKET_RECEIVED_EVENT);	
		state = can_protocol_state_process(event);	
		BYTES_EQUAL(CAN_PROTOCOL_REPLY_LOST_STATE, state);	

		event = CAN_PROTOCOL_REPLY_EVENT;
		state = can_protocol_state_process(event);
		state = can_protocol_state_process(event);		
		BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);
	}

	void can_protocol_receive(void)
	{
		canProtocolState_t state;
		canProtocolEvent_t event = CAN_PROTOCOL_DUMMY_EVENT;	
		uint8_t data[4];
		
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
		rxMsg.Data[6] = (uint8_t)((can_protocol_getCtrlID() & 0xFF00) >> 8);	
		rxMsg.Data[7] = (uint8_t)(can_protocol_getCtrlID() & 0x00FF);	
		can_command_setEvent(CAN_COMMAND_PACKET_RECEIVED_EVENT);
	
		state = can_protocol_state_process(event);	
		BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);	
	
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
	
		state = can_protocol_state_process(event);	
		BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);	

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
	
		state = can_protocol_state_process(event);	
		BYTES_EQUAL(CAN_PROTOCOL_RCV_COMMAND_STATE, state);	
	
		data[0] = 1;
		data[1] = 2;
		data[2] = 3;
		data[3] = 5;
	
		can_protocol_setReply(0x03, 8, data);
		event = CAN_PROTOCOL_REPLY_EVENT;
		state = can_protocol_state_process(event);

		event = CAN_PROTOCOL_DUMMY_EVENT;
		state = can_protocol_state_process(event);
		BYTES_EQUAL(CAN_PROTOCOL_REPLYING_COMMAND_STATE, state);

		LONGS_EQUAL(0x0509, txMsg.StdId);
		BYTES_EQUAL(CAN_ID_STD, txMsg.IDE);
		BYTES_EQUAL(8, txMsg.DLC);
		BYTES_EQUAL(0x89, txMsg.Data[0]);
		BYTES_EQUAL(0x00, txMsg.Data[1]);	
		BYTES_EQUAL(0x00, txMsg.Data[2]);	// command	
		BYTES_EQUAL(0x03, txMsg.Data[3]);	// command_type
		BYTES_EQUAL(0x00, txMsg.Data[4]);	// 
		BYTES_EQUAL(0x08, txMsg.Data[5]);	// data_length
		BYTES_EQUAL(0x00, txMsg.Data[6]);	// 
		BYTES_EQUAL((uint8_t)can_protocol_getCtrlID(), txMsg.Data[7]);	// message_id
	
		state = can_protocol_state_process(event);	
		BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);	

		LONGS_EQUAL(0x0509, txMsg.StdId);
		BYTES_EQUAL(CAN_ID_STD, txMsg.IDE);
		BYTES_EQUAL(6, txMsg.DLC);
		BYTES_EQUAL(0x89, txMsg.Data[0]);
		BYTES_EQUAL(0x81, txMsg.Data[1]);	
		BYTES_EQUAL(0x01, txMsg.Data[2]);	// command	
		BYTES_EQUAL(0x02, txMsg.Data[3]);	// command_type
		BYTES_EQUAL(0x03, txMsg.Data[4]);	// 
		BYTES_EQUAL(0x05, txMsg.Data[5]);	// data_length
	}
	
	void can_protocol_send(void)
	{
		canProtocolState_t state;
		canProtocolEvent_t event = CAN_PROTOCOL_DUMMY_EVENT;	
	
		state = can_protocol_state_process(event);
		BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);	
	
		can_protocol_setSend(&command);
		event = CAN_PROTOCOL_SEND_EVENT;
		state = can_protocol_state_process(event);	

		BYTES_EQUAL(CAN_PROTOCOL_SENDING_COMMAND_STATE, state);	
	
		event = CAN_PROTOCOL_DUMMY_EVENT;
		state = can_protocol_state_process(event);	
		state = can_protocol_state_process(event);		

		BYTES_EQUAL(CAN_PROTOCOL_SENT_COMMAND_STATE, state);	

		rxMsg.StdId = 0x0509;
		rxMsg.IDE = CAN_ID_STD;
		rxMsg.DLC = 8;
		rxMsg.Data[0] = 0x89;
		/* data as below */
		rxMsg.Data[1] = 0x00;
		rxMsg.Data[2] = 0;	
		rxMsg.Data[3] = 3;	
		rxMsg.Data[4] = 0;	
		rxMsg.Data[5] = 7;	
		rxMsg.Data[6] = (uint8_t)((can_protocol_getRdrID() & 0xFF00) >> 8);	
		rxMsg.Data[7] = (uint8_t)(can_protocol_getRdrID() & 0x00FF);	
		can_command_setEvent(CAN_COMMAND_PACKET_RECEIVED_EVENT);

		state = can_protocol_state_process(event);	
	
		rxMsg.StdId = 0x0509;
		rxMsg.IDE = CAN_ID_STD;
		rxMsg.DLC = 5;
		rxMsg.Data[0] = 0x89;
		/* data as below */
		rxMsg.Data[1] = 0x81;
		rxMsg.Data[2] = 0x0A;	
		rxMsg.Data[3] = 0x0B;	
		rxMsg.Data[4] = 0x0C;	
		can_command_setEvent(CAN_COMMAND_PACKET_RECEIVED_EVENT);
	
		state = can_protocol_state_process(event);	
		BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);
	}

	void can_protocol_send_with_timeout(void)
	{
		canProtocolState_t state;
		canProtocolEvent_t event = CAN_PROTOCOL_DUMMY_EVENT;	
	
		state = can_protocol_state_process(event);

		BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);	

		can_protocol_setSend(&command);
		event = CAN_PROTOCOL_SEND_EVENT;
		state = can_protocol_state_process(event);	

		BYTES_EQUAL(CAN_PROTOCOL_SENDING_COMMAND_STATE, state);	
	
		event = CAN_PROTOCOL_DUMMY_EVENT;
		state = can_protocol_state_process(event);	
		state = can_protocol_state_process(event);		

		BYTES_EQUAL(CAN_PROTOCOL_SENT_COMMAND_STATE, state);	

		event = CAN_PROTOCOL_TIMEOUT_EVENT;
		state = can_protocol_state_process(event);	

		BYTES_EQUAL(CAN_PROTOCOL_SENDING_COMMAND_STATE, state);	
	
		event = CAN_PROTOCOL_DUMMY_EVENT;	
		state = can_protocol_state_process(event);		
		state = can_protocol_state_process(event);	

		BYTES_EQUAL(CAN_PROTOCOL_SENT_COMMAND_STATE, state);	

		rxMsg.StdId = 0x0509;
		rxMsg.IDE = CAN_ID_STD;
		rxMsg.DLC = 7;
		rxMsg.Data[0] = 0x09;
		/* data as below */
		rxMsg.Data[1] = 0;
		rxMsg.Data[2] = 3;	
		rxMsg.Data[3] = 0;	
		rxMsg.Data[4] = 4;	
		rxMsg.Data[5] = (uint8_t)((can_protocol_getRdrID() & 0xFF00) >> 8);	
		rxMsg.Data[6] = (uint8_t)(can_protocol_getRdrID() & 0x00FF);;	
		can_command_setEvent(CAN_COMMAND_PACKET_RECEIVED_EVENT);

		state = can_protocol_state_process(event);	
		
		BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);	
	}

    void setup()
    {
		memset(&rxMsg, 0, sizeof(CanRxMsg));

		can_protocol_init();
    }

    void teardown()
    {
		can_protocol_deinit();
    }
};

TEST(CANProtocolLayer, testTransmitter0)
{
//	uint8_t ret;
	canCommand_t command;
	canProtocolState_t state;
	canProtocolEvent_t event = CAN_PROTOCOL_DUMMY_EVENT;	

	state = can_protocol_state_process(event);
	BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);
	
	command.identifier = 0x09;
	command.message_type = 0x0a;
	command.command = 0x00;
	command.command_type = 0x01;
	command.data_length = 4;
	command.message_id = can_protocol_getRdrID();
	command.data = (uint8_t*)malloc(0);

	can_protocol_setSend(&command);
	
	event = CAN_PROTOCOL_SEND_EVENT;
	
	state = can_protocol_state_process(event);	
	BYTES_EQUAL(CAN_PROTOCOL_SENDING_COMMAND_STATE, state);	
	
	event = CAN_PROTOCOL_DUMMY_EVENT;
	state = can_protocol_state_process(event);	

	BYTES_EQUAL(CAN_PROTOCOL_SENT_COMMAND_STATE, state);	
}

TEST(CANProtocolLayer, testTransmitter1)
{
//	uint8_t ret;
	canCommand_t command;
	canProtocolState_t state;
	canProtocolEvent_t event = CAN_PROTOCOL_DUMMY_EVENT;	

	state = can_protocol_state_process(event);
	BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);	


	command.identifier = 0x09;
	command.message_type = 0x0a;
	command.command = 0x00;
	command.command_type = 0x01;
	command.data_length = 7;
	command.message_id = can_protocol_getRdrID();
	command.data = (uint8_t*)malloc(3);
	command.data[0] = 4;
	command.data[1] = 5;
	command.data[2] = 6;

	can_protocol_setSend(&command);
	
	event = CAN_PROTOCOL_SEND_EVENT;
	
	state = can_protocol_state_process(event);	
	BYTES_EQUAL(CAN_PROTOCOL_SENDING_COMMAND_STATE, state);	
	
	event = CAN_PROTOCOL_DUMMY_EVENT;
	state = can_protocol_state_process(event);	
	state = can_protocol_state_process(event);		
	BYTES_EQUAL(CAN_PROTOCOL_SENT_COMMAND_STATE, state);	
	
	rxMsg.StdId = 0x0509;
	rxMsg.IDE = CAN_ID_STD;
	rxMsg.DLC = 7;
	rxMsg.Data[0] = 0x09;
	/* data as below */
	rxMsg.Data[1] = 0;
	rxMsg.Data[2] = 1;	
	rxMsg.Data[3] = 0;	
	rxMsg.Data[4] = 4;	
	rxMsg.Data[5] = 0;	
	rxMsg.Data[6] = 0;	
	can_command_setEvent(CAN_COMMAND_PACKET_RECEIVED_EVENT);

	state = can_protocol_state_process(event);	
	can_protocol_getReceived(&command);
	
	LONGS_EQUAL(1, can_protocol_getRdrID());

}

TEST(CANProtocolLayer, testTransmitter2)
{
//	uint8_t ret;
	canCommand_t command;
	canProtocolState_t state;
	canProtocolEvent_t event = CAN_PROTOCOL_DUMMY_EVENT;	

	state = can_protocol_state_process(event);
	BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);	

	command.identifier = 0x09;
	command.message_type = 0x0a;
	command.command = 0x00;
	command.command_type = 0x01;
	command.data_length = 7;
	command.message_id = can_protocol_getRdrID();
	command.data = (uint8_t*)malloc(3);
	command.data[0] = 4;
	command.data[1] = 5;
	command.data[2] = 6;

	can_protocol_setSend(&command);
	
	event = CAN_PROTOCOL_SEND_EVENT;
	
	state = can_protocol_state_process(event);	
	BYTES_EQUAL(CAN_PROTOCOL_SENDING_COMMAND_STATE, state);	
	
	event = CAN_PROTOCOL_DUMMY_EVENT;

	state = can_protocol_state_process(event);	
	state = can_protocol_state_process(event);		
	BYTES_EQUAL(CAN_PROTOCOL_SENT_COMMAND_STATE, state);	

	event = CAN_PROTOCOL_TIMEOUT_EVENT;
	state = can_protocol_state_process(event);	
	BYTES_EQUAL(CAN_PROTOCOL_SENDING_COMMAND_STATE, state);	
	
	event = CAN_PROTOCOL_DUMMY_EVENT;	
	state = can_protocol_state_process(event);		

	state = can_protocol_state_process(event);	
	BYTES_EQUAL(CAN_PROTOCOL_SENT_COMMAND_STATE, state);	
	
	rxMsg.StdId = 0x0509;
	rxMsg.IDE = CAN_ID_STD;
	rxMsg.DLC = 7;
	rxMsg.Data[0] = 0x09;
	/* data as below */
	rxMsg.Data[1] = 0;
	rxMsg.Data[2] = 3;	
	rxMsg.Data[3] = 0;	
	rxMsg.Data[4] = 4;	
	rxMsg.Data[5] = 0;	
	rxMsg.Data[6] = 0;	
	can_command_setEvent(CAN_COMMAND_PACKET_RECEIVED_EVENT);
	state = can_protocol_state_process(event);	
	BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);

	LONGS_EQUAL(1, can_protocol_getRdrID());
	can_protocol_getReceived(&command);	
	BYTES_EQUAL(0x09, command.identifier);
	BYTES_EQUAL(0x03, command.command_type);
	
	/****************************/
	state = can_protocol_state_process(event);
	BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);	

	command.identifier = 0x09;
	command.message_type = 0x0a;
	command.command = 0x00;
	command.command_type = 0x01;
	command.data_length = 7;
	command.message_id = can_protocol_getRdrID();
	command.data = (uint8_t*)malloc(3);
	command.data[0] = 4;
	command.data[1] = 5;
	command.data[2] = 6;

	can_protocol_setSend(&command);
	
	event = CAN_PROTOCOL_SEND_EVENT;
	
	state = can_protocol_state_process(event);	
	BYTES_EQUAL(CAN_PROTOCOL_SENDING_COMMAND_STATE, state);	
	
}

TEST(CANProtocolLayer, testTranceiver)
{
	/****1st send****/		
	command.identifier = 0x09;
	command.message_type = 0x0a;
	command.command = 0x00;
	command.command_type = 0x01;
	command.data_length = 7;
	command.message_id = can_protocol_getRdrID();
	command.data = (uint8_t*)malloc(3);
	command.data[0] = 4;
	command.data[1] = 5;
	command.data[2] = 6;

	can_protocol_send_with_timeout();

	LONGS_EQUAL(1, can_protocol_getRdrID());
	can_protocol_getReceived(&command);	
	BYTES_EQUAL(0x09, command.identifier);
	BYTES_EQUAL(0x03, command.command_type);
	
	/****2nd send****/
	command.identifier = 0x09;
	command.message_type = 0x0a;
	command.command = 0x00;
	command.command_type = 0x01;
	command.data_length = 7;
	command.message_id = can_protocol_getRdrID();
	command.data = (uint8_t*)malloc(3);
	command.data[0] = 4;
	command.data[1] = 5;
	command.data[2] = 6;

	can_protocol_send();
	
	LONGS_EQUAL(2, can_protocol_getRdrID());
	can_protocol_getReceived(&command);	
	BYTES_EQUAL(0x09, command.identifier);
	BYTES_EQUAL(0x03, command.command_type);
	BYTES_EQUAL(0x0a, command.message_type);
	BYTES_EQUAL(0, command.command);
	BYTES_EQUAL(3, command.command_type);
	BYTES_EQUAL(10, command.data[0]);
	BYTES_EQUAL(11, command.data[1]);
	BYTES_EQUAL(12, command.data[2]);	

	/****1st receive****/	
	can_protocol_receive_with_resend();
	
	can_protocol_getReceived(&command);

	BYTES_EQUAL(0x08, command.identifier);
	BYTES_EQUAL(0x0a, command.message_type);
	BYTES_EQUAL(0, command.command);
	BYTES_EQUAL(1, command.command_type);

	/****2nd receive****/	
	can_protocol_receive();

	can_protocol_getReceived(&command);

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

	/****3rd send****/		
	command.identifier = 0x09;
	command.message_type = 0x0a;
	command.command = 0x00;
	command.command_type = 0x01;
	command.data_length = 7;
	command.message_id = can_protocol_getRdrID();
	command.data = (uint8_t*)malloc(3);
	command.data[0] = 4;
	command.data[1] = 5;
	command.data[2] = 6;

	can_protocol_send_with_timeout();
	
	LONGS_EQUAL(3, can_protocol_getRdrID());
	can_protocol_getReceived(&command);	
	BYTES_EQUAL(0x09, command.identifier);
	BYTES_EQUAL(0x03, command.command_type);
	
	/****3rd receive****/
	can_protocol_receive_with_resend();

	/****4th send****/		
	command.identifier = 0x09;
	command.message_type = 0x0a;
	command.command = 0x00;
	command.command_type = 0x01;
	command.data_length = 10;
	command.message_id = can_protocol_getRdrID();
	command.data = (uint8_t*)malloc(6);
	command.data[0] = 4;
	command.data[1] = 5;
	command.data[2] = 6;
	command.data[3] = 7;
	command.data[4] = 8;
	command.data[5] = 9;

	can_protocol_send_with_timeout();

	LONGS_EQUAL(4, can_protocol_getRdrID());
	can_protocol_getReceived(&command);	
	BYTES_EQUAL(0x09, command.identifier);
	BYTES_EQUAL(0x03, command.command_type);

	/****4th receive****/	
	can_protocol_receive();
}



TEST(CANProtocolLayer, testReceiver0)
{
//	uint8_t ret;
	canCommand_t commandIn;
	canProtocolState_t state;
	canProtocolEvent_t event = CAN_PROTOCOL_DUMMY_EVENT;

	state = can_protocol_state_process(event);
	BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);

	rxMsg.StdId = 0x0508;
	rxMsg.IDE = CAN_ID_STD;
	rxMsg.DLC = 7;
	rxMsg.Data[0] = 0x08;
	/* data as below */
	rxMsg.Data[1] = 0;	// command
	rxMsg.Data[2] = 1;	// command_type
	rxMsg.Data[3] = 0;	
	rxMsg.Data[4] = 4;	// data_length
	rxMsg.Data[5] = 0;	
	rxMsg.Data[6] = 0;	// message_id
	can_command_setEvent(CAN_COMMAND_PACKET_RECEIVED_EVENT);
	
	state = can_protocol_state_process(event);	
	BYTES_EQUAL(CAN_PROTOCOL_RCV_COMMAND_STATE, state);	
	
	can_protocol_getReceived(&commandIn);
	
	can_protocol_setReply(0x03, 4, NULL);
	event = CAN_PROTOCOL_REPLY_EVENT;
	state = can_protocol_state_process(event);
	state = can_protocol_state_process(event);	
	
	BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);	

	rxMsg.StdId = 0x0508;
	rxMsg.IDE = CAN_ID_STD;
	rxMsg.DLC = 7;
	rxMsg.Data[0] = 0x08;
	/* data as below */
	rxMsg.Data[1] = 0;	// command
	rxMsg.Data[2] = 1;	// command_type
	rxMsg.Data[3] = 0;	
	rxMsg.Data[4] = 4;	// data_length
	rxMsg.Data[5] = 0;	
	rxMsg.Data[6] = 0;	// message_id	
	can_command_setEvent(CAN_COMMAND_PACKET_RECEIVED_EVENT);	
	state = can_protocol_state_process(event);	
	BYTES_EQUAL(CAN_PROTOCOL_REPLY_LOST_STATE, state);	

	event = CAN_PROTOCOL_REPLY_EVENT;
	state = can_protocol_state_process(event);
	state = can_protocol_state_process(event);		
	
	BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);	
	
}

TEST(CANProtocolLayer, testReceiver1)
{
//	uint8_t ret;
	uint8_t data[4];
	canCommand_t commandIn;
	canProtocolState_t state;
	canProtocolEvent_t event = CAN_PROTOCOL_DUMMY_EVENT;

	state = can_protocol_state_process(event);
	BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);
	
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
	rxMsg.Data[6] = 0;	
	rxMsg.Data[7] = 0;	
	can_command_setEvent(CAN_COMMAND_PACKET_RECEIVED_EVENT);
	
	state = can_protocol_state_process(event);	
	BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);	
	
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
	
	state = can_protocol_state_process(event);	
	BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);	

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
	
	state = can_protocol_state_process(event);	
	BYTES_EQUAL(CAN_PROTOCOL_RCV_COMMAND_STATE, state);	
	
	can_protocol_getReceived(&commandIn);

	BYTES_EQUAL(0x09, commandIn.identifier);
	BYTES_EQUAL(0x0a, commandIn.message_type);
	BYTES_EQUAL(0, commandIn.command);
	BYTES_EQUAL(1, commandIn.command_type);
	BYTES_EQUAL(6, commandIn.data[0]);
	BYTES_EQUAL(7, commandIn.data[1]);
	BYTES_EQUAL(8, commandIn.data[2]);
	BYTES_EQUAL(9, commandIn.data[3]);
	BYTES_EQUAL(10, commandIn.data[4]);	
	BYTES_EQUAL(11, commandIn.data[5]);		
	BYTES_EQUAL(12, commandIn.data[6]);	
	BYTES_EQUAL(13, commandIn.data[7]);		
	BYTES_EQUAL(14, commandIn.data[8]);		
	
	data[0] = 1;
	data[1] = 2;
	data[2] = 3;
	data[3] = 4;
	
	can_protocol_setReply(0x03, 8, data);
	event = CAN_PROTOCOL_REPLY_EVENT;
	state = can_protocol_state_process(event);

	event = CAN_PROTOCOL_DUMMY_EVENT;
	state = can_protocol_state_process(event);
	LONGS_EQUAL(0x0509, txMsg.StdId);
	BYTES_EQUAL(CAN_ID_STD, txMsg.IDE);
	BYTES_EQUAL(8, txMsg.DLC);
	BYTES_EQUAL(0x89, txMsg.Data[0]);
	BYTES_EQUAL(0x00, txMsg.Data[1]);	
	BYTES_EQUAL(0x00, txMsg.Data[2]);	// command	
	BYTES_EQUAL(0x03, txMsg.Data[3]);	// command_type
	BYTES_EQUAL(0x00, txMsg.Data[4]);	// 
	BYTES_EQUAL(0x08, txMsg.Data[5]);	// data_length
	BYTES_EQUAL(0x00, txMsg.Data[6]);	// 
	BYTES_EQUAL(0x00, txMsg.Data[7]);	// message_id

	state = can_protocol_state_process(event);	
	BYTES_EQUAL(CAN_PROTOCOL_IDLE_STATE, state);	

	LONGS_EQUAL(0x0509, txMsg.StdId);
	BYTES_EQUAL(CAN_ID_STD, txMsg.IDE);
	BYTES_EQUAL(6, txMsg.DLC);
	BYTES_EQUAL(0x89, txMsg.Data[0]);
	BYTES_EQUAL(0x81, txMsg.Data[1]);	
	BYTES_EQUAL(0x01, txMsg.Data[2]);	// command	
	BYTES_EQUAL(0x02, txMsg.Data[3]);	// command_type
	BYTES_EQUAL(0x03, txMsg.Data[4]);	// 
	BYTES_EQUAL(0x04, txMsg.Data[5]);	// data_length
	BYTES_EQUAL(0x00, txMsg.Data[6]);	// 
	BYTES_EQUAL(0x00, txMsg.Data[7]);	// message_id
	
	state = can_protocol_state_process(event);
}
