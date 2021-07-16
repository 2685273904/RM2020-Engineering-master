/**
  ******************************************************************************
  * @file    Buzzer.c
  * @author  Hare
  * @version V1.0
  * @date    
  * @brief   ��Դ����������
  ******************************************************************************
  */
  
  
#include "Buzzer.h"

#define Do 523  
#define Re 587  
#define Mi 659  
#define Fa 698  
#define Sol 784  
#define La 880  
#define Si 988


/**
  * @brief  ��Դ��������ʼ��
  * @param  None
  * @retval None
  */
void Buzzer_Init(void){
  /* TIM12��90M/90/20000=50HZ */
  TIM12_PWMOutput(89,0,0);
}



/**
  * @brief  ��Դ�������ر�
  * @param  None
  * @retval None
  */
void Buzzer_OFF(void){
   TIM_SetComparex[0](TIM12, 0);//ռ�ձ�Ϊ0
}


/**
  * @brief  ��Դ����������
  * @param[in]  tone  ������Ƶ�� ��λHz
  * @param[in]  vol  ��������������Χ0-100
  * @retval None
  */
void Buzzer_setTone(uint16_t tone, uint8_t vol){
  uint32_t autoReload = 1000000.0f/(float)tone;//��������
  uint32_t compare = (float)autoReload*(float)vol/100.0f;//����ռ�ձ�
  TIM_SetAutoreload(TIM12, autoReload);//��������
  TIM_SetComparex[0](TIM12, compare);//#
}

void Buzzer_play(void){
  Buzzer_setTone(Do, 30);
  vTaskDelay(110);
  Buzzer_setTone(Mi, 40);
  vTaskDelay(110);
  Buzzer_setTone(Sol, 50);
  vTaskDelay(110);
  Buzzer_setTone(La, 60);
  vTaskDelay(110);
  Buzzer_OFF();
  
}


