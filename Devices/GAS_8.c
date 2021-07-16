/**
  ******************************************************************************
  * @file    GAS_8.c
  * @author  Hare
  * @version V1.0
  * @date    
  * @brief   八路气阀控制器应用函数接口
  ******************************************************************************
  */
  
  
#include "GAS_8.h"

GAS8_t GAS8s[8];

/**
* @brief  把气阀数据发送到指定ID的气阀控制板上
  * @param[in]  p_GAS  气阀数据包指针
  * @param[in]  gasID  气阀ID，板子的ID灯闪几下就是多少，范围1-8
  * @retval None
  */
void GAS8_sendData(void)
{
  uint8_t data[8];  /* CAN数据包 */
  
  for(int i = 0; i < 8; i++)
  {
    data[i] = GAS8s[i].data.dataPack;
  }

	
	
	CAN_SendData(CAN2, CAN_ID_STD, GAS8_SEND_ID, data);
  
}



/**
  * @brief  气阀位置一
  * @param  None
  * @retval None
  */
void GAS8_setBits(uint8_t gasID, uint8_t pos){
	GAS8s[gasID].data.dataPack |= pos;
  GAS8_sendData();
}

/**
  * @brief  气阀位置零
  * @param  None
  * @retval None
  */
void GAS8_resetBits(uint8_t gasID, uint8_t pos){
	GAS8s[gasID].data.dataPack &= ~pos;
  GAS8_sendData();
}

/**
  * @brief  气阀位翻转
  * @param  None
  * @retval None
  */
void GAS8_toggleBits(uint8_t gasID, uint8_t pos){
	GAS8s[gasID].data.dataPack ^= pos;
  GAS8_sendData();
}

/**
  * @brief  气阀位写入
  * @param  None
  * @retval None
  */
void GAS8_writeBits(uint8_t gasID, uint8_t pos, bool status){
	if(status)
    GAS8_setBits(gasID, pos);
  else
    GAS8_resetBits(gasID, pos);
  
  GAS8_sendData();
}

/**
  * @brief  气阀数据写入
  * @param  None
  * @retval None
  */
void GAS8_writeData(uint8_t gasID, uint8_t data){
  GAS8s[gasID].data.dataPack = data;
  GAS8_sendData();
}





