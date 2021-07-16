#ifndef __LIFTING_CONTROL_H
#define __LIFTING_CONTROL_H
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "user_common.h"
#include "M3508_Motor.h"
#include "PID.h"
/*------------------------G L O B A L - M A C R O S --------------------------*/

/**   ������������ͷ��
 *    1              2
 *
 *        7       8     ��̧������3508��
 *
 *    4              3
 *    ��ȡ��������β��
*/

/* ����CAN1�ϵ�3508 */
#define LIFTING_M3508_ID_L       0x207    /* 7�ţ��� */
#define LIFTING_M3508_ID_R       0x208    /* 8�ţ��� */

#define LIFTING_M3508_READID_START	LIFTING_M3508_ID_L
#define LIFTING_M3508_READID_END	  LIFTING_M3508_ID_R
//#define LIFTING_M3508_SENDID		    0x200

#define LIFTING_MAX_SPEED         6600		/* ���������ٶ� */

#define LIFTING_SPEED_NORMAL		1980//ԭ����6300
//#define LIFTING_SPEED_FAST		  4800
#define LIFTING_SPEED_SLOW		  1500//ԭ����1500

#define LIFTING_FULL_TRAVEL   423090    /* ���г� :423090*/
#define LIFTING_CATCH_POS     340000     /* ��ȡ�߶��г� */

#define LIFTING_DOWNLIMIT_OFFSET     26000     /* ��͵���λƫ����ϸ��� */

/*-------------------------G L O B A L - T Y P E S----------------------------*/
typedef enum{
  LIFTING_MODE_OFF = 0,
  LIFTING_MODE_POSITION = 1,
  LIFTING_MODE_MANUAL = 2,
  LIFTING_MODE_LOCK = 3,
  
}LiftingMode_e;


/**
 *  -------------     ��ߵ�MAX����ȡ�㣩
 * --------------     
 *
 *
 *
 *
 *----------------    ��͵�MIN
 */

typedef struct{
  M3508s_t M3508s[2];
  
  int32_t targetPosition;
  int32_t targetSpeed;
  positionpid_t pid_pos;
  
  uint16_t speedLimit;
  uint16_t timeCounter;  /* ������ʱ��ʱ���ļ����� */
  uint8_t isInit;
  uint8_t isFinish;
  uint8_t UpLimit;     /* ��ߵ���λ */
  uint8_t DownLimit;   /* ��͵���λ */
  
  int32_t maxPos;         /* ��ߵ�λ�� */
  int32_t catchPos;       /* ��ȡλ�� */
  int32_t minPos;         /* ��͵�λ�� */
  LiftingMode_e mode;
	uint8_t resetAngleflage;
}Lifting_t;

/*----------------------G L O B A L - D E F I N E S---------------------------*/
extern Lifting_t Lifting;
/*-----------G L O B A L - F U N C T I O N S - P R O T O T Y P E S------------*/
void Lift_setM3508Current(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);
void Lifting_Init(void);
void Lifting_resetAngle(void);
void Lifting_processing(void);
void Lifting_setMode(LiftingMode_e mode);
void Lifting_setSpeed(int32_t speed);
uint8_t Lifting_setPosition(int32_t pos);
//void Lifting_setMode(LiftingMode_e mode);
/*----------------------------------FILE OF END-------------------------------*/
#endif /* __LIFTING_CONTROL_H */
