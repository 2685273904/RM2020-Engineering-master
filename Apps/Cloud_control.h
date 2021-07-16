#ifndef __CLOUD_CONTROL_H
#define __CLOUD_CONTROL_H

#include "user_common.h"
#include "M6623_Motor.h"
#include "GY_IMU.h"

#define Cloud_Mode_Normal		0
#define Cloud_Mode_WithIMU		1

/**
 *      图传
 *   ------------
 *        | 
 *       ( ) pitch 6
 *      -----
 *       |_|  yaw 5
 */
/* 挂在CAN2上的3510 */
#define CLOUD_M6623_ID_YAW       0x205    /* 1号：yaw */
#define CLOUD_M6623_ID_PITCH     0x206    /* 2号：pitch */

#define CLOUD_M6623_READID_START	CLOUD_M6623_ID_YAW
#define CLOUD_M6623_READID_END	  CLOUD_M6623_ID_PITCH
#define CLOUD_M6623_SENDID		    0x1FF

#define Cloud_LpfAttFactor 0.1f

/* 机械角度限位 */
#define Cloud_Yaw_Min			    1730.0f		//Left
#define Cloud_Yaw_Max			    6703.0f		//Right
#define Cloud_Yaw_Center		  2420.0f		//Center
#define Cloud_Yaw_CenterINV		  6610.0f		//CenterINV

#define Cloud_Yaw_TV            4428.0f  //新赛季可能用来看屏幕

#define Cloud_Yaw_delta			  (Cloud_Yaw_Max - Cloud_Yaw_Min)

#define Cloud_Pitch_Min		  	4259.0f		//Down
#define Cloud_Pitch_Max		  	5685.0f		//Upper
#define Cloud_Pitch_Center		5006.0f		//Center
#define Cloud_Pitch_delta	  	(Cloud_Pitch_Max - Cloud_Pitch_Min)

/* IMU接收Buff大小 */
#define CLOUD_GY_IMU_BUFFSIZE 	30

typedef enum{
  CLOUD_MODE_OFF = 0,
  CLOUD_MODE_MEC = 1,    /* 机械角度模式 */
	CLOUD_MODE_IMU = 3,    /* IMU模式 */
  CLOUD_MODE_LOCK = 4,
}CloudMode_e;

typedef struct{
  M6623s_t M6623s[2];
  
	float targetYawRaw;			//云台目标yaw轴原始数据
	float targetPitchRaw;		//云台目标pitch轴原始数据
  
	float LpfAttFactor;			//云台滤波系数
	float targetYawLPF;			//云台yaw轴滤波后数据
	float targetPitchLPF;		//云台pitch轴滤波后数据
  
  GY_IMU_t IMU;       //陀螺仪
	float IMUtargetYawRaw;		//云台目标yaw轴原始数据
	float IMUtargetPitchRaw;	//云台目标pitch轴原始数据
  
	float IMUtargetYawLPF;		//云台yaw轴滤波后数据
	float IMUtargetPitchLPF;	//云台pitch轴滤波后数据
  
	float IMUYawAngleMax;		//云台IMU最大角度(右)
	float IMUYawAngleMin;		//云台IMU最小角度(左)
  
	float IMUPitchAngleMax;		//云台IMU最大角度(下)
	float IMUPitchAngleMin;		//云台IMU最小角度(上)
  
	positionpid_t YawAttitude_pid;			//云台yaw轴姿态pid
	positionpid_t YawSpeed_pid;			//云台yaw轴速度pid
  
	positionpid_t RollAttitude_pid;			//云台roll轴姿态pid
	positionpid_t RollSpeed_pid;			//云台roll轴速度pid
  
  uint8_t inventFB;
  
	CloudMode_e mode;						//云台控制模式
}Cloud_t;

extern Cloud_t Cloud;
extern uint8_t Cloud_GY_IMU_RXBUFF[CLOUD_GY_IMU_BUFFSIZE];
void Cloud_setMode(CloudMode_e mode);
void Cloud_setCurrent(int16_t iqyaw, int16_t iqpitch, int16_t iq7, int16_t iq8);
void Cloud_Init(void);
void Cloud_setIMUPosForced(float posYaw, float posPitch);
void Cloud_setDeltaPos(float delta_yaw, float delta_pitch);
void Cloud_setMecPosForced(float posYaw, float posPitch);
void Cloud_processing(void);
float Cloud_getYawAngleWithCenter(void);

#endif /* __CLOUD_CONTROL_H */
