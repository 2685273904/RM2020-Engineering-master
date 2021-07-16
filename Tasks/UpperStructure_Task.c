/**
  ******************************************************************************
  * @file    UpperStructure_Task.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   �ϲ��˶���������
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
/* �����������񴴽�֮��õ���Ӧֵ���Ժ�������������ʱ����Ҫͨ�������������� */
TaskHandle_t xHandleTaskUpperStructure;

/*------------------G L O B A L - F U N C T I O N S --------------------------*/
/**
  * @brief  �ϲ�����
  * @param  pvParameters ���񱻴���ʱ�������Ĳ���
  * @retval None
  */
void vTaskUpperStructure(void *pvParameters)
{
	
	TickType_t xLastWakeTime;
	/* ��ȡ��ǰ��ϵͳʱ�� */
	xLastWakeTime = xTaskGetTickCount();
  	
	UpperStructure_Init();
	
  xEventGroupSetBits(xSystemInitEventGroup, EVENT_BIT_UPPERSTRUCTURE_INIT_FINISH);
	
	while(1)
	{
    //ANO_Send_Data_V3(100, 100,100,100);
		UpperStructure_processing();
   
    
		vTaskDelayUntil(&xLastWakeTime, 10);  /* 10ms���� */
	}	
  
}
/*-----------------------------------FILE OF END------------------------------*/
