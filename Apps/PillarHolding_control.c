/**
  ******************************************************************************
  * @file    PillarHolding_control.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   抱柱机构控制函数接口
  ******************************************************************************
  */
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "PillarHolding_control.h"  
#include "UpperStructure.h"
#include "Hall_IO.h"

#include "Chassis_control.h" 
#include "Cloud_control.h" 
#include "UpperStructure.h" 
#include "Lifting_control.h" 
/*-----------L O C A L - F U N C T I O N S - P R O T O T Y P E S--------------*/
PillarHolding_t PillarHolding;
/*------------------G L O B A L - F U N C T I O N S --------------------------*/
/**
  * @brief  抱柱初始化，配置参数
  * @param  None
  * @retval None
  */
void PillarHolding_Init(void){
  DYMX64AT_Init();  /* 舵机初始化 */
  DYMX64AT_setSyncTarAng(1,PILLARHOLDING_USING_SERVOID,PILLARHOLDING_SERVO_LOCK_ANGLE);
  
	
	

  }
///**
//  * @brief  抱柱初始化，配置参数
//  * @param  None
//  * @retval None
//  */
//void PillarHolding_Init(void){
// 
//     DYMX64AT_Init();  /* 舵机初始化 */
//  DYMX64AT_setSyncTarAng(1,PILLARHOLDING_USING_SERVOID,PILLARHOLDING_SERVO_LOCK_ANGLE);
//	
//  /* 夹子复位 */
//  while(!PillarHolding_setClamp(0)){//松开夹子
//    delay_ms(10);
//  }
//	
//  M3508_clearTurnCounter(&PillarHolding.M3508s[0]);
//  M3508_clearTurnCounter(&PillarHolding.M3508s[1]);
//	PillarHolding.M3508s[0].targetAngle = PillarHolding.M3508s[0].totalAngle;
//  PillarHolding.M3508s[1].targetAngle = PillarHolding.M3508s[1].totalAngle;
//  
//  /* 电机PID */
//	for(int i = 0; i < 2; i++){
//		
//    PositionPID_paraReset(&PillarHolding.M3508s[i].pid_speed, 4.0f, 0.4f, 0.0f, 16000, 16000);//速度环
//	}
//  
//  PositionPID_paraReset(&PillarHolding.pid_pos, 6.0f, 0.0f, 0.0f, 1000, 1000);//使目标速度限幅在1000
//  
//	PillarHolding.mode = PILLARHOLDING_MODE_LOCK;
//	
//}
//	

/**
  * @brief  抱柱设置夹子状态
  * @param[in]  isClamp 0松开，1夹住
  * @retval 正在执行返回0
  *         夹成功  0x11  夹失败  0x10
  *         松成功  0x21  
  */
