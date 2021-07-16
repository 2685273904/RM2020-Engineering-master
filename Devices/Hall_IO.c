/**
  ******************************************************************************
  * @file    Hall_IO.c
  * @author  Lxy
  * @version V1.0
  * @date    
  * @brief   霍尔、光电传感器
  ******************************************************************************
  */

#include "Hall_IO.h"



/**
* @brief  IO型霍尔传感器初始化
  * @param  None
  * @retval None
  */
void Hall_IO_Init(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体
	RCC_AHB1PeriphClockCmd(Hall_LIFT_UPLIMIT_GPIO_CLK, ENABLE); 	//初始化GPIO时钟	
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;   //输入模式
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	//上拉  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //速度选择
  
	GPIO_InitStructure.GPIO_Pin   = Hall_LIFT_DOWNLIMIT_PIN;	 
	GPIO_Init(Hall_LIFT_DOWNLIMIT_GPIO_PORT, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin   = Hall_LIFT_UPLIMIT_PIN;	 
	GPIO_Init(Hall_LIFT_UPLIMIT_GPIO_PORT, &GPIO_InitStructure);
  

  
  GPIO_InitStructure.GPIO_Pin   = Hall_CATCHERPULS_OPEN_LOCK_PIN;	 //没用上
	GPIO_Init(Hall_CATCHERPULS_OPEN_GPIO_PORT, &GPIO_InitStructure);

}



/**
* @brief  光电门初始化
  * @param  None
  * @retval None
  */
void Photogate_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体
	RCC_AHB1PeriphClockCmd(Hall_LIFT_UPLIMIT_GPIO_CLK, ENABLE); 	//初始化GPIO时钟	
 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;   //输入模式
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//浮空
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //速度选择	

	GPIO_InitStructure.GPIO_Pin   = Hall_PILLARHOLDING_LOCK_PIN;//PA2 用于光电门
	GPIO_Init(Hall_PILLARHOLDING_LOCK_GPIO_PORT, &GPIO_InitStructure);

}




