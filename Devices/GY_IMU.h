#ifndef __GY_IMU_H
#define __GY_IMU_H

#include "user_common.h"


#define GY_IMU_PACKAGE_LENGTH 	18

typedef struct{
	vector_t gyro;
	eular_t eular;
	float lastYaw;
	float totalYaw;
	int16_t turnCount;
	uint8_t devStatus;
	uint8_t infoUpdateFlag;
	uint16_t infoUpdateFrame;
	uint8_t offLineFlag;
}GY_IMU_t;

void GY_IMU_Init(uint32_t p_buff, uint32_t buffSize);
void GY_IMU_getInfo(uint8_t* buff, uint16_t dataLength, GY_IMU_t* dataOut);



#endif /* __GY_IMU_H */