clampState_e PillarHolding_setClamp(uint8_t isClamp){
  static uint16_t timeCounter = 0;
  static uint16_t stepCounter = 0;
  static clampState_e temp_state = CLAMP_RUNNING; /* 内部状态指示 */
  static clampState_e state = CLAMP_RUNNING; /* 返回状态指示 */
  static uint8_t last_req = 0; /* 上次请求 */
  
  if(last_req != isClamp && stepCounter == 0){
    last_req = isClamp;
  }

  if(last_req)  /* 夹住 */
  {
    switch (stepCounter)
    {
      case 0:
        
        if (Hall_PILLARHOLDING_LOCK())  /* 一开始是夹的状态 */ //霍尔传感器
        {
          state = CLAMP_CLOSE_SUCCESS;
        }
        else{
          /* 初始化参数 */
          state = CLAMP_RUNNING;
          timeCounter = 0;
          stepCounter++;
        }
        break;

      case 1:
        timeCounter++;
        /* 锁开 */
        if (timeCounter == 1)
        {
          DYMX64AT_setSyncTarAng(1, PILLARHOLDING_USING_SERVOID, PILLARHOLDING_SERVO_UNLOCK_ANGLE);
        }
        /* 延时 */
        if(timeCounter > 20){
          timeCounter = 0;
          stepCounter++;
        }
        break;

      case 2:
        /* 关夹 */
        GAS8_setBits(PILLARHOLDING_USING_GASID, PILLARHOLDING_GASPOS);
        stepCounter++;
        break;

      case 3:
        /* 等待关夹 */
        if (Hall_PILLARHOLDING_LOCK())//传感器
        {
          /* 关成功 */
          timeCounter = 0;
          temp_state = CLAMP_CLOSE_SUCCESS;
          stepCounter++;
        }
        else
        {
          timeCounter++;
          /* 关夹超时,执行失败 */
          if (timeCounter > 250)//2.5s
          {
            timeCounter = 0;
            GAS8_resetBits(PILLARHOLDING_USING_GASID, PILLARHOLDING_GASPOS);//失败就回到原位
            temp_state = CLAMP_CLOSE_FAILED;
            stepCounter++;
          }
        }

        break;
      case 4:
        /* 延时,锁夹,刷新返回状态,复位状态 */
        timeCounter++;
        if (timeCounter > 20)
        {
          timeCounter = 0;
          /* 锁夹 */
          DYMX64AT_setSyncTarAng(1, PILLARHOLDING_USING_SERVOID, PILLARHOLDING_SERVO_LOCK_ANGLE);
          stepCounter = 0;
          state = temp_state;
        }
        break;

        default:
          break;
    }
  }
  else /* 松开 */
  {
    switch (stepCounter)
    {
      case 0:
        if (!Hall_PILLARHOLDING_LOCK()) /* 一开始是松的状态 */
        {
          /* 锁关 */
          DYMX64AT_setSyncTarAng(1, PILLARHOLDING_USING_SERVOID, PILLARHOLDING_SERVO_LOCK_ANGLE);
          state = CLAMP_OPEN_SUCCESS;
        }
        else
        {
          /* 初始化参数 */
          state = CLAMP_RUNNING;
          timeCounter = 0;
          stepCounter++;
        }
        break;

      case 1:
        timeCounter++;
        /* 锁开 */
        if (timeCounter == 1)
        {
          DYMX64AT_setSyncTarAng(1, PILLARHOLDING_USING_SERVOID, PILLARHOLDING_SERVO_UNLOCK_ANGLE);
        }
        /* 延时 */
        if (timeCounter > 20)
        {
          timeCounter = 0;
          stepCounter++;
        }
        break;

      case 2:
        /* 开夹 */
        GAS8_resetBits(PILLARHOLDING_USING_GASID, PILLARHOLDING_GASPOS);
        stepCounter++;
        break;
      case 3:
        /* 延时,锁夹,刷新返回状态,复位状态 */
        timeCounter++;
        if (timeCounter > 40)
        {
          timeCounter = 0;
          /* 锁夹 */
          DYMX64AT_setSyncTarAng(1, PILLARHOLDING_USING_SERVOID, PILLARHOLDING_SERVO_LOCK_ANGLE);
          stepCounter = 0;
          state = CLAMP_OPEN_SUCCESS;
        }
        break;

      default:
        break;
    }
  }

  return state;
}
	
/**
  * @brief  设置旋转速度
  * @param[in]  speed
  * @retval None
  */
void PillarHolding_setTurnSpeed(int16_t speed){
	//PillarHolding.M3508s[0].targetSpeed = PillarHolding.M3508s[1].targetSpeed = speed;
  PillarHolding.tagetSpeed = speed;
}

	
/**
  * @brief  抱柱控制
  * @param  None
  * @retval None
  */
