/*

*/

#include "userCode.h"

void Init(void){

	/*------------------BSP初始化------------------*/
	CAN1_QuickInit();
	CAN2_QuickInit();
	DR16_receiverInit(); 
	USART2_QuickInit(9600);
	delay_ms(500);
	/*---------------Devices初始化----------------*/
	LED_Init();
	BulletCatcher_Init();
	//Lifting_Init();
	/*---------------Apps初始化----------------*/
	delay_ms(500);
	TIM6_CounterMode(89, 9999);
	
}


void setup(void){
  
  
}


void loop(void){
	// put your main code here, to run repeatedly:
	


}




