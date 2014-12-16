#ifndef __CAN_OS_H
#define	__CAN_OS_H

#define	_CPPUTEST_

#include <stdlib.h>
#include <stdint.h>
#ifdef _CPPUTEST_
#include <memory.h>
#include "CAN_helper.h"
#else
#include <string.h>
#endif

#define CAN_ID_RANGE	63
#define CAN_PACKET_SIZE 6
#define CAN_MAX_DATA_LENGTH	150 
#define CAN_MAX_PACKET_COUNT  (CAN_MAX_DATA_LENGTH/CAN_PACKET_SIZE)+((CAN_MAX_DATA_LENGTH%CAN_PACKET_SIZE)?1:0)

#define CAN_FIRST_PACKET             0
#define CAN_GENERAL_PACKET           1
#define CAN_LAST_PACKET              2
#define CAN_ACK_PACKET               3

#define MESSAGE_TYPE_RANGE  15

typedef struct
{
	unsigned nodeID      :7;
	unsigned messageType :4;
	unsigned             :5;
}canIDDefineBit_t;

typedef union
{
	canIDDefineBit_t bit;
	uint16_t bitField;
}canID_t;

typedef struct
{
	unsigned nodeID     :7;
	unsigned segment    :1;
}canHeaderBit_t;

typedef union
{
	canHeaderBit_t bit;
	uint8_t  bitField;
}canHeader_t;

typedef struct
{
	unsigned packetCnt   :6;
	unsigned segmentType :2;
}canSegmentBit_t;

typedef union
{
	canSegmentBit_t  bit;
	uint8_t bitField;
}canSegment_t;

typedef struct
{
	canID_t identifier;
	canHeader_t  header;
	canSegment_t segment;
	uint8_t length;
	uint8_t total_packet;
	uint8_t last_packet_length;
	uint8_t data[CAN_MAX_DATA_LENGTH];
}canMessage_t;

uint8_t can_message_construct(canMessage_t*, uint8_t, uint16_t, uint8_t, uint8_t*);

#endif