void PillarHolding_processing(void){
  
  PillarHolding.isLock = Hall_PILLARHOLDING_LOCK();
  
  LED_LIST(6, PillarHolding.isLock);//指示灯#
  
  if(PillarHolding.mode == PILLARHOLDING_MODE_OFF){
		PillarHolding.M3508s[0].outCurrent = PillarHolding.M3508s[1].outCurrent = 0;//失能
		return;
	}
  
  if(dr16_data.offLineFlag || PillarHolding.mode == PILLARHOLDING_MODE_LOCK){
    PillarHolding.M3508s[0].targetSpeed = PillarHolding.M3508s[1].targetSpeed = 0;//目标速度为0，用电机伺服的力锁住
	}
  
  if(PillarHolding.isStart){
    if(PillarHolding.type == PILLARHOLDING_STEPUP){
      PillarHolding_climbUpStep();//上岛
    }
    else if(PillarHolding.type == PILLARHOLDING_STEPDOWN){
     PillarHolding_climbDownStep();//下岛
    } 
    
  }

  if(PillarHolding.turnEnable){
    float turnspeed = Position_PID(&PillarHolding.pid_pos, PillarHolding.tagetAngle, Cloud.IMU.totalYaw);
    PillarHolding_setTurnSpeed(turnspeed);//抱柱的摩擦轮旋转上岛
  }
  else{
    PositionPID_clear(&PillarHolding.pid_pos);
     PillarHolding_setTurnSpeed(0);//0伺服住
  }
  
  
  if((!PillarHolding.M3508s[0].offLineFlag) && (!PillarHolding.M3508s[1].offLineFlag)){
    for(int i = 0; i < 2; i++){
      /* 角度PID计算(外环) */
      //int M3508_PIDOut = Position_PID(&Lifting.M3508s[i].pid_angle, Lifting.M3508s[i].targetAngle, Lifting.M3508s[i].totalAngle);
      /* 速度PID计算(内环) */
      PillarHolding.M3508s[i].outCurrent = Position_PID(&PillarHolding.M3508s[i].pid_speed,\
			PillarHolding.tagetSpeed, PillarHolding.M3508s[i].realSpeed);
      /* 清标志位 */    //同向转动
      PillarHolding.M3508s[i].infoUpdateFlag = 0;
    }
  }
  
  //Climbing_setM3508Current(Climbing.M3508s[0].outCurrent,Climbing.M3508s[1].outCurrent, 0,0);
  
}

/**
  * @brief  抱柱步骤：上岛
  * @param[in]  speed
  * @retval None
  */
