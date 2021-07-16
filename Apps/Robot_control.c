/**
  ******************************************************************************
  * @file    Robot_control.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   让机器人使能，失能
  ******************************************************************************
  */
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "Robot_control.h"
#include "Command_Task.h"
#include "UpperStructure.h"
#include "Lifting_control.h"
#include "BulletCatcher_control.h"
#include "Cloud_control.h"
#include "Chassis_control.h"

/*------------------G L O B A L - F U N C T I O N S --------------------------*/
/**
  * @brief  机器人使能
  * @param  None
  * @retval None
  */
void Robot_Enable(void)
{		
	//Cloud_setMode(CLOUD_MODE_MEC);
	Lifting_setMode(LIFTING_MODE_POSITION);
	BulletCatcher_setMode(CATCHER_MODE_NORMAL);
	Chassis_setMode(CHASSIS_MODE_NORMAL );

}

/**
  * @brief  机器人失能
  * @param  None
  * @retval None
  */
void Robot_Disability(void)
{
	Chassis_setMode(CHASSIS_MODE_OFF);
	//Cloud_setMode(CLOUD_MODE_OFF);
	Lifting_setMode(LIFTING_MODE_OFF);
	BulletCatcher_setMode(CATCHER_MODE_OFF);

}
/*-----------------------------------FILE OF END------------------------------*/
