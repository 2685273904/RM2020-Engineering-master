/**
  ******************************************************************************
  * @file    Command_Task.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   操作命令解析控制任务，用于底盘，云台等控制命令分发
  * @Note  
  ******************************************************************************
  */
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "Command_Task.h"
#include "UpperStructure.h"
#include "Lifting_control.h"
#include "BulletCatcher_control.h"
#include "Cloud_control.h"
#include "Chassis_control.h"
#include "Magazine_control.h"
#include "GAS_8.h"
#include "Robot_control.h"
#include "PlayRoom_Task.h"
/*-----------L O C A L - F U N C T I O N S - P R O T O T Y P E S--------------*/
TaskHandle_t xHandleTaskCommand = NULL;
EventGroupHandle_t xCommandEventGroup = NULL;

ControlStatus_t ControlStatus;
float Chassis_Vx, Chassis_Vy, Chassis_Vz;
/* 缓冲 */
#define KEY_ACC_TIME     500  //按键后500ms内速度系数从0->1的提升
ramp_t fb_ramp = RAMP_GEN_DAFAULT;
ramp_t lr_ramp = RAMP_GEN_DAFAULT;
ramp_t cw_ramp = RAMP_GEN_DAFAULT;

uint16_t offTimeCounter = 0;
int16_t cloud_delta_yaw = 0;


/*------------------G L O B A L - F U N C T I O N S --------------------------*/

/**
  * @brief  组合键判断处理 
  * @param  None
  * @retval None
  */
void Control_combineKeyScan(void)
{
  //如果仅有Ctrl或shift键先按下，并且没有其他键按下时判断为组合键
  if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_CTRL, KEY_FULL_MATCH)){
    ControlStatus.isCombineKey = CombineKey_Ctrl;
  }
  else if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_SHIFT, KEY_FULL_MATCH)){
    ControlStatus.isCombineKey = CombineKey_Shift;
  }
  else if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_CTRL|KEYBOARD_PRESSED_SHIFT, KEY_FULL_MATCH)){
    ControlStatus.isCombineKey = CombineKey_Ctrl_Shift;
  }
	else if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_NONE, KEY_FULL_MATCH)){
			//如果组合键全部松开则关闭组合键
			ControlStatus.isCombineKey = CombineKey_None;
	}
	
}

/**
  * @brief  非组合键处理 
  * @param  None
  * @retval None
  */
void Control_notCombineKeyHandle(void)
{  
  /* 在正常模式按shift加速，ctrl减速 */
  if(ControlStatus.SpecialMode == SpecialMode_Normal){
    if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_SHIFT, KEY_HAVE_MATCH)){
      Chassis.speedLimit = CHASSIS_SPEED_FAST;
    }
    else if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_CTRL, KEY_HAVE_MATCH)){
      Chassis.speedLimit = CHASSIS_SPEED_SLOW;
    }
    else{
      Chassis.speedLimit = CHASSIS_SPEED_NORMAL;
    }
  }
  
  /* 前后左右自旋 */
  if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_W,KEY_HAVE_MATCH )){
    Chassis_Vy = Chassis.speedLimit*ramp_calc(&fb_ramp);//前
  }
  else if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_S, KEY_HAVE_MATCH)){
    Chassis_Vy = -Chassis.speedLimit*ramp_calc(&fb_ramp);//后
  }
  else{
    Chassis_Vy = 0;
    ramp_init(&fb_ramp, KEY_ACC_TIME/10);
  }
  
  if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_D, KEY_HAVE_MATCH)){
    Chassis_Vx = Chassis.speedLimit*ramp_calc(&lr_ramp);//右
  }
  else if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_A, KEY_HAVE_MATCH)){
    Chassis_Vx = -Chassis.speedLimit*ramp_calc(&lr_ramp);//左
  }
  else{
    Chassis_Vx = 0;
    ramp_init(&lr_ramp, KEY_ACC_TIME/10);
  }
  
  if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_E, KEY_HAVE_MATCH)){
    Chassis_Vz = Chassis.speedLimit*ramp_calc(&cw_ramp);//自旋E
  }
  else if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_Q, KEY_HAVE_MATCH)){
    Chassis_Vz = -Chassis.speedLimit*ramp_calc(&cw_ramp);//反向自旋Q
  }
  else{
    Chassis_Vz = 0;
    ramp_init(&cw_ramp, KEY_ACC_TIME/10);
  }
  
}
/**
  * @brief  键处理 
  * @param  None
  * @retval None
  */
