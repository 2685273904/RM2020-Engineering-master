#ifndef __LASER_H
#define __LASER_H

#include "user_common.h"


#define LASER_GPIO_CLK             	RCC_AHB1Periph_GPIOG
#define LASER_PIN                  	GPIO_Pin_13
#define LASER_GPIO_PORT           	GPIOG

#define LASER_ON()				GPIO_SetBits(LASER_GPIO_PORT, LASER_PIN)
#define LASER_OFF()				GPIO_ResetBits(LASER_GPIO_PORT, LASER_PIN)
#define LASER(isOn) 			GPIO_WriteBit(LASER_GPIO_PORT,LASER_PIN, (BitAction)isOn)
#define LASER_TOGGLE() 			GPIO_ToggleBits(LASER_GPIO_PORT, LASER_PIN)


void Laser_Init(void);

#endif /* __LASER_H */
