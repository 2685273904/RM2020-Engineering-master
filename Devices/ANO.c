/**
  ******************************************************************************
  * @file    ANO.c
  * @author  lxy
  * @version V1.0
  * @date    
  * @brief   匿名上位机
  ******************************************************************************
  */
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "ANO.h"
/*-----------L O C A L - F U N C T I O N S - P R O T O T Y P E S--------------*/
int8_t send_buf[15]= {0};
int8_t send_bufs[30]= {0};
int16_t Temp_Target[8] = {0};
/*------------------G L O B A L - F U N C T I O N S --------------------------*/
void ANO_Send_Data_Init(int16_t Target1,int16_t Target2,int16_t Target3,int16_t Target4,\
	int16_t Target5,int16_t Target6,int16_t Target7,int16_t Target8)
{
  Temp_Target[0] = Target1;
  Temp_Target[1] = Target2;
  Temp_Target[2] = Target3;
  Temp_Target[3] = Target4;
  Temp_Target[4] = Target5;
  Temp_Target[5] = Target6;
  Temp_Target[6] = Target7;
  Temp_Target[7] = Target8;

}


//void ANO_Send_Data_V3(int16_t Temp_Target1,int16_t Temp_Now1,int16_t Temp_Target2,int16_t Temp_Now2)
//{
//  int8_t data_sum=0;
//	int i=0,cout=0;
//	send_buf[cout++]=0xAA;                   //0
//	send_buf[cout++]=0x01;                   //1
//	send_buf[cout++]=0xAF;                   //2
//	send_buf[cout++]=0xF1;                   //3
//	send_buf[cout++]=0;                      //4
//	send_buf[cout++]=Temp_Target1>>8;        //5
//	send_buf[cout++]=Temp_Target1;           //6
//	send_buf[cout++]=Temp_Now1>>8;           //7
//	send_buf[cout++]=Temp_Now1;              //8
//	send_buf[cout++]=Temp_Target2>>8;        //9
//	send_buf[cout++]=Temp_Target2;           //10
//	send_buf[cout++]=Temp_Now2>>8;           //11
//	send_buf[cout++]=Temp_Now2;              //12
//	send_buf[4]=cout-5;
//	for(i=0;i<cout;i++)
//	{
//	data_sum+=send_buf[i];
//	}
//	send_buf[cout++]=data_sum;
//	
//	for(i=0;i<cout;i++)
//	{
//	USART_sendChar(USART6,send_buf[i]);
//	}
//}


/*-----------------------------------FILE OF END------------------------------*/

