  RM2020_Engineer  
  =======
  @ author  LXY 
  @ version  V0.016  

  Update
  ========
  @ author  LXY   
  2020赛季：
  * 2020-01-13 开始调试2020工程，可自动抬升高度，手动夹取，添加救援，小弹仓代码
  * 2020-01-14 夹取位置可自动左右
  * 2020-01-16 发现夹取失败时不能判断为失败的bug
  * 2020-02-14 加入资源岛抢弹流程
  * 2020-08-20 修正夹取判断bug（能够判断失败或成功）
  * 2020-10-01 加入升降摄像头的舵机代码，打开大弹药箱时要抬一下摄像头，不然会撞到
  ******************************************************************************
  I/O口与外设：
  ========
  * 时钟：SYSCLK-180M  
	  APB1-45M，Timer-90M  
      APB2-90M，Timer-180M  

  *	CAN:  
    CAN1: PD0(RX), PD1(TX)      底盘电机1,2,3,4，抬升7,8 
    CAN2: PB12(RX), PB13(TX)    夹取1,2
    
    * 底盘、抬升电机ID (CAN1)   

                   （车尾）  
          |\|= 3              4 =|/|

                == 8       7 ==   （抬升机构3508）

          |/|= 2              1 =|\|
               夹取机构（车头）

    * 云台电机ID  （暂无云台）

            -------------
                  | 
                 ( ) pitch 
                -----
                 |_|  yaw 

    * 夹取电机ID (CAN2)  

			  |          |
			  |          |
			  |          |
			  ---（1）----
				   |
			------------------
		   |				  |
		   |				  |（2）
		   |				  |


                       

  *	串口：  
    USART1:     PB7(RX)            DMA1-通道4数据流5  DR遥控器  
	USART2：	PD5(TX), PD6(RX)   					  
    USART3：	PD8(TX), PD9(RX)   DMA1-通道4数据流1  云台IMU  
    USART6：	PG9(RX), PG14(TX)  DMA2-通道5数据流1  裁判系统
  
  * GPIO：  
    PA0：    抬升霍尔 上限位  
    PA1：    抬升霍尔 下限位
	PA2：    光电门
	PA3：    
    * 霍尔

            -------------     最高点MAX
              |       |
              |       |
            -------------     夹取高度
              |       |
            -------------     最低点MIN
             (_)     (_)
          
 
 *	气阀控制器：  
	ID:2
	2：	  小弹丸弹仓伸出
	3：	  弹弹药箱
    4：	  夹子上的夹取
	5：	  第二排伸出	
	6：	  大弹仓仓门
	8：	  小弹仓仓门

	ID:1
	

  中断优先级配置：（优先级分组4）
  =======
  * CAN1 接收中断：	  5, 0  
  * CAN2 接收中断：	  5, 0  
  * USART1 空闲中断：	6, 0    
  * USART6 空闲中断：	6, 0  

  PID参数(此处未更新)：
  =======