void PillarHolding_climbUpStep(void){
  clampState_e clamp_state = CLAMP_RUNNING;
  switch (PillarHolding.nowStep)
  {
    case 0:
      if (!UpperStructrue.isOpen)//看是否在抬升状态
      {
        UpperStructure_open(Lifting.maxPos);  /* 抬升 */
      }
      else
      {
        /* 更改底盘中心 */
        Chassis_setOffset(CHASSIS_HOLDING_OFFSET_X, CHASSIS_HOLDING_OFFSET_Y);
        PillarHolding.nowStep++;
      }

      break;
    case 1:
      /* 下一步请求（由操作手发送） */
      if (PillarHolding.nextReq)
      {
        /* 底盘失能 */
        Chassis_setMode(CHASSIS_MODE_OFF);
        /* 夹柱 */
        clamp_state = PillarHolding_setClamp(1);
        if(clamp_state == CLAMP_CLOSE_SUCCESS){
          PillarHolding.nextReq = 0;
          /* 底盘使能 */
          Chassis_setMode(CHASSIS_MODE_FOLLOW);
          /* 夹成功，下一步 */
          PillarHolding.nowStep++;
        }
        else if(clamp_state == CLAMP_CLOSE_FAILED){
          /* 底盘使能 */
          Chassis_setMode(CHASSIS_MODE_FOLLOW);
          /* 夹失败，不跳转 */
          PillarHolding.nextReq = 0;//返回上一步
        }
        
      }
      else if (PillarHolding.backReq)
      {
        /* 没有上一步 */
        PillarHolding.backReq = 0;
      }
      break;

    case 2:

      if (PillarHolding.nextReq)
      {
        /* 下一步请求:抬升 */
        if (UpperStructrue.isOpen)
        {
          UpperStructure_close();
        }
        else
        {
          PillarHolding.nextReq = 0;
          /* 保存当前角度,启动旋转 */
          PillarHolding.lastAngle = Cloud.IMU.totalYaw;
          PillarHolding.tagetAngle = PillarHolding.lastAngle + 180;
          PillarHolding.turnEnable = 1;//摩擦轮和底盘同时旋转
          PillarHolding.nowStep++;
        }
      }
			
      else if (PillarHolding.backReq)
      {
        /* 操作返回上一步，松夹*/
        clamp_state = PillarHolding_setClamp(0);
        if(clamp_state == CLAMP_OPEN_SUCCESS){
          PillarHolding.backReq = 0;
          /* 松夹成功，上一步 */
          PillarHolding.nowStep--;
        }
      }
      break;

    case 3:
      /* 等待旋转180度 */
      if (abs(PillarHolding.tagetAngle - Cloud.IMU.totalYaw) < 5 &&
          abs(PillarHolding.M3508s[0].realSpeed) < 50)
      {
        PillarHolding.turnEnable = 0;
        PillarHolding_setTurnSpeed(0);
        /* 更改底盘中心 */
        Chassis_setOffset(0, 0);
        PillarHolding.nowStep++;
      }
      break;

    case 4:
      /* 操作成功，松夹 */
      clamp_state = PillarHolding_setClamp(0);
      if(clamp_state == CLAMP_OPEN_SUCCESS){
        
        PillarHolding.nowStep++;
      }
      break;
      
    case 5:
      if (!UpperStructrue.isOpen)
      {
        /* 抬升到夹取高度 */
        UpperStructure_open(Lifting.catchPos);
      }
      else
      {
        /* 动作完成 */
        PillarHolding.nowStep = 0;
        PillarHolding.nextReq = 0;
        PillarHolding.backReq = 0;
        PillarHolding.isStart = 0;
        PillarHolding.isFinish = 1;
      }
      break;

    default:

      break;
  }
}

/**
  * @brief  抱柱步骤：下岛
  * @param[in]  speed
  * @retval None
  */
