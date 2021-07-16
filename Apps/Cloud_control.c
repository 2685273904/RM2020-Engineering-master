/**
  ******************************************************************************
  * @file    Cloud_control.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   云台控制函数接口
  ******************************************************************************
  */

#include "Cloud_control.h"

Cloud_t Cloud;
uint8_t Cloud_GY_IMU_RXBUFF[CLOUD_GY_IMU_BUFFSIZE];
/**
  * @brief  设置M6623电机电流值（id号为5,6），M2006电机电流值（id 7）
  * @param  iqyaw 	对应yaw轴电机的电流值，范围-5000~0~5000
  *			iqpitch	对应pitch轴电机的电流值，范围-5000~0~5000
  *			iq7，拨弹电机电流值，范围-10000~0~10000
  * @retval None
  */
void Cloud_setCurrent(int16_t iqyaw, int16_t iqpitch, int16_t iq7, int16_t iq8){

	uint8_t data[8];
	
	//数据格式详见说明书P32
	data[0] = iqyaw >> 8;
	data[1] = iqyaw;
	data[2] = iqpitch >> 8;
	data[3] = iqpitch;
	data[4] = iq7 >> 8;
	data[5] = iq7;
	data[6] = iq8 >> 8;
	data[7] = iq8;
	
	CAN_SendData(CAN2, CAN_ID_STD, CLOUD_M6623_SENDID, data);//0x1FF，M3510电机ID：1、2
	
}	


/**
  * @brief  云台初始化，配置参数并归位云台
  * @param  None
  * @retval None
  */
void Cloud_Init(void){
	Cloud.LpfAttFactor = Cloud_LpfAttFactor;
  
  M6623_clearTurnCounter(&Cloud.M6623s[0]);//清零累积圈数
  M6623_clearTurnCounter(&Cloud.M6623s[1]);
	
	Cloud.targetYawRaw = Cloud.targetYawLPF = Cloud.M6623s[0].totalAngle;
	Cloud.targetPitchRaw = Cloud.targetPitchLPF = Cloud.M6623s[1].totalAngle;
	Cloud.mode = CLOUD_MODE_LOCK;//
	GY_IMU_Init((uint32_t)Cloud_GY_IMU_RXBUFF, CLOUD_GY_IMU_BUFFSIZE);//串口陀螺仪
	PositionPID_paraReset(&Cloud.M6623s[0].pid_angle, 6.0f, 0.0f, 28.0f, 20000, 0);    /* yaw */
  PositionPID_paraReset(&Cloud.M6623s[1].pid_angle, 6.0f, 0.0f, 28.0f, 20000, 0);    /* pitch */
	
	
}
/**
  * @brief  设置云台模式
  * @param[in]  mode
  * @retval None
  */
void Cloud_setMode(CloudMode_e mode){
	Cloud.mode = mode;
}

/**
  * @brief  获取云台Yaw旋转角度（以中心为0点）
  * @param[in]  None
  * @retval 角度
  */
float Cloud_getYawAngleWithCenter(void){
	return (Cloud.M6623s[0].totalAngle - Cloud_Yaw_Center)/22.75f;
	
}



/**
  * @brief  设置云台变化坐标
  * @param[in]  delta_yaw
  *				      delta_pitch
  * @retval None
  */
void Cloud_setIMUPosForced(float posYaw, float posPitch){
	Cloud.IMUtargetYawLPF = Cloud.IMUtargetYawRaw = posYaw;
	Cloud.IMUtargetPitchLPF = Cloud.IMUtargetPitchRaw = posPitch;
}

/**
  * @brief  设置云台变化坐标
  * @param[in]  delta_yaw
  *				      delta_pitch
  * @retval None
  */
