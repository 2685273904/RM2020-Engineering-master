/**
  ******************************************************************************
  * @file    DynamixelServo.c
  * @author  Hare
  * @version V1.0
  * @date    
  * @brief   MX64-AT���ֶ�������ӿ�
  ******************************************************************************
  */
#include "DynamixelServo.h"
#include <stdarg.h> 
 

void DYMX64AT_Init(void){
  USART2_QuickInit(1000000);
  
}


/**
	* @Data    2019-01-10 14:07
	* @brief   ͬ��д����Ŀ��Ƕȣ�������Ŀ���ޡ�(Ŀ��Ƕȳ���Ϊ 2 Byte)
	* @param   Num : ����������������Ƕ����ID��Ŀ��Ƕ�
	*          ... :�����ID��Ŀ��Ƕ�
	* @retval  void
	*/
void DYMX64AT_setSyncTarAng(uint8_t Num,...)
{
	/***Length = ((L + 1) * N) + 4,	L:Data Length,N:Number of Dynamixel***/
	uint8_t 		Length=3*Num+4;
	uint16_t    crc = 0;
	uint8_t 		sendBuff[Length+4];
	uint16_t		dataBuff[2*Num];/*�����ڴ棬��Ŷ����Ŀ��Ƕ�*/
	
	va_list args;
	va_start(args, Num);/*argsָ���������б��еĵ�һ����ѡ����Num*/
	
	for(int i=0;i<2*Num;i++)/*��ȡID��Ŀ��Ƕ�*/
	{
		dataBuff[i]=va_arg(args,int);
	}
	
	sendBuff[0]=0xFF;
	sendBuff[1]=0xFF;
	sendBuff[2]=0xFE;/*���ж��,�㲥ģʽ*/
	sendBuff[3]=Length;
	sendBuff[4]=DYNAMIXELSERVO_SYNC_WRITE;/*ͬ��д������*/
  sendBuff[5]=0x1E;/*λַ*/
	sendBuff[6]=0x02;/*�������ݳ��ȣ�Ŀ��Ƕȳ���Ϊ 2 Byte*/
	
	for(int i=0;i<Num;i++)/*���ID��Ŀ��Ƕ�*/
	{
		sendBuff[7+3*i]=dataBuff[2*i];
		sendBuff[8+3*i]=dataBuff[2*i+1];
		sendBuff[9+3*i]=dataBuff[2*i+1]>>8;
	}
	
	for(int i=0;i<Length+1;i++)/*У���*/
	{
		crc+=sendBuff[i+2];
	}

	sendBuff[Length+3]=~crc;
	
	for(int i = 0; i < Length+4; i++){
    USART_sendChar(USART2, sendBuff[i]);
    
  }
   
}

void DYMX64AT_setRebootMsg(uint8_t ID)//�������
{
 uint8_t sendBuff[9];
 
  uint16_t crc=0;

 
  sendBuff[0]=0xFF;/*֡ͷ*/
  sendBuff[1]=0xFF;
  sendBuff[2]=ID;/*ID*/
  sendBuff[3]=0x02;/*�����ݳ���*/
 
  sendBuff[4]=DYNAMIXELSERVO_REBOOT;//0x08ָ��

  for(int i=0;i<3;i++)
  {
    crc+=sendBuff[i+2];
  }
  sendBuff[5]=~crc;

  
  for(int i = 0; i < 6; i++){
    USART_sendChar(USART2, sendBuff[i]);
    
  }
   
}
