/**
  ******************************************************************************
  * @file    BulletCatcher_control.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   夹取机构控制函数接口
  ******************************************************************************
  */
	
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "BulletCatcher_control.h"
#include "Hall_IO.h"
#include "PlayRoom_Task.h"
/*-----------L O C A L - F U N C T I O N S - P R O T O T Y P E S--------------*/
BulletCatcher_t BulletCatcher;
BulletCatcher_test_t BulletCatcher_test;
BigBank_t BigBank;
/*-------------------------- D E F I N E S -----------------------------------*/
/**
 *  夹取准备角度  倒子弹角度
 *              |    /
 *              |   /
 *              |  /
 *180°---------.----------0°
 * 夹取角度  收回角度
 */

/* 对应的增量角度，相当于初始位置，单位转成编码器*/
#define CATCHER_CLOSE_ANGLE       0                                     /* 电机初始位置（收回） */
#define CATCHER_READY_ANGLE       M3508_P19ANGLE_TO_ENCODER(160.0f)     /* 电机准备位置 （准备） */   
#define CATCHER_GETBOX_ANGLE      M3508_P19ANGLE_TO_ENCODER(350.0f)     /* 电机夹取位置 （夹取）*/ 
#define CATCHER_GETBULLET_ANGLE_PRE   M3508_P19ANGLE_TO_ENCODER(25.0f)  /* 电机倒子弹位置（倒弹1） */ 
#define CATCHER_GETBULLET_ANGLE   M3508_P19ANGLE_TO_ENCODER(40.0f)      /* 电机倒子弹位置（倒弹） */ 
#define CATCHER_ELEVATEBOX_ANGLE  M3508_P19ANGLE_TO_ENCODER(270.0f)     /* 举高弹药箱位置 */


#define CATCHER_RIGHT_POSITION    230000    /* 上层位置右 */
#define CATCHER_LEFT_POSITION     -233509   /* 上层位置左 */



/* 用到的气阀 */
#define CATCHER_USING_GASID    1                     /*ID:2*/
#define CATCHER_GASPOS          GAS_POS_4      /* 夹子气阀 */
#define CATCHER_PULS_GASPOS     GAS_POS_5  /* 夹子伸出气阀 */
#define CATCHER_BIUBIU_GASPOS   GAS_POS_3  /* 弹弹药箱气阀 */
#define CATCHER_BANK_GASPOS     GAS_POS_6  /* 大弹药箱气阀 */

#define CATCHER_ALL_GASPOS      (CATCHER_GASPOS | \
                                 CATCHER_PULS_GASPOS | \
                                 CATCHER_BIUBIU_GASPOS | \
                                 CATCHER_BANK_GASPOS)

/* 用到的舵机 */
#define CATCHER_USING_SERVOID     1           //ID：1
#define CATCHER_SERVO_MAX_ANGLE   4080        //最高角度
#define CATCHER_SERVO_MIN_ANGLE   2050        //最低角度，会打到大弹舱门
#define CATCHER_SERVO_MAGAZINE_ANGLE   2269   //该角度不影响开关弹仓门的开关
/*------------------G L O B A L - F U N C T I O N S --------------------------*/
/**
  * @brief  夹取初始化，配置参数
  * @param  None
  * @retval None
  */
void BulletCatcher_Init(void){
  /* 清空累计圈数 */	
  M3508_clearTurnCounter(&BulletCatcher.M3508s[0]);
  M3508_clearTurnCounter(&BulletCatcher.M3508s[1]);
  

  /* 角度复位 */
 BulletCatcher_resetAngle();
 BulletCatcher_resetPosition();
  
	BulletCatcher.M3508s[0].targetAngle =BulletCatcher.M3508s[0].totalAngle;
  BulletCatcher.M3508s[1].targetAngle = BulletCatcher.M3508s[1].totalAngle;

	PositionPID_paraReset(&BulletCatcher.M3508s[0].pid_speed, 4.0f, 0.1f, 0.0f, 16000, 16000);
  PositionPID_paraReset(&BulletCatcher.M3508s[0].pid_angle, 0.1f, 0.0f, 0.0f, 3000, 2500);
	PositionPID_paraReset(&BulletCatcher.M3508s[1].pid_speed, 4.0f, 0.1f, 0.0f, 16000, 16000);
  PositionPID_paraReset(&BulletCatcher.M3508s[1].pid_angle, 0.1f, 0.0f, 0.0f, 5500, 2500);
	
  BulletCatcher.mode=CATCHER_MODE_NORMAL;

	BulletCatcher.M3508s[1].targetAngle=BulletCatcher.middlePos;
	BulletCatcher.movement=CATCHER_MODE_POSITION;
	
	DYMX64AT_Init();
	DYMX64AT_setSyncTarAng(1,CATCHER_USING_SERVOID,CATCHER_SERVO_MIN_ANGLE);
	
}

