/**
  ******************************************************************************
  * @file    M6623_Motor.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   6623无刷电机，配套6623电调驱动应用函数接口
  ******************************************************************************
  */
  
  
#include "M6623_Motor.h"




///**
//  * @brief  设置M6623电机电流值（id号为1~4）
//  * @param  iqyaw 	对应yaw轴电机的电流值，范围-5000~0~5000
//  *			iqpitch	对应pitch轴电机的电流值，范围-5000~0~5000
//  * @retval None
//  */
//void M6623_setCurrent(int16_t iqyaw, int16_t iqpitch, int16_t iq7, int16_t iq8){

//	uint8_t data[8];
//	
//	//数据格式详见说明书P32
//	data[0] = iqyaw >> 8;
//	data[1] = iqyaw;
//	data[2] = iqpitch >> 8;
//	data[3] = iqpitch;
//	data[4] = iq7 >> 8;
//	data[5] = iq7;
//	data[6] = iq8 >> 8;
//	data[7] = iq8;
//	
//	CAN_SendData(CAN1, CAN_ID_STD, M6623_SENDID, data);
//	
//}	

/**
  * @brief  6623校准函数
  * @param  None
  * @retval None
  */
void M6623_calibration(void){

	uint8_t data[8];
	
	//数据格式详见说明书P7
	data[0] = 'c';
	data[1] = 0;
	data[2] = 0;
	data[3] = 0;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = 0;
	
	CAN_SendData(CAN1, CAN_ID_STD, M6623_CALIBRATIONID, data);
	
}	



/**
  * @brief  从CAN报文中获取M6623电机信息
  * @param  RxMessage 	CAN报文接收结构体指针
  * @retval None
  */
void M6623_getInfo(M6623s_t *p_M6623, CanRxMsg RxMessage){
	//报文id确认
//	if((RxMessage.StdId < M6623_READID_START) || (RxMessage.StdId > M6623_READID_END))
//		return;
//	uint32_t StdId;
//	StdId = RxMessage.StdId - M6623_READID_START;
	
	//解包数据，数据格式详见6623电调说明书P8
	p_M6623->realAngle = (uint16_t)(RxMessage.Data[0]<<8 | RxMessage.Data[1]);
	p_M6623->realCurrent = (int16_t)(RxMessage.Data[2]<<8 | RxMessage.Data[3]);
	//info[StdId]->setCurrent = (int16_t)(RxMessage->Data[4]<<8 | RxMessage->Data[5]);
	
	//过零检测
	if(p_M6623->realAngle - p_M6623->lastAngle < -6000){
		p_M6623->turnCount++;
	}
	
	if(p_M6623->lastAngle - p_M6623->realAngle < -6000){
		p_M6623->turnCount--;
	}
	
	p_M6623->lastAngle =  p_M6623->realAngle;
	//总角度
	p_M6623->totalAngle = p_M6623->realAngle + (8192*p_M6623->turnCount);
	//帧率
	p_M6623->infoUpdateFrame++;
	//更新标志
	p_M6623->infoUpdateFlag = 1;
	
}	

/**
  * @brief  清零M3510累积角度
  * #param[out] p_M3508     指向3510的结构体
  * @param[in]  RxMessage 	CAN报文接收结构体
  * @retval int32_t  返回清空前的圈数
  */
int32_t M6623_clearTurnCounter(M6623s_t *p_M6623){
	int16_t tempCount = p_M6623->turnCount;
  p_M6623->lastAngle = p_M6623->totalAngle = p_M6623->realAngle;
  p_M6623->turnCount = 0;
  
	return tempCount;
}


///**
//  * @brief  设定M6623电机的目标电流值
//  * @param  motorName 	电机名字 @ref M6623Name_e
//  *			current		电流值，范围 -16384~0~16384
//  * @retval None
//  */
//void M6623_setTargetCurrent(M6623Name_e motorName, int16_t current){
//	M6623s[motorName].targetCurrent = current;
//}

/**
  * @brief  设定M6623电机的目标角度
  * @param  motorName 	电机名字 @ref M6623Name_e
  *			angle		电流值，范围 0~8191
  * @retval None
  */
void M6623_setTargetAngle(M6623s_t *p_M6623, int32_t angle){
	p_M6623->targetAngle = angle;
}


/**
  * @brief  获取M6623电机的角度pid误差值
  * @param  motorName 	电机名字 @ref M6623Name_e
  * @retval 误差绝对值
  */
//uint16_t M6623_getAngleAbsErr(M6623Name_e motorName){
//	return abs(M6623s[motorName].pid_angle.err);
//	
//}

///**
//  * @brief  设定M6623电机的目标速度
//  * @param  motorName 	电机名字 @ref M6623Name_e
//  *			speed		电流值，范围 -16384~0~16384
//  * @retval None
//  */
//void M6623_setTargetSpeed(M6623Name_e motorName, int16_t speed){
//	M6623s[motorName].targetSpeed = speed;
//}





