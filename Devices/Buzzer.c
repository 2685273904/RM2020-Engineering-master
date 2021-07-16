/**
  ******************************************************************************
  * @file    Buzzer.c
  * @author  Hare
  * @version V1.0
  * @date    
  * @brief   无源蜂鸣器驱动
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
  * @brief  无源蜂鸣器初始化
  * @param  None
  * @retval None
  */
void Buzzer_Init(void){
  /* TIM12：90M/90/20000=50HZ */
  TIM12_PWMOutput(89,0,0);
}



/**
  * @brief  无源蜂鸣器关闭
  * @param  None
  * @retval None
  */
void Buzzer_OFF(void){
   TIM_SetComparex[0](TIM12, 0);//占空比为0
}


/**
  * @brief  无源蜂鸣器音量
  * @param[in]  tone  蜂鸣器频率 单位Hz
  * @param[in]  vol  蜂鸣器音量，范围0-100
  * @retval None
  */
void Buzzer_setTone(uint16_t tone, uint8_t vol){
  uint32_t autoReload = 1000000.0f/(float)tone;//计算周期
  uint32_t compare = (float)autoReload*(float)vol/100.0f;//计算占空比
  TIM_SetAutoreload(TIM12, autoReload);//设置周期
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