/**
  * @brief  大弹丸弹仓气缸开关
  * @param  isOpen   1开，0关
  * @retval None
  */
void BulletCatcher_writeBank(void){
	if(BigBank.start){
	DYMX64AT_setSyncTarAng(1,CATCHER_USING_SERVOID,CATCHER_SERVO_MAGAZINE_ANGLE);
	 BigBank.timeCounter++;
	if(BigBank.timeCounter==20){
	
  if(!BigBank.bankOpenReq){
    GAS8_setBits(CATCHER_USING_GASID, CATCHER_BANK_GASPOS);
		BigBank.bankOpenReq=1;
  }
  else{
    GAS8_resetBits(CATCHER_USING_GASID, CATCHER_BANK_GASPOS);
		BigBank.bankOpenReq=0;
  }
 }
	if(BigBank.timeCounter==30){
		DYMX64AT_setSyncTarAng(1,CATCHER_USING_SERVOID,CATCHER_SERVO_MIN_ANGLE);
		BigBank.start=0;
		BigBank.timeCounter= 0;	
		vTaskSuspend(xHandleTaskPlayRoom);
	 }
	}
}

/**
  * @brief  直接设置3508的输出电流
  * @param  None
  * @retval None
  */

void BulletCatcher_setM3508Current(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4){

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
	
	CAN_SendData(CAN2, CAN_ID_STD, 0x200, data);
	
}





/**
  * @brief 设置夹子上M3508[0]的角度
  * @param[in]  Angle   目标角度
  * @retval 
  */
uint8_t BulletCatcher_setTargetAngle(float angle){
  BulletCatcher.M3508s[0].targetAngle = angle;//设置目标角度
  /* 爪完成动作 */
  if(abs(BulletCatcher.M3508s[0].totalAngle - BulletCatcher.M3508s[0].targetAngle) < 2000 &&
      abs(BulletCatcher.M3508s[0].realSpeed) < 50)
  {
    return 1;
  }
  return 0;
}

/**
  * @brief 判定夹子是否打到箱子上
  * @param[in]  Angle   目标角度
  * @retval 
  */
uint8_t BulletCatcher_decide(void){
  
  /* 爪完成动作 */
  if(BulletCatcher.M3508s[0].realCurrent<-3500  //低速2000时是3000，高速可以改高点
		)
  {
    return 1;
  }
  return 0;
}

/**
  * @brief  初始化重置夹取的角度，调用后夹取会收回并设置为初始位置
  * @param  None
  * @retval None
  */
void BulletCatcher_resetAngle(void){
  BulletCatcher.catcherInit = 0;
  BulletCatcher.timeCounter = 0;  /* 计数器 */
  uint16_t count = 0;   /* 设备离线用计数器 */
  
  while(BulletCatcher.catcherInit == 0)//直到复位成功再跳出循环
  {
    BulletCatcher_setM3508Current(1500,0, 0, 0);
    delay_ms(10);
    if(BulletCatcher.M3508s[0].infoUpdateFlag)
    {
      BulletCatcher.M3508s[0].infoUpdateFlag  = 0;
      count = 0;
      /* 等待电机有电流，并速度较小说明已复位 */   
      if(abs(BulletCatcher.M3508s[0].realSpeed) < 2 && abs(BulletCatcher.M3508s[0].realCurrent) > 1300)
      {
        BulletCatcher.timeCounter++;
        /* 速度为几乎为0一段时间，复位完成 */
        if(BulletCatcher.timeCounter > 50)//
        {
          BulletCatcher.nowStep = 0;
          BulletCatcher.timeCounter = 0;
          BulletCatcher.catcherInit = 1;
        }
      }
      else
      {
        BulletCatcher.timeCounter = 0;
      }
      
    }
		
    else
    {
      count++;
      /* 设备离线 */
      if(count > 200)//2秒没收到电机发来的报文视为电机离线了#
      {
        /* 电机启动后默认角度在3800左右,一般在3000-4500之间 */
        BulletCatcher.M3508s[0].totalAngle = 3800;
        BulletCatcher.timeCounter = 0;//
        BulletCatcher.nowStep = 0;
        return;
        
      }
    }
    
  }
  
  /* 电机释放 */
  BulletCatcher_setM3508Current(0, 0, 0, 0);
  delay_ms(100);
  
  /* 计算角度 */
  BulletCatcher.closePos = BulletCatcher.M3508s[0].totalAngle;
  BulletCatcher.readlyPos = BulletCatcher.closePos - CATCHER_READY_ANGLE;
  BulletCatcher.getBoxPos = BulletCatcher.closePos - CATCHER_GETBOX_ANGLE;
  BulletCatcher.getBulletPos_pre = BulletCatcher.closePos - CATCHER_GETBULLET_ANGLE_PRE;
  BulletCatcher.getBulletPos = BulletCatcher.closePos - CATCHER_GETBULLET_ANGLE;
	BulletCatcher.elevateBoxPos = BulletCatcher.closePos- CATCHER_ELEVATEBOX_ANGLE;
}