void Control_KeyHandle(void)
{
//		static int counter = 0;

  switch(ControlStatus.isCombineKey){
    /* ---------------------------------Ctrl组合键-Begin------------------------------------- */
    case CombineKey_Ctrl:
      
      /* 减速运动 */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_W |
                                        KEYBOARD_PRESSED_S |
                                        KEYBOARD_PRESSED_A |
                                        KEYBOARD_PRESSED_D,
                                        KEY_HAVE_MATCH))
      {
         if(ControlStatus.SpecialMode == SpecialMode_Normal){
            ControlStatus.isCombineKey = CombineKey_None;
         }
      }
      
       /* 正常模式 Ctrl+Q */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_Q, KEY_FULL_MATCH)){
        Chassis.speedLimit = CHASSIS_SPEED_NORMAL;
        ControlStatus.SpecialMode = SpecialMode_Normal;
       // Cloud.inventFB = 0;
      }
      
      /* 夹取模式 Ctrl+E */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_E, KEY_FULL_MATCH)){
          ControlStatus.SpecialMode = SpecialMode_Supply;
          Chassis.speedLimit = CHASSIS_SPEED_SLOW;
				  
					if(UpperStructrue.isOpen)
					{
						BulletCatcher_open();  /*复位功能：防止出意外*/
					}
          //Cloud.inventFB = 1;
      }
			
			/*大资源岛四角Ctrl+F （寒假）*/ 
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_F, KEY_FULL_MATCH)){
				if((ControlStatus.SpecialMode == SpecialMode_Supply) && UpperStructrue.isOpen){
						BulletCatcher_begin(CATCHER_BOX_TWO);
				}
      }
			
//       /*看屏幕模式 Ctrl+X */
//			if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_X, KEY_FULL_MATCH)){
//				ControlStatus.SpecialMode = SpecialMode_LookTv;
//				Chassis.speedLimit = CHASSIS_SPEED_SLOW;
//				
//				
//			}
			
      /* 关闭小弹仓 Ctrl+G */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_G, KEY_FULL_MATCH)){
					Magazine.close=1;
      }

      /* 转头 Ctrl+V */
//      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_V,  KEY_FULL_MATCH)){
//				
//        if(Cloud.inventFB){
//          Cloud.inventFB = 0;
//        }  
//        else{
//          Cloud.inventFB = 1; 
//        }
//        
//      }
      
      /* 关大弹丸弹仓 Ctrl+R */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_R, KEY_FULL_MATCH)){
         if(ControlStatus.SpecialMode == SpecialMode_Supply || ControlStatus.SpecialMode == SpecialMode_Normal){
            //BulletCatcher_writeBank(0);
         }
      }
    break;
    /* ---------------------------------Ctrl组合键-End--------------------------------------- */
      
    /* ---------------------------------Shitf组合键-Begin------------------------------------ */
    case CombineKey_Shift:
      
      /* 加速运动 */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_W |
                                        KEYBOARD_PRESSED_S |
                                        KEYBOARD_PRESSED_A |
                                        KEYBOARD_PRESSED_D,
                                        KEY_HAVE_MATCH))
      {
         if(ControlStatus.SpecialMode == SpecialMode_Normal){
            ControlStatus.isCombineKey = CombineKey_None;
         }
      }
			
        /* 小资源岛连续夹三箱 Shift+F */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_F, KEY_FULL_MATCH)){

          if((ControlStatus.SpecialMode == SpecialMode_Supply) && UpperStructrue.isOpen){

							BulletCatcher_begin(CATCHER_BOX_THREE);
          }
				}
			
			if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_C, KEY_FULL_MATCH)){
         if(ControlStatus.SpecialMode == SpecialMode_Supply || ControlStatus.SpecialMode == SpecialMode_Normal){
          Magazine.nextReq=1;
         }
      }