void Cloud_setDeltaPos(float delta_yaw, float delta_pitch){
  if(dr16_data.offLineFlag){
		delta_yaw = delta_pitch = 0.0f;
    return;
	}
  
  switch(Cloud.mode){    
    case CLOUD_MODE_LOCK:
      delta_yaw = delta_pitch = 0.0f;
    break;
    
    case CLOUD_MODE_MEC:
      Cloud.targetYawRaw += -delta_yaw;//emmm根据实际吧搞
      Cloud.targetPitchRaw += -delta_pitch;
      
      //云台限幅
      if(Cloud.targetYawRaw > Cloud_Yaw_Max){
        Cloud.targetYawRaw = Cloud_Yaw_Max;
      }
      if(Cloud.targetYawRaw < Cloud_Yaw_Min){
        Cloud.targetYawRaw = Cloud_Yaw_Min;
      }
        
      if(Cloud.targetPitchRaw > Cloud_Pitch_Max){
        Cloud.targetPitchRaw = Cloud_Pitch_Max;
      }
      if(Cloud.targetPitchRaw < Cloud_Pitch_Min) {
        Cloud.targetPitchRaw = Cloud_Pitch_Min;
      }
    break;
    
    case CLOUD_MODE_IMU://传入参数的大小:机械—>陀螺仪
      /* 360->8191 */
			Cloud.IMUtargetYawRaw += delta_yaw*(360.0f/8191.0f);
      Cloud.IMUtargetPitchRaw -= delta_pitch*(360.0f/8191.0f);
    break;
    
    default:
      delta_yaw = delta_pitch = 0.0f;
    break;
    
    
  }
}

/**
  * @brief  强制设置云台机械坐标（绕过缓冲区）
  * @param[in]  posYaw
  *				posPitch
  * @retval None
  */
void Cloud_setMecPosForced(float posYaw, float posPitch){
	Cloud.targetYawLPF = Cloud.targetYawRaw = posYaw;
	Cloud.targetPitchLPF = Cloud.targetPitchRaw = posPitch;
}


/**
  * @brief  云台控制处理-普通模式
  * @param[in]  delta_yaw		航向角变化量
  *				delta_pitch		俯仰角变化量
  *				shoot	射击使能
  *				mode	模式 - 除Status_ControlOFF外，其他正常控制
  * @retval None
  */
void Cloud_processing(void){
	
	switch(Cloud.mode){
    case CLOUD_MODE_OFF:
      //以当前位置为下次启动位置
      Cloud_setMecPosForced(Cloud.M6623s[0].totalAngle, Cloud.M6623s[1].totalAngle);
      Cloud_setIMUPosForced(Cloud.IMU.totalYaw, Cloud.IMU.eular.roll);//这行没用上
      Cloud.M6623s[0].outCurrent = Cloud.M6623s[1].outCurrent = 0;
      Cloud_setCurrent(Cloud.M6623s[0].outCurrent, Cloud.M6623s[1].outCurrent, 0, 0);
      return;
		 break;
    
    case CLOUD_MODE_LOCK:
    break;
    /* -------------------------机械角度-BEGIN---------------------------------- */
    case CLOUD_MODE_MEC:
     // Cloud_setIMUPosForced(Cloud.IMU.totalYaw, Cloud.IMU.eular.roll);
    

    break;
    /* ---------------------------机械角度-END---------------------------------- */
		
		 default:
      break;
		
	}
	
	//平滑处理begin
  Filter_IIRLPF(&Cloud.targetYawRaw, &Cloud.targetYawLPF, Cloud.LpfAttFactor);
  Filter_IIRLPF(&Cloud.targetPitchRaw, &Cloud.targetPitchLPF, Cloud.LpfAttFactor);
  //平滑处理end
  
  M6623_setTargetAngle(&Cloud.M6623s[0], Cloud.targetYawLPF);
  M6623_setTargetAngle(&Cloud.M6623s[1], Cloud.targetPitchLPF);
	
	 if(!Cloud.M6623s[0].offLineFlag){
		  Cloud.M6623s[0].outCurrent = Position_PID(&Cloud.M6623s[0].pid_angle, Cloud.M6623s[0].targetAngle, Cloud.M6623s[0].totalAngle);
		   Cloud.M6623s[0].infoUpdateFlag = 0;
	 }
	 
	 if(!Cloud.M6623s[1].offLineFlag){
		  /* 角度PID计算(外环) */
      Cloud.M6623s[1].outCurrent = Position_PID(&Cloud.M6623s[1].pid_angle, Cloud.M6623s[1].targetAngle, Cloud.M6623s[1].totalAngle);
		   /* 清标志位 */
      Cloud.M6623s[1].infoUpdateFlag = 0;
	 } 
		 Cloud_setCurrent(Cloud.M6623s[0].outCurrent, Cloud.M6623s[1].outCurrent, 0, 0);
	
}

