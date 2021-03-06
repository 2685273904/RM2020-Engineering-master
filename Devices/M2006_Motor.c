/**
  ******************************************************************************
  * @file    M2006_Motor.c
  * @author  Hare
  * @version V1.0
  * @date    
  * @brief   M2006无刷电机(拨弹用)，配套C610电调驱动应用函数接口
  ******************************************************************************
  */
  
  
#include "M2006_Motor.h"

M2006s_t M2006s[1] = {{.targetAngle = M2006_FIRSTANGLE}};


/**
  * @brief  设置M2006电机电流值（id号为7）M2006与6623共用发送函数
  * @param  iqx (x:5) 对应id号电机的电流值，范围-10000~0~10000
  * @retval None
  */
//void M2006_setCurrent(int16_t iq3){

//	uint8_t data[8];
//	
//	//数据格式详见说明书P32
////	data[0] = iq1 >> 8;
////	data[1] = iq1;
////	data[2] = iq2 >> 8;
////	data[3] = iq2;
//	data[4] = iq3 >> 8;
//	data[5] = iq3;
////	data[6] = iq4 >> 8;
////	data[7] = iq4;
//	
//	CAN_SendData(CAN1, CAN_ID_STD, M2006_SENDID, data);
//	
//}	


/**
  * @brief  从CAN报文中获取M2006电机信息
  * @param[in]  RxMessage 	CAN报文接收结构体
  * @retval None
  */
void M2006_getInfo(CanRxMsg RxMessage){
	//报文id确认
	if((RxMessage.StdId < M2006_READID_START) || (RxMessage.StdId > M2006_READID_END))
		return;
	uint32_t StdId;
	StdId = RxMessage.StdId - M2006_READID_START;
	
	//解包数据，数据格式详见C610电调说明书P9
	M2006s[StdId].realAngle = (uint16_t)(RxMessage.Data[0]<<8 | RxMessage.Data[1]);
	M2006s[StdId].realSpeed = (int16_t)(RxMessage.Data[2]<<8 | RxMessage.Data[3]);
	M2006s[StdId].realTorque = (int16_t)(RxMessage.Data[4]<<8 | RxMessage.Data[5]);
	
	if(M2006s[StdId].realAngle - M2006s[StdId].lastAngle < -6000){
		M2006s[StdId].turnCount++;
	}
	
	if(M2006s[StdId].lastAngle - M2006s[StdId].realAngle < -6000){
		M2006s[StdId].turnCount--;
	}
	
	M2006s[StdId].lastAngle =  M2006s[StdId].realAngle;
	
	M2006s[StdId].totalAngle = M2006s[StdId].realAngle + (8192*M2006s[StdId].turnCount);
	
	M2006s[StdId].infoUpdateFrame++;
	M2006s[StdId].infoUpdateFlag = 1;
}	


///**
//  * @brief  设定M3508电机的目标电流值
//  * @param  motorName 	电机名字 @ref M3508Name_e
//  *			current		电流值，范围 -16384~0~16384
//  * @retval None
//  */
//inline void M3508_setTargetCurrent(M3508Name_e motorName, int16_t current){
//	M3508s[motorName].targetCurrent = current;
//}

/**
  * @brief  设定M2006电机的目标角度
  * @param  motorName 	电机名字 @ref M2006Name_e
  *			angle		电流值，范围 0~8191
  * @retval None
  */
void M2006_setTargetAngle(M2006Name_e motorName, int32_t angle){
	M2006s[motorName].targetAngle = angle;
}

/**
  * @brief  设定M3508电机的目标速度
  * @param  motorName 	电机名字 @ref M3508Name_e
  *			speed		电流值，范围 -10000~0~10000
  * @retval None
  */
void M2006_setTargetSpeed(M2006Name_e motorName, int16_t speed){
	M2006s[motorName].targetSpeed = speed;
}



