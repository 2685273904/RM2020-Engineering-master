/**
  ******************************************************************************
  * @file    Mutually_control.c
  * @author  LXY
  * @version V1.0
  * @date    2020/01/13
  * @brief   场地交互救援
  ******************************************************************************
  */
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "Mutually_control.h"
/*-----------L O C A L - F U N C T I O N S - P R O T O T Y P E S--------------*/
Mutually_t Mutually;
/*-------------------------- D E F I N E S -----------------------------------*/
#define MUTUALLY_USING_GASID             1      /*ID:2*/
#define MUTUALLY_DOWN_GASPOS          GAS_POS_3 /*向下的气阀*/
#define MUTUALLY_REACH_GASPOS         GAS_POS_6 /*伸出的气阀*/

/*------------------G L O B A L - F U N C T I O N S --------------------------*/
/**
  * @brief  开启交互救援
  * @param  None
  * @retval None
  */
void Mutually_start(void)
{
	static uint8_t  open_NowStep = 0;

	if(Mutually.open){
		switch(open_NowStep){
				case 0:
				 GAS8_setBits(MUTUALLY_USING_GASID, MUTUALLY_DOWN_GASPOS);
					open_NowStep++;
			 break;
			 
			 case 1:
				 Mutually.timeCounter++;
			 if( Mutually.timeCounter>10){
				 open_NowStep++;
				 Mutually.timeCounter=0;
				 GAS8_setBits(MUTUALLY_USING_GASID, MUTUALLY_REACH_GASPOS);
			 }
			 break;
			 
			 case 2:
				Mutually.timeCounter++;
			 if( Mutually.timeCounter>10){
			 Mutually.timeCounter=0;
			 Mutually.open=0;
			 open_NowStep=0;
			 }	 
			 break;
			 
		}
 }
}


/**
  * @brief  关闭交互救援
  * @param  None
  * @retval None
  */
void Mutually_close(void)
{
	static uint8_t  close_NowStep = 0;

		if(Mutually.close){
		switch(close_NowStep)
		{
			case 0:
				GAS8_resetBits(MUTUALLY_USING_GASID, MUTUALLY_REACH_GASPOS);
		    close_NowStep++;
			break;

			case 1:
				Mutually.timeCounter++;
				if(Mutually.timeCounter>10){
					GAS8_resetBits(MUTUALLY_USING_GASID, MUTUALLY_DOWN_GASPOS);
					close_NowStep++;
					Mutually.timeCounter=0;
				}
			break;

			case 2:
				Mutually.timeCounter++;
				if(Mutually.timeCounter>10){
					Mutually.close=0;
					close_NowStep=0;
					Mutually.timeCounter=0;
				}
		 break;

		}
  }
}
/*-----------------------------------FILE OF END------------------------------*/

