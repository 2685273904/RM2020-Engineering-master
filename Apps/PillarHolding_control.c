/**
  ******************************************************************************
  * @file    PillarHolding_control.c
  * @author  LXY
  * @version V1.0
  * @date    
  * @brief   �����������ƺ����ӿ�
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
  * @brief  ������ʼ�������ò���
  * @param  None
  * @retval None
  */
void PillarHolding_Init(void){
  DYMX64AT_Init();  /* �����ʼ�� */
  DYMX64AT_setSyncTarAng(1,PILLARHOLDING_USING_SERVOID,PILLARHOLDING_SERVO_LOCK_ANGLE);
  
	
	

  }
///**
//  * @brief  ������ʼ�������ò���
//  * @param  None
//  * @retval None
//  */
//void PillarHolding_Init(void){
// 
//     DYMX64AT_Init();  /* �����ʼ�� */
//  DYMX64AT_setSyncTarAng(1,PILLARHOLDING_USING_SERVOID,PILLARHOLDING_SERVO_LOCK_ANGLE);
//	
//  /* ���Ӹ�λ */
//  while(!PillarHolding_setClamp(0)){//�ɿ�����
//    delay_ms(10);
//  }
//	
//  M3508_clearTurnCounter(&PillarHolding.M3508s[0]);
//  M3508_clearTurnCounter(&PillarHolding.M3508s[1]);
//	PillarHolding.M3508s[0].targetAngle = PillarHolding.M3508s[0].totalAngle;
//  PillarHolding.M3508s[1].targetAngle = PillarHolding.M3508s[1].totalAngle;
//  
//  /* ���PID */
//	for(int i = 0; i < 2; i++){
//		
//    PositionPID_paraReset(&PillarHolding.M3508s[i].pid_speed, 4.0f, 0.4f, 0.0f, 16000, 16000);//�ٶȻ�
//	}
//  
//  PositionPID_paraReset(&PillarHolding.pid_pos, 6.0f, 0.0f, 0.0f, 1000, 1000);//ʹĿ���ٶ��޷���1000
//  
//	PillarHolding.mode = PILLARHOLDING_MODE_LOCK;
//	
//}
//	

/**
  * @brief  �������ü���״̬
  * @param[in]  isClamp 0�ɿ���1��ס
  * @retval ����ִ�з���0
  *         �гɹ�  0x11  ��ʧ��  0x10
  *         �ɳɹ�  0x21  
  */
