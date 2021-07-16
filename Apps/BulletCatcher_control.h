#ifndef __BULLETCATCHER_CONTROL_H
#define __BULLETCATCHER_CONTROL_H
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "user_common.h"
#include "M3508_Motor.h"
#include "GAS_8.h"
#include "DynamixelServo.h"
/*------------------------G L O B A L - M A C R O S --------------------------*/
/**
*			  |          |
*			  |          |
*			  |          |
*			  ---��1��----
*				     |
*		 -----------------
*	  |  			         |
*	  |   	   			   |��2��
*	  |						     |
*
 */
/* ����CAN2�ϵ�3508 */
#define CATCHER_M3508_ID_L       0x201    /* 1�ţ������� */  
#define CATCHER_M3508_ID_R       0x202    /* 2�ţ�����ĵ�� */  


#define CATCHER_M3508_READID_START	CATCHER_M3508_ID_L
#define CATCHER_M3508_READID_END	  CATCHER_M3508_ID_R
#define CATCHER_M3508_SENDID		    0x200  
 

#define CATCHER_MAX_SPEED         6600.0f		/* ��������ٶ� */

#define CATCHER_SPEED_NORMAL		4300.0f
#define CATCHER_SPEED_FAST		  6600.0f
#define CATCHER_SPEED_SLOW		  3000.0f

//#define Catcher_MODE_NORMAL     0
//#define LIFTING_MODE_FOLLEW     1

/*-------------------------G L O B A L - T Y P E S----------------------------*/

typedef enum{
	CATCHER_MODE_OFF = 0,
  CATCHER_MODE_NORMAL = 1,
  CATCHER_MODE_LOCK = 3,
}BulletCatcherMode_e;

typedef enum{
  CATCHER_TYPE_D1,    /* ��һ�ŵ�ҩ�� */
  CATCHER_TYPE_D2,    /* �ڶ��ŵ�ҩ�� */
}BulletCatcherType_e;


typedef enum{
	CATCHER_MODE_MANUAL=1,
	CATCHER_MODE_POSITION=2,
}BulletCatcherMovement_e;

typedef enum{
	CATCHER_BOX_THREE = 1,
	CATCHER_BOX_TWO = 2,
	CATCHER_BOX_SINGLE = 3,
  CATCHER_BOX_MID1 = 4, 
  CATCHER_BOX_MID2 = 5,    
	CATCHER_BOX_LEFT2 = 6,    
  CATCHER_BOX_RIGHT2 = 7,
}GetBoxMode_e;

typedef struct{
  M3508s_t M3508s[2];
  uint8_t catcherInit;    /* ��ȡ��ʼ����ϱ�־λ */
  uint8_t catcherIsReady; /* ��ȡ׼���ñ�־λ */
  uint8_t catchRequest;   /* ��ȡ�����־λ */
  uint8_t catchIsPlus;    /* ��ȡ�쳤��־λ */
  uint8_t nowStep;        /* ���̲���ָʾ */
  uint8_t catchFinish;    /* ��ȡ��ɱ�־λ */
	uint8_t resetPositionFinish;
  BulletCatcherType_e catchType;      /* ��ȡ���ͱ�־ */
  
	BulletCatcherMovement_e movement;
	
  /* ����õ�λ�� */
  int32_t closePos;
  int32_t readlyPos;
  int32_t getBulletPos;
  int32_t getBulletPos_pre;
  int32_t getBoxPos;
	int32_t elevateBoxPos;
	
  /* ���������ƶ���λ�� */	
  int32_t middlePos;
	int32_t leftPos;
	int32_t rightPos;
	
	/*�������õ�*/
	int8_t Start;          /*�����俪ʼ��־λ*/
	int8_t stepNumber;     /*�������䲽��*/
	int8_t alignSucceed;   /*��ȡ�ɹ���־λ��ͨ��*/
  uint16_t timeCounter;  /* ������ʱ��ʱ���ļ����� */
  int8_t smallIslandflag;/*С����־λ*/
	
	/*����Դ��������*/
	int8_t Island; //����Դ�������Ӳ���
	int8_t robBox; //����Դ���������ӱ�־λ
	int8_t readyBox; //׼���ÿ���ȥ����һ����
	int8_t robCount;//���������õļ�����
  BulletCatcherMode_e mode;
	GetBoxMode_e getBoxMode;
	
	int8_t test;
}BulletCatcher_t;

typedef struct{
  uint8_t close_req;   /* �ر������־λ */
  uint8_t open_req;    /* �������־λ */
  uint8_t isOpen;    /* �򿪱�־λ */
  uint8_t timecount;

}BulletCatcher_test_t;


typedef struct{
  uint8_t biuReq;
  uint8_t biuFinish;
  uint16_t timeCounter;  /* ������ʱ��ʱ���ļ����� */
  uint8_t nowStep;
  
  
}BulletBiuBiuController_t;

typedef struct{
  uint8_t bankOpenReq;
  uint8_t Finish;
	uint8_t Open;
	uint8_t OFF;
  uint16_t timeCounter;  /* ������ʱ��ʱ���ļ����� */
  uint8_t nowStep;
	uint8_t start;
}BigBank_t;




/*----------------------G L O B A L - D E F I N E S---------------------------*/
extern BulletCatcher_t BulletCatcher;
extern BulletCatcher_test_t BulletCatcher_test;
extern BigBank_t BigBank;
/*-----------G L O B A L - F U N C T I O N S - P R O T O T Y P E S------------*/
void BulletCatcher_Init(void);
uint8_t BulletCatcher_setTargetAngle(float angle);
void BulletCatcher_setM3508Current(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);
void BulletCatcher_resetAngle(void);
uint8_t BulletCatcher_open(void);
void BulletCatcher_processing(void);
uint8_t BulletCatcher_close(void);
void BulletCatcher_setMode(BulletCatcherMode_e mode);
void BulletCatcher_writeBank(void);

void BulletCatcher_testOpenReq(void);
void BulletCatcher_testCloseReq(void);

void BulletCatcher_move(void);
void BulletCatcher_setSpeed(float speed);
void BulletCatcher_resetPosition(void);
void BulletCatcher_threeProcessing(void);
void BulletCatcher_begin(GetBoxMode_e mode);

uint8_t BulletCatcher_setPosition(float angle);
uint8_t BulletCatcher_decide(void);
void BulletCatcher_lootProcessing(void);

void BulletCatcher_wave(void);
/*----------------------------------FILE OF END-------------------------------*/
 
#endif /* __BULLETCATCHER_CONTROL_H */
