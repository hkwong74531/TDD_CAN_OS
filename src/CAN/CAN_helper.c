#include "CAN_helper.h"

CanTxMsg txMsg;
CanRxMsg rxMsg;

void CAN_SetTxMsg(uint16_t id, uint32_t id_type, uint8_t dlc, uint8_t* data)
{
//	CanTxMsg txMsg;

	if(IS_CAN_IDTYPE(id_type))
	{
		txMsg.RTR=CAN_RTR_DATA;

		txMsg.IDE=id_type;
		if(id_type == CAN_ID_EXT)
		{
			txMsg.ExtId=id;
		}
		else
		{
			txMsg.StdId=id;
		}

		txMsg.DLC=dlc;
		memcpy(txMsg.Data, data, dlc);

//  		CAN_Transmit(CAN1, txMsg);	
  	}
}

void CAN_GetRxMsg(uint32_t* id, uint32_t* id_type, uint8_t* dlc, uint8_t* data)
{
	(*id_type) = rxMsg.IDE;

	if(rxMsg.IDE == CAN_ID_EXT)
	{
		(*id) = rxMsg.ExtId;
	}
	else// if(rxMsg.IDE == CAN_ID_STD)
	{
		(*id) = rxMsg.StdId;
	}
	
	(*dlc) = rxMsg.DLC;
	memcpy(data, rxMsg.Data, rxMsg.DLC);
}
