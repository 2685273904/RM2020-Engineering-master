#ifndef __HALL_IO_H
#define __HALL_IO_H

#include "user_common.h"




#define Hall_LIFT_UPLIMIT_GPIO_CLK             	RCC_AHB1Periph_GPIOA
#define Hall_LIFT_UPLIMIT_GPIO_PORT           	GPIOA
#define Hall_LIFT_UPLIMIT_PIN                  	GPIO_Pin_0

#define Hall_LIFT_DOWNLIMIT_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define Hall_LIFT_DOWNLIMIT_GPIO_PORT           GPIOA
#define Hall_LIFT_DOWNLIMIT_PIN                 GPIO_Pin_1


#define Hall_PILLARHOLDING_LOCK_GPIO_CLK        RCC_AHB1Periph_GPIOA
#define Hall_PILLARHOLDING_LOCK_GPIO_PORT       GPIOA
#define Hall_PILLARHOLDING_LOCK_PIN             GPIO_Pin_2

#define Hall_CATCHERPULS_OPEN_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define Hall_CATCHERPULS_OPEN_GPIO_PORT         GPIOA
#define Hall_CATCHERPULS_OPEN_LOCK_PIN          GPIO_Pin_3


/* 感应到返回1 */
#define Hall_LIFT_UPLIMIT()         !GPIO_ReadInputDataBit(Hall_LIFT_UPLIMIT_GPIO_PORT,Hall_LIFT_UPLIMIT_PIN)
#define Hall_LIFT_DOWNLIMIT()       !GPIO_ReadInputDataBit(Hall_LIFT_DOWNLIMIT_GPIO_PORT,Hall_LIFT_DOWNLIMIT_PIN)
#define Hall_PILLARHOLDING_LOCK()   GPIO_ReadInputDataBit(Hall_PILLARHOLDING_LOCK_GPIO_PORT,Hall_PILLARHOLDING_LOCK_PIN)

void Hall_IO_Init(void);
void Photogate_IO_Init(void);
#endif /* __HALL_IO_H */