void PillarHolding_climbDownStep(void){
  clampState_e clamp_state = CLAMP_RUNNING;
  switch (PillarHolding.nowStep)
  {
    case 0:
      /* 如果抬升着就下降 */
      if (UpperStructrue.isOpen)
      {
        UpperStructure_close();
      }
      else
      {
        /* 稍微抬升 */
        Lifting_setPosition(LIFTING_CLIMBDOWN_POS);//
        /* 更改底盘中心 */
        Chassis_setOffset(CHASSIS_HOLDING_OFFSET_X, CHASSIS_HOLDING_OFFSET_Y);
        PillarHolding.nowStep++;
      }
      break;
    case 1:
      /* 下一步请求 */
      if (PillarHolding.nextReq)
      {
        /* 底盘失能 */
        Chassis_setMode(CHASSIS_MODE_OFF);
        /* 夹柱 */
        clamp_state = PillarHolding_setClamp(1);
        if(clamp_state == CLAMP_CLOSE_SUCCESS){
          /* 底盘使能 */
          Chassis_setMode(CHASSIS_MODE_FOLLOW);
          /* 延时 */
          if(PillarHolding.timeCounter++>30){
            PillarHolding.timeCounter = 0;
            /* 抬升降最低 */
            Lifting_setPosition(LIFTING_DOWNLIMIT_OFFSET);
            PillarHolding.nextReq = 0;
            /* 夹成功，下一步 */
            PillarHolding.nowStep++;
          }
        }
        else if(clamp_state == CLAMP_CLOSE_FAILED){
          /* 底盘使能 */
          Chassis_setMode(CHASSIS_MODE_FOLLOW);
          /* 稍微抬升 */
          Lifting_setPosition(LIFTING_CLIMBDOWN_POS);
          /* 夹失败，不跳转 */
          PillarHolding.nextReq = 0;
        }
        
      }
      else if (PillarHolding.backReq)
      {
        /* 没有上一步 */
        PillarHolding.backReq = 0;
      }
      break;

    case 2:
      /* 下一步请求 */
      if (PillarHolding.nextReq)
      {
        /* 保存当前角度,启动旋转 */
        PillarHolding.nextReq = 0;
        PillarHolding.lastAngle = Cloud.IMU.totalYaw;
        PillarHolding.tagetAngle = PillarHolding.lastAngle + 160;
        PillarHolding.turnEnable = 1;
        PillarHolding.nowStep++;
      }
      else if (PillarHolding.backReq)
      {
        /* 稍微抬升 */
        Lifting_setPosition(LIFTING_CLIMBDOWN_POS);
        /* 操作返回上一步，松夹*/
        clamp_state = PillarHolding_setClamp(0);
        if(clamp_state == CLAMP_OPEN_SUCCESS){
          PillarHolding.backReq = 0;
          /* 松夹成功，上一步 */
          PillarHolding.nowStep--;
        }
      }
      
      break;

    case 3:
      /* 等待旋转180度 */
      if (abs(PillarHolding.tagetAngle - Cloud.IMU.totalYaw) < 10 &&
          abs(PillarHolding.M3508s[0].realSpeed) < 500)
      {
        PillarHolding.turnEnable = 0;
        PillarHolding_setTurnSpeed(0);
        /* 更改底盘中心 */
        Chassis_setOffset(0, 0);
        PillarHolding.nowStep++;
      }
      break;

    case 4:
      /* 操作成功，下降 */
      if (!UpperStructrue.isOpen)
      {
        /* 车下降（上层抬升） */
        UpperStructure_open(Lifting.catchPos);
      }
      else
      {
        PillarHolding.nowStep++;
      }
      break;
      
    case 5:
      /* 操作成功，松夹 */
      clamp_state = PillarHolding_setClamp(0);
      if(clamp_state == CLAMP_OPEN_SUCCESS){
        
        /* 动作完成 */
        PillarHolding.nowStep = 0;
        PillarHolding.nextReq = 0;
        PillarHolding.backReq = 0;
        PillarHolding.isStart = 0;
        PillarHolding.isFinish = 1;
      }
      break;

    default:

      break;
  }
}

/**
  * @brief  开始上/下岛模式
  * @param  type  上岛模式/下岛模式
  * @retval None
  */
void PillarHolding_start(PillarHoldingType_e type){
  if(!PillarHolding.isStart){
    PillarHolding.type = type;
    PillarHolding.nowStep = 0;
    PillarHolding.nextReq = 0;
    PillarHolding.backReq = 0;;
    PillarHolding.isStart = 1;
    PillarHolding.turnEnable = 0;
    PillarHolding.isFinish = 0;
    PillarHolding.isClose = 0;
  }
}

/**
  * @brief  结束上/下岛模式
  * @param   type  上岛模式/下岛模式
  * @retval None
  */
void PillarHolding_close(void){
  if(!PillarHolding.isClose){
    /* 更改底盘中心 */
    Chassis_setOffset(0, 0);
    PillarHolding_setClamp(0);
    PillarHolding.type = PILLARHOLDING_STEPUP;
    PillarHolding.nowStep = 0;
    PillarHolding.nextReq = 0;
    PillarHolding.backReq = 0;;
    PillarHolding.isStart = 0;
    PillarHolding.turnEnable = 0;
    PillarHolding.isClose = 1;
  }
}


/**
  * @brief  手动进行下一步
  * @param  None
  * @retval None
  */
void PillarHolding_nextReq(void){
  if(PillarHolding.isStart){
    PillarHolding.nextReq = 1;
  }
}


/**
  * @brief  手动返回上一步
  * @param  None
  * @retval None
  */
void PillarHolding_backReq(void){
  if(PillarHolding.isStart){
    PillarHolding.backReq = 1;
  }
}
/*-----------------------------------FILE OF END------------------------------*/


