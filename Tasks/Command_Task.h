#ifndef __COMMAND_TASK_H
#define __COMMAND_TASK_H

#include "user_common.h"

typedef enum{
	Control_ByRC = 3,
	Control_ByPC = 1,
	Control_OFF = 2,
  Control_LOCK = 4,
}ControlBy_e;
 
typedef enum{
	SpecialMode_Normal = 0,
  SpecialMode_StandBy = 1,	/* ׼���˶�ģʽ */
	SpecialMode_Climbing_Up = 2,	/* �ϵ�ģʽ */
  SpecialMode_Climbing_Down = 3,	/* �µ�ģʽ */
	SpecialMode_Supply = 4, 	/* ȡ��ģʽ */
  SpecialMode_Give = 5, 	/* ����ģʽ */
	SpecialMode_LookTv=6,
}SpecialMode_e;

typedef enum{
  CombineKey_None = 0x00,
  CombineKey_Ctrl,
  CombineKey_Shift,
  CombineKey_Ctrl_Shift,
}CombineKey_e;


typedef struct{
	ControlBy_e Controlby;			//���Ʒ�ʽ
	SpecialMode_e SpecialMode;			//����ģʽ
	CombineKey_e isCombineKey;			//ʹ����ϼ�
  uint8_t cloud_look;           /* ������̨�� */
}ControlStatus_t;



extern TaskHandle_t xHandleTaskCommand;
extern EventGroupHandle_t xCommandEventGroup;
extern ControlStatus_t ControlStatus;
void vTaskCommand(void *pvParameters);

#endif /* __KEY_TASK_H */