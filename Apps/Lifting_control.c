/**
  ******************************************************************************
  * @file    Lifting_control.c
  * @author  lxy
  * @version V1.0
  * @date    
  * @brief   ̧���������ƺ����ӿ�
  ******************************************************************************
  */
  
/*--------------------- I N C L U D E - F I L E S ----------------------------*/
#include "Lifting_control.h"
#include "Hall_IO.h"

/*-----------L O C A L - F U N C T I O N S - P R O T O T Y P E S--------------*/
Lifting_t Lifting;

/*------------------G L O B A L - F U N C T I O N S --------------------------*/
void Lift_setM3508Current(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4){

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
	
	CAN_SendData(CAN1, CAN_ID_STD, 0x1FF, data);
	
}

/**
  * @brief  ̧����ʼ�������ò���
  * @param  None
  * @retval None
  */
void Lifting_Init(void){
  M3508_clearTurnCounter(&Lifting.M3508s[0]);//����Ȧ��#
  M3508_clearTurnCounter(&Lifting.M3508s[1]);
  
  Lifting.isInit = 0;
  Lifting.speedLimit = LIFTING_SPEED_NORMAL;//�޷�6300#
  
  /* ���PID */
	for(int i = 0; i < 2; i++){
		
    PositionPID_paraReset(&Lifting.M3508s[i].pid_speed, 4.0f, 0.4f, 0.0f, 16000, 16000);
	}
  
  /* ����PID */
  PositionPID_paraReset(&Lifting.pid_pos, 0.1f, 0.0f, 0.0f, 6300, 3300);
	
	Lifting.mode = LIFTING_MODE_LOCK;//����ģʽ#
}

	


//	/**
//  * @brief  û�д�����ʱ����ʼ��ʱ��λ��
//  * @param  None
//  * @retval None
//  */
//void Lifting_resetAngle(void){
//	Lifting.resetAngleflage=0;
//	int count=0;
//	 while( Lifting.resetAngleflage==0)
//	 {
//		 
//	 Lift_setM3508Current(0, 0, 1500, -1500);
//		  delay_ms(10);
//	 if(Lifting.M3508s[1].infoUpdateFlag==1){
//		 
//		    if(abs(Lifting.M3508s[1].realSpeed) < 2 && abs(Lifting.M3508s[1].realCurrent) > 900)
//      {
//        Lifting.timeCounter++;
//        /* �ٶ�Ϊ����Ϊ0һ��ʱ�䣬��λ��� */
//        if(Lifting.timeCounter > 50)//
//        {

//					Lifting.timeCounter = 0; 
//					Lift_setM3508Current(0, 0, 0, 0);//�ͷŵ��
//					Lifting.resetAngleflage=1;
//					Lifting.isInit = 1;
//        }
//      }
//      else
//      {
//        Lifting.timeCounter = 0;
//      }
//	  } 
//	 else
//    {
//      count++;
//      /* �豸���� */
//      if(count > 200)//����
//      {
//        /* ���������Ĭ�ϽǶ���3800���� */
//				Lifting.M3508s[0].totalAngle = 3800;
//				Lifting.timeCounter = 0;
//        return;
//      }
//		 }
//		
//  }
//}

	/**
  * @brief  �����ֶ�ģʽʱ��̧���ٶ�
  * @param[in]  speed
  * @retval None
  */
	void Lifting_setSpeed(int32_t speed){
	Lifting.targetSpeed = speed;  
}
	
/**
  * @brief  ����̧��λ��
  * @param[in]  mode
  * @retval None
  */
uint8_t Lifting_setPosition(int32_t pos){
  
  if(!Lifting.isInit){//��ʼ��ʧ��ʱ������ȥ
    return 0;
  }
  
  if(pos > Lifting.maxPos)
    pos = Lifting.maxPos;
  
  if(pos < Lifting.minPos)
    pos = Lifting.minPos;
  
	Lifting.targetPosition = pos;
  /* ��ɶ��� */
  if(abs(Lifting.targetPosition - Lifting.M3508s[1].totalAngle) < 2000 &&
      abs(Lifting.M3508s[1].realSpeed) < 50)
  {
    return 1;
  }
  return 0;
}


/**
  * @brief  ̧������
  * @param[in]  Vz		̧���ٶȣ����ϸ���
*				mode	ģʽ - ��Status_ControlOFF�⣬������������
  * @retval None
  */