/**
  * @brief  设置夹取模式
  * @param[in]  mode
  * @retval None
  */
void BulletCatcher_setMode(BulletCatcherMode_e mode){
	BulletCatcher.mode = mode;
}


/**
  * @brief  夹取控制处理
  * @param  mode	模式 - 除Status_ControlOFF外，其他正常控制
  * @retval None
  */
void BulletCatcher_processing(void){
	
	if(BulletCatcher.mode == CATCHER_MODE_OFF){
		BulletCatcher.M3508s[0].outCurrent = 0;
		BulletCatcher.M3508s[1].outCurrent = 0;
		return;
	}
	
	  /* 夹取打开并有夹取请求 */
  if(BulletCatcher.catcherIsReady && BulletCatcher.catchRequest)
	{
	 switch(BulletCatcher.nowStep)
	 {
		 case 0:
			  BulletCatcher.catchFinish = 0;//清零夹取完成标志位
			  BulletCatcher.alignSucceed=0;//清零夹取成功标志位
		    BulletCatcher.test=0;
        /* 夹取第二排？ */
        if(BulletCatcher.catchType == CATCHER_TYPE_D2){
          if(BulletCatcher.timeCounter < 1){
            GAS8_setBits(CATCHER_USING_GASID, CATCHER_PULS_GASPOS);
						BulletCatcher.timeCounter++;
          }
					else{
						/* 第一步：延时，等待伸出气缸伸出（当取岛上弹药箱） */
						BulletCatcher.timeCounter++;        
						if(BulletCatcher.timeCounter > 10)//100ms
						{
							BulletCatcher.timeCounter = 0;
							BulletCatcher.nowStep++;  /* 跳至下一步 */
						}
					}
         
        }
				else{
            BulletCatcher.nowStep++;  /* 跳至下一步 */
        }
         
     
			break;
      case 1:
        /* 第一步：爪完成转出 */
           BulletCatcher_setTargetAngle(BulletCatcher.getBoxPos);
			
        
        /* 夹子打到弹药箱，判定夹取失败 */
          if(BulletCatcher_decide())
        {
          BulletCatcher.timeCounter++;
          if(BulletCatcher.timeCounter > 30)
          {

            BulletCatcher.timeCounter = 0;
            BulletCatcher.nowStep = 7;  /* 跳至结束 */
          }
        }
				/* 夹子能到达目标位置，判定夹取成功 */
				else if( abs(BulletCatcher.M3508s[0].realSpeed)<30&&\
							 	 abs(BulletCatcher.M3508s[0].totalAngle - BulletCatcher.M3508s[0].targetAngle)< 1500
				&&BulletCatcher.M3508s[0].realCurrent>-2000)
				{
					BulletCatcher.timeCounter++;
          if(BulletCatcher.timeCounter > 3)
          {
						
            BulletCatcher.timeCounter = 0;
						BulletCatcher.nowStep++; 
          }
					
				}
        else
        {
          BulletCatcher.timeCounter = 0;
        }
        
      break;
        
      case 2:
        /* 第二步：延时，气缸夹取 */
        BulletCatcher.timeCounter++;
        if(BulletCatcher.timeCounter > 3)
        {
          /* 气缸夹取 */
          GAS8_setBits(CATCHER_USING_GASID, CATCHER_GASPOS);
          BulletCatcher.timeCounter = 0; 
					BulletCatcher.alignSucceed=1;//判定第一箱对准成功了
          BulletCatcher.nowStep++;  /* 跳至下一步 */
        }
       
        
      break;
        
      case 3:
        /* 第三步：延时，保证夹紧 */
        BulletCatcher.timeCounter++;        
        if(BulletCatcher.timeCounter > 9)
        {
          BulletCatcher.timeCounter = 0;
          
          if(BulletCatcher.catchType == CATCHER_TYPE_D1){
            BulletCatcher.nowStep += 2;  /* 跳至下下步 */
          }
          else if(BulletCatcher.catchType == CATCHER_TYPE_D2){
            /* 夹取第二排，伸出气缸收回 */
            GAS8_resetBits(CATCHER_USING_GASID, CATCHER_PULS_GASPOS);
            BulletCatcher.nowStep++;  /* 跳至下一步 */
          }
        }
        
      break;
        
      case 4:
        /* 第四步：延时，等待伸出气缸收回（夹取第二排时） */
        BulletCatcher.timeCounter++;        
        if(BulletCatcher.timeCounter > 30)
        {
          BulletCatcher.timeCounter = 0;
          BulletCatcher.nowStep++;  /* 跳至下一步 */
        }
        
      break;
        
				
      case 5:

          /* 第四步：爪完成转入 10° */
          if(BulletCatcher_setTargetAngle(BulletCatcher.getBulletPos))
          {
						
						if(!BulletCatcher.test){ /*出车后可以试试不要延时，或者改改判定角度*/
							if(BulletCatcher.Start){
								BulletCatcher.stepNumber++;/*连续夹取：开始移动至下一箱的位置*/
								BulletCatcher.test=1;
								}
							if(BulletCatcher.robBox){/*抢夺箱子：准备好了，可以移动到下个目标的位置*/
								BulletCatcher.readyBox=1;
								BulletCatcher.test=1;
							}
						}
						
						BulletCatcher.timeCounter++;
            /* 延时（大资源岛一箱20颗弹丸，延时加长） */					
            if(BulletCatcher.timeCounter>15&&!BulletCatcher.smallIslandflag){
              BulletCatcher.timeCounter = 0;
              BulletCatcher.nowStep++;  /* 跳至下一步 */		
            }
						
            /* 延时（小资源岛一箱3颗弹丸，延时短） */						
						else if(BulletCatcher.timeCounter>5&&BulletCatcher.smallIslandflag){
              BulletCatcher.timeCounter = 0;
              BulletCatcher.nowStep++;  /* 跳至下一步 */
						}
						
          }
          /* 超时：长时间无法达到目标 */
          else if(abs(BulletCatcher.M3508s[0].realSpeed) < 5){
            BulletCatcher.timeCounter++;
            if(BulletCatcher.timeCounter > 50){
              BulletCatcher.timeCounter = 0;
              BulletCatcher.nowStep++;  /* 跳至下一步 */
							
            }
          }
          else{
            BulletCatcher.timeCounter = 0;
          }

      break;
      
      case 6:
        /* 第五步：气缸夹取松开 */
        GAS8_resetBits(CATCHER_USING_GASID, CATCHER_GASPOS);
        /* 延时 */
        BulletCatcher.timeCounter++;        
        if(BulletCatcher.timeCounter > 10){
          BulletCatcher.timeCounter = 0;
          BulletCatcher.nowStep++;  /* 跳至下一步 */
        }
      break;
      case 7:
        /* 第六步：弹弹药箱，夹子转出 */
				if(BulletCatcher.alignSucceed)
				{
					GAS8_setBits(CATCHER_USING_GASID, CATCHER_BIUBIU_GASPOS);
				}
        /* 伸收回（夹第二排失败时跳到这里能关气缸） */
        if(BulletCatcher.catchType == CATCHER_TYPE_D2){
          GAS8_resetBits(CATCHER_USING_GASID, CATCHER_PULS_GASPOS);
        }
				
        BulletCatcher_setTargetAngle(BulletCatcher.readlyPos);//回到准备位置
        /* 延时 */
        BulletCatcher.timeCounter++;        
        if(BulletCatcher.timeCounter > 25){
          BulletCatcher.timeCounter = 0;
          BulletCatcher.nowStep++;  /* 跳至下一步 */
        }
        
      break;
      
     case 8:
        /* 关弹弹药箱，完成 */
        GAS8_resetBits(CATCHER_USING_GASID, CATCHER_BIUBIU_GASPOS);
		   //连续夹三箱流程：夹取失败+不是在夹第一箱时仍进行下一步
				if(!BulletCatcher.alignSucceed&&BulletCatcher.stepNumber&&BulletCatcher.Start)
				{
				 BulletCatcher.stepNumber++;
				 BulletCatcher.alignSucceed=1;
				}
				if(!BulletCatcher.alignSucceed&&BulletCatcher.robBox)
				{
					BulletCatcher.readyBox=1;
				}
				
		    BulletCatcher.nowStep = 0;
				BulletCatcher.catcherIsReady = 0;//用RC控制夹取时再解除注释
				BulletCatcher.catchRequest = 0;
				BulletCatcher.catchFinish = 1;

				
     break;
      
      default:
        BulletCatcher_open();
      break;
	 }
	
	}
		/* 角度PID计算(外环) */
	int M3508_PIDOut = Position_PID(&BulletCatcher.M3508s[0].pid_angle, \
	 BulletCatcher.M3508s[0].targetAngle,BulletCatcher.M3508s[0].totalAngle);
	/* 速度PID计算(内环) */
	BulletCatcher.M3508s[0].outCurrent =Position_PID(&BulletCatcher.M3508s[0].pid_speed,\
	 M3508_PIDOut, BulletCatcher.M3508s[0].realSpeed);
	
}

