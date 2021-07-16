#ifndef __PILLARHOLDING_CONTROL_H
#define __PILLARHOLDING_CONTROL_H
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "user_common.h"
#include "M3508_Motor.h"
#include "DynamixelServo.h"
#include "GAS_8.h"
/*------------------------G L O B A L - M A C R O S --------------------------*/
/*
 *          ______     ______
 *          |    /     \    |
 *          |   (       )   |
 *          |   (3)   (4)   |
 */
/* 挂在CAN2上的3508 */
#define PILLARHOLDING_M3508_ID_L       0x203    /* 5号：左 */
#define PILLARHOLDING_M3508_ID_R       0x204    /* 6号：右 */

#define PILLARHOLDING_M3508_READID_START	LIFTING_M3508_ID_L
#define PILLARHOLDING_M3508_READID_END	  LIFTING_M3508_ID_R
#define PILLARHOLDING_M3508_SENDID		    0x1FF

//#define LIFTING_LPF_RATIO       0.04f     /* 抬升低通系数 */

#define PILLARHOLDING_MAX_SPEED         6600.0f		/* 单电机最大速度 */

#define PILLARHOLDING_SPEED_NORMAL		4300.0f
#define PILLARHOLDING_SPEED_FAST		  6600.0f
#define PILLARHOLDING_SPEED_SLOW		  3000.0f


/* 底盘与摩擦轮直径比，乘到底盘上 */
#define PILLARHOLDING_WHEEL_RATIO		  (52.0f/150.0f)



/* 用到的气阀 */
#define PILLARHOLDING_USING_GASID     0
#define PILLARHOLDING_GASPOS          GAS_POS_4       /* 抱柱气阀 */

/* 用到的舵机 */
#define PILLARHOLDING_USING_SERVOID     1
#define PILLARHOLDING_SERVO_MAX_ANGLE   3072
#define PILLARHOLDING_SERVO_MIN_ANGLE   2050
#define PILLARHOLDING_SERVO_CENTRE_ANGLE 2561


#define PILLARHOLDING_SERVO_LOCK_ANGLE      PILLARHOLDING_SERVO_MIN_ANGLE//锁住
#define PILLARHOLDING_SERVO_UNLOCK_ANGLE    PILLARHOLDING_SERVO_MAX_ANGLE//没锁


/* 不要小于最低点限位偏差22000，正上负下  */
#define LIFTING_CLIMBDOWN_POS     38000     /* 下岛稍微抬升高度 */
/*-------------------------G L O B A L - T Y P E S----------------------------*/

typedef enum{
  PILLARHOLDING_MODE_OFF = 0,
  PILLARHOLDING_MODE_NORMAL = 1,
  PILLARHOLDING_MODE_LOCK = 3,
}PillarHoldingMode_e;


typedef enum{
  PILLARHOLDING_STEPUP = 0,
  PILLARHOLDING_STEPDOWN = 1,
}PillarHoldingType_e;

typedef enum{
  CLAMP_RUNNING = 0x00,//开始
  CLAMP_CLOSE_SUCCESS = 0x11,//关闭夹子成功
  CLAMP_CLOSE_FAILED = 0x10,//关闭夹子失败
  CLAMP_OPEN_SUCCESS = 0x21, //打开夹子成功
}clampState_e;

typedef struct{
  M3508s_t M3508s[2];
  positionpid_t pid_pos;
  float lastAngle;  /* 旋转前角度 */
  float tagetAngle; /* 旋转后角度 */
  int16_t tagetSpeed; /* 目标旋转速度 */
  int speedLimit;
  uint8_t isClose;  /* 是否关闭标志位 */
  uint8_t isStart;  /* 动作开始 */
  uint8_t isFinish;  /* 动作完成 */
  uint8_t isLock;     /* 已抱紧 */
  uint8_t turnEnable;     /* 可旋转 */
  PillarHoldingType_e type;     /* 上岛或下岛 */
  uint8_t nowStep;  /* 当前步骤 */
  uint8_t nextReq;  /* 下一步请求 */
  uint8_t backReq;  /* 下一步请求 */
  
  uint16_t timeCounter;  /* 用于延时或超时检测的计数器 */
	
  PillarHoldingMode_e mode;
	
}PillarHolding_t;
/*----------------------G L O B A L - D E F I N E S---------------------------*/
extern PillarHolding_t PillarHolding;
/*-----------G L O B A L - F U N C T I O N S - P R O T O T Y P E S------------*/
void PillarHolding_Init(void);
clampState_e PillarHolding_setClamp(uint8_t isClamp);
void PillarHolding_setTurnSpeed(int16_t speed);
void PillarHolding_processing(void);
void PillarHolding_climbUpStep(void);
void PillarHolding_climbDownStep(void);
void PillarHolding_start(PillarHoldingType_e type);
void PillarHolding_close(void);
void PillarHolding_nextReq(void);
void PillarHolding_backReq(void);
/*----------------------------------FILE OF END-------------------------------*/
#endif /* __PILLARHOLDING_CONTROL_H */

