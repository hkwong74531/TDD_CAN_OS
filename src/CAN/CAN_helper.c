#include "CAN_helper.h"

void CAN_SetTxMsg(CanTxMsg* txMsg, uint16_t id, uint32_t id_type, uint8_t dlc, uint8_t* data)
{
	if(IS_CAN_IDTYPE(id_type))
	{
		txMsg->RTR=CAN_RTR_DATA;

		txMsg->IDE=id_type;
		if(id_type == CAN_ID_EXT)
		{
			txMsg->ExtId=id;
		}
		else
		{
			txMsg->StdId=id;
		}

		txMsg->DLC=dlc;
		memcpy(txMsg->Data, data, dlc);

//  		CAN_Transmit(CAN1, txMsg);	
  	}
}