clampState_e PillarHolding_setClamp(uint8_t isClamp){
  static uint16_t timeCounter = 0;
  static uint16_t stepCounter = 0;
  static clampState_e temp_state = CLAMP_RUNNING; /* �ڲ�״ָ̬ʾ */
  static clampState_e state = CLAMP_RUNNING; /* ����״ָ̬ʾ */
  static uint8_t last_req = 0; /* �ϴ����� */
  
  if(last_req != isClamp && stepCounter == 0){
    last_req = isClamp;
  }

  if(last_req)  /* ��ס */
  {
    switch (stepCounter)
    {
      case 0:
        
        if (Hall_PILLARHOLDING_LOCK())  /* һ��ʼ�Ǽе�״̬ */ //����������
        {
          state = CLAMP_CLOSE_SUCCESS;
        }
        else{
          /* ��ʼ������ */
          state = CLAMP_RUNNING;
          timeCounter = 0;
          stepCounter++;
        }
        break;

      case 1:
        timeCounter++;
        /* ���� */
        if (timeCounter == 1)
        {
          DYMX64AT_setSyncTarAng(1, PILLARHOLDING_USING_SERVOID, PILLARHOLDING_SERVO_UNLOCK_ANGLE);
        }
        /* ��ʱ */
        if(timeCounter > 20){
          timeCounter = 0;
          stepCounter++;
        }
        break;

      case 2:
        /* �ؼ� */
        GAS8_setBits(PILLARHOLDING_USING_GASID, PILLARHOLDING_GASPOS);
        stepCounter++;
        break;

      case 3:
        /* �ȴ��ؼ� */
        if (Hall_PILLARHOLDING_LOCK())//������
        {
          /* �سɹ� */
          timeCounter = 0;
          temp_state = CLAMP_CLOSE_SUCCESS;
          stepCounter++;
        }
        else
        {
          timeCounter++;
          /* �ؼг�ʱ,ִ��ʧ�� */
          if (timeCounter > 250)//2.5s
          {
            timeCounter = 0;
            GAS8_resetBits(PILLARHOLDING_USING_GASID, PILLARHOLDING_GASPOS);//ʧ�ܾͻص�ԭλ
            temp_state = CLAMP_CLOSE_FAILED;
            stepCounter++;
          }
        }

        break;
      case 4:
        /* ��ʱ,����,ˢ�·���״̬,��λ״̬ */
        timeCounter++;
        if (timeCounter > 20)
        {
          timeCounter = 0;
          /* ���� */
          DYMX64AT_setSyncTarAng(1, PILLARHOLDING_USING_SERVOID, PILLARHOLDING_SERVO_LOCK_ANGLE);
          stepCounter = 0;
          state = temp_state;
        }
        break;

        default:
          break;
    }
  }
  else /* �ɿ� */
  {
    switch (stepCounter)
    {
      case 0:
        if (!Hall_PILLARHOLDING_LOCK()) /* һ��ʼ���ɵ�״̬ */
        {
          /* ���� */
          DYMX64AT_setSyncTarAng(1, PILLARHOLDING_USING_SERVOID, PILLARHOLDING_SERVO_LOCK_ANGLE);
          state = CLAMP_OPEN_SUCCESS;
        }
        else
        {
          /* ��ʼ������ */
          state = CLAMP_RUNNING;
          timeCounter = 0;
          stepCounter++;
        }
        break;

      case 1:
        timeCounter++;
        /* ���� */
        if (timeCounter == 1)
        {
          DYMX64AT_setSyncTarAng(1, PILLARHOLDING_USING_SERVOID, PILLARHOLDING_SERVO_UNLOCK_ANGLE);
        }
        /* ��ʱ */
        if (timeCounter > 20)
        {
          timeCounter = 0;
          stepCounter++;
        }
        break;

      case 2:
        /* ���� */
        GAS8_resetBits(PILLARHOLDING_USING_GASID, PILLARHOLDING_GASPOS);
        stepCounter++;
        break;
      case 3:
        /* ��ʱ,����,ˢ�·���״̬,��λ״̬ */
        timeCounter++;
        if (timeCounter > 40)
        {
          timeCounter = 0;
          /* ���� */
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
  * @brief  ������ת�ٶ�
  * @param[in]  speed
  * @retval None
  */
void PillarHolding_setTurnSpeed(int16_t speed){
	//PillarHolding.M3508s[0].targetSpeed = PillarHolding.M3508s[1].targetSpeed = speed;
  PillarHolding.tagetSpeed = speed;
}

	
/**
  * @brief  ��������
  * @param  None
  * @retval None
  */
void PillarHolding_processing(void){
  
  PillarHolding.isLock = Hall_PILLARHOLDING_LOCK();
  
  LED_LIST(6, PillarHolding.isLock);//ָʾ��#
  
  if(PillarHolding.mode == PILLARHOLDING_MODE_OFF){
		PillarHolding.M3508s[0].outCurrent = PillarHolding.M3508s[1].outCurrent = 0;//ʧ��
		return;
	}
  
  if(dr16_data.offLineFlag || PillarHolding.mode == PILLARHOLDING_MODE_LOCK){
    PillarHolding.M3508s[0].targetSpeed = PillarHolding.M3508s[1].targetSpeed = 0;//Ŀ���ٶ�Ϊ0���õ���ŷ�������ס
	}
  
  if(PillarHolding.isStart){
    if(PillarHolding.type == PILLARHOLDING_STEPUP){
      PillarHolding_climbUpStep();//�ϵ�
    }
    else if(PillarHolding.type == PILLARHOLDING_STEPDOWN){
     PillarHolding_climbDownStep();//�µ�
    } 
    
  }

  if(PillarHolding.turnEnable){
    float turnspeed = Position_PID(&PillarHolding.pid_pos, PillarHolding.tagetAngle, Cloud.IMU.totalYaw);
    PillarHolding_setTurnSpeed(turnspeed);//������Ħ������ת�ϵ�
  }
  else{
    PositionPID_clear(&PillarHolding.pid_pos);
     PillarHolding_setTurnSpeed(0);//0�ŷ�ס
  }
  
  
  if((!PillarHolding.M3508s[0].offLineFlag) && (!PillarHolding.M3508s[1].offLineFlag)){
    for(int i = 0; i < 2; i++){
      /* �Ƕ�PID����(�⻷) */
      //int M3508_PIDOut = Position_PID(&Lifting.M3508s[i].pid_angle, Lifting.M3508s[i].targetAngle, Lifting.M3508s[i].totalAngle);
      /* �ٶ�PID����(�ڻ�) */
      PillarHolding.M3508s[i].outCurrent = Position_PID(&PillarHolding.M3508s[i].pid_speed,\
			PillarHolding.tagetSpeed, PillarHolding.M3508s[i].realSpeed);
      /* ���־λ */    //ͬ��ת��
      PillarHolding.M3508s[i].infoUpdateFlag = 0;
    }
  }
  
  //Climbing_setM3508Current(Climbing.M3508s[0].outCurrent,Climbing.M3508s[1].outCurrent, 0,0);
  
}

/**
  * @brief  �������裺�ϵ�
  * @param[in]  speed
  * @retval None
  */
void PillarHolding_climbUpStep(void){
  clampState_e clamp_state = CLAMP_RUNNING;
  switch (PillarHolding.nowStep)
  {
    case 0:
      if (!UpperStructrue.isOpen)//���Ƿ���̧��״̬
      {
        UpperStructure_open(Lifting.maxPos);  /* ̧�� */
      }
      else
      {
        /* ���ĵ������� */
        Chassis_setOffset(CHASSIS_HOLDING_OFFSET_X, CHASSIS_HOLDING_OFFSET_Y);
        PillarHolding.nowStep++;
      }

      break;
    case 1:
      /* ��һ�������ɲ����ַ��ͣ� */
      if (PillarHolding.nextReq)
      {
        /* ����ʧ�� */
        Chassis_setMode(CHASSIS_MODE_OFF);
        /* ���� */
        clamp_state = PillarHolding_setClamp(1);
        if(clamp_state == CLAMP_CLOSE_SUCCESS){
          PillarHolding.nextReq = 0;
          /* ����ʹ�� */
          Chassis_setMode(CHASSIS_MODE_FOLLOW);
          /* �гɹ�����һ�� */
          PillarHolding.nowStep++;
        }
        else if(clamp_state == CLAMP_CLOSE_FAILED){
          /* ����ʹ�� */
          Chassis_setMode(CHASSIS_MODE_FOLLOW);
          /* ��ʧ�ܣ�����ת */
          PillarHolding.nextReq = 0;//������һ��
        }
        
      }
      else if (PillarHolding.backReq)
      {
        /* û����һ�� */
        PillarHolding.backReq = 0;
      }
      break;

    case 2:

      if (PillarHolding.nextReq)
      {
        /* ��һ������:̧�� */
        if (UpperStructrue.isOpen)
        {
          UpperStructure_close();
        }
        else
        {
          PillarHolding.nextReq = 0;
          /* ���浱ǰ�Ƕ�,������ת */
          PillarHolding.lastAngle = Cloud.IMU.totalYaw;
          PillarHolding.tagetAngle = PillarHolding.lastAngle + 180;
          PillarHolding.turnEnable = 1;//Ħ���ֺ͵���ͬʱ��ת
          PillarHolding.nowStep++;
        }
      }
			
      else if (PillarHolding.backReq)
      {
        /* ����������һ�����ɼ�*/
        clamp_state = PillarHolding_setClamp(0);
        if(clamp_state == CLAMP_OPEN_SUCCESS){
          PillarHolding.backReq = 0;
          /* �ɼгɹ�����һ�� */
          PillarHolding.nowStep--;
        }
      }
      break;

    case 3:
      /* �ȴ���ת180�� */
      if (abs(PillarHolding.tagetAngle - Cloud.IMU.totalYaw) < 5 &&
          abs(PillarHolding.M3508s[0].realSpeed) < 50)
      {
        PillarHolding.turnEnable = 0;
        PillarHolding_setTurnSpeed(0);
        /* ���ĵ������� */
        Chassis_setOffset(0, 0);
        PillarHolding.nowStep++;
      }
      break;

    case 4:
      /* �����ɹ����ɼ� */
      clamp_state = PillarHolding_setClamp(0);
      if(clamp_state == CLAMP_OPEN_SUCCESS){
        
        PillarHolding.nowStep++;
      }
      break;
      
    case 5:
      if (!UpperStructrue.isOpen)
      {
        /* ̧������ȡ�߶� */
        UpperStructure_open(Lifting.catchPos);
      }
      else
      {
        /* ������� */
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
  * @brief  �������裺�µ�
  * @param[in]  speed
  * @retval None
  */
void PillarHolding_climbDownStep(void){
  clampState_e clamp_state = CLAMP_RUNNING;
  switch (PillarHolding.nowStep)
  {
    case 0:
      /* ���̧���ž��½� */
      if (UpperStructrue.isOpen)
      {
        UpperStructure_close();
      }
      else
      {
        /* ��΢̧�� */
        Lifting_setPosition(LIFTING_CLIMBDOWN_POS);//
        /* ���ĵ������� */
        Chassis_setOffset(CHASSIS_HOLDING_OFFSET_X, CHASSIS_HOLDING_OFFSET_Y);
        PillarHolding.nowStep++;
      }
      break;
    case 1:
      /* ��һ������ */
      if (PillarHolding.nextReq)
      {
        /* ����ʧ�� */
        Chassis_setMode(CHASSIS_MODE_OFF);
        /* ���� */
        clamp_state = PillarHolding_setClamp(1);
        if(clamp_state == CLAMP_CLOSE_SUCCESS){
          /* ����ʹ�� */
          Chassis_setMode(CHASSIS_MODE_FOLLOW);
          /* ��ʱ */
          if(PillarHolding.timeCounter++>30){
            PillarHolding.timeCounter = 0;
            /* ̧������� */
            Lifting_setPosition(LIFTING_DOWNLIMIT_OFFSET);
            PillarHolding.nextReq = 0;
            /* �гɹ�����һ�� */
            PillarHolding.nowStep++;
          }
        }
        else if(clamp_state == CLAMP_CLOSE_FAILED){
          /* ����ʹ�� */
          Chassis_setMode(CHASSIS_MODE_FOLLOW);
          /* ��΢̧�� */
          Lifting_setPosition(LIFTING_CLIMBDOWN_POS);
          /* ��ʧ�ܣ�����ת */
          PillarHolding.nextReq = 0;
        }
        
      }
      else if (PillarHolding.backReq)
      {
        /* û����һ�� */
        PillarHolding.backReq = 0;
      }
      break;

    case 2:
      /* ��һ������ */
      if (PillarHolding.nextReq)
      {
        /* ���浱ǰ�Ƕ�,������ת */
        PillarHolding.nextReq = 0;
        PillarHolding.lastAngle = Cloud.IMU.totalYaw;
        PillarHolding.tagetAngle = PillarHolding.lastAngle + 160;
        PillarHolding.turnEnable = 1;
        PillarHolding.nowStep++;
      }
      else if (PillarHolding.backReq)
      {
        /* ��΢̧�� */
        Lifting_setPosition(LIFTING_CLIMBDOWN_POS);
        /* ����������һ�����ɼ�*/
        clamp_state = PillarHolding_setClamp(0);
        if(clamp_state == CLAMP_OPEN_SUCCESS){
          PillarHolding.backReq = 0;
          /* �ɼгɹ�����һ�� */
          PillarHolding.nowStep--;
        }
      }
      
      break;

    case 3:
      /* �ȴ���ת180�� */
      if (abs(PillarHolding.tagetAngle - Cloud.IMU.totalYaw) < 10 &&
          abs(PillarHolding.M3508s[0].realSpeed) < 500)
      {
        PillarHolding.turnEnable = 0;
        PillarHolding_setTurnSpeed(0);
        /* ���ĵ������� */
        Chassis_setOffset(0, 0);
        PillarHolding.nowStep++;
      }
      break;

    case 4:
      /* �����ɹ����½� */
      if (!UpperStructrue.isOpen)
      {
        /* ���½����ϲ�̧���� */
        UpperStructure_open(Lifting.catchPos);
      }
      else
      {
        PillarHolding.nowStep++;
      }
      break;
      
    case 5:
      /* �����ɹ����ɼ� */
      clamp_state = PillarHolding_setClamp(0);
      if(clamp_state == CLAMP_OPEN_SUCCESS){
        
        /* ������� */
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
  * @brief  ��ʼ��/�µ�ģʽ
  * @param  type  �ϵ�ģʽ/�µ�ģʽ
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
  * @brief  ������/�µ�ģʽ
  * @param   type  �ϵ�ģʽ/�µ�ģʽ
  * @retval None
  */
void PillarHolding_close(void){
  if(!PillarHolding.isClose){
    /* ���ĵ������� */
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
  * @brief  �ֶ�������һ��
  * @param  None
  * @retval None
  */
void PillarHolding_nextReq(void){
  if(PillarHolding.isStart){
    PillarHolding.nextReq = 1;
  }
}


/**
  * @brief  �ֶ�������һ��
  * @param  None
  * @retval None
  */
void PillarHolding_backReq(void){
  if(PillarHolding.isStart){
    PillarHolding.backReq = 1;
  }
}
/*-----------------------------------FILE OF END------------------------------*/


