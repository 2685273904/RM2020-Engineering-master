#ifndef __CHIASSIS_CONTROL_H
#define __CHIASSIS_CONTROL_H
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "user_common.h"
#include "M3508_Motor.h"
/*-------------------------- D E F I N E S -----------------------------------*/
#define CHASSIS_WHEELBASE_FB   406.0f   /* ǰ���־� */
#define CHASSIS_WHEELBASE_LR   550.0f   /* �����־� */

#define CHASSIS_WHEELBASE_CF   (CHASSIS_WHEELBASE_FB/2.0f)   /* �������ĵ�ǰ�־� */
#define CHASSIS_WHEELBASE_CR   (CHASSIS_WHEELBASE_LR/2.0f)   /* �������ĵ����־� */

/* �������ĵ���̨���� */
#define CHASSIS_CLOUD_OFFSET_X  0.0f
#define CHASSIS_CLOUD_OFFSET_Y  -185.0f

/* �������ĵ��������� */
#define CHASSIS_HOLDING_OFFSET_X  0.0f
#define CHASSIS_HOLDING_OFFSET_Y  -335.0f

/* ����CAN1�ϵ�3508 */
#define CHASSIS_M3508_LU       0x201    /* 1�ţ����� */
#define CHASSIS_M3508_RU       0x202    /* 2�ţ����� */
#define CHASSIS_M3508_RD       0x203    /* 3�ţ����� */
#define CHASSIS_M3508_LD       0x204    /* 4�ţ����� */

#define CHASSIS_M3508_READID_START	CHASSIS_M3508_LU
#define CHASSIS_M3508_READID_END	  CHASSIS_M3508_LD
#define CHASSIS_M3508_SENDID		    0x200

#define CHASSIS_LPF_RATIO       0.04f     /* ���̵�ͨϵ�� */

#define WHEEL_MAX_SPEED         6600.0f		/* ��������ٶ� */

#define CHASSIS_MAX_SPEED_Y	    6600.0f		/* ����ǰ������ٶ� */
#define CHASSIS_MAX_SPEED_X	    6600.0f		/* ����ƽ������ٶ� */
#define CHASSIS_MAX_SPEED_W	    6600.0f		/* ������������ٶ� */

#define CHASSIS_SPEED_NORMAL		4300.0f
#define CHASSIS_SPEED_FAST		  6600.0f
#define CHASSIS_SPEED_SLOW		  1800.0f

/*-------------------------G L O B A L - T Y P E S----------------------------*/

typedef enum{
  CHASSIS_MODE_OFF = 0,
  CHASSIS_MODE_NORMAL = 1,    /* ��������̨ģʽģʽ */
	CHASSIS_MODE_FOLLOW = 2,    /* ���̸�����̨ģʽ */
  CHASSIS_MODE_LOCK = 3,
	CHASSIS_MODE_REVERSE=4,     /*�Գ�βΪ�������ģʽ*/
}ChassisMode_e;


typedef struct{
  M3508s_t M3508s[4];  /* ����3508���x4 */
	float targetXRaw;		/* ����x��ԭʼ���� */
	float targetYRaw;		/* ����y��ԭʼ���� */
	float targetZRaw;		/* ����z��ԭʼ���� */
  
	float LpfAttFactor;	/* �����˲�ϵ�� */
	float targetXLPF;		/* ����x���˲������� */
	float targetYLPF;		/* ����y���˲������� */
	float targetZLPF;		/* ����z���˲������� */
  
	float speedLimit;		/* �����ٶ����� */
  
  float yawRad;
  float centerOffsetX;  /* ��ת�㵽��������x���ϵľ��� */
  float centerOffsetY;  /* ��ת�㵽��������y���ϵľ��� */
  
	float FollowtargetYawRaw;	/* ����Ŀ��yaw�������̨ԭʼ���� */
	float FollowtargetYawLPF;	/* ����yaw�������̨�˲������� */
	positionpid_t FollowYawAttitude_pid;		//����yaw�������̨pid
  
	ChassisMode_e mode;								//���̿���ģʽ
	uint8_t swingFlag;							//Ť����־λ
	float swingSpeed;					//Ť���ٶ�
	uint8_t PowerOverflowFlag;					//�����ʱ�־λ
}Chassis_t;
/*----------------------G L O B A L - D E F I N E S---------------------------*/
extern Chassis_t Chassis;
/*-----------G L O B A L - F U N C T I O N S - P R O T O T Y P E S------------*/
void Chassis_Init(void);
void Chassis_processing(void);
void Chassis_setSpeed(float Vx, float Vy, float VOmega, float yawRad);
void Chassis_setM3508Current(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);

void Chassis_setMode(ChassisMode_e mode);
void Chassis_setOffset(float offsetX, float offsetY);
/*----------------------------------FILE OF END-------------------------------*/

#endif /* __CHIASSIS_CONTROL_H */
