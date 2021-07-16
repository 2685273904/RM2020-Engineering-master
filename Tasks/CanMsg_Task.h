#ifndef __CANMSG_TASK_H 
#define __CANMSG_TASK_H 
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "user_common.h"

/*-------------------------G L O B A L - T Y P E S----------------------------*/
typedef struct
{
  uint8_t     CANx;               /*1：CAN1  2：CAN2*/
  CanTxMsg    SendCanTxMsg;       /*要发送的数据缓存*/
}CanSend_t;



typedef struct
{
	uint8_t     CANx;               //CAN编号     1 CAN1      2 CAN2
  CanRxMsg    ReceiveCanRxMsg;       //接收数据
}CanReceive_Param_t;


  
/*--------------------- I N C L U D E - F I L E S ----------------------------*/

/*-------------------------G L O B A L - T Y P E S----------------------------*/

extern QueueHandle_t xCanSendQueue;
extern QueueHandle_t xCan1RxQueue ;
extern QueueHandle_t xCan2RxQueue ;

/*------------------------G L O B A L - M A C R O S --------------------------*/



/*----------------------G L O B A L - D E F I N E S---------------------------*/



/*-----------G L O B A L - F U N C T I O N S - P R O T O T Y P E S------------*/

void CanTaskCreate(void);


#endif	// __TASK_CAN_H
/*----------------------------------FILE OF END-------------------------------*/