//      /*  Shift+R */
//      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_R, KEY_FULL_MATCH)){
//         
//      }
//   
    break;
    /* ---------------------------------Shitf组合键-End-------------------------------------- */
    
    
    /* ---------------------------------非组合键-Begin--------------------------------------- */
    case CombineKey_None:
      Control_notCombineKeyHandle();
    
      /* 夹取，取第一排中间弹药箱 F */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_F, KEY_FULL_MATCH)){
          if((ControlStatus.SpecialMode == SpecialMode_Supply) && UpperStructrue.isOpen){
							BulletCatcher_begin(CATCHER_BOX_MID1);
          }
          
					
      }
			
		  /* 夹取，取第二排左侧弹药箱 Z */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_Z, KEY_FULL_MATCH)){
          if((ControlStatus.SpecialMode == SpecialMode_Supply) && UpperStructrue.isOpen){
							BulletCatcher_begin(CATCHER_BOX_LEFT2);
          }
				}
			
			/* 夹取，取第二排中间弹药箱 X */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_X, KEY_FULL_MATCH)){
          if((ControlStatus.SpecialMode == SpecialMode_Supply) && UpperStructrue.isOpen){
							BulletCatcher_begin(CATCHER_BOX_MID2);
          }
				}
     
			/* 夹取，取第二排右侧弹药箱 C */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_C, KEY_FULL_MATCH)){
          if((ControlStatus.SpecialMode == SpecialMode_Supply) && UpperStructrue.isOpen){
							BulletCatcher_begin(CATCHER_BOX_RIGHT2);
          }
				}			
			
      /* 开大弹丸弹仓 R */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_R, KEY_FULL_MATCH)){
         if(ControlStatus.SpecialMode == SpecialMode_Supply || ControlStatus.SpecialMode == SpecialMode_Normal){
            //BulletCatcher_writeBank(1);
					 BigBank.start=1;
					 vTaskResume(xHandleTaskPlayRoom);
         }
      }
			 /* 开小弹丸弹仓伸缩 G */
			if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_G, KEY_FULL_MATCH)){
//         if(ControlStatus.SpecialMode == SpecialMode_Supply || ControlStatus.SpecialMode == SpecialMode_Normal){
          Magazine.open=1;
					vTaskResume(xHandleTaskPlayRoom);
//         }
      }

      /* 开拖车 鼠标左 */
      if(dr16_mouseKeyLeft.flag.keyPressedJump){
        if(ControlStatus.SpecialMode == SpecialMode_Normal){
          GAS8_setBits(1, GAS_POS_4);
        }
      }
      
      
      /* 关拖车 鼠标右 */
      if(dr16_mouseKeyRight.flag.keyPressedJump){
        if(ControlStatus.SpecialMode == SpecialMode_Normal){
          GAS8_resetBits(1, GAS_POS_4);
        }
      }
      
//			        /* 按v云台动，底盘不动, 手动环顾四周 */
//        if(dr16_data.keyBoard.press_V){
//          ControlStatus.cloud_look = 1;
//          cloud_delta_yaw = dr16_data.mouse.x*6.0f;
//          
//        }
//			    else{
//          ControlStatus.cloud_look = 0;
//          cloud_delta_yaw = 0;
//					}
//					

