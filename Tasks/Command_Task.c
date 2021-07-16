/**
  ******************************************************************************
  * @file    Command_Task.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   ����������������������ڵ��̣���̨�ȿ�������ַ�
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
/* ���� */
#define KEY_ACC_TIME     500  //������500ms���ٶ�ϵ����0->1������
ramp_t fb_ramp = RAMP_GEN_DAFAULT;
ramp_t lr_ramp = RAMP_GEN_DAFAULT;
ramp_t cw_ramp = RAMP_GEN_DAFAULT;

uint16_t offTimeCounter = 0;
int16_t cloud_delta_yaw = 0;


/*------------------G L O B A L - F U N C T I O N S --------------------------*/

/**
  * @brief  ��ϼ��жϴ��� 
  * @param  None
  * @retval None
  */
void Control_combineKeyScan(void)
{
  //�������Ctrl��shift���Ȱ��£�����û������������ʱ�ж�Ϊ��ϼ�
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
			//�����ϼ�ȫ���ɿ���ر���ϼ�
			ControlStatus.isCombineKey = CombineKey_None;
	}
	
}

/**
  * @brief  ����ϼ����� 
  * @param  None
  * @retval None
  */
void Control_notCombineKeyHandle(void)
{  
  /* ������ģʽ��shift���٣�ctrl���� */
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
  
  /* ǰ���������� */
  if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_W,KEY_HAVE_MATCH )){
    Chassis_Vy = Chassis.speedLimit*ramp_calc(&fb_ramp);//ǰ
  }
  else if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_S, KEY_HAVE_MATCH)){
    Chassis_Vy = -Chassis.speedLimit*ramp_calc(&fb_ramp);//��
  }
  else{
    Chassis_Vy = 0;
    ramp_init(&fb_ramp, KEY_ACC_TIME/10);
  }
  
  if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_D, KEY_HAVE_MATCH)){
    Chassis_Vx = Chassis.speedLimit*ramp_calc(&lr_ramp);//��
  }
  else if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_A, KEY_HAVE_MATCH)){
    Chassis_Vx = -Chassis.speedLimit*ramp_calc(&lr_ramp);//��
  }
  else{
    Chassis_Vx = 0;
    ramp_init(&lr_ramp, KEY_ACC_TIME/10);
  }
  
  if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_E, KEY_HAVE_MATCH)){
    Chassis_Vz = Chassis.speedLimit*ramp_calc(&cw_ramp);//����E
  }
  else if(dr16_keyboard_isPressedKey(KEYBOARD_PRESSED_Q, KEY_HAVE_MATCH)){
    Chassis_Vz = -Chassis.speedLimit*ramp_calc(&cw_ramp);//��������Q
  }
  else{
    Chassis_Vz = 0;
    ramp_init(&cw_ramp, KEY_ACC_TIME/10);
  }
  
}
/**
  * @brief  ������ 
  * @param  None
  * @retval None
  */
void Control_KeyHandle(void)
{
//		static int counter = 0;

  switch(ControlStatus.isCombineKey){
    /* ---------------------------------Ctrl��ϼ�-Begin------------------------------------- */
    case CombineKey_Ctrl:
      
      /* �����˶� */
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
      
       /* ����ģʽ Ctrl+Q */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_Q, KEY_FULL_MATCH)){
        Chassis.speedLimit = CHASSIS_SPEED_NORMAL;
        ControlStatus.SpecialMode = SpecialMode_Normal;
       // Cloud.inventFB = 0;
      }
      
      /* ��ȡģʽ Ctrl+E */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_E, KEY_FULL_MATCH)){
          ControlStatus.SpecialMode = SpecialMode_Supply;
          Chassis.speedLimit = CHASSIS_SPEED_SLOW;
				  
					if(UpperStructrue.isOpen)
					{
						BulletCatcher_open();  /*��λ���ܣ���ֹ������*/
					}
          //Cloud.inventFB = 1;
      }
			
			/*����Դ���Ľ�Ctrl+F �����٣�*/ 
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_F, KEY_FULL_MATCH)){
				if((ControlStatus.SpecialMode == SpecialMode_Supply) && UpperStructrue.isOpen){
						BulletCatcher_begin(CATCHER_BOX_TWO);
				}
      }
			
