#ifndef __M6623_MOTOR_H
#define __M6623_MOTOR_H

#include "user_common.h"

#define M6623_CALIBRATIONID		0x3F0

/* �Ƕ�ת������ֵ */
#define M6623_ANGLE_TO_ENCODER(x)   (8192.0f/360.0f * x)
/* ������ֵת�Ƕ� */
#define M6623_ENCODER_TO_ANGLE(x)   (x/8192.0f/360.0f)

typedef struct{
	uint16_t realAngle;			//�������Ļ�е�Ƕ�
	int32_t totalAngle;			//�ۻ��ܹ��Ƕ�
	int16_t realCurrent;		//��������ʵ�ʵ���
	int16_t targetSpeed;			//Ŀ���ٶ�
	int32_t targetAngle;			//Ŀ��Ƕ�
	int16_t outCurrent;				//�������
	uint16_t lastAngle;			//�ϴεĽǶ�
	int16_t turnCount;			//ת����Ȧ��
	positionpid_t pid_speed;		//����ٶ�pid
	positionpid_t pid_angle;		//�Ƕȵ��pid
	uint8_t infoUpdateFlag;		//��Ϣ��ȡ���±�־
	uint16_t infoUpdateFrame;	//֡��
	uint8_t offLineFlag;		//�豸���߱�־
}M6623s_t;



//void M6623_setCurrent(int16_t iqyaw, int16_t iqpitch, int16_t iq7, int16_t iq8);
void M6623_calibration(void);
void M6623_getInfo(M6623s_t *p_M6623, CanRxMsg RxMessage);
int32_t M6623_clearTurnCounter(M6623s_t *p_M6623);
void M6623_setTargetAngle(M6623s_t *p_M6623, int32_t angle);


#endif /* __M6623_MOTOR_H */
