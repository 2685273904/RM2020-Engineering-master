/**
  ******************************************************************************
  * @file    CAN.c
  * @author  Hare
  * @version V1.0
  * @date    2019-01-15
  * @brief   CAN应用函数接口
  ******************************************************************************
  */
  
  
#include "CAN.h"
#include "CanMsg_Task.h"
/**
  * @brief  CAN1快速初始化
  * @param  None
  * @retval None
  */
void CAN1_QuickInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	
	/* 使能 GPIO 时钟 */
	RCC_AHB1PeriphClockCmd(CAN1_RX_GPIO_CLK | CAN1_TX_GPIO_CLK, ENABLE);
	//使能CAN1时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	/* GPIO初始化 */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	
	/* 配置Tx引脚  */
	GPIO_InitStructure.GPIO_Pin = CAN1_TX_Pin;  
	GPIO_Init(CAN1_TX_GPIO_PORT, &GPIO_InitStructure);

	/* 配置Rx引脚 */
	GPIO_InitStructure.GPIO_Pin = CAN1_RX_Pin;
	GPIO_Init(CAN1_RX_GPIO_PORT, &GPIO_InitStructure);
	
	/* 连接 IO 到 CAN1_Tx*/
	GPIO_PinAFConfig(CAN1_TX_GPIO_PORT, CAN1_TX_PINSOURCE, GPIO_AF_CAN1);

	/*  连接 IO 到 CAN1_Rx*/
	GPIO_PinAFConfig(CAN1_RX_GPIO_PORT, CAN1_RX_PINSOURCE, GPIO_AF_CAN1);
	
	/************************CAN 模式配置*********************************************/
	CAN_InitStructure.CAN_TTCM = DISABLE;			   //MCR-TTCM  时间触发通信模式
	CAN_InitStructure.CAN_ABOM = ENABLE;			   //MCR-ABOM  自动离线管理 
	CAN_InitStructure.CAN_AWUM = ENABLE;			   //MCR-AWUM  使用自动唤醒模式
	CAN_InitStructure.CAN_NART = DISABLE;			   //MCR-NART  禁止报文自动重传	  DISABLE-自动重传
	CAN_InitStructure.CAN_RFLM = DISABLE;			   //MCR-RFLM  接收FIFO 锁定模式  DISABLE-溢出时新报文会覆盖原有报文  
	CAN_InitStructure.CAN_TXFP = DISABLE;			   //MCR-TXFP  发送FIFO优先级 DISABLE-优先级取决于报文标示符
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;  	   //Mode  工作模式
	
	CAN_InitStructure.CAN_SJW = CAN1_SJW;		   	   //BTR-SJW 重新同步跳跃宽度 2个时间单元
	
	/* ss=1 bs1=5 bs2=3 位时间宽度为(1+3+5) 波特率即为时钟周期tq*(1+3+5)  */
	CAN_InitStructure.CAN_BS1 = CAN1_BS1;		   //BTR-TS1 时间段1 占用了6个时间单元
	CAN_InitStructure.CAN_BS2 = CAN1_BS2;		   //BTR-TS1 时间段2 占用了3个时间单元	
	
	/* CAN Baudrate = 1 MBps (1MBps已为stm32的CAN最高速率) (CAN 时钟频率为 APB 1 = 45 MHz) */
	CAN_InitStructure.CAN_Prescaler = CAN1_Prescaler;		   ////BTR-BRP 波特率分频器  定义了时间单元的时间长度 45/(1+3+5)/5=1 Mbps
	CAN_Init(CAN1, &CAN_InitStructure);
	

	/*********************CAN筛选器初始化***********************************************/
	CAN_FilterInitStructure.CAN_FilterNumber = 0;									//筛选器组0
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;					//工作在ID掩码模式
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;				//筛选器位宽为单个32位。
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0 ;			//筛选器被关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;			//使能筛选器
	/* 使能筛选器，按照标志的内容进行比对筛选，扩展ID不是如下的就抛弃掉，是的话，会存入FIFO0。 */
	
	//不进行过滤
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;				//要筛选的ID高位 
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000; 				//要筛选的ID低位 
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;			//筛选器高16位每位不须匹配
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;			//筛选器低16位每位不须匹配
	
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	NVIC_Config(CAN1_RX0_IRQn, CAN1_MAIN_PRIORITY, CAN1_SUB_PRIORITY);
	/* FIFO0 message pending interrupt */
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

/**
  * @brief  CAN2快速初始化
  * @param  None
  * @retval None
  */
