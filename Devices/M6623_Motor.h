#ifndef __M6623_MOTOR_H
#define __M6623_MOTOR_H

#include "user_common.h"

#define M6623_CALIBRATIONID		0x3F0

/* 角度转编码器值 */
#define M6623_ANGLE_TO_ENCODER(x)   (8192.0f/360.0f * x)
/* 编码器值转角度 */
#define M6623_ENCODER_TO_ANGLE(x)   (x/8192.0f/360.0f)

typedef struct{
	uint16_t realAngle;			//读回来的机械角度
	int32_t totalAngle;			//累积总共角度
	int16_t realCurrent;		//读回来的实际电流
	int16_t targetSpeed;			//目标速度
	int32_t targetAngle;			//目标角度
	int16_t outCurrent;				//输出电流
	uint16_t lastAngle;			//上次的角度
	int16_t turnCount;			//转过的圈数
	positionpid_t pid_speed;		//电机速度pid
	positionpid_t pid_angle;		//角度电机pid
	uint8_t infoUpdateFlag;		//信息读取更新标志
	uint16_t infoUpdateFrame;	//帧率
	uint8_t offLineFlag;		//设备离线标志
}M6623s_t;



//void M6623_setCurrent(int16_t iqyaw, int16_t iqpitch, int16_t iq7, int16_t iq8);
void M6623_calibration(void);
void M6623_getInfo(M6623s_t *p_M6623, CanRxMsg RxMessage);
int32_t M6623_clearTurnCounter(M6623s_t *p_M6623);
void M6623_setTargetAngle(M6623s_t *p_M6623, int32_t angle);


#endif /* __M6623_MOTOR_H */