/**
  * @brief  夹取打开，初始化参数
  * @param  None
  * @retval 打开成功后返回1，否则返回0
  */
uint8_t BulletCatcher_open(void){
  /* 爪完成转出 */

    BulletCatcher_setTargetAngle(BulletCatcher.readlyPos);
    GAS8_resetBits(CATCHER_USING_GASID, CATCHER_GASPOS |
                                        CATCHER_PULS_GASPOS |
                                        CATCHER_BIUBIU_GASPOS);
    BulletCatcher.timeCounter = 0;
    BulletCatcher.catchFinish = 0;
		BulletCatcher.alignSucceed = 0;
    BulletCatcher.catchRequest = 0;
    BulletCatcher.nowStep = 0;
    BulletCatcher.catcherIsReady = 1;
		BulletCatcher.robBox=0;
		BulletCatcher.readyBox=0;
		BulletCatcher.robCount=0;
		BulletCatcher.Island=0;
	  BulletCatcher.stepNumber=0;
		BulletCatcher.smallIslandflag=0;
	
	if(BulletCatcher_setTargetAngle(BulletCatcher.readlyPos)&&\
		BulletCatcher_setPosition(BulletCatcher.middlePos))
	{
    return 1;
	}
	else{
	  return 0;
	}
}

/**
  * @brief  夹取关闭，收回
  * @param  None
  * @retval 关闭成功后返回1
  */
