#include "PlayRoom_Task.h"
#include "Magazine_control.h"
#include "BulletCatcher_control.h"
TaskHandle_t xHandleTaskPlayRoom;

void vTaskPlayRoom(void *pvParameters)
{
	
	TickType_t xLastWakeTime;
	/* 获取当前的系统时间 */
	xLastWakeTime = xTaskGetTickCount();
  
	
	while(1)
	{
		
		Magazine_processing();
		BulletCatcher_writeBank();

		vTaskDelayUntil(&xLastWakeTime, 10);  /* 10ms周期 */
	}	
  
}

