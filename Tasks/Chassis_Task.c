/**
  ******************************************************************************
  * @file    Chassis_Task.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   ����+��̨�˶���������
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
/* �����������񴴽�֮��õ���Ӧֵ���Ժ�������������ʱ����Ҫͨ�������������� */
TaskHandle_t xHandleTaskChassis = NULL;
/*------------------G L O B A L - F U N C T I O N S --------------------------*/
/**
  * @brief  ����+��̨����
  * @param  pvParameters ���񱻴���ʱ�������Ĳ���
  * @retval None
  */
void vTaskChassis(void *pvParameters)
{
	
	TickType_t xLastWakeTime;
	/* ��ȡ��ǰ��ϵͳʱ�� */
	xLastWakeTime = xTaskGetTickCount();
  
  //Cloud_Init();  
  Chassis_Init();
  
  xEventGroupSetBits(xSystemInitEventGroup, EVENT_BIT_CHASSIS_INIT_FINISH);
	
  int counter = 0;
	
	while(1)
	{
    if(++counter >100){
			LED_RED_TOGGLE();/* 1s���� */
			counter = 0;
		}
    
		
		//Cloud_processing();
		Chassis_processing();
		//ANO_Send_Data_V3(Cloud.targetYawRaw, Cloud.M6623s[0].totalAngle,100,100);//��λ��
   
    
		vTaskDelayUntil(&xLastWakeTime, 10);  /* 10ms���� */
	}	
  
}
/*-----------------------------------FILE OF END------------------------------*/