void Lifting_processing(void){
  float Vz = 0;
  
  Lifting.UpLimit = Hall_LIFT_UPLIMIT();//��ȡ������������ֵ��ȡ��#
  Lifting.DownLimit = Hall_LIFT_DOWNLIMIT();
  
  LED_LIST(4, Lifting.UpLimit);//ȡ��д��ָʾ��#
  LED_LIST(5, Lifting.DownLimit);
  
  /* ��ʼ��λ�� */
  if(!Lifting.isInit){
    if(Lifting.M3508s[1].infoUpdateFlag){
      Lifting.M3508s[1].infoUpdateFlag = 0;
      
      /* �ȴ���������λ�����ٶȽ�С˵���Ѹ�λ */
      if(Hall_LIFT_DOWNLIMIT() && abs(Lifting.M3508s[1].realSpeed) < 2){
        Lifting.timeCounter++;
        /* �ٶ�Ϊ����Ϊ0һ��ʱ�䣬��λ��� */
        if(Lifting.timeCounter > 50)
        {
          Lifting.timeCounter = 0;
          Lifting.isInit = 1;
          
          Lifting.minPos = Lifting.M3508s[1].totalAngle + LIFTING_DOWNLIMIT_OFFSET;
          Lifting.catchPos = Lifting.minPos + LIFTING_CATCH_POS;
          Lifting.maxPos = Lifting.minPos + LIFTING_FULL_TRAVEL;
          
          Lifting.M3508s[0].targetAngle = Lifting.M3508s[0].totalAngle;
          Lifting.M3508s[1].targetAngle = Lifting.M3508s[1].totalAngle;
//          Lifting.targetPosition = Lifting.minPos;//Ŀ��ֵ
					  
        }
      }
      else{
        Lifting.timeCounter = 0;
      }
    }
    return;
  }
  
   
  switch(Lifting.mode){
    case LIFTING_MODE_OFF:      /* ʧ��ģʽ */
      Lifting.M3508s[0].outCurrent = Lifting.M3508s[1].outCurrent = 0;
      Lifting.M3508s[1].targetAngle = Lifting.M3508s[1].totalAngle;
      Lifting.targetPosition = Lifting.M3508s[1].totalAngle;//

		
      return;
    break;
    
    case LIFTING_MODE_MANUAL:   /* �ֶ�ģʽ */  
      Lifting.M3508s[1].targetAngle = Lifting.M3508s[1].totalAngle;
      Lifting.targetPosition = Lifting.M3508s[1].totalAngle;
      Vz = Lifting.targetSpeed;
    break;
    
    case LIFTING_MODE_POSITION:   /* ����ģʽ */
      Vz = Position_PID(&Lifting.pid_pos, Lifting.targetPosition, Lifting.M3508s[1].totalAngle);
    break;
    
    default:
      
    break;
    
  }
  
  
  /* ң�������� */
  if(dr16_data.offLineFlag || Lifting.mode == LIFTING_MODE_LOCK){
    Vz = 0;
	}
  
  /* �������� */
  if(Lifting.UpLimit){
    if(Vz>0){
      Vz = 0;
    }
	}
  
  /* �������� */
  if(Lifting.DownLimit){
    if(Vz<0){
      Vz = 0;
    }
	}
  
  /* ���� */
  if((Lifting.M3508s[1].totalAngle - Lifting.minPos < 50000) || \
		(Lifting.maxPos - Lifting.M3508s[1].totalAngle < 50000))
	{
    Lifting.speedLimit -=400;
    if(Lifting.speedLimit < LIFTING_SPEED_SLOW)
			{
      Lifting.speedLimit = LIFTING_SPEED_SLOW;
      }
    
  }
  else{
    Lifting.speedLimit +=200;
    if(Lifting.speedLimit > LIFTING_SPEED_NORMAL){
      Lifting.speedLimit = LIFTING_SPEED_NORMAL;
      
    }
    
  }
  
  //�ٶ�����
	VAL_LIMIT(Vz, -Lifting.speedLimit, Lifting.speedLimit);
  
  Lifting.M3508s[0].targetSpeed = -Vz;
  Lifting.M3508s[1].targetSpeed = Vz;
  
  
  
  if((!Lifting.M3508s[0].offLineFlag) && (!Lifting.M3508s[1].offLineFlag)){
    for(int i = 0; i < 2; i++){
      /* �ٶ�PID����(�ڻ�) */
      Lifting.M3508s[i].outCurrent = Position_PID(&Lifting.M3508s[i].pid_speed, \
			Lifting.M3508s[i].targetSpeed, Lifting.M3508s[i].realSpeed);
      /* ���־λ */
      Lifting.M3508s[i].infoUpdateFlag = 0;
    }
  }
  
  
  
  
}
/**
  * @brief  ����̧��ģʽ
  * @param[in]  mode
  * @retval None
  */
void Lifting_setMode(LiftingMode_e mode){
	Lifting.mode = mode;
}




/*-----------------------------------FILE OF END------------------------------*/




