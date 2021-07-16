/**
  ******************************************************************************
  * @file    Chassis_control.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   底盘控制函数接口
  ******************************************************************************
  */
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "Chassis_control.h"
/*------------------------G L O B A L - M A C R O S --------------------------*/
Chassis_t Chassis;
/*------------------G L O B A L - F U N C T I O N S --------------------------*/
/**
  * @brief  底盘初始化，配置参数
  * @param  None
  * @retval None
  */
void Chassis_Init(void){
	Chassis.LpfAttFactor = CHASSIS_LPF_RATIO;
	Chassis.speedLimit = CHASSIS_SPEED_NORMAL;

	Chassis.mode = CHASSIS_MODE_LOCK;
	Chassis.PowerOverflowFlag = 0;
  
  //底盘电机PID
	for(int i = 0; i < 4; i++){
		PositionPID_paraReset(&Chassis.M3508s[i].pid_speed, 4.0f, 1.0f, 1.0f, 16000, 16000);
	}

}	

float rotate_ratio[4];
/**
  * @brief  麦克纳姆轮速度解算
  * @param[in]  Vx		x轴速度
  * @param[in]	Vy		y轴速度
  * @param[in]	VOmega	自转速度
  * @param[out]	Speed	速度
  * @retval None
  */
void MecanumCalculate(float Vx, float Vy, float VOmega, int16_t *Speed)
{
  float tempSpeed[4];
	float maxSpeed = 0.0f;
	float param = 1.0f;
  
  
	float max_rotate_ratio = 1.0f;
	
	//速度限制
	VAL_LIMIT(Vx, -CHASSIS_MAX_SPEED_X, CHASSIS_MAX_SPEED_X);  
	VAL_LIMIT(Vy, -CHASSIS_MAX_SPEED_Y, CHASSIS_MAX_SPEED_Y);  
	VAL_LIMIT(VOmega, -CHASSIS_MAX_SPEED_W, CHASSIS_MAX_SPEED_W);  
  
  rotate_ratio[0] = (CHASSIS_WHEELBASE_CF+CHASSIS_WHEELBASE_CR
                      -(Chassis.centerOffsetX - Chassis.centerOffsetY))
                      /(CHASSIS_WHEELBASE_CF+CHASSIS_WHEELBASE_CR);
  
  rotate_ratio[1] = (CHASSIS_WHEELBASE_CF+CHASSIS_WHEELBASE_CR
                      -(-Chassis.centerOffsetX - Chassis.centerOffsetY))
                      /(CHASSIS_WHEELBASE_CF+CHASSIS_WHEELBASE_CR);
  
  rotate_ratio[2] = (CHASSIS_WHEELBASE_CF+CHASSIS_WHEELBASE_CR
                      -(-Chassis.centerOffsetX + Chassis.centerOffsetY))
                      /(CHASSIS_WHEELBASE_CF+CHASSIS_WHEELBASE_CR);
  
  rotate_ratio[3] = (CHASSIS_WHEELBASE_CF+CHASSIS_WHEELBASE_CR
                      -(Chassis.centerOffsetX + Chassis.centerOffsetY))
                      /(CHASSIS_WHEELBASE_CF+CHASSIS_WHEELBASE_CR);
                      
  //寻找bili
  for(uint8_t i = 0; i < 4; i++)
  {
    if(abs(rotate_ratio[i]) > max_rotate_ratio)
    {
      max_rotate_ratio = abs(rotate_ratio[i]);
    }
  }
  
	
	//四轮速度分解
	tempSpeed[0] = Vx + Vy + VOmega*rotate_ratio[0];
  tempSpeed[1] = Vx - Vy + VOmega*rotate_ratio[1];
  tempSpeed[2] = -Vx - Vy + VOmega*rotate_ratio[2];
  tempSpeed[3] = -Vx + Vy + VOmega*rotate_ratio[3];

  //寻找最大速度
  for(uint8_t i = 0; i < 4; i++)
  {
    if(abs(tempSpeed[i]) > maxSpeed)
    {
      maxSpeed = abs(tempSpeed[i]);
    }
  }


  //速度分配
  if(maxSpeed > WHEEL_MAX_SPEED)
  {
    param = (float)WHEEL_MAX_SPEED / maxSpeed;
  }
	
	Speed[0] = tempSpeed[0] * param;
	Speed[1] = tempSpeed[1] * param;
	Speed[2] = tempSpeed[2] * param;
	Speed[3] = tempSpeed[3] * param;
	
}



void Chassis_powerLimit(M3508s_t *p_M3508s, float currentLimit){
	float current_Sum = 0.0f;
	
	/* 计算电流和 */
	current_Sum = abs(p_M3508s[0].outCurrent) + abs(p_M3508s[1].outCurrent) \
					+ abs(p_M3508s[2].outCurrent) + abs(p_M3508s[3].outCurrent);
	
	/* 功率分配 */
	if(current_Sum > currentLimit){
		p_M3508s[0].outCurrent = currentLimit * p_M3508s[0].outCurrent / current_Sum;
		p_M3508s[1].outCurrent = currentLimit * p_M3508s[1].outCurrent / current_Sum;
		p_M3508s[2].outCurrent = currentLimit * p_M3508s[2].outCurrent / current_Sum;
		p_M3508s[3].outCurrent = currentLimit * p_M3508s[3].outCurrent / current_Sum;
	}
}