uint8_t BulletCatcher_close(void){
  /* 爪完成转入 */
//  if(BulletCatcher_setTargetAngle(CATCHER_IN_ANGLE))
//  {

	GAS8_resetBits(CATCHER_USING_GASID, CATCHER_GASPOS |
																			CATCHER_PULS_GASPOS |
																			CATCHER_BIUBIU_GASPOS);
	BulletCatcher.timeCounter = 0;
	BulletCatcher.catchFinish = 1;
	BulletCatcher.catchRequest = 0;
	BulletCatcher.nowStep = 0;
	BulletCatcher.catcherIsReady = 0;
	BulletCatcher.stepNumber=0;
	BulletCatcher.Start=0;
	BulletCatcher.timeCounter=0;
	BulletCatcher.robBox=0;
	BulletCatcher.Island=0;
	BulletCatcher.smallIslandflag=0;	
	
	if(BulletCatcher_setPosition(BulletCatcher.middlePos)&&\
		BulletCatcher_setTargetAngle(BulletCatcher.closePos))//因车没装防护，暂时换了个角度代替
	{
    return 1;
  }
	else{
  return 0;
	}
}


/**
  * @brief  手动夹取
  * @param  None
  * @retval 
  */
//下面这两个函数是手动夹取!!!

void BulletCatcher_testOpenReq(void){
  if(!BulletCatcher_test.isOpen){
   BulletCatcher_setTargetAngle(BulletCatcher.getBoxPos);
		BulletCatcher_test.timecount++;
		
		if(BulletCatcher_test.timecount>50){
    GAS8_setBits(CATCHER_USING_GASID, CATCHER_GASPOS);
    BulletCatcher_test.isOpen = 1;
    BulletCatcher_test.open_req = 0;
			BulletCatcher_test.timecount=0;
	 }
  }
}

void BulletCatcher_testCloseReq(void){
  if(BulletCatcher_test.isOpen){
   BulletCatcher_setTargetAngle(BulletCatcher.getBulletPos);
		BulletCatcher_test.timecount++;
		if(BulletCatcher_test.timecount>120){
    GAS8_resetBits(CATCHER_USING_GASID,CATCHER_GASPOS);
		BulletCatcher_test.timecount=0;
    BulletCatcher_test.isOpen = 0;
    BulletCatcher_test.close_req = 0;
		}
  }
}

/**
  * @brief  夹子移动方式
  * @param  None
  * @retval 
  */
