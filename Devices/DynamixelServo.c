/**
  ******************************************************************************
  * @file    DynamixelServo.c
  * @author  Hare
  * @version V1.0
  * @date    
  * @brief   MX64-AT数字舵机驱动接口
  ******************************************************************************
  */
#include "DynamixelServo.h"
#include <stdarg.h> 
 

void DYMX64AT_Init(void){
  USART2_QuickInit(1000000);
  
}


/**
	* @Data    2019-01-10 14:07
	* @brief   同步写入舵机目标角度，参数数目不限。(目标角度长度为 2 Byte)
	* @param   Num : 舵机的数量，后面是舵机的ID和目标角度
	*          ... :舵机的ID和目标角度
	* @retval  void
	*/
void DYMX64AT_setSyncTarAng(uint8_t Num,...)
{
	/***Length = ((L + 1) * N) + 4,	L:Data Length,N:Number of Dynamixel***/
	uint8_t 		Length=3*Num+4;
	uint16_t    crc = 0;
	uint8_t 		sendBuff[Length+4];
	uint16_t		dataBuff[2*Num];/*申请内存，存放舵机与目标角度*/
	
	va_list args;
	va_start(args, Num);/*args指向函数参数列表中的第一个可选参数Num*/
	
	for(int i=0;i<2*Num;i++)/*获取ID与目标角度*/
	{
		dataBuff[i]=va_arg(args,int);
	}
	
	sendBuff[0]=0xFF;
	sendBuff[1]=0xFF;
	sendBuff[2]=0xFE;/*所有舵机,广播模式*/
	sendBuff[3]=Length;
	sendBuff[4]=DYNAMIXELSERVO_SYNC_WRITE;/*同步写入命令*/
  sendBuff[5]=0x1E;/*位址*/
	sendBuff[6]=0x02;/*单个数据长度，目标角度长度为 2 Byte*/
	
	for(int i=0;i<Num;i++)/*打包ID与目标角度*/
	{
		sendBuff[7+3*i]=dataBuff[2*i];
		sendBuff[8+3*i]=dataBuff[2*i+1];
		sendBuff[9+3*i]=dataBuff[2*i+1]>>8;
	}
	
	for(int i=0;i<Length+1;i++)/*校验和*/
	{
		crc+=sendBuff[i+2];
	}

	sendBuff[Length+3]=~crc;
	
	for(int i = 0; i < Length+4; i++){
    USART_sendChar(USART2, sendBuff[i]);
    
  }
   
}

void DYMX64AT_setRebootMsg(uint8_t ID)//重启舵机
{
 uint8_t sendBuff[9];
 
  uint16_t crc=0;

 
  sendBuff[0]=0xFF;/*帧头*/
  sendBuff[1]=0xFF;
  sendBuff[2]=ID;/*ID*/
  sendBuff[3]=0x02;/*总数据长度*/
 
  sendBuff[4]=DYNAMIXELSERVO_REBOOT;//0x08指令

  for(int i=0;i<3;i++)
  {
    crc+=sendBuff[i+2];
  }
  sendBuff[5]=~crc;

  
  for(int i = 0; i < 6; i++){
    USART_sendChar(USART2, sendBuff[i]);
    
  }
   
}
