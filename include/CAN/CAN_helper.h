#ifndef D_CANTest_H
#define D_CANTest_H

#include <stdlib.h>
#include <stdint.h>
#include <memory.h>

#define CAN_Id_Standard       ((uint32_t)0x00000000)  /*!< Standard Id */
#define CAN_Id_Extended       ((uint32_t)0x00000004)  /*!< Extended Id */
#define IS_CAN_IDTYPE(IDTYPE) (((IDTYPE) == CAN_Id_Standard) || ((IDTYPE) == CAN_Id_Extended))

#define CAN_RTR_Data                ((uint32_t)0x00000000)  /*!< Data frame */
#define CAN_RTR_Remote              ((uint32_t)0x00000002)  /*!< Remote frame */
#define IS_CAN_RTR(RTR) (((RTR) == CAN_RTR_Data) || ((RTR) == CAN_RTR_Remote))

#define CAN_ID_STD            CAN_Id_Standard           
#define CAN_ID_EXT            CAN_Id_Extended
#define CAN_RTR_DATA          CAN_RTR_Data         
#define CAN_RTR_REMOTE        CAN_RTR_Remote
							   
/** 
  * @brief  CAN Tx message structure definition  
  */

typedef struct
{
  uint32_t StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

  uint32_t ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */

  uint8_t IDE;     /*!< Specifies the type of identifier for the message that 
                        will be transmitted. This parameter can be a value 
                        of @ref CAN_identifier_type */

  uint8_t RTR;     /*!< Specifies the type of frame for the message that will 
                        be transmitted. This parameter can be a value of 
                        @ref CAN_remote_transmission_request */

  uint8_t DLC;     /*!< Specifies the length of the frame that will be 
                        transmitted. This parameter can be a value between 
                        0 to 8 */

  uint8_t Data[8]; /*!< Contains the data to be transmitted. It ranges from 0 
                        to 0xFF. */
} CanTxMsg;

/** 
  * @brief  CAN Rx message structure definition  
  */

typedef struct
{
  uint32_t StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

  uint32_t ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */

  uint8_t IDE;     /*!< Specifies the type of identifier for the message that 
                        will be received. This parameter can be a value of 
                        @ref CAN_identifier_type */

  uint8_t RTR;     /*!< Specifies the type of frame for the received message.
                        This parameter can be a value of 
                        @ref CAN_remote_transmission_request */

  uint8_t DLC;     /*!< Specifies the length of the frame that will be received.
                        This parameter can be a value between 0 to 8 */

  uint8_t Data[8]; /*!< Contains the data to be received. It ranges from 0 to 
                        0xFF. */

  uint8_t FMI;     /*!< Specifies the index of the filter the message stored in 
                        the mailbox passes through. This parameter can be a 
                        value between 0 to 0xFF */
} CanRxMsg;

void CAN_SetTxMsg(CanTxMsg*, uint16_t, uint32_t, uint8_t, uint8_t*);

#endif  /* D_CANTest_H */