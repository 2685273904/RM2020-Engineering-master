/**
  ******************************************************************************
  * @file    SystemInit_Tasks.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   ��ʼ������
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
  * @brief  ϵͳ�������ʼ�������񴴽�����ִ��һ��
  * @param  pvParameters ���񱻴���ʱ�������Ĳ���
  * @retval None
  */
void vTaskSystemInit(void *pvParameters)
{
	taskENTER_CRITICAL();/*�����ٽ���*/
  EventBits_t uxBits;
  
	/*------------------BSP��ʼ��------------------*/
	LED_Init();
	CAN1_QuickInit();
	CAN2_QuickInit();
	DR16_receiverInit(); 
	//USART6_QuickInit(9600);
	JudgeSystem_Init();//����ϵͳ��ʼ��#
	Hall_IO_Init();
	Photogate_IO_Init();
	Buzzer_Init();
  vTaskDelay(500); 
	Buzzer_play();
	/*----------------------end--------------------*/
	
	/* �����¼���־�� */
	xSystemInitEventGroup = xEventGroupCreate();
	
	
		/* ����CAN�շ����� */
	CanTaskCreate(); 
	
	/*�˳��ٽ���*/
	taskEXIT_CRITICAL(); /*�˳��ٽ���*/
	

	/* �������񴴽���ʼ */
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

	/* ���𵯲ֿ������� */
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
	//������ɾ��������
  vTaskDelete(NULL);
	
}
/*-----------------------------------FILE OF END------------------------------*/
