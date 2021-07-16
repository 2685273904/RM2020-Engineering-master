#ifndef __CHIASSIS_CONTROL_H
#define __CHIASSIS_CONTROL_H
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "user_common.h"
#include "M3508_Motor.h"
/*-------------------------- D E F I N E S -----------------------------------*/
#define CHASSIS_WHEELBASE_FB   406.0f   /* 前后轮距 */
#define CHASSIS_WHEELBASE_LR   550.0f   /* 左右轮距 */

#define CHASSIS_WHEELBASE_CF   (CHASSIS_WHEELBASE_FB/2.0f)   /* 底盘中心到前轮距 */
#define CHASSIS_WHEELBASE_CR   (CHASSIS_WHEELBASE_LR/2.0f)   /* 底盘中心到右轮距 */

/* 底盘中心到云台距离 */
#define CHASSIS_CLOUD_OFFSET_X  0.0f
#define CHASSIS_CLOUD_OFFSET_Y  -185.0f

/* 底盘中心到抱柱距离 */
#define CHASSIS_HOLDING_OFFSET_X  0.0f
#define CHASSIS_HOLDING_OFFSET_Y  -335.0f

/* 挂在CAN1上的3508 */
#define CHASSIS_M3508_LU       0x201    /* 1号：左上 */
#define CHASSIS_M3508_RU       0x202    /* 2号：右上 */
#define CHASSIS_M3508_RD       0x203    /* 3号：右下 */
#define CHASSIS_M3508_LD       0x204    /* 4号：左下 */

#define CHASSIS_M3508_READID_START	CHASSIS_M3508_LU
#define CHASSIS_M3508_READID_END	  CHASSIS_M3508_LD
#define CHASSIS_M3508_SENDID		    0x200

#define CHASSIS_LPF_RATIO       0.04f     /* 底盘低通系数 */

#define WHEEL_MAX_SPEED         6600.0f		/* 单轮最大速度 */

#define CHASSIS_MAX_SPEED_Y	    6600.0f		/* 底盘前后最快速度 */
#define CHASSIS_MAX_SPEED_X	    6600.0f		/* 底盘平移最快速度 */
#define CHASSIS_MAX_SPEED_W	    6600.0f		/* 底盘自旋最快速度 */

#define CHASSIS_SPEED_NORMAL		4300.0f
#define CHASSIS_SPEED_FAST		  6600.0f
#define CHASSIS_SPEED_SLOW		  1800.0f

/*-------------------------G L O B A L - T Y P E S----------------------------*/

typedef enum{
  CHASSIS_MODE_OFF = 0,
  CHASSIS_MODE_NORMAL = 1,    /* 不跟随云台模式模式 */
	CHASSIS_MODE_FOLLOW = 2,    /* 底盘跟随云台模式 */
  CHASSIS_MODE_LOCK = 3,
	CHASSIS_MODE_REVERSE=4,     /*以车尾为正方向的模式*/
}ChassisMode_e;


typedef struct{
  M3508s_t M3508s[4];  /* 底盘3508电机x4 */
	float targetXRaw;		/* 底盘x轴原始数据 */
	float targetYRaw;		/* 底盘y轴原始数据 */
	float targetZRaw;		/* 底盘z轴原始数据 */
  
	float LpfAttFactor;	/* 底盘滤波系数 */
	float targetXLPF;		/* 底盘x轴滤波后数据 */
	float targetYLPF;		/* 底盘y轴滤波后数据 */
	float targetZLPF;		/* 底盘z轴滤波后数据 */
  
	float speedLimit;		/* 底盘速度限制 */
  
  float yawRad;
  float centerOffsetX;  /* 旋转点到底盘中心x轴上的距离 */
  float centerOffsetY;  /* 旋转点到底盘中心y轴上的距离 */
  
	float FollowtargetYawRaw;	/* 底盘目标yaw轴跟随云台原始数据 */
	float FollowtargetYawLPF;	/* 底盘yaw轴跟随云台滤波后数据 */
	positionpid_t FollowYawAttitude_pid;		//底盘yaw轴跟随云台pid
  
	ChassisMode_e mode;								//底盘控制模式
	uint8_t swingFlag;							//扭腰标志位
	float swingSpeed;					//扭腰速度
	uint8_t PowerOverflowFlag;					//超功率标志位
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
