#include "PlayRoom_Task.h"
#include "Magazine_control.h"
#include "BulletCatcher_control.h"
TaskHandle_t xHandleTaskPlayRoom;

void vTaskPlayRoom(void *pvParameters)
{
	
	TickType_t xLastWakeTime;
	/* ��ȡ��ǰ��ϵͳʱ�� */
	xLastWakeTime = xTaskGetTickCount();
  
	
	while(1)
	{
		
		Magazine_processing();
		BulletCatcher_writeBank();

		vTaskDelayUntil(&xLastWakeTime, 10);  /* 10ms���� */
	}	
  
}

