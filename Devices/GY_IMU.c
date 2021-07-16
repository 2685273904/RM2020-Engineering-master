/**
  ******************************************************************************
  * @file    GY_IMU.c
  * @author  Hare
  * @version V1.0
  * @date    
  * @brief   GY_BNO055姿态模块应用函数接口
  ******************************************************************************
  */
  
  
#include "GY_IMU.h"

/**
  * @brief  GY_IMU串口初始化
  * @param  None
  * @retval None
  */
void GY_IMU_Init(uint32_t p_buff, uint32_t buffSize){
	
	USART3_QuickInit(115200);
	USART3_RXDMA_Config(p_buff, buffSize);
	
}






/**
  * @brief  GY_IMU数据包校验
  * @param[in]  buff	数据包
  * @retval 	0 校验失败，1校验通过
  */
uint8_t GY_IMU_Checksum(uint8_t* buff){
	uint8_t sum = 0;
	
	for(uint8_t i=0; i<GY_IMU_PACKAGE_LENGTH-1; i++){
		sum += buff[i]; 
	}
	/*校验帧尾*/
	if(sum == buff[GY_IMU_PACKAGE_LENGTH-1]){
		//校验通过
		return 1;
	}
	else{
		return 0;
	}
	
}

/**
  * @brief  GY_IMU数据包校验
  * @param[in]  buff	数据包
  * @retval 	0 校验失败，1校验通过
  */
void GY_IMU_getInfo(uint8_t* buff, uint16_t dataLength, GY_IMU_t* dataOut){
	
	for(uint16_t n = 0; n < dataLength; ){
		//匹配帧头
		if( (buff[n] == 0x5A) && (buff[n+1] == 0x5A) ){
			if(GY_IMU_Checksum(&buff[n])){
				int16_t DATA[6];
				DATA[0]=(buff[n+4]<<8) | buff[n+5];
				DATA[1]=(buff[n+6]<<8) | buff[n+7];
				DATA[2]=(buff[n+8]<<8) | buff[n+9];
				DATA[3]=(buff[n+10]<<8) | buff[n+11];
				DATA[4]=(buff[n+12]<<8) | buff[n+13];
				DATA[5]=(buff[n+14]<<8) | buff[n+15];
				
				dataOut->gyro.x = (float)DATA[0]/16;
				dataOut->gyro.y = (float)DATA[1]/16;
				dataOut->gyro.z = (float)DATA[2]/16;
				
				dataOut->eular.yaw = (float)((uint16_t)DATA[3])/100;
				dataOut->eular.roll = (float)DATA[4]/100;
				dataOut->eular.pitch = (float)DATA[5]/100;
				
				dataOut->devStatus = buff[n+16];
				
				if(dataOut->eular.yaw - dataOut->lastYaw < -300){
					dataOut->turnCount++;
				}
				
				if(dataOut->lastYaw - dataOut->eular.yaw < -300){
					dataOut->turnCount--;
				}
				
				dataOut->lastYaw =  dataOut->eular.yaw;
				
				dataOut->totalYaw = dataOut->eular.yaw + (360.0f * dataOut->turnCount);
				
				dataOut->infoUpdateFrame++;
				dataOut->infoUpdateFlag = 1;
				
				return;
			}
			else{
				n++;
			}
			
		}
		else{
			n++;
		}
	}
	
}