/**
  * @brief  设置M3508电机电流值（id号为1~4） CAN1
  * @param  iqx (x:1~4) 对应id号电机的电流值，范围-16384~0~16384
  * @retval None
  */
void Chassis_setM3508Current(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4){

	uint8_t data[8];
	
	//数据格式详见说明书P32
	data[0] = iq1 >> 8;
	data[1] = iq1;
	data[2] = iq2 >> 8;
	data[3] = iq2;
	data[4] = iq3 >> 8;
	data[5] = iq3;
	data[6] = iq4 >> 8;
	data[7] = iq4;
	
	CAN_SendData(CAN1, CAN_ID_STD, CHASSIS_M3508_SENDID, data);
	
}	

/**
  * @brief  设置底盘模式
  * @param[in]  mode
  * @retval None
  */
void Chassis_setMode(ChassisMode_e mode){
	Chassis.mode = mode;
}

/**
  * @brief  设置底盘速度
  * @param[in]  mode
  * @retval None
  */
void Chassis_setSpeed(float Vx, float Vy, float VOmega, float yawRad){
	Chassis.targetXRaw = Vx;
  Chassis.targetYRaw = Vy;
  Chassis.targetZRaw = VOmega;
  Chassis.yawRad = yawRad;
}


/**
  * @brief  设置底盘中心偏差
  * @param[in]  mode
  * @retval None
  */
void Chassis_setOffset(float offsetX, float offsetY){
	Chassis.centerOffsetX = offsetX;
  Chassis.centerOffsetY = offsetY;
}



/**
  * @brief  底盘控制处理-跟随云台
  * @param[in]  Vx		x轴速度，右正左负
  *				Vy		y轴速度，前正后负
  *				Omega	自旋速度， 右正左负
  *				mode	模式 - 除Status_ControlOFF外，其他正常控制
  * @retval None
  */
void Chassis_processing(void){
  
  if(dr16_data.offLineFlag){
		Chassis.targetXRaw = Chassis.targetYRaw = Chassis.targetZRaw = 0.0f;
	}
  
  float temp_Vx = 0;
  
  switch(Chassis.mode){
    case CHASSIS_MODE_OFF:
      Chassis_setM3508Current(0, 0, 0, 0);
    return;
    
    case CHASSIS_MODE_LOCK:
      Chassis.targetXRaw = Chassis.targetYRaw = Chassis.targetZRaw = 0.0f;
    break;
    
    case CHASSIS_MODE_NORMAL:
      //不需要转换
    break;
    
		case CHASSIS_MODE_REVERSE:
			Chassis.targetXRaw=-Chassis.targetXRaw;
		  Chassis.targetYRaw=-Chassis.targetYRaw;
		  Chassis.targetZRaw=Chassis.targetZRaw;
		break;
		
    case CHASSIS_MODE_FOLLOW:
      //Chassis.yawRad = Cloud_getYawAngleWithCenter()*DEG_TO_RAD;
      temp_Vx = Chassis.targetXRaw;
      Chassis.targetXRaw = Chassis.targetXRaw*cos(Chassis.yawRad) - Chassis.targetYRaw*sin(Chassis.yawRad);
      Chassis.targetYRaw = Chassis.targetYRaw*cos(Chassis.yawRad) + temp_Vx*sin(Chassis.yawRad);
    break;
    
    default:
      break;
  }
  
  
  
  //平滑处理
	Filter_IIRLPF(&Chassis.targetXRaw, &Chassis.targetXLPF, Chassis.LpfAttFactor);
	Filter_IIRLPF(&Chassis.targetYRaw, &Chassis.targetYLPF, Chassis.LpfAttFactor);
	Filter_IIRLPF(&Chassis.targetZRaw, &Chassis.targetZLPF, Chassis.LpfAttFactor);
	
	float tempVOmega = 0.0f;
	tempVOmega = Chassis.targetZLPF;
	
	
	int16_t speed[4];
	/* 麦轮解算 */
	MecanumCalculate(Chassis.targetXLPF, Chassis.targetYLPF, tempVOmega, speed);
	
  /* PID计算  */
	for(int i = 0; i<4; i++){
		if(!Chassis.M3508s[i].offLineFlag && Chassis.M3508s[i].infoUpdateFlag){
			Chassis.M3508s[i].targetSpeed = speed[i];
			//PID计算
			Chassis.M3508s[i].outCurrent = Position_PID(&Chassis.M3508s[i].pid_speed,
			Chassis.M3508s[i].targetSpeed, Chassis.M3508s[i].realSpeed); 
			//清标志位
			Chassis.M3508s[i].infoUpdateFlag = 0;
		}
	}
	
	
	//设定电机电流值
	Chassis_setM3508Current(Chassis.M3508s[0].outCurrent, Chassis.M3508s[1].outCurrent,
	Chassis.M3508s[2].outCurrent, Chassis.M3508s[3].outCurrent);
	
  
}
/*-----------------------------------FILE OF END------------------------------*/
