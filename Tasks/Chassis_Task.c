/**
  ******************************************************************************
  * @file    Chassis_Task.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   底盘+云台运动控制任务
  ******************************************************************************
  */
  
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "Chassis_Task.h"
#include "Chassis_control.h"
#include "Cloud_control.h"
#include "SystemInit_Tasks.h"
#include "ANO.h"
#include "Buzzer.h"
/*-----------L O C A L - F U N C T I O N S - P R O T O T Y P E S--------------*/
/* 任务句柄，任务创建之后得到对应值，以后想操作这个任务时都需要通过这个句柄来操作 */
TaskHandle_t xHandleTaskChassis = NULL;
/*------------------G L O B A L - F U N C T I O N S --------------------------*/
/**
  * @brief  底盘+云台任务
  * @param  pvParameters 任务被创建时传进来的参数
  * @retval None
  */
void vTaskChassis(void *pvParameters)
{
	
	TickType_t xLastWakeTime;
	/* 获取当前的系统时间 */
	xLastWakeTime = xTaskGetTickCount();
  
  //Cloud_Init();  
  Chassis_Init();
  
  xEventGroupSetBits(xSystemInitEventGroup, EVENT_BIT_CHASSIS_INIT_FINISH);
	
  int counter = 0;
	
	while(1)
	{
    if(++counter >100){
			LED_RED_TOGGLE();/* 1s周期 */
			counter = 0;
		}
    
		
		//Cloud_processing();
		Chassis_processing();
		//ANO_Send_Data_V3(Cloud.targetYawRaw, Cloud.M6623s[0].totalAngle,100,100);//上位机
   
    
		vTaskDelayUntil(&xLastWakeTime, 10);  /* 10ms周期 */
	}	
  
}
/*-----------------------------------FILE OF END------------------------------*/
