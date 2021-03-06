/**
  ******************************************************************************
  * @file    GPIO/GPIO_IOToggle/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "user_common.h"
#include "BulletCatcher_control.h"
#include "GAS_8.h"
#include "Lifting_control.h"
#include "Cloud_control.h"
#include "Chassis_control.h"
#include "UpperStructure.h"
#include "GY_IMU.h"
#include "CanMsg_Task.h"
/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
//void SVC_Handler(void)
//{
//}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

void xPortSysTickHandler(void);

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)	/*??????????????????*/
  {
    xPortSysTickHandler();	
  }
}



		



/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s/startup_stm32f429x.s).    */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @brief  USART1?????????????????????DR?????????
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
	
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		//??????DMA
		DMA_Cmd(USART1_RX_DMA_STREAM, DISABLE);
		
		//printf("%d\n",DMA_GetCurrDataCounter(USART1_RX_DMA_STREAM));
    
		//??????DMAbuff???????????????????????????
		if (DMA_GetCurrDataCounter(USART1_RX_DMA_STREAM) == 2)
		{
			//printf("%s\n", DR16_rxBuff);
			
			DR16_dataProcess(DR16_rxBuff);
      LED_GREEN_TOGGLE();
		}
		
		//???????????????????????????+2????????????
    DMA_SetCurrDataCounter(USART1_RX_DMA_STREAM,DR16_DBUS_PACKSIZE+2);
		//??????DMA
		DMA_Cmd(USART1_RX_DMA_STREAM, ENABLE);
		
		
		//???????????????????????????
		(void)USART1->DR;
		(void)USART1->SR;
		//???????????????????????????????????????????????????????????????
		//USART_ClearITPendingBit(USART1, USART_IT_IDLE);
	}
}


/**
  * @brief  USART2??????????????????
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void){

	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		//??????DMA
		DMA_Cmd(USART2_RX_DMA_STREAM, DISABLE);
		
		uint16_t DMA_Counter = DMA_GetCurrDataCounter(USART2_RX_DMA_STREAM);
		
		
		//????????????????????????
    DMA_SetCurrDataCounter(USART2_RX_DMA_STREAM,20);
		//??????DMA
		DMA_Cmd(USART2_RX_DMA_STREAM, ENABLE);
		
		
		//???????????????????????????
		(void)USART2->DR;
		(void)USART2->SR;
		//???????????????????????????????????????????????????????????????
		//USART_ClearITPendingBit(USART2, USART_IT_IDLE);
	}
}


/**
  * @brief  USART3??????????????????
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void){

	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{
		//??????DMA
		DMA_Cmd(USART3_RX_DMA_STREAM, DISABLE);
		
		uint16_t DMA_Counter = DMA_GetCurrDataCounter(USART3_RX_DMA_STREAM);
		
		GY_IMU_getInfo(Cloud_GY_IMU_RXBUFF, CLOUD_GY_IMU_BUFFSIZE-DMA_Counter, &Cloud.IMU);//?????????????????????????????????
		//????????????????????????
    DMA_SetCurrDataCounter(USART3_RX_DMA_STREAM,CLOUD_GY_IMU_BUFFSIZE);
		//??????DMA
		DMA_Cmd(USART3_RX_DMA_STREAM, ENABLE);
		
		
		//???????????????????????????
		(void)USART3->DR;
		(void)USART3->SR;
		//???????????????????????????????????????????????????????????????
		//USART_ClearITPendingBit(USART3, USART_IT_IDLE);
	}
}

/**
  * @brief  USART6?????????????????????????????????
  * @param  None
  * @retval None
  */
void USART6_IRQHandler(void){
	
	if(USART_GetITStatus(USART6, USART_IT_IDLE) != RESET)
	{
		//??????DMA
		DMA_Cmd(USART6_RX_DMA_STREAM, DISABLE);
		
		
		uint16_t DMA_Counter = DMA_GetCurrDataCounter(USART6_RX_DMA_STREAM);
		
		Judge_getInfo(JudgeSystem_rxBuff, JUDGESYSTEM_PACKSIZE-DMA_Counter);//????????????????????????
		
		//????????????????????????
    DMA_SetCurrDataCounter(USART6_RX_DMA_STREAM,JUDGESYSTEM_PACKSIZE);
		//??????DMA
		DMA_Cmd(USART6_RX_DMA_STREAM, ENABLE);
		
		
		//???????????????????????????
		(void)USART6->DR;
		(void)USART6->SR;
		//???????????????????????????????????????????????????????????????
		//USART_ClearITPendingBit(USART1, USART_IT_IDLE);
	}
}


/**
  * @brief  CAN1 FIFO0 ????????????????????????
  * @param  None
  * @retval None
  */
void CAN1_RX0_IRQHandler(void){
	//????????????
	if(CAN_GetITStatus(CAN1, CAN_IT_FMP0)){
		CanRxMsg RxMessage;
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;		
		
		CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
		xQueueSendFromISR(xCan1RxQueue, &RxMessage, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	}
	
}

/**
  * @brief  CAN2 FIFO0 ????????????????????????
  * @param  None
  * @retval None
  */
void CAN2_RX0_IRQHandler(void){
	//????????????
	if(CAN_GetITStatus(CAN2, CAN_IT_FMP0))
	{
		CanRxMsg RxMessage;	
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;		
		
		CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);
		xQueueSendFromISR(xCan2RxQueue, &RxMessage, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		
    
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
	
	}
}


/**
  * @brief  TIM6 DAC ??????????????????
  * @param  None
  * @retval None
  */
void TIM6_DAC_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
	{
		static int counter = 0;
		
			if(++counter >100){
			LED_GREEN_TOGGLE();
			counter = 0;
		}
		
		TIM_ClearITPendingBit(TIM6 , TIM_IT_Update);  		 
	}		 	
}


/**
  * @brief  TIM7 ??????????????????
  * @param  None
  * @retval None
  */
void TIM7_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET){	
		static int counter = 0;
		
		//??????????????????
		if(++counter >20){
			//DevicesMonitor_update();
			counter = 0;
		}
		
		
		
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);  		 
	}		 	
}


/**
  * @}
  */ 

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
