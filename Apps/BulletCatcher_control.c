/**
  ******************************************************************************
  * @file    BulletCatcher_control.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   ��ȡ�������ƺ����ӿ�
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
 *  ��ȡ׼���Ƕ�  ���ӵ��Ƕ�
 *              |    /
 *              |   /
 *              |  /
 *180��---------.----------0��
 * ��ȡ�Ƕ�  �ջؽǶ�
 */

/* ��Ӧ�������Ƕȣ��൱�ڳ�ʼλ�ã���λת�ɱ�����*/
#define CATCHER_CLOSE_ANGLE       0                                     /* �����ʼλ�ã��ջأ� */
#define CATCHER_READY_ANGLE       M3508_P19ANGLE_TO_ENCODER(160.0f)     /* ���׼��λ�� ��׼���� */   
#define CATCHER_GETBOX_ANGLE      M3508_P19ANGLE_TO_ENCODER(350.0f)     /* �����ȡλ�� ����ȡ��*/ 
#define CATCHER_GETBULLET_ANGLE_PRE   M3508_P19ANGLE_TO_ENCODER(25.0f)  /* ������ӵ�λ�ã�����1�� */ 
#define CATCHER_GETBULLET_ANGLE   M3508_P19ANGLE_TO_ENCODER(40.0f)      /* ������ӵ�λ�ã������� */ 
#define CATCHER_ELEVATEBOX_ANGLE  M3508_P19ANGLE_TO_ENCODER(270.0f)     /* �ٸߵ�ҩ��λ�� */


#define CATCHER_RIGHT_POSITION    230000    /* �ϲ�λ���� */
#define CATCHER_LEFT_POSITION     -233509   /* �ϲ�λ���� */



/* �õ������� */
#define CATCHER_USING_GASID    1                     /*ID:2*/
#define CATCHER_GASPOS          GAS_POS_4      /* �������� */
#define CATCHER_PULS_GASPOS     GAS_POS_5  /* ����������� */
#define CATCHER_BIUBIU_GASPOS   GAS_POS_3  /* ����ҩ������ */
#define CATCHER_BANK_GASPOS     GAS_POS_6  /* ��ҩ������ */

#define CATCHER_ALL_GASPOS      (CATCHER_GASPOS | \
                                 CATCHER_PULS_GASPOS | \
                                 CATCHER_BIUBIU_GASPOS | \
                                 CATCHER_BANK_GASPOS)

/* �õ��Ķ�� */
#define CATCHER_USING_SERVOID     1           //ID��1
#define CATCHER_SERVO_MAX_ANGLE   4080        //��߽Ƕ�
#define CATCHER_SERVO_MIN_ANGLE   2050        //��ͽǶȣ���򵽴󵯲���
#define CATCHER_SERVO_MAGAZINE_ANGLE   2269   //�ýǶȲ�Ӱ�쿪�ص����ŵĿ���
/*------------------G L O B A L - F U N C T I O N S --------------------------*/
/**
  * @brief  ��ȡ��ʼ�������ò���
  * @param  None
  * @retval None
  */
