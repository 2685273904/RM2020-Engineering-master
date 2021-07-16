/**
  ******************************************************************************
  * @file    CanMsg_Task.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   CAN发送、接收任务
  ******************************************************************************
  */
	
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "CanMsg_Task.h"
#include "CAN.h"
#include "BulletCatcher_control.h"
#include "GAS_8.h"
#include "Lifting_control.h"
#include "Cloud_control.h"
#include "Chassis_control.h"
#include "UpperStructure.h"
#include "GY_IMU.h"
/*-------------------------- D E F I N E S -----------------------------------*/
QueueHandle_t xCanSendQueue= NULL;
QueueHandle_t xCan1RxQueue = NULL;
QueueHandle_t xCan2RxQueue = NULL;

static TaskHandle_t xHandleCan1Receive = NULL;
static TaskHandle_t xHandleCan2Receive = NULL;
static TaskHandle_t xHandleCanSend = NULL;

float ZGyroModuleAngle=0.0f;

/*-----------L O C A L - F U N C T I O N S - P R O T O T Y P E S--------------*/

static void vTaskCanSend(void *Parameters);
static void vTaskCan1Receive(void *pvParameters);
static void vTaskCan2Receive(void *pvParameters);
static void CAN1_DataDecoding(CanRxMsg rxMessage);

/*------------------G L O B A L - F U N C T I O N S --------------------------*/
void CanTaskCreate(void)
{
	/* 创建队列  */
	xCanSendQueue=xQueueCreate(20,sizeof(CanSend_t));
	xCan1RxQueue =xQueueCreate(20,sizeof(CanRxMsg));
	xCan2RxQueue =xQueueCreate(20,sizeof(CanRxMsg));
	
	/* CAN1接收任务  */	
 	xTaskCreate(vTaskCan1Receive,            /* 任务函数  */
						"vTaskCan1Receive",            /* 任务名    */ 
						128,       			               /* 任务栈大小*/
						NULL,                          /* 任务参数  */
						5,       			                 /* 任务优先级*/
						&xHandleCan1Receive);          /* 任务句柄  */ 
	
	/* CAN2接收任务  */		
	xTaskCreate(vTaskCan2Receive,            
						"vTaskCan2Receive",          
						128,       			   
						NULL,                 
						5,       			   
						&xHandleCan2Receive); 
	
	/* CAN发送任务  */	
 	xTaskCreate(vTaskCanSend,            
						"vTaskCanSend",          
						128,       			   
						NULL,                 
						4,       			   
						&xHandleCanSend); 
}

/*---------------------L O C A L - F U N C T I O N S--------------------------*/
	/**
	* @Data    2019-01-11 11:13
* @brief   CAN发送函数
	* @param   void
	* @retval  void
	*/
static void vTaskCanSend(void *Parameters)
{
	CanSend_t canSendData;
	
	while(1)
	{
		xQueueReceive(xCanSendQueue, &canSendData, portMAX_DELAY);
			
		if(canSendData.CANx == 1)
		{     	
			CAN_Transmit(CAN1, &(canSendData.SendCanTxMsg));
		}
		else
		{
			CAN_Transmit(CAN2, &(canSendData.SendCanTxMsg));
		} 
	}
}


/*------------------------------80 Chars Limit--------------------------------*/
	/**
	* @Data    2019-01-11 11:13
	* @brief   CAN1接收函数
	* @param   void
	* @retval  void
	*/

static void vTaskCan1Receive(void *pvParameters)
{
	CanRxMsg RxMessage;
  while(1)
	{
		
	  xQueueReceive(xCan1RxQueue, &RxMessage,portMAX_DELAY);
		
    switch(RxMessage.StdId){
      /* 底盘3508 x4 */
      case CHASSIS_M3508_RU:
      case CHASSIS_M3508_RD:
      case CHASSIS_M3508_LD:
      case CHASSIS_M3508_LU:
      {
        uint32_t StdId;
        StdId = RxMessage.StdId - CHASSIS_M3508_READID_START;
        M3508_getInfo(&Chassis.M3508s[StdId], RxMessage);
      }
      break;
            
      
      /* 抬升3508 x2 */
      case LIFTING_M3508_ID_L:
        M3508_getInfo(&Lifting.M3508s[0], RxMessage);
      break;
      case LIFTING_M3508_ID_R:
        M3508_getInfo(&Lifting.M3508s[1], RxMessage);
      break;
			
      default:
        break;
    }

	}
}

/*------------------------------80 Chars Limit--------------------------------*/
	/**
	* @Data    2019-01-11 11:13
	* @brief   CAN2接收函数
	* @param   void
	* @retval  void
	*/

static void vTaskCan2Receive(void *pvParameters)
{
	CanRxMsg RxMessage;
  while(1)
	{
	  xQueueReceive(xCan2RxQueue, &RxMessage,portMAX_DELAY);
		
    switch(RxMessage.StdId){
      /* 抓取3508 x2 ，0：夹子上，1：侧面 */
      case CATCHER_M3508_ID_L:
        M3508_getInfo(&BulletCatcher.M3508s[0], RxMessage);
      break;
      case CATCHER_M3508_ID_R:
        M3508_getInfo(&BulletCatcher.M3508s[1], RxMessage);
      break;
      
      /* 云台电机x2 */
      case CLOUD_M6623_ID_YAW:
        M6623_getInfo(&Cloud.M6623s[0],RxMessage);
      break;
      case CLOUD_M6623_ID_PITCH:
        M6623_getInfo(&Cloud.M6623s[1],RxMessage);
      break;

      default:
        break;
    }
		

	}
}