//       /*����Ļģʽ Ctrl+X */
//			if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_X, KEY_FULL_MATCH)){
//				ControlStatus.SpecialMode = SpecialMode_LookTv;
//				Chassis.speedLimit = CHASSIS_SPEED_SLOW;
//				
//				
//			}
			
      /* �ر�С���� Ctrl+G */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_G, KEY_FULL_MATCH)){
					Magazine.close=1;
      }

      /* תͷ Ctrl+V */
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
      
      /* �ش��赯�� Ctrl+R */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_R, KEY_FULL_MATCH)){
         if(ControlStatus.SpecialMode == SpecialMode_Supply || ControlStatus.SpecialMode == SpecialMode_Normal){
            //BulletCatcher_writeBank(0);
         }
      }
    break;
    /* ---------------------------------Ctrl��ϼ�-End--------------------------------------- */
      
    /* ---------------------------------Shitf��ϼ�-Begin------------------------------------ */
    case CombineKey_Shift:
      
      /* �����˶� */
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
			
        /* С��Դ������������ Shift+F */
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
    /* ---------------------------------Shitf��ϼ�-End-------------------------------------- */
    
    
    /* ---------------------------------����ϼ�-Begin--------------------------------------- */
    case CombineKey_None:
      Control_notCombineKeyHandle();
    
      /* ��ȡ��ȡ��һ���м䵯ҩ�� F */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_F, KEY_FULL_MATCH)){
          if((ControlStatus.SpecialMode == SpecialMode_Supply) && UpperStructrue.isOpen){
							BulletCatcher_begin(CATCHER_BOX_MID1);
          }
          
					
      }
			
		  /* ��ȡ��ȡ�ڶ�����൯ҩ�� Z */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_Z, KEY_FULL_MATCH)){
          if((ControlStatus.SpecialMode == SpecialMode_Supply) && UpperStructrue.isOpen){
							BulletCatcher_begin(CATCHER_BOX_LEFT2);
          }
				}
			
			/* ��ȡ��ȡ�ڶ����м䵯ҩ�� X */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_X, KEY_FULL_MATCH)){
          if((ControlStatus.SpecialMode == SpecialMode_Supply) && UpperStructrue.isOpen){
							BulletCatcher_begin(CATCHER_BOX_MID2);
          }
				}
     
			/* ��ȡ��ȡ�ڶ����Ҳ൯ҩ�� C */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_C, KEY_FULL_MATCH)){
          if((ControlStatus.SpecialMode == SpecialMode_Supply) && UpperStructrue.isOpen){
							BulletCatcher_begin(CATCHER_BOX_RIGHT2);
          }
				}			
			
      /* �����赯�� R */
      if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_R, KEY_FULL_MATCH)){
         if(ControlStatus.SpecialMode == SpecialMode_Supply || ControlStatus.SpecialMode == SpecialMode_Normal){
            //BulletCatcher_writeBank(1);
					 BigBank.start=1;
					 vTaskResume(xHandleTaskPlayRoom);
         }
      }
			 /* ��С���赯������ G */
			if(dr16_keyboard_isJumpKeyPressed(KEYBOARD_PRESSED_G, KEY_FULL_MATCH)){
//         if(ControlStatus.SpecialMode == SpecialMode_Supply || ControlStatus.SpecialMode == SpecialMode_Normal){
          Magazine.open=1;
					vTaskResume(xHandleTaskPlayRoom);
//         }
      }

      /* ���ϳ� ����� */
      if(dr16_mouseKeyLeft.flag.keyPressedJump){
        if(ControlStatus.SpecialMode == SpecialMode_Normal){
          GAS8_setBits(1, GAS_POS_4);
        }
      }
      
      
      /* ���ϳ� ����� */
      if(dr16_mouseKeyRight.flag.keyPressedJump){
        if(ControlStatus.SpecialMode == SpecialMode_Normal){
          GAS8_resetBits(1, GAS_POS_4);
        }
      }
      
//			        /* ��v��̨�������̲���, �ֶ��������� */
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
    /* ---------------------------------����ϼ�-End----------------------------------------- */
    default:
      break;
    
  
  
 }
}
/**
  * @brief  ������������
  * @param  pvParameters ���񱻴���ʱ�������Ĳ���
  * @retval None
  */