void CAN2_QuickInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	
	/* 使能 GPIO 时钟 */
	RCC_AHB1PeriphClockCmd(CAN2_RX_GPIO_CLK | CAN2_TX_GPIO_CLK, ENABLE);
	//使能CAN2时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
	
	/* GPIO初始化 */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	
	/* 配置Tx引脚  */
	GPIO_InitStructure.GPIO_Pin = CAN2_TX_Pin;  
	GPIO_Init(CAN2_TX_GPIO_PORT, &GPIO_InitStructure);

	/* 配置Rx引脚 */
	GPIO_InitStructure.GPIO_Pin = CAN2_RX_Pin;
	GPIO_Init(CAN2_RX_GPIO_PORT, &GPIO_InitStructure);
	
	/* 连接 IO 到 CAN2_Tx*/
	GPIO_PinAFConfig(CAN2_TX_GPIO_PORT, CAN2_TX_PINSOURCE, GPIO_AF_CAN2);

	/*  连接 IO 到 CAN2_Rx*/
	GPIO_PinAFConfig(CAN2_RX_GPIO_PORT, CAN2_RX_PINSOURCE, GPIO_AF_CAN2);
	
	/************************CAN 模式配置*********************************************/
	CAN_InitStructure.CAN_TTCM = DISABLE;			   //MCR-TTCM  时间触发通信模式
	CAN_InitStructure.CAN_ABOM = ENABLE;			   //MCR-ABOM  自动离线管理 
	CAN_InitStructure.CAN_AWUM = ENABLE;			   //MCR-AWUM  使用自动唤醒模式
	CAN_InitStructure.CAN_NART = DISABLE;			   //MCR-NART  禁止报文自动重传	  DISABLE-自动重传
	CAN_InitStructure.CAN_RFLM = DISABLE;			   //MCR-RFLM  接收FIFO 锁定模式  DISABLE-溢出时新报文会覆盖原有报文  
	CAN_InitStructure.CAN_TXFP = DISABLE;			   //MCR-TXFP  发送FIFO优先级 DISABLE-优先级取决于报文标示符
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;  	   //Mode  工作模式
	
	CAN_InitStructure.CAN_SJW = CAN2_SJW;		   	   //BTR-SJW 重新同步跳跃宽度 2个时间单元
	
	/* ss=1 bs1=5 bs2=3 位时间宽度为(1+9+4) 波特率即为时钟周期tq*(1+9+4)  */
	CAN_InitStructure.CAN_BS1 = CAN2_BS1;		   //BTR-TS1 时间段1 占用了6个时间单元
	CAN_InitStructure.CAN_BS2 = CAN2_BS2;		   //BTR-TS1 时间段2 占用了3个时间单元	
	
	/* CAN Baudrate = 1 MBps (1MBps已为stm32的CAN最高速率) (CAN 时钟频率为 APB 1 = 42 MHz) */
	CAN_InitStructure.CAN_Prescaler = CAN2_Prescaler;		   ////BTR-BRP 波特率分频器  定义了时间单元的时间长度 42/(1+9+4)/3=1 Mbps
	CAN_Init(CAN2, &CAN_InitStructure);
	

	/*********************CAN筛选器初始化***********************************************/
	CAN_FilterInitStructure.CAN_FilterNumber = 14;									//筛选器组0
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;					//工作在ID掩码模式
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;				//筛选器位宽为单个32位。
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;			//筛选器被关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;			//使能筛选器
	/* 使能筛选器，按照标志的内容进行比对筛选，扩展ID不是如下的就抛弃掉，是的话，会存入FIFO0。 */
	
	//不进行过滤
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;				//要筛选的ID高位 
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000; 				//要筛选的ID低位 
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;			//筛选器高16位每位不须匹配
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;			//筛选器低16位每位不须匹配
	
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	NVIC_Config(CAN2_RX0_IRQn, CAN2_MAIN_PRIORITY, CAN2_SUB_PRIORITY);
	/* FIFO0 message pending interrupt */
	CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);
}

	 
/**
  * @brief  CAN发送数据
  * @param  CANx 		CAN编号
  * 		id_type ·	id类型 CAN_ID_STD， CAN_ID_EXT
  *			id			id号
  * 		data[8]		8个数据
  * @retval None
  */
void CAN_SendData(CAN_TypeDef* CANx, uint8_t id_type, uint32_t id, uint8_t data[8])
{	  
	CanSend_t canSendData;
	//CanTxMsg TxMessage;
	
	if(CANx==CAN1)
	{
		canSendData.CANx=1;
	}
	else if(CANx==CAN2)
	{
		canSendData.CANx=2;
	}
	
	
	if(id_type == CAN_Id_Standard){
		canSendData.SendCanTxMsg.StdId = id;						 
	}
	else{
		canSendData.SendCanTxMsg.ExtId = id;					 //ID号
	}
	
	canSendData.SendCanTxMsg.IDE = id_type;					 //ID类型
	canSendData.SendCanTxMsg.RTR = CAN_RTR_DATA;				 //发送的为数据
	canSendData.SendCanTxMsg.DLC = 0x08;						 //数据长度为8字节
	
	/*设置要发送的数据*/
	for (uint8_t i = 0; i < 8; i++)
	{
		canSendData.SendCanTxMsg.Data[i] = data[i];
	}
  
//	CAN_Transmit(CANx, &TxMessage);
	xQueueSend(xCanSendQueue, &canSendData, 20);
}