//        
			
    break;
    /* ---------------------------------非组合键-End----------------------------------------- */
    default:
      break;
    
  
  
 }
}
/**
  * @brief  控制命令任务
  * @param  pvParameters 任务被创建时传进来的参数
  * @retval None
  */
void vTaskCommand(void *pvParameters)
{
	TickType_t xLastWakeTime;
	/* 获取当前的系统时间 */
	xLastWakeTime = xTaskGetTickCount();
	static int counter = 0;
	 // int16_t cloud_delta_yaw = 0;
	while(1){
			Chassis_Vx = 0; Chassis_Vy = 0; Chassis_Vz = 0;
			DR16_KeyProcessing();
		/* ---------------------控制模式(左摇杆)-Begin------------------- */
	switch(dr16_data.rc.s_left)
		{
			case  DR16_SWITCH_UP:
				/* -------------------电脑控制-Begin------------------------- */
  	//	Cloud.mode=CLOUD_MODE_OFF;
//			Chassis_setMode(CHASSIS_MODE_LOCK);//停下
		
				ControlStatus.Controlby = Control_ByPC;
				/*使能机器人*/
				Robot_Enable();
        /* 按键处理 */
        Control_combineKeyScan();
        Control_KeyHandle();
        
        

//					if(ControlStatus.cloud_look == 0){
//					if(ControlStatus.SpecialMode == SpecialMode_LookTv){
//						Cloud.targetYawRaw =Cloud_Yaw_TV;
//					}
//					else{
//          if(Cloud.inventFB){
//            Cloud.targetYawRaw = Cloud_Yaw_CenterINV;
//          }
//          else{
//            Cloud.targetYawRaw = Cloud_Yaw_Center;
//          }
//				 }	
//			  }
					
        /* 平时鼠标控制底盘自旋 */
        if(!ControlStatus.cloud_look){
          Chassis_Vz += dr16_data.mouse.x*80.0f;
        }
        
       Chassis_setSpeed(Chassis_Vx, Chassis_Vy, Chassis_Vz, Cloud_getYawAngleWithCenter() * DEG_TO_RAD); 
        
//        Cloud_setDeltaPos(cloud_delta_yaw, -dr16_data.mouse.y*1.50f);
			
			break;
			/* -------------------电脑控制-End--------------------------- */
				
			case DR16_SWITCH_MID:
				ControlStatus.Controlby = Control_ByRC;
		 /* ---------------------RC控制-Begin-------------------------- */
			 Chassis_setOffset(0,0);
				if(dr16_data.rc.s_right==DR16_SWITCH_UP)
				{
					BulletCatcher.movement=CATCHER_MODE_POSITION;
					Lifting_setPosition(Lifting.minPos);
					BulletCatcher_close();
					BulletCatcher_setMode(CATCHER_MODE_NORMAL);
					Chassis_setMode(CHASSIS_MODE_NORMAL );
					Chassis_setSpeed((float)dr16_data.rc.ch2_LX / DR16_ROCKER_MAXVALUE * CHASSIS_SPEED_NORMAL,
					(float)dr16_data.rc.ch3_LY / DR16_ROCKER_MAXVALUE * CHASSIS_SPEED_NORMAL,
					(float)dr16_data.rc.ch0_RX / DR16_ROCKER_MAXVALUE * CHASSIS_SPEED_NORMAL,
					0);							
				}
					if(dr16_data.rc.s_right==DR16_SWITCH_MID)
				{
					BulletCatcher_setMode(CATCHER_MODE_NORMAL);
					Chassis_setMode(CHASSIS_MODE_NORMAL );
					BulletCatcher.movement=CATCHER_MODE_POSITION;					
					Chassis_setSpeed((float)dr16_data.rc.ch2_LX / DR16_ROCKER_MAXVALUE * CHASSIS_SPEED_NORMAL,
					(float)dr16_data.rc.ch3_LY / DR16_ROCKER_MAXVALUE * CHASSIS_SPEED_NORMAL,
					(float)dr16_data.rc.ch0_RX / DR16_ROCKER_MAXVALUE * CHASSIS_SPEED_NORMAL,
					0);
					
					Lifting_setMode(LIFTING_MODE_POSITION);
					Lifting_setPosition(Lifting.catchPos);//抬高到夹取高度
					
//					
//					UpperStructure_open(Lifting.catchPos);
					
//					Lifting_setMode(LIFTING_MODE_MANUAL);
//					Lifting.targetSpeed=(float)dr16_data.rc.ch1_RY*3;/*手动抬升*/
				

					if(dr16_data.rc.ch4_DW==-660){
					BulletCatcher_setPosition(BulletCatcher.leftPos);
					
					}
					if(dr16_data.rc.ch4_DW==660){
					BulletCatcher_setPosition(BulletCatcher.rightPos);
					}
					
					if(dr16_data.rc.ch4_DW==0){
						BulletCatcher_setPosition(BulletCatcher.middlePos);
						
					}
					
					
					
					}
			if(dr16_data.rc.s_right==DR16_SWITCH_DOWN )
			{
				
         BulletCatcher_setMode(CATCHER_MODE_NORMAL);
					Chassis_setMode(CHASSIS_MODE_NORMAL );
				
					BulletCatcher.movement=CATCHER_MODE_MANUAL;
				BulletCatcher_setSpeed(dr16_data.rc.ch0_RX*3);//手动左右
				
					Chassis_setSpeed((float)dr16_data.rc.ch2_LX / DR16_ROCKER_MAXVALUE * CHASSIS_SPEED_NORMAL,
					(float)dr16_data.rc.ch3_LY / DR16_ROCKER_MAXVALUE * CHASSIS_SPEED_NORMAL,
					0,0);
				
			}
			
			break;
		  /* ---------------------RC控制-End-------------------------- */
			case DR16_SWITCH_DOWN:

			/* -------------------失能控制-Begin------------------------ */
				/*失能机器人*/
			ControlStatus.Controlby = Control_OFF;
			Robot_Disability();	
			break;
		}
		 /* ---------------------控制模式-End--------------------------- */
    
    if(ControlStatus.SpecialMode != SpecialMode_Normal && ControlStatus.Controlby == Control_ByPC){
      offTimeCounter = 0;
    }
    
    if(ControlStatus.Controlby == Control_ByRC){
      offTimeCounter = 101;
    }
		
		/* ---------------------特殊模式-Begin--------------------------- */
		if(ControlStatus.Controlby == Control_ByPC){
    switch(ControlStatus.SpecialMode){
      case SpecialMode_Normal:
      //  JudgeSYS.ClientUploadInfo.data.mask = MONITOR_SEND_MODE_NORMAL;
        if(UpperStructrue.isOpen || UpperStructrue.openReq){
          UpperStructure_close();
        }

        if(ControlStatus.Controlby == Control_ByPC && !UpperStructrue.isOpen && offTimeCounter<100){
          Lifting.mode=LIFTING_MODE_OFF;//失能抬升后再使能抬升，因为久了抬升会一边高一边低
          offTimeCounter++;
        }
        
       
      break;
      
      case SpecialMode_Supply://取弹模式#
       // JudgeSYS.ClientUploadInfo.data.mask = MONITOR_SEND_MODE_SUPPLY;
        if(!UpperStructrue.isOpen){
          UpperStructure_open(Lifting.catchPos);
        }
      break;
        
			case SpecialMode_LookTv:
								Chassis_setMode(CHASSIS_MODE_REVERSE);
      break;

       /* ---------------------特殊模式-End--------------------------- */
      default:
        break;
    }
	}
			if(++counter >100){
			LED_GREEN_TOGGLE();
			counter = 0;
		}
		vTaskDelayUntil(&xLastWakeTime, 10);
	}
	
}

/*-----------------------------------FILE OF END------------------------------*/
