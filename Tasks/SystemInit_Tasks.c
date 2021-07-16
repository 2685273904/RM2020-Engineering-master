/**
  ******************************************************************************
  * @file    SystemInit_Tasks.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   初始化任务
  ******************************************************************************
  */
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "SystemInit_Tasks.h"
#include "BulletCatcher_control.h"
#include "UpperStructure_Task.h"
#include "Command_Task.h"
#include "CanMsg_Task.h"
#include "Lifting_control.h"
#include "Chassis_Task.h"
#include "Buzzer.h"
#include "DevicesMonitor_Tasks.h"
#include "PlayRoom_Task.h"
#include "Hall_IO.h"

/*-----------L O C A L - F U N C T I O N S - P R O T O T Y P E S--------------*/
EventGroupHandle_t xSystemInitEventGroup = NULL;

/*------------------G L O B A L - F U N C T I O N S --------------------------*/
/**
  * @brief  系统、外设初始化，任务创建，仅执行一次
  * @param  pvParameters 任务被创建时传进来的参数
  * @retval None
  */
void vTaskSystemInit(void *pvParameters)
{
	taskENTER_CRITICAL();/*进入临界区*/
  EventBits_t uxBits;
  
	/*------------------BSP初始化------------------*/
	LED_Init();
	CAN1_QuickInit();
	CAN2_QuickInit();
	DR16_receiverInit(); 
	//USART6_QuickInit(9600);
	JudgeSystem_Init();//裁判系统初始化#
	Hall_IO_Init();
	Photogate_IO_Init();
	Buzzer_Init();
  vTaskDelay(500); 
	Buzzer_play();
	/*----------------------end--------------------*/
	
	/* 创建事件标志组 */
	xSystemInitEventGroup = xEventGroupCreate();
	
	
		/* 创建CAN收发任务 */
	CanTaskCreate(); 
	
	/*退出临界区*/
	taskEXIT_CRITICAL(); /*退出临界区*/
	

	/* 控制任务创建开始 */
  xTaskCreate(vTaskDevicesMonitor,
							"Task DevicesMonitor",
							128,
							NULL,
							1,
							&xHandleDevicesMonitor);
							
	xTaskCreate(vTaskChassis,
							"Task Chassis",
							256,
							NULL,
							2,
							&xHandleTaskChassis);
							
							
	xTaskCreate(vTaskUpperStructure,
							"Task UpperStructure",
							512,
							NULL,
							3,
							&xHandleTaskUpperStructure);
							
	xTaskCreate(vTaskPlayRoom,
							"vTask PlayRoom",
							128,
							NULL,
							1,
							&xHandleTaskPlayRoom);		

	/* 挂起弹仓开关任务 */
	vTaskSuspend(xHandleTaskPlayRoom);
							
													
  uxBits = xEventGroupWaitBits(xSystemInitEventGroup,
                               EVENT_BIT_CHASSIS_INIT_FINISH|EVENT_BIT_UPPERSTRUCTURE_INIT_FINISH,
                               pdFALSE,
															 pdTRUE,
															 portMAX_DELAY);
  Buzzer_play();

  xTaskCreate(vTaskCommand,
							"Task Command",
							512,
							NULL,
							4,
							&xHandleTaskCommand);
   
  //DEBUG_INFO("System Init Finish...\r\n");
//  SEGGER_RTT_printf(0, "[INFO] System Init Finish...\r\n");
	//运行完删除本任务
  vTaskDelete(NULL);
	
}
/*-----------------------------------FILE OF END------------------------------*/
