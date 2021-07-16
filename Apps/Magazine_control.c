/**
  ******************************************************************************
  * @file    Magazine_control.c
  * @author  LXY
  * @version V1.0
  * @date    2020/01/13
  * @brief   С����
  ******************************************************************************
  */
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "Magazine_control.h"
#include "PlayRoom_Task.h"
/*-----------L O C A L - F U N C T I O N S - P R O T O T Y P E S--------------*/
Magazine_t Magazine;
/*-------------------------- D E F I N E S -----------------------------------*/
#define MAGAZINE_USING_GASID             1     /*ID:2*/
#define MAGAZINE_REACH_GASPOS        GAS_POS_2 /*С�������������*/
#define MAGAZINE_OPEN_GASPOS         GAS_POS_8 /*С���ֲ��ſ��ص�����*/

/*------------------G L O B A L - F U N C T I O N S --------------------------*/
/**
  * @brief  ��С���赯�֣�������Ƶ�ĳ�500ms���Զ��򿪲��ţ�
  * @param  None
  * @retval None
  */
void Magazine_Open(void)
{
	static uint8_t  open_NowStep = 0;
		
	if(Magazine.open){
	 switch(open_NowStep)
	 {
		 case 0:
			 /*��С�������*/
			 GAS8_setBits(MAGAZINE_USING_GASID, MAGAZINE_REACH_GASPOS);
				open_NowStep++;
		 break;
		 
		 case 1:
			 Magazine.timeCounter++;
		 if( Magazine.timeCounter>60){
			 open_NowStep++;
			 Magazine.timeCounter=0;
		 }
		 break;
		 
		 case 2:
//			 if(Magazine.nextReq){
		 /*��С���ղ���*/
			 GAS8_setBits(MAGAZINE_USING_GASID, MAGAZINE_OPEN_GASPOS);
			 Magazine.open=0;
			 open_NowStep=0;
			 Magazine.timeCounter=0;
			 Magazine.nextReq=0;
//			 }
		 break;
	  }
	}
}

/**
  * @brief  �ر�С���赯��
  * @param  None
  * @retval None
  */
 void Magazine_Close(void)
 {
	  static uint8_t  close_NowStep = 0;

		if(Magazine.close){
		switch(close_NowStep)
		{
			case 0:
				/*�ر�С���ղ���*/
				GAS8_resetBits(MAGAZINE_USING_GASID, MAGAZINE_OPEN_GASPOS);
		    close_NowStep++;
			break;

			case 1:
				Magazine.timeCounter++;
			if(Magazine.timeCounter>10){
				/*�ر�С�������*/
				GAS8_resetBits(MAGAZINE_USING_GASID, MAGAZINE_REACH_GASPOS);
				close_NowStep++;
				Magazine.timeCounter=0;
			}
			break;

			case 2:
				Magazine.timeCounter++;
			if(Magazine.timeCounter>10){
				Magazine.close=0;
				close_NowStep=0;
				Magazine.timeCounter=0;
				vTaskSuspend(xHandleTaskPlayRoom);
		  }
			
		 break;

		}
 }
}
 
/**
  * @brief  С�������к���
  * @param  None
  * @retval None
  */
void Magazine_processing(void)
{
	Magazine_Close();
	Magazine_Open();
	
}
/*-----------------------------------FILE OF END------------------------------*/
