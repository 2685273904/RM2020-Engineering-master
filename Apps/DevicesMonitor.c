/**
  ******************************************************************************
  * @file    DevicesMonitor.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   外部设备帧率检测函数接口
  ******************************************************************************
  */
  
  
#include "DevicesMonitor.h"
#include "DR16_Receiver.h"
#include "GY_IMU.h"
#include "Chassis_control.h"
#include "Cloud_control.h"
#include "Lifting_control.h"
#include "BulletCatcher_control.h"

/**
  * @brief  外接设备帧率清零，每200ms清零一次
  * @param  None
  * @retval None
  */
void DevicesMonitor_update(void){

//	//云台IMU
//	if(Cloud.IMU.infoUpdateFrame < 5){
//		Cloud.IMU.offLineFlag = 1;
//	}
//	else{
//		Cloud.IMU.offLineFlag = 0;
//	}
//	Cloud.IMU.infoUpdateFrame = 0;
//	
	//底盘M3508
	for(int i = 0; i<4; i++){
		if(Chassis.M3508s[i].infoUpdateFrame < 5){
			Chassis.M3508s[i].offLineFlag = 1;
		}
		else{
			Chassis.M3508s[i].offLineFlag = 0;
		}
		Chassis.M3508s[i].infoUpdateFrame = 0;
	}
	
	//抬升M3508
	for(int i = 0; i<2; i++){
		if(Lifting.M3508s[i].infoUpdateFrame < 5){
			Lifting.M3508s[i].offLineFlag = 1;
		}
		else{
			Lifting.M3508s[i].offLineFlag = 0;
		}
		Lifting.M3508s[i].infoUpdateFrame = 0;
	}
	
	//夹取M3508
	for(int i = 0; i<2; i++){
		if(BulletCatcher.M3508s[i].infoUpdateFrame < 5){
			BulletCatcher.M3508s[i].offLineFlag = 1;
		}
		else{
			BulletCatcher.M3508s[i].offLineFlag = 0;
		}
		BulletCatcher.M3508s[i].infoUpdateFrame = 0;
	}	
	
	//M3510
	for(int i = 0; i<2; i++){
		if(Cloud.M6623s[i].infoUpdateFrame < 5){
			Cloud.M6623s[i].offLineFlag = 1;
		}
		else{
			Cloud.M6623s[i].offLineFlag = 0;
		}
		Cloud.M6623s[i].infoUpdateFrame = 0;
	}
	
	
	//遥控器
	if(dr16_data.infoUpdateFrame < 5){
		dr16_data.offLineFlag = 1;
		LED_GREEN(0);
	}
	else{
		dr16_data.offLineFlag = 0;
	}
	dr16_data.infoUpdateFrame = 0;
	
	
	//裁判系统
	if(JudgeSYS.infoUpdateFrame < 1){
		JudgeSYS.offLineFlag = 1;
	}
	else{
		JudgeSYS.offLineFlag = 0;
	}
	JudgeSYS.infoUpdateFrame = 0;
	
}
