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
/* ����CAN2�ϵ�3508 */
#define PILLARHOLDING_M3508_ID_L       0x203    /* 5�ţ��� */
#define PILLARHOLDING_M3508_ID_R       0x204    /* 6�ţ��� */

#define PILLARHOLDING_M3508_READID_START	LIFTING_M3508_ID_L
#define PILLARHOLDING_M3508_READID_END	  LIFTING_M3508_ID_R
#define PILLARHOLDING_M3508_SENDID		    0x1FF

//#define LIFTING_LPF_RATIO       0.04f     /* ̧����ͨϵ�� */

#define PILLARHOLDING_MAX_SPEED         6600.0f		/* ���������ٶ� */

#define PILLARHOLDING_SPEED_NORMAL		4300.0f
#define PILLARHOLDING_SPEED_FAST		  6600.0f
#define PILLARHOLDING_SPEED_SLOW		  3000.0f


/* ������Ħ����ֱ���ȣ��˵������� */
#define PILLARHOLDING_WHEEL_RATIO		  (52.0f/150.0f)



/* �õ������� */
#define PILLARHOLDING_USING_GASID     0
#define PILLARHOLDING_GASPOS          GAS_POS_4       /* �������� */

/* �õ��Ķ�� */
#define PILLARHOLDING_USING_SERVOID     1
#define PILLARHOLDING_SERVO_MAX_ANGLE   3072
#define PILLARHOLDING_SERVO_MIN_ANGLE   2050
#define PILLARHOLDING_SERVO_CENTRE_ANGLE 2561


#define PILLARHOLDING_SERVO_LOCK_ANGLE      PILLARHOLDING_SERVO_MIN_ANGLE//��ס
#define PILLARHOLDING_SERVO_UNLOCK_ANGLE    PILLARHOLDING_SERVO_MAX_ANGLE//û��


/* ��ҪС����͵���λƫ��22000�����ϸ���  */
#define LIFTING_CLIMBDOWN_POS     38000     /* �µ���΢̧���߶� */
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
  CLAMP_RUNNING = 0x00,//��ʼ
  CLAMP_CLOSE_SUCCESS = 0x11,//�رռ��ӳɹ�
  CLAMP_CLOSE_FAILED = 0x10,//�رռ���ʧ��
  CLAMP_OPEN_SUCCESS = 0x21, //�򿪼��ӳɹ�
}clampState_e;

typedef struct{
  M3508s_t M3508s[2];
  positionpid_t pid_pos;
  float lastAngle;  /* ��תǰ�Ƕ� */
  float tagetAngle; /* ��ת��Ƕ� */
  int16_t tagetSpeed; /* Ŀ����ת�ٶ� */
  int speedLimit;
  uint8_t isClose;  /* �Ƿ�رձ�־λ */
  uint8_t isStart;  /* ������ʼ */
  uint8_t isFinish;  /* ������� */
  uint8_t isLock;     /* �ѱ��� */
  uint8_t turnEnable;     /* ����ת */
  PillarHoldingType_e type;     /* �ϵ����µ� */
  uint8_t nowStep;  /* ��ǰ���� */
  uint8_t nextReq;  /* ��һ������ */
  uint8_t backReq;  /* ��һ������ */
  
  uint16_t timeCounter;  /* ������ʱ��ʱ���ļ����� */
	
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