void vTaskCommand(void *pvParameters)
{
	TickType_t xLastWakeTime;
	/* ��ȡ��ǰ��ϵͳʱ�� */
	xLastWakeTime = xTaskGetTickCount();
	static int counter = 0;
	 // int16_t cloud_delta_yaw = 0;
	while(1){
			Chassis_Vx = 0; Chassis_Vy = 0; Chassis_Vz = 0;
			DR16_KeyProcessing();
		/* ---------------------����ģʽ(��ҡ��)-Begin------------------- */
	switch(dr16_data.rc.s_left)
		{
			case  DR16_SWITCH_UP:
				/* -------------------���Կ���-Begin------------------------- */
  	//	Cloud.mode=CLOUD_MODE_OFF;
//			Chassis_setMode(CHASSIS_MODE_LOCK);//ͣ��
		
				ControlStatus.Controlby = Control_ByPC;
				/*ʹ�ܻ�����*/
				Robot_Enable();
        /* �������� */
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
					
        /* ƽʱ�����Ƶ������� */
        if(!ControlStatus.cloud_look){
          Chassis_Vz += dr16_data.mouse.x*80.0f;
        }
        
       Chassis_setSpeed(Chassis_Vx, Chassis_Vy, Chassis_Vz, Cloud_getYawAngleWithCenter() * DEG_TO_RAD); 
        
//        Cloud_setDeltaPos(cloud_delta_yaw, -dr16_data.mouse.y*1.50f);
			
			break;
			/* -------------------���Կ���-End--------------------------- */
				
			case DR16_SWITCH_MID:
				ControlStatus.Controlby = Control_ByRC;
		 /* ---------------------RC����-Begin-------------------------- */
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
					Lifting_setPosition(Lifting.catchPos);//̧�ߵ���ȡ�߶�
					
//					
//					UpperStructure_open(Lifting.catchPos);
					
//					Lifting_setMode(LIFTING_MODE_MANUAL);
//					Lifting.targetSpeed=(float)dr16_data.rc.ch1_RY*3;/*�ֶ�̧��*/
				

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
				BulletCatcher_setSpeed(dr16_data.rc.ch0_RX*3);//�ֶ�����
				
					Chassis_setSpeed((float)dr16_data.rc.ch2_LX / DR16_ROCKER_MAXVALUE * CHASSIS_SPEED_NORMAL,
					(float)dr16_data.rc.ch3_LY / DR16_ROCKER_MAXVALUE * CHASSIS_SPEED_NORMAL,
					0,0);
				
			}
			
			break;
		  /* ---------------------RC����-End-------------------------- */
			case DR16_SWITCH_DOWN:

			/* -------------------ʧ�ܿ���-Begin------------------------ */
				/*ʧ�ܻ�����*/
			ControlStatus.Controlby = Control_OFF;
			Robot_Disability();	
			break;
		}
		 /* ---------------------����ģʽ-End--------------------------- */
    
    if(ControlStatus.SpecialMode != SpecialMode_Normal && ControlStatus.Controlby == Control_ByPC){
      offTimeCounter = 0;
    }
    
    if(ControlStatus.Controlby == Control_ByRC){
      offTimeCounter = 101;
    }
		
		/* ---------------------����ģʽ-Begin--------------------------- */
		if(ControlStatus.Controlby == Control_ByPC){
    switch(ControlStatus.SpecialMode){
      case SpecialMode_Normal:
      //  JudgeSYS.ClientUploadInfo.data.mask = MONITOR_SEND_MODE_NORMAL;
        if(UpperStructrue.isOpen || UpperStructrue.openReq){
          UpperStructure_close();
        }

        if(ControlStatus.Controlby == Control_ByPC && !UpperStructrue.isOpen && offTimeCounter<100){
          Lifting.mode=LIFTING_MODE_OFF;//ʧ��̧������ʹ��̧������Ϊ����̧����һ�߸�һ�ߵ�
          offTimeCounter++;
        }
        
       
      break;
      
      case SpecialMode_Supply://ȡ��ģʽ#
       // JudgeSYS.ClientUploadInfo.data.mask = MONITOR_SEND_MODE_SUPPLY;
        if(!UpperStructrue.isOpen){
          UpperStructure_open(Lifting.catchPos);
        }
      break;
        
			case SpecialMode_LookTv:
								Chassis_setMode(CHASSIS_MODE_REVERSE);
      break;

       /* ---------------------����ģʽ-End--------------------------- */
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