void BulletCatcher_move(void)
{
	if(BulletCatcher.mode == CATCHER_MODE_OFF){
		BulletCatcher.M3508s[1].targetAngle = BulletCatcher.M3508s[1].totalAngle;
		BulletCatcher.M3508s[1].outCurrent=0;
		return;
	}
	float Vx = 0;
 	 switch(BulletCatcher.movement){
    
    case CATCHER_MODE_MANUAL:   /* 手动模式 */ 
      BulletCatcher.M3508s[1].targetAngle = BulletCatcher.M3508s[1].totalAngle;
      Vx = BulletCatcher.M3508s[1].targetSpeed;//给予指定速度
    break;
    
    case CATCHER_MODE_POSITION:   /* 定点模式 */
      Vx = Position_PID(&BulletCatcher.M3508s[1].pid_angle,
  		BulletCatcher.M3508s[1].targetAngle,BulletCatcher.M3508s[1].totalAngle);//转到指定点
    break;
    
    default:
      
    break;
		
  }
  if(!BulletCatcher.M3508s[1].offLineFlag){
		
		BulletCatcher.M3508s[1].outCurrent = Position_PID(&BulletCatcher.M3508s[1].pid_speed, \
		Vx, BulletCatcher.M3508s[1].realSpeed);
		/* 清标志位 */
		BulletCatcher.M3508s[1].infoUpdateFlag = 0;
		
    }
		
 }


 /**
  * @brief  设置夹子左右移动的速度
  * @param  speed 设置速度大小
  * @retval 
  */
void BulletCatcher_setSpeed(float speed)
{
	BulletCatcher.M3508s[1].targetAngle = BulletCatcher.M3508s[1].totalAngle;
	BulletCatcher.M3508s[1].targetSpeed=speed;
	
}


 /**
  * @brief  控制夹子左右移动的电机初始化
  * @param  None
  * @retval 
  */
void BulletCatcher_resetPosition(void)
{
	
	BulletCatcher.catcherInit = 0;
	BulletCatcher.timeCounter = 0;  /* 计数器 */
	uint16_t count = 0;   /* 设备离线用计数器 */
	uint8_t veer=0;
	int8_t mun=0;
	while(!BulletCatcher.catcherInit){

		if(!veer){
			BulletCatcher_setM3508Current(0,1300, 0, 0);
				mun++;
		}
		else{
			BulletCatcher_setM3508Current(0,-1300, 0, 0);
				mun--;
		}
    delay_ms(10);
		
    if(BulletCatcher.M3508s[1].infoUpdateFlag)
    {
			BulletCatcher.M3508s[1].infoUpdateFlag = 0;
      count = 0;

        /*转向*/
        if(mun>60)
        {
          veer =1;
        }
				if(mun<-60)
				{
					veer=0;
				}
			
		BulletCatcher.resetPositionFinish=Hall_PILLARHOLDING_LOCK();
			/*达到光电门所在的位置*/
		if(BulletCatcher.resetPositionFinish)
		{
				BulletCatcher_setM3508Current(0,0,0,0);//释放电机
	 			/* 计算角度 */
				BulletCatcher.catcherInit=1;
        BulletCatcher.middlePos=BulletCatcher.M3508s[1].totalAngle;
				BulletCatcher.leftPos=BulletCatcher.middlePos+CATCHER_LEFT_POSITION;
				BulletCatcher.rightPos=BulletCatcher.middlePos+CATCHER_RIGHT_POSITION;
		}

		 }
		
		 else
    {
      count++;
      /* 设备离线 */
      if(count > 200)
      {
        /* 电机启动后默认角度在3800左右 */
        BulletCatcher.M3508s[1].totalAngle = 3800;
        BulletCatcher.timeCounter = 0;
        BulletCatcher.nowStep = 0;
        return;
      }
	  }
  }
	delay_ms(100);


}

 /**
  * @brief  设置夹子定点位置
  * @param  angle 目标角度
  * @retval 
  */
uint8_t BulletCatcher_setPosition(float angle){

  BulletCatcher.M3508s[1].targetAngle = angle;//设置目标角度

  /* 检测爪完成动作 */
  if(abs(BulletCatcher.M3508s[1].totalAngle - BulletCatcher.M3508s[1].targetAngle) < 2000 &&
      abs(BulletCatcher.M3508s[1].realSpeed) < 50)//检测是否转到目标角度，接近目标角度且real速度小#
  {
    return 1;
  }
  return 0;
}

 /**
  * @brief  连续夹三箱流程
  * @param  None
  * @retval 
  */
