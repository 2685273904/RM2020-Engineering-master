/**
  ******************************************************************************
  * @file    UpperStructure_Task.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   上层运动控制任务
  ******************************************************************************
  */
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "SystemInit_Tasks.h"
#include "UpperStructure_Task.h"
#include "UpperStructure.h"
#include "Lifting_control.h"
#include "BulletCatcher_control.h"
#include "ANO.h"
/*-----------L O C A L - F U N C T I O N S - P R O T O T Y P E S--------------*/
/* 任务句柄，任务创建之后得到对应值，以后想操作这个任务时都需要通过这个句柄来操作 */
TaskHandle_t xHandleTaskUpperStructure;

/*------------------G L O B A L - F U N C T I O N S --------------------------*/
/**
  * @brief  上层任务
  * @param  pvParameters 任务被创建时传进来的参数
  * @retval None
  */
void vTaskUpperStructure(void *pvParameters)
{
	
	TickType_t xLastWakeTime;
	/* 获取当前的系统时间 */
	xLastWakeTime = xTaskGetTickCount();
  	
	UpperStructure_Init();
	
  xEventGroupSetBits(xSystemInitEventGroup, EVENT_BIT_UPPERSTRUCTURE_INIT_FINISH);
	
	while(1)
	{
    //ANO_Send_Data_V3(100, 100,100,100);
		UpperStructure_processing();
   
    
		vTaskDelayUntil(&xLastWakeTime, 10);  /* 10ms周期 */
	}	
  
}
/*-----------------------------------FILE OF END------------------------------*/
