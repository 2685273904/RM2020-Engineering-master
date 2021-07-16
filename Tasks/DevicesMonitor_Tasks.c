/**
  ******************************************************************************
  * @file    DevicesMonitor_Tasks.c
  * @author  LXY
  * @version V1.0
  * @date    2019-11-09
  * @brief   ��������״̬���������Ҫ�������߼��
  ******************************************************************************
  */

#include "DevicesMonitor_Tasks.h"
#include "MC01_JudgeSystem.h"//����ϵͳЭ��

TaskHandle_t xHandleDevicesMonitor = NULL;

/**
  * @brief  ����֡�ʼ������
  * @param  pvParameters ���񱻴���ʱ�������Ĳ���
  * @retval None
  */
void vTaskDevicesMonitor(void *pvParameters)
	{
	while(1)
	{
		DevicesMonitor_update();//�����豸�Ƿ����
		Judge_sendDataToClient();//�����ϵͳ������Ϣ�����ͻ���#
    JudgeSYS.ClientUploadInfo.data.data1 = JudgeSYS.sendDataCounter;
    JudgeSYS.ClientUploadInfo.data.data2 = 22.3333;
		vTaskDelay(100);
	}
	
	
	
}