void BulletCatcher_threeProcessing(void)
{
	static uint16_t count = 0;
	if(BulletCatcher.Start){
	switch(BulletCatcher.stepNumber)
	{
		case 0:
			/*第一箱夹取中...*/
		break;
		
		case 1:
			if(BulletCatcher.alignSucceed){
			if (BulletCatcher_setPosition(BulletCatcher.leftPos)){
				count++;
				if(count>5&&BulletCatcher.catchFinish){
					count=0;
					BulletCatcher.stepNumber++;
					/*夹取请求还有夹取第几排*/
					BulletCatcher.catchType = CATCHER_TYPE_D1;
					BulletCatcher.catchRequest = 1;
					BulletCatcher.catcherIsReady = 1;//
				}
			}
	}
			/*第一箱没有对准，重新来过*/
			else{
			BulletCatcher.Start=0;
			BulletCatcher.stepNumber=0;
			}
			break;
		
		case 2:
			/*第二箱夹取中...*/
		break;
		
		case 3:
			 if (BulletCatcher_setPosition(BulletCatcher.rightPos)){
				count++;
				if(count>5&& BulletCatcher.catchFinish){
					count=0;
					BulletCatcher.stepNumber++;
					/*夹取请求置1还有夹取第几排*/
					BulletCatcher.catchType = CATCHER_TYPE_D1;
					BulletCatcher.catchRequest =  1;//请求
					BulletCatcher.catcherIsReady = 1;//准备
						}
					}
			 break;
			
		case 4:
			/*第三箱夹取中...*/
		break;
		
		case 5:
			/*回到原处*/
			if (BulletCatcher_setPosition(BulletCatcher.middlePos)){
			 count++;
			 if(count>5&& BulletCatcher.catchFinish){
				count=0;
				BulletCatcher.stepNumber=0;
				BulletCatcher.smallIslandflag=0;				 
				BulletCatcher.Start=0;

			//	BulletCatcher.catchFinish=0;
				//BulletCatcher.catcherIsReady = 0;//准备 用rc控制时解除注释#
					
				}
			}
			 break;
			
	}
	
 }
}

 /**
* @brief  资源岛抢弹药箱流程
  * @param  None
  * @retval 
  */
void BulletCatcher_lootProcessing(void)
{
	if(BulletCatcher.robBox)
	{
		switch(BulletCatcher.Island)
		{

			
			case 0:
				/*夹取第一排中间位置的弹药箱*/
				if(BulletCatcher.readyBox)
				{
					if(BulletCatcher_setPosition(BulletCatcher.middlePos)&&BulletCatcher.catchFinish)
					{
							BulletCatcher.robCount++;
							if(BulletCatcher.robCount>5)
							{
								BulletCatcher.catchType = CATCHER_TYPE_D1;
								BulletCatcher.catchRequest = 1;
								BulletCatcher.catcherIsReady = 1;//
								BulletCatcher.readyBox=0;//2020.7.23加
								BulletCatcher.robCount=0;
								BulletCatcher.Island++;
							}
					}
				}
				break;
				
			case 1:
				break;
			
			case 2:
				/*夹取第二排中间位置弹药箱*/
				if(BulletCatcher.readyBox)
				{
					if(BulletCatcher_setPosition(BulletCatcher.middlePos)&&BulletCatcher.catchFinish)
					{
								BulletCatcher.robCount++;
								if(BulletCatcher.robCount>5)
								{
									BulletCatcher.catchType = CATCHER_TYPE_D2;
									BulletCatcher.catchRequest = 1;
									BulletCatcher.catcherIsReady = 1;//用PC端控制就可以把这个删除掉
									BulletCatcher.readyBox=0;
									BulletCatcher.robCount=0;
									BulletCatcher.Island++;	
								}
					}
					else{
						BulletCatcher.robCount=0;
					}
			}
				break;

			case 3:
				break;
			
			case 4:
				/*夹取第二排左边位置弹药箱*/
				if(BulletCatcher.readyBox)
				{				
					if(BulletCatcher_setPosition(BulletCatcher.leftPos)&&BulletCatcher.catchFinish)
						{
								BulletCatcher.robCount++;
								if(BulletCatcher.robCount>5)
								{
									BulletCatcher.catchType = CATCHER_TYPE_D2;
									BulletCatcher.catchRequest = 1;
									BulletCatcher.catcherIsReady = 1;//
									BulletCatcher.readyBox=0;
									BulletCatcher.robCount=0;
									BulletCatcher.Island++;	
								}
						}
					else{
						BulletCatcher.robCount=0;
					}
				}
				break;

			case 5:
				break;
			
			case 6:
				/*夹取第二排右边位置弹药箱*/
				if(BulletCatcher.readyBox)
				{				
					if(BulletCatcher_setPosition(BulletCatcher.rightPos)&&BulletCatcher.catchFinish)
						{
								BulletCatcher.robCount++;
								if(BulletCatcher.robCount>5)
								{
									BulletCatcher.catchType = CATCHER_TYPE_D2;
									BulletCatcher.catchRequest = 1;
									BulletCatcher.catcherIsReady = 1;//
									BulletCatcher.readyBox=0;
									BulletCatcher.Island++;				
									BulletCatcher.robCount=0;
								}
						}
				 else{
						BulletCatcher.robCount=0;
			  	 }
				}
			break;

			case 7:
				break;
			
		}			
	}
	
}

