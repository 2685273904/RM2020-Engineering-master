#ifndef __M2006_MOTOR_H
#define __M2006_MOTOR_H

#include "user_common.h"

#define M2006_READID_START	0x207
#define M2006_READID_END	0x207
#define M2006_SENDID		0x1FF


//#define M2006_LOADANGLE		36864			/* 电机拨一个弹需要转的角度数  4.5*8191 （2006 8孔拨弹）*/
//#define M2006_LOADANGLE		49146			/* 电机拨一个弹需要转的角度数  6*8191 （2006 6孔拨弹）*/
//#define M2006_LOADANGLE		42125			/* 电机拨一个弹需要转的角度数  5.142*8191 （2006 7孔拨弹）*/
//#define M2006_LOADANGLE		22116			/* 电机拨一个弹需要转的角度数  2.7*8191 （3508 7孔拨弹）*/

//#define M2006_LOADCIRCLE	5			/* 电机拨一个弹需要转的圈数 */
//#define M2006_LOADSPEED		1800		/* 电机拨弹时的转速 */
#define M2006_FIRSTANGLE		3800		/* 电机初始位置 */

extern M2006s_t M2006s[1];


//void M2006_setCurrent(int16_t iq3);     /* M2006与6623共用发送函数 */
void M2006_getInfo(CanRxMsg RxMessage);
void M2006_setTargetSpeed(M2006Name_e motorName, int16_t speed);
void M2006_setTargetAngle(M2006Name_e motorName, int32_t angle);

#endif /* __M2006_MOTOR_H */
