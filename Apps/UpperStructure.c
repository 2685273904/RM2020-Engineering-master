/**
  ******************************************************************************
  * @file    UpperStructure.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   上层结构控制（抬升+夹取）
  ******************************************************************************
  */

/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "UpperStructure.h"
#include "Lifting_control.h"
#include "BulletCatcher_control.h"
 #include "Magazine_control.h"
 #include "DynamixelServo.h"
#include "Hall_IO.h"
/*-----------L O C A L - F U N C T I O N S - P R O T O T Y P E S--------------*/

UpperStructrue_t UpperStructrue;
/*------------------G L O B A L - F U N C T I O N S --------------------------*/
void UpperStructure_Init(void){
	Lifting_Init();
	BulletCatcher_Init();
 
}




/**
* @brief  上层结构控制任务
  * @param  pvParameters 任务被创建时传进来的参数
  * @retval None
  */
void UpperStructure_processing(void)
{
	Lifting_processing();
	
	BulletCatcher_processing();
	BulletCatcher_move();

	BulletCatcher_threeProcessing();
	BulletCatcher_lootProcessing();
	
	UpperStructure_M3508executeAngle();
	
//		Magazine_processing();
//	BulletCatcher_writeBank();

}

/**
* @brief  关闭上层任务
  * @param  None
  * @retval None
  */
uint8_t UpperStructure_close(void){
  
	if(BulletCatcher_close()){
  uint8_t isDown = Lifting_setPosition(Lifting.minPos); 
  if(isDown||Lifting.DownLimit){
    
    UpperStructrue.isOpen = 0;//
    UpperStructrue.nowtep = 0;
    UpperStructrue.openReq = 0;//
  }
}
  return 0;//寒假把isDown改成了0#
}

/**
* @brief  开启上层任务
  * @param  pos  抬升上升的高度
  * @retval None
  */
uint8_t UpperStructure_open(int32_t pos){
	
	
  if(UpperStructrue.nowtep < 1&&BulletCatcher_open()){//这个先把夹取弄成准备模式再升上去的
    UpperStructrue.nowtep++;//。。。
    UpperStructrue.openReq = 1;//
  }
	else{
  uint8_t isUpper = Lifting_setPosition(pos);
  if(isUpper || Lifting.UpLimit){
    UpperStructrue.isOpen = 1;//上层任务成功打开
    UpperStructrue.openReq = 0;
    UpperStructrue.nowtep = 0;
  }
 }
  return 0;
}

/**
  * @brief 使夹子和抬升上的M3508转到指定位置（要修改）
  * @param[in]  Angle   目标角度
  * @retval 
  */
void UpperStructure_M3508executeAngle(void)
{

	BulletCatcher_setM3508Current(BulletCatcher.M3508s[0].outCurrent,BulletCatcher.M3508s[1].outCurrent,\
	0, 0);
 
	Lift_setM3508Current(0, 0,Lifting.M3508s[0].outCurrent, Lifting.M3508s[1].outCurrent);
}



/*-----------------------------------FILE OF END------------------------------*/