/**
  * @brief  PC端中夹取控制
  * @param  mode 开启哪个夹取模式
  * @retval None
  */
void BulletCatcher_begin(GetBoxMode_e mode)
{
 BulletCatcher.getBoxMode=mode;
	
 switch(mode)
 {
	 case CATCHER_BOX_THREE:
		 /*小资源岛夹三箱*/
		BulletCatcher.Start=1;
		BulletCatcher.catchType = CATCHER_TYPE_D1;
		BulletCatcher.catchRequest = 1;
		BulletCatcher.catcherIsReady = 1;
	  BulletCatcher.smallIslandflag=1;
		BulletCatcher.stepNumber=0;
	  break;
	 
	 case CATCHER_BOX_TWO:
		 /*大资源岛四角*/
		BulletCatcher.Start=1;
		BulletCatcher.catchFinish=1;
		BulletCatcher.stepNumber=1;
		BulletCatcher.alignSucceed=1;
	  BulletCatcher.smallIslandflag=0;
	  break;
	 
	 case CATCHER_BOX_SINGLE:
		 /*单项赛*/
		BulletCatcher.Start=1;
		BulletCatcher.catchType = CATCHER_TYPE_D2;
		BulletCatcher.catchRequest = 1;
		BulletCatcher.catcherIsReady = 1;
		BulletCatcher.stepNumber=0;
	  BulletCatcher.smallIslandflag=0;
	  break;
	 
	 /*大资源岛争夺弹药箱*/
	 case CATCHER_BOX_MID1:
		 /*夹取第一排中间位置的弹药箱*/
		BulletCatcher.Island=0;
		if(!BulletCatcher.robBox)
		{
			BulletCatcher.readyBox=1;
			BulletCatcher.robBox=1;
			BulletCatcher.catchFinish=1;
		}
		 break;
	 
	 case CATCHER_BOX_MID2:
		 /*夹取第二排中间位置的弹药箱*/
		BulletCatcher.Island=2;
		if(!BulletCatcher.robBox)
		{
			BulletCatcher.readyBox=1;
			BulletCatcher.robBox=1;
			BulletCatcher.catchFinish=1;
		}
		 break;
	 
	 case CATCHER_BOX_LEFT2:
		 /*夹取第二排左边的弹药箱*/
		BulletCatcher.Island=4;
		if(!BulletCatcher.robBox)
		{
			BulletCatcher.readyBox=1;
			BulletCatcher.robBox=1;
			BulletCatcher.catchFinish=1;
		}	 
		 break;
	 
	 case CATCHER_BOX_RIGHT2:
		 /*夹取第二排右边的弹药箱*/
		BulletCatcher.Island=6;
		if(!BulletCatcher.robBox)
		{
			BulletCatcher.readyBox=1;
			BulletCatcher.robBox=1;
			BulletCatcher.catchFinish=1;
		}
	 break;
	 
	 
	 default:
    break;
	 
 }
	
	
}



/**
  * @brief  用于摆摊招新挥手
  * @param  None
  * @retval None
  */
void BulletCatcher_wave(void)
{
	static uint8_t time=0;
	static uint8_t stepNumber=0;
	
	switch(stepNumber)
	{
		case 0:
			if(BulletCatcher_setTargetAngle(BulletCatcher.getBulletPos))
			{
				time++;
				if(time==20)
				{
					stepNumber++;
					time=0;
				}
				
			}
	break;
			
		case 1:
       	if(BulletCatcher_setTargetAngle(BulletCatcher.getBulletPos_pre))
			{
				time++;
				if(time==20)
				{
					stepNumber=0;
					time=0;
				}
			}
		break;
		
  }
}


/*-----------------------------------FILE OF END------------------------------*/
