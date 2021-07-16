/**
  ******************************************************************************
  * @file    UpperStructure.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   �ϲ�ṹ���ƣ�̧��+��ȡ��
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
* @brief  �ϲ�ṹ��������
  * @param  pvParameters ���񱻴���ʱ�������Ĳ���
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
* @brief  �ر��ϲ�����
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
  return 0;//���ٰ�isDown�ĳ���0#
}

/**
* @brief  �����ϲ�����
  * @param  pos  ̧�������ĸ߶�
  * @retval None
  */
uint8_t UpperStructure_open(int32_t pos){
	
	
  if(UpperStructrue.nowtep < 1&&BulletCatcher_open()){//����ȰѼ�ȡŪ��׼��ģʽ������ȥ��
    UpperStructrue.nowtep++;//������
    UpperStructrue.openReq = 1;//
  }
	else{
  uint8_t isUpper = Lifting_setPosition(pos);
  if(isUpper || Lifting.UpLimit){
    UpperStructrue.isOpen = 1;//�ϲ�����ɹ���
    UpperStructrue.openReq = 0;
    UpperStructrue.nowtep = 0;
  }
 }
  return 0;
}

/**
  * @brief ʹ���Ӻ�̧���ϵ�M3508ת��ָ��λ�ã�Ҫ�޸ģ�
  * @param[in]  Angle   Ŀ��Ƕ�
  * @retval 
  */
void UpperStructure_M3508executeAngle(void)
{

	BulletCatcher_setM3508Current(BulletCatcher.M3508s[0].outCurrent,BulletCatcher.M3508s[1].outCurrent,\
	0, 0);
 
	Lift_setM3508Current(0, 0,Lifting.M3508s[0].outCurrent, Lifting.M3508s[1].outCurrent);
}



/*-----------------------------------FILE OF END------------------------------*/
