/**
  ******************************************************************************
  * @file    DevicesMonitor_Tasks.c
  * @author  LXY
  * @version V1.0
  * @date    2019-11-09
  * @brief   各种外设状态监控任务，主要用于离线检测
  ******************************************************************************
  */

#include "DevicesMonitor_Tasks.h"
#include "MC01_JudgeSystem.h"//裁判系统协议

TaskHandle_t xHandleDevicesMonitor = NULL;

/**
  * @brief  外设帧率监控任务
  * @param  pvParameters 任务被创建时传进来的参数
  * @retval None
  */
void vTaskDevicesMonitor(void *pvParameters)
	{
	while(1)
	{
		DevicesMonitor_update();//检查各设备是否掉线
		Judge_sendDataToClient();//向裁判系统发送信息—到客户端#
    JudgeSYS.ClientUploadInfo.data.data1 = JudgeSYS.sendDataCounter;
    JudgeSYS.ClientUploadInfo.data.data2 = 22.3333;
		vTaskDelay(100);
	}
	
	
	
}