void BulletCatcher_Init(void){
  /* ����ۼ�Ȧ�� */	
  M3508_clearTurnCounter(&BulletCatcher.M3508s[0]);
  M3508_clearTurnCounter(&BulletCatcher.M3508s[1]);
  

  /* �Ƕȸ�λ */
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
  * @brief  ���赯�����׿���
  * @param  isOpen   1����0��
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
  * @brief  ֱ������3508���������
  * @param  None
  * @retval None
  */

void BulletCatcher_setM3508Current(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4){

	uint8_t data[8];
	
	//���ݸ�ʽ���˵����P32
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
  * @brief ���ü�����M3508[0]�ĽǶ�
  * @param[in]  Angle   Ŀ��Ƕ�
  * @retval 
  */
uint8_t BulletCatcher_setTargetAngle(float angle){
  BulletCatcher.M3508s[0].targetAngle = angle;//����Ŀ��Ƕ�
  /* צ��ɶ��� */
  if(abs(BulletCatcher.M3508s[0].totalAngle - BulletCatcher.M3508s[0].targetAngle) < 2000 &&
      abs(BulletCatcher.M3508s[0].realSpeed) < 50)
  {
    return 1;
  }
  return 0;
}

/**
  * @brief �ж������Ƿ��������
  * @param[in]  Angle   Ŀ��Ƕ�
  * @retval 
  */
uint8_t BulletCatcher_decide(void){
  
  /* צ��ɶ��� */
  if(BulletCatcher.M3508s[0].realCurrent<-3500  //����2000ʱ��3000�����ٿ��Ըĸߵ�
		)
  {
    return 1;
  }
  return 0;
}

/**
  * @brief  ��ʼ�����ü�ȡ�ĽǶȣ����ú��ȡ���ջز�����Ϊ��ʼλ��
  * @param  None
  * @retval None
  */
void BulletCatcher_resetAngle(void){
  BulletCatcher.catcherInit = 0;
  BulletCatcher.timeCounter = 0;  /* ������ */
  uint16_t count = 0;   /* �豸�����ü����� */
  
  while(BulletCatcher.catcherInit == 0)//ֱ����λ�ɹ�������ѭ��
  {
    BulletCatcher_setM3508Current(1500,0, 0, 0);
    delay_ms(10);
    if(BulletCatcher.M3508s[0].infoUpdateFlag)
    {
      BulletCatcher.M3508s[0].infoUpdateFlag  = 0;
      count = 0;
      /* �ȴ�����е��������ٶȽ�С˵���Ѹ�λ */   
      if(abs(BulletCatcher.M3508s[0].realSpeed) < 2 && abs(BulletCatcher.M3508s[0].realCurrent) > 1300)
      {
        BulletCatcher.timeCounter++;
        /* �ٶ�Ϊ����Ϊ0һ��ʱ�䣬��λ��� */
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
      /* �豸���� */
      if(count > 200)//2��û�յ���������ı�����Ϊ���������#
      {
        /* ���������Ĭ�ϽǶ���3800����,һ����3000-4500֮�� */
        BulletCatcher.M3508s[0].totalAngle = 3800;
        BulletCatcher.timeCounter = 0;//
        BulletCatcher.nowStep = 0;
        return;
        
      }
    }
    
  }
  
  /* ����ͷ� */
  BulletCatcher_setM3508Current(0, 0, 0, 0);
  delay_ms(100);
  
  /* ����Ƕ� */
  BulletCatcher.closePos = BulletCatcher.M3508s[0].totalAngle;
  BulletCatcher.readlyPos = BulletCatcher.closePos - CATCHER_READY_ANGLE;
  BulletCatcher.getBoxPos = BulletCatcher.closePos - CATCHER_GETBOX_ANGLE;
  BulletCatcher.getBulletPos_pre = BulletCatcher.closePos - CATCHER_GETBULLET_ANGLE_PRE;
  BulletCatcher.getBulletPos = BulletCatcher.closePos - CATCHER_GETBULLET_ANGLE;
	BulletCatcher.elevateBoxPos = BulletCatcher.closePos- CATCHER_ELEVATEBOX_ANGLE;
}




/**
  * @brief  ���ü�ȡģʽ
  * @param[in]  mode
  * @retval None
  */
void BulletCatcher_setMode(BulletCatcherMode_e mode){
	BulletCatcher.mode = mode;
}


/**
  * @brief  ��ȡ���ƴ���
  * @param  mode	ģʽ - ��Status_ControlOFF�⣬������������
  * @retval None
  */
void BulletCatcher_processing(void){
	
	if(BulletCatcher.mode == CATCHER_MODE_OFF){
		BulletCatcher.M3508s[0].outCurrent = 0;
		BulletCatcher.M3508s[1].outCurrent = 0;
		return;
	}
	
	  /* ��ȡ�򿪲��м�ȡ���� */
  if(BulletCatcher.catcherIsReady && BulletCatcher.catchRequest)
	{
	 switch(BulletCatcher.nowStep)
	 {
		 case 0:
			  BulletCatcher.catchFinish = 0;//�����ȡ��ɱ�־λ
			  BulletCatcher.alignSucceed=0;//�����ȡ�ɹ���־λ
		    BulletCatcher.test=0;
        /* ��ȡ�ڶ��ţ� */
        if(BulletCatcher.catchType == CATCHER_TYPE_D2){
          if(BulletCatcher.timeCounter < 1){
            GAS8_setBits(CATCHER_USING_GASID, CATCHER_PULS_GASPOS);
						BulletCatcher.timeCounter++;
          }
					else{
						/* ��һ������ʱ���ȴ���������������ȡ���ϵ�ҩ�䣩 */
						BulletCatcher.timeCounter++;        
						if(BulletCatcher.timeCounter > 10)//100ms
						{
							BulletCatcher.timeCounter = 0;
							BulletCatcher.nowStep++;  /* ������һ�� */
						}
					}
         
        }
				else{
            BulletCatcher.nowStep++;  /* ������һ�� */
        }
         
     
			break;
      case 1:
        /* ��һ����צ���ת�� */
           BulletCatcher_setTargetAngle(BulletCatcher.getBoxPos);
			
        
        /* ���Ӵ򵽵�ҩ�䣬�ж���ȡʧ�� */
          if(BulletCatcher_decide())
        {
          BulletCatcher.timeCounter++;
          if(BulletCatcher.timeCounter > 30)
          {

            BulletCatcher.timeCounter = 0;
            BulletCatcher.nowStep = 7;  /* �������� */
          }
        }
				/* �����ܵ���Ŀ��λ�ã��ж���ȡ�ɹ� */
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
        /* �ڶ�������ʱ�����׼�ȡ */
        BulletCatcher.timeCounter++;
        if(BulletCatcher.timeCounter > 3)
        {
          /* ���׼�ȡ */
          GAS8_setBits(CATCHER_USING_GASID, CATCHER_GASPOS);
          BulletCatcher.timeCounter = 0; 
					BulletCatcher.alignSucceed=1;//�ж���һ���׼�ɹ���
          BulletCatcher.nowStep++;  /* ������һ�� */
        }
       
        
      break;
        
      case 3:
        /* ����������ʱ����֤�н� */
        BulletCatcher.timeCounter++;        
        if(BulletCatcher.timeCounter > 9)
        {
          BulletCatcher.timeCounter = 0;
          
          if(BulletCatcher.catchType == CATCHER_TYPE_D1){
            BulletCatcher.nowStep += 2;  /* �������²� */
          }
          else if(BulletCatcher.catchType == CATCHER_TYPE_D2){
            /* ��ȡ�ڶ��ţ���������ջ� */
            GAS8_resetBits(CATCHER_USING_GASID, CATCHER_PULS_GASPOS);
            BulletCatcher.nowStep++;  /* ������һ�� */
          }
        }
        
      break;
        
      case 4:
        /* ���Ĳ�����ʱ���ȴ���������ջأ���ȡ�ڶ���ʱ�� */
        BulletCatcher.timeCounter++;        
        if(BulletCatcher.timeCounter > 30)
        {
          BulletCatcher.timeCounter = 0;
          BulletCatcher.nowStep++;  /* ������һ�� */
        }
        
      break;
        
				
      case 5:

          /* ���Ĳ���צ���ת�� 10�� */
          if(BulletCatcher_setTargetAngle(BulletCatcher.getBulletPos))
          {
						
						if(!BulletCatcher.test){ /*������������Բ�Ҫ��ʱ�����߸ĸ��ж��Ƕ�*/
							if(BulletCatcher.Start){
								BulletCatcher.stepNumber++;/*������ȡ����ʼ�ƶ�����һ���λ��*/
								BulletCatcher.test=1;
								}
							if(BulletCatcher.robBox){/*�������ӣ�׼�����ˣ������ƶ����¸�Ŀ���λ��*/
								BulletCatcher.readyBox=1;
								BulletCatcher.test=1;
							}
						}
						
						BulletCatcher.timeCounter++;
            /* ��ʱ������Դ��һ��20�ŵ��裬��ʱ�ӳ��� */					
            if(BulletCatcher.timeCounter>15&&!BulletCatcher.smallIslandflag){
              BulletCatcher.timeCounter = 0;
              BulletCatcher.nowStep++;  /* ������һ�� */		
            }
						
            /* ��ʱ��С��Դ��һ��3�ŵ��裬��ʱ�̣� */						
						else if(BulletCatcher.timeCounter>5&&BulletCatcher.smallIslandflag){
              BulletCatcher.timeCounter = 0;
              BulletCatcher.nowStep++;  /* ������һ�� */
						}
						
          }
          /* ��ʱ����ʱ���޷��ﵽĿ�� */
          else if(abs(BulletCatcher.M3508s[0].realSpeed) < 5){
            BulletCatcher.timeCounter++;
            if(BulletCatcher.timeCounter > 50){
              BulletCatcher.timeCounter = 0;
              BulletCatcher.nowStep++;  /* ������һ�� */
							
            }
          }
          else{
            BulletCatcher.timeCounter = 0;
          }

      break;
      
      case 6:
        /* ���岽�����׼�ȡ�ɿ� */
        GAS8_resetBits(CATCHER_USING_GASID, CATCHER_GASPOS);
        /* ��ʱ */
        BulletCatcher.timeCounter++;        
        if(BulletCatcher.timeCounter > 10){
          BulletCatcher.timeCounter = 0;
          BulletCatcher.nowStep++;  /* ������һ�� */
        }
      break;
      case 7:
        /* ������������ҩ�䣬����ת�� */
				if(BulletCatcher.alignSucceed)
				{
					GAS8_setBits(CATCHER_USING_GASID, CATCHER_BIUBIU_GASPOS);
				}
        /* ���ջأ��еڶ���ʧ��ʱ���������ܹ����ף� */
        if(BulletCatcher.catchType == CATCHER_TYPE_D2){
          GAS8_resetBits(CATCHER_USING_GASID, CATCHER_PULS_GASPOS);
        }
				
        BulletCatcher_setTargetAngle(BulletCatcher.readlyPos);//�ص�׼��λ��
        /* ��ʱ */
        BulletCatcher.timeCounter++;        
        if(BulletCatcher.timeCounter > 25){
          BulletCatcher.timeCounter = 0;
          BulletCatcher.nowStep++;  /* ������һ�� */
        }
        
      break;
      
     case 8:
        /* �ص���ҩ�䣬��� */
        GAS8_resetBits(CATCHER_USING_GASID, CATCHER_BIUBIU_GASPOS);
		   //�������������̣���ȡʧ��+�����ڼе�һ��ʱ�Խ�����һ��
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
				BulletCatcher.catcherIsReady = 0;//��RC���Ƽ�ȡʱ�ٽ��ע��
				BulletCatcher.catchRequest = 0;
				BulletCatcher.catchFinish = 1;

				
     break;
      
      default:
        BulletCatcher_open();
      break;
	 }
	
	}
		/* �Ƕ�PID����(�⻷) */
	int M3508_PIDOut = Position_PID(&BulletCatcher.M3508s[0].pid_angle, \
	 BulletCatcher.M3508s[0].targetAngle,BulletCatcher.M3508s[0].totalAngle);
	/* �ٶ�PID����(�ڻ�) */
	BulletCatcher.M3508s[0].outCurrent =Position_PID(&BulletCatcher.M3508s[0].pid_speed,\
	 M3508_PIDOut, BulletCatcher.M3508s[0].realSpeed);
	
}

/**
  * @brief  ��ȡ�򿪣���ʼ������
  * @param  None
  * @retval �򿪳ɹ��󷵻�1�����򷵻�0
  */
uint8_t BulletCatcher_open(void){
  /* צ���ת�� */

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
  * @brief  ��ȡ�رգ��ջ�
  * @param  None
  * @retval �رճɹ��󷵻�1
  */
uint8_t BulletCatcher_close(void){
  /* צ���ת�� */
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
		BulletCatcher_setTargetAngle(BulletCatcher.closePos))//��ûװ��������ʱ���˸��Ƕȴ���
	{
    return 1;
  }
	else{
  return 0;
	}
}


/**
  * @brief  �ֶ���ȡ
  * @param  None
  * @retval 
  */
//�����������������ֶ���ȡ!!!

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
  * @brief  �����ƶ���ʽ
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
    
    case CATCHER_MODE_MANUAL:   /* �ֶ�ģʽ */ 
      BulletCatcher.M3508s[1].targetAngle = BulletCatcher.M3508s[1].totalAngle;
      Vx = BulletCatcher.M3508s[1].targetSpeed;//����ָ���ٶ�
    break;
    
    case CATCHER_MODE_POSITION:   /* ����ģʽ */
      Vx = Position_PID(&BulletCatcher.M3508s[1].pid_angle,
  		BulletCatcher.M3508s[1].targetAngle,BulletCatcher.M3508s[1].totalAngle);//ת��ָ����
    break;
    
    default:
      
    break;
		
  }
  if(!BulletCatcher.M3508s[1].offLineFlag){
		
		BulletCatcher.M3508s[1].outCurrent = Position_PID(&BulletCatcher.M3508s[1].pid_speed, \
		Vx, BulletCatcher.M3508s[1].realSpeed);
		/* ���־λ */
		BulletCatcher.M3508s[1].infoUpdateFlag = 0;
		
    }
		
 }


 /**
  * @brief  ���ü��������ƶ����ٶ�
  * @param  speed �����ٶȴ�С
  * @retval 
  */
void BulletCatcher_setSpeed(float speed)
{
	BulletCatcher.M3508s[1].targetAngle = BulletCatcher.M3508s[1].totalAngle;
	BulletCatcher.M3508s[1].targetSpeed=speed;
	
}


 /**
  * @brief  ���Ƽ��������ƶ��ĵ����ʼ��
  * @param  None
  * @retval 
  */
void BulletCatcher_resetPosition(void)
{
	
	BulletCatcher.catcherInit = 0;
	BulletCatcher.timeCounter = 0;  /* ������ */
	uint16_t count = 0;   /* �豸�����ü����� */
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

        /*ת��*/
        if(mun>60)
        {
          veer =1;
        }
				if(mun<-60)
				{
					veer=0;
				}
			
		BulletCatcher.resetPositionFinish=Hall_PILLARHOLDING_LOCK();
			/*�ﵽ��������ڵ�λ��*/
		if(BulletCatcher.resetPositionFinish)
		{
				BulletCatcher_setM3508Current(0,0,0,0);//�ͷŵ��
	 			/* ����Ƕ� */
				BulletCatcher.catcherInit=1;
        BulletCatcher.middlePos=BulletCatcher.M3508s[1].totalAngle;
				BulletCatcher.leftPos=BulletCatcher.middlePos+CATCHER_LEFT_POSITION;
				BulletCatcher.rightPos=BulletCatcher.middlePos+CATCHER_RIGHT_POSITION;
		}

		 }
		
		 else
    {
      count++;
      /* �豸���� */
      if(count > 200)
      {
        /* ���������Ĭ�ϽǶ���3800���� */
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
  * @brief  ���ü��Ӷ���λ��
  * @param  angle Ŀ��Ƕ�
  * @retval 
  */
uint8_t BulletCatcher_setPosition(float angle){

  BulletCatcher.M3508s[1].targetAngle = angle;//����Ŀ��Ƕ�

  /* ���צ��ɶ��� */
  if(abs(BulletCatcher.M3508s[1].totalAngle - BulletCatcher.M3508s[1].targetAngle) < 2000 &&
      abs(BulletCatcher.M3508s[1].realSpeed) < 50)//����Ƿ�ת��Ŀ��Ƕȣ��ӽ�Ŀ��Ƕ���real�ٶ�С#
  {
    return 1;
  }
  return 0;
}

 /**
  * @brief  ��������������
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
			/*��һ���ȡ��...*/
		break;
		
		case 1:
			if(BulletCatcher.alignSucceed){
			if (BulletCatcher_setPosition(BulletCatcher.leftPos)){
				count++;
				if(count>5&&BulletCatcher.catchFinish){
					count=0;
					BulletCatcher.stepNumber++;
					/*��ȡ�����м�ȡ�ڼ���*/
					BulletCatcher.catchType = CATCHER_TYPE_D1;
					BulletCatcher.catchRequest = 1;
					BulletCatcher.catcherIsReady = 1;//
				}
			}
	}
			/*��һ��û�ж�׼����������*/
			else{
			BulletCatcher.Start=0;
			BulletCatcher.stepNumber=0;
			}
			break;
		
		case 2:
			/*�ڶ����ȡ��...*/
		break;
		
		case 3:
			 if (BulletCatcher_setPosition(BulletCatcher.rightPos)){
				count++;
				if(count>5&& BulletCatcher.catchFinish){
					count=0;
					BulletCatcher.stepNumber++;
					/*��ȡ������1���м�ȡ�ڼ���*/
					BulletCatcher.catchType = CATCHER_TYPE_D1;
					BulletCatcher.catchRequest =  1;//����
					BulletCatcher.catcherIsReady = 1;//׼��
						}
					}
			 break;
			
		case 4:
			/*�������ȡ��...*/
		break;
		
		case 5:
			/*�ص�ԭ��*/
			if (BulletCatcher_setPosition(BulletCatcher.middlePos)){
			 count++;
			 if(count>5&& BulletCatcher.catchFinish){
				count=0;
				BulletCatcher.stepNumber=0;
				BulletCatcher.smallIslandflag=0;				 
				BulletCatcher.Start=0;

			//	BulletCatcher.catchFinish=0;
				//BulletCatcher.catcherIsReady = 0;//׼�� ��rc����ʱ���ע��#
					
				}
			}
			 break;
			
	}
	
 }
}

 /**
* @brief  ��Դ������ҩ������
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
				/*��ȡ��һ���м�λ�õĵ�ҩ��*/
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
								BulletCatcher.readyBox=0;//2020.7.23��
								BulletCatcher.robCount=0;
								BulletCatcher.Island++;
							}
					}
				}
				break;
				
			case 1:
				break;
			
			case 2:
				/*��ȡ�ڶ����м�λ�õ�ҩ��*/
				if(BulletCatcher.readyBox)
				{
					if(BulletCatcher_setPosition(BulletCatcher.middlePos)&&BulletCatcher.catchFinish)
					{
								BulletCatcher.robCount++;
								if(BulletCatcher.robCount>5)
								{
									BulletCatcher.catchType = CATCHER_TYPE_D2;
									BulletCatcher.catchRequest = 1;
									BulletCatcher.catcherIsReady = 1;//��PC�˿��ƾͿ��԰����ɾ����
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
				/*��ȡ�ڶ������λ�õ�ҩ��*/
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
				/*��ȡ�ڶ����ұ�λ�õ�ҩ��*/
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
  * @brief  PC���м�ȡ����
  * @param  mode �����ĸ���ȡģʽ
  * @retval None
  */
void BulletCatcher_begin(GetBoxMode_e mode)
{
 BulletCatcher.getBoxMode=mode;
	
 switch(mode)
 {
	 case CATCHER_BOX_THREE:
		 /*С��Դ��������*/
		BulletCatcher.Start=1;
		BulletCatcher.catchType = CATCHER_TYPE_D1;
		BulletCatcher.catchRequest = 1;
		BulletCatcher.catcherIsReady = 1;
	  BulletCatcher.smallIslandflag=1;
		BulletCatcher.stepNumber=0;
	  break;
	 
	 case CATCHER_BOX_TWO:
		 /*����Դ���Ľ�*/
		BulletCatcher.Start=1;
		BulletCatcher.catchFinish=1;
		BulletCatcher.stepNumber=1;
		BulletCatcher.alignSucceed=1;
	  BulletCatcher.smallIslandflag=0;
	  break;
	 
	 case CATCHER_BOX_SINGLE:
		 /*������*/
		BulletCatcher.Start=1;
		BulletCatcher.catchType = CATCHER_TYPE_D2;
		BulletCatcher.catchRequest = 1;
		BulletCatcher.catcherIsReady = 1;
		BulletCatcher.stepNumber=0;
	  BulletCatcher.smallIslandflag=0;
	  break;
	 
	 /*����Դ�����ᵯҩ��*/
	 case CATCHER_BOX_MID1:
		 /*��ȡ��һ���м�λ�õĵ�ҩ��*/
		BulletCatcher.Island=0;
		if(!BulletCatcher.robBox)
		{
			BulletCatcher.readyBox=1;
			BulletCatcher.robBox=1;
			BulletCatcher.catchFinish=1;
		}
		 break;
	 
	 case CATCHER_BOX_MID2:
		 /*��ȡ�ڶ����м�λ�õĵ�ҩ��*/
		BulletCatcher.Island=2;
		if(!BulletCatcher.robBox)
		{
			BulletCatcher.readyBox=1;
			BulletCatcher.robBox=1;
			BulletCatcher.catchFinish=1;
		}
		 break;
	 
	 case CATCHER_BOX_LEFT2:
		 /*��ȡ�ڶ�����ߵĵ�ҩ��*/
		BulletCatcher.Island=4;
		if(!BulletCatcher.robBox)
		{
			BulletCatcher.readyBox=1;
			BulletCatcher.robBox=1;
			BulletCatcher.catchFinish=1;
		}	 
		 break;
	 
	 case CATCHER_BOX_RIGHT2:
		 /*��ȡ�ڶ����ұߵĵ�ҩ��*/
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
  * @brief  ���ڰ�̯���»���
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
