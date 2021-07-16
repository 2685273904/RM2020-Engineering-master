#ifndef __CLOUD_CONTROL_H
#define __CLOUD_CONTROL_H

#include "user_common.h"
#include "M6623_Motor.h"
#include "GY_IMU.h"

#define Cloud_Mode_Normal		0
#define Cloud_Mode_WithIMU		1

/**
 *      ͼ��
 *   ------------
 *        | 
 *       ( ) pitch 6
 *      -----
 *       |_|  yaw 5
 */
/* ����CAN2�ϵ�3510 */
#define CLOUD_M6623_ID_YAW       0x205    /* 1�ţ�yaw */
#define CLOUD_M6623_ID_PITCH     0x206    /* 2�ţ�pitch */

#define CLOUD_M6623_READID_START	CLOUD_M6623_ID_YAW
#define CLOUD_M6623_READID_END	  CLOUD_M6623_ID_PITCH
#define CLOUD_M6623_SENDID		    0x1FF

#define Cloud_LpfAttFactor 0.1f

/* ��е�Ƕ���λ */
#define Cloud_Yaw_Min			    1730.0f		//Left
#define Cloud_Yaw_Max			    6703.0f		//Right
#define Cloud_Yaw_Center		  2420.0f		//Center
#define Cloud_Yaw_CenterINV		  6610.0f		//CenterINV

#define Cloud_Yaw_TV            4428.0f  //������������������Ļ

#define Cloud_Yaw_delta			  (Cloud_Yaw_Max - Cloud_Yaw_Min)

#define Cloud_Pitch_Min		  	4259.0f		//Down
#define Cloud_Pitch_Max		  	5685.0f		//Upper
#define Cloud_Pitch_Center		5006.0f		//Center
#define Cloud_Pitch_delta	  	(Cloud_Pitch_Max - Cloud_Pitch_Min)

/* IMU����Buff��С */
#define CLOUD_GY_IMU_BUFFSIZE 	30

typedef enum{
  CLOUD_MODE_OFF = 0,
  CLOUD_MODE_MEC = 1,    /* ��е�Ƕ�ģʽ */
	CLOUD_MODE_IMU = 3,    /* IMUģʽ */
  CLOUD_MODE_LOCK = 4,
}CloudMode_e;

typedef struct{
  M6623s_t M6623s[2];
  
	float targetYawRaw;			//��̨Ŀ��yaw��ԭʼ����
	float targetPitchRaw;		//��̨Ŀ��pitch��ԭʼ����
  
	float LpfAttFactor;			//��̨�˲�ϵ��
	float targetYawLPF;			//��̨yaw���˲�������
	float targetPitchLPF;		//��̨pitch���˲�������
  
  GY_IMU_t IMU;       //������
	float IMUtargetYawRaw;		//��̨Ŀ��yaw��ԭʼ����
	float IMUtargetPitchRaw;	//��̨Ŀ��pitch��ԭʼ����
  
	float IMUtargetYawLPF;		//��̨yaw���˲�������
	float IMUtargetPitchLPF;	//��̨pitch���˲�������
  
	float IMUYawAngleMax;		//��̨IMU���Ƕ�(��)
	float IMUYawAngleMin;		//��̨IMU��С�Ƕ�(��)
  
	float IMUPitchAngleMax;		//��̨IMU���Ƕ�(��)
	float IMUPitchAngleMin;		//��̨IMU��С�Ƕ�(��)
  
	positionpid_t YawAttitude_pid;			//��̨yaw����̬pid
	positionpid_t YawSpeed_pid;			//��̨yaw���ٶ�pid
  
	positionpid_t RollAttitude_pid;			//��̨roll����̬pid
	positionpid_t RollSpeed_pid;			//��̨roll���ٶ�pid
  
  uint8_t inventFB;
  
	CloudMode_e mode;						//��̨����ģʽ
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
