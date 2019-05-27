//!
//!$说明：本实验结合CC2541蓝牙模块实现手机APP控制平衡车的前进，后退及转弯                  $
//###########################################################################
//$ file        main.c                  $
//$ author      HeDong                  $
//$ version     V1.0                    $
//$ project     F28069APPControl        $
//###########################################################################

#include "DSP28x_Project.h"     // 头文件

__interrupt void TIMER0_ISR(void);   // TIMER0_ISR()函数声明

float Angle_Balance=0,Gyro_Balance=0,Gyro_Turn=0;  //平衡倾角、平衡陀螺仪
int Balance_Pwm=0,Velocity_Pwm=0,Turn_Pwm=0;
int Moto1=0,Moto2=0;
int Encoder_Left=0,Encoder_Right=0;   // 左右轮编码器
Uint32 distan=0;   // 超声波测距相关变量
Uint32 cap_val=0;
Uint8 Bi_zhang=0;
Uint8 Flag_Stop=1;
Uint8 Flag_qian=0,Flag_hou=0,Flag_left=0,Flag_right=0,Flag_sudu=2,Turn_rate=50;//蓝牙遥控相关变量


//Flash烧写相关变量
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
extern Uint16 RamfuncsLoadSize;

//###########################################################################
// $ 函数名称:  int main(void)     $
//###########################################################################
int main(void)
{
   InitSysCtrl();   // 初始化系统时钟，80MHz
   DINT;   // 清除所有中断
   InitPieCtrl();   // 初始化PIE控制寄存器
   IER = 0x0000;   // 禁止CPU中断
   IFR = 0x0000;   // 清除CPU中断标志
   InitPieVectTable();   // 初始化PIE中断表

   //Flash烧写相关语句
   memcpy(&RamfuncsRunStart,&RamfuncsLoadStart,(Uint32)&RamfuncsLoadSize);
   InitFlash();

   InitLEDGPIO();   // LED GPIO初始化
   Init_KEY_GPIO();   // Key GPIO初始化
   /*********moto配置***********/
   InitPWM1(2000);   // EPwm1模块初始化
   InitPWM2(2000);   // EPwm2模块初始化
   /*********qep配置***********/
   InitEQep1Gpio();   // QEP1功能引脚初始化
   InitEQep2Gpio();   // QEP2功能引脚初始化
   POSSPEED_Init();   // QEP1和QEP2模块初始化
   /*********定时器配置***********/
   EALLOW;
   PieVectTable.TINT0=&TIMER0_ISR;   // TINT0的中断映射
   EDIS;

   InitCpuTimers();   //初始化TIMER0
   ConfigCpuTimer(&CpuTimer0,80,5000);   // 设置为5ms做控制周期

	/********ECAP配置***********/
   EALLOW;
   PieVectTable.ECAP3_INT=&Ecap3_isr;
   EDIS;
   Init_ECap3();   // Ecap3模块初始化
   /*********SCI配置***********/
	EALLOW;
	PieVectTable.SCIRXINTA = &scia_rx_isr;
	EDIS;
	scia_init();
	/********IIC配置***********/
   Init_I2CA();   // 初始化IIC模块
   MPU6050_Init();   // MPU6050初始化
   DELAY_US(10000);   // 延时
   /********************/
   IER |=M_INT1;   // 允许CPU级中断
   PieCtrlRegs.PIEIER1.bit.INTx7  =1;   // 允许Timer0的PIE中断

   IER |=M_INT4;    // 允许CPU级中断
   PieCtrlRegs.PIEIER4.bit.INTx3 =1;   // 开ECap3的PIE中断

   IER |=M_INT9;   // 允许CPU级中断
   PieCtrlRegs.PIEIER9.bit.INTx1 = 1;   // 开Scia的接收PIE中断

   EINT;   // 使能全局中断
   ERTM;   // 使能全局中断DBGM

   Turn_rate = 120;
   DELAY_US(10000000);
   //Flag_qian = 1;
   //DELAY_US(1500000);

   Flag_left = 1;
   DELAY_US(640000);
   Flag_left = 0;
   //Flag_qian = 0;
/*
   for(;;)
   {
	   LEDOverTurn();   // LED灯闪烁

		DELAY_US(500000);   // 延时

   }
*/
}

//###########################################################################
// $ 函数名称:  TIMER0_ISR(void)            $
// $ 函数功能:  Timer0的中断服务函数                                            $
//###########################################################################
__interrupt void TIMER0_ISR(void)
{
	Encoder_Left=Read_Encoder(1);   // 读取左轮编码器数据
	Encoder_Right=Read_Encoder(2);   // 读取右轮编码器数据
	ReadDistance();   // 计算距离
	Key();   // 按键启停信号
	MPU6050_Read();   // 度取MPU6050数据
	Get_Angle();   // 计算结果
	Balance_Pwm =balance(Angle_Balance,Gyro_Balance);   // 计算平衡控制量
	Velocity_Pwm=velocity(Encoder_Left,Encoder_Right);    // 计算速度环控制量
	Turn_Pwm=turn(Encoder_Left,Encoder_Right,Gyro_Turn);   // 计算转向环控制量
    Moto1=Balance_Pwm+Velocity_Pwm-Turn_Pwm;
	Moto2=Balance_Pwm+Velocity_Pwm+Turn_Pwm;
	Xianfu_Pwm();   // 限赋
	if(Turn_Off(Angle_Balance)==0)   // 倾角保护
	Set_Pwm(Moto1,Moto2);   // 设置PWM的值
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;    // 允许TIMER0的PIE中断
}


//###########################################################################
// $ 函数名称:  balance(float Angle,float Gyro)     $
// $ 函数功能:  直立平衡PID控制函数                                                                               $
//###########################################################################
int balance(float Angle,float Gyro)
{
     float Bias,kp=350,kd=0.88;//72 0.24
	 int balance;
	 Bias=Angle-ZHONGZHI;       //===求出平衡的角度中值 和机械相关
	 balance=(int)(kp*Bias+Gyro*kd);   //===求出平衡控制的电机PWM PD控制
	 return balance;
}


//###########################################################################
// $ 函数名称:  velocity(int encoder_left,int encoder_right)     $
// $ 函数功能:  速度环PID控制函数                                                                                             $
//###########################################################################
int velocity(int encoder_left,int encoder_right)
{
	   static float Velocity=0,Encoder_Least=0,Encoder=0,Movement=0;
		static float Encoder_Integral=0,Target_Velocity=0;
		float kp=150,ki=0.73;//46 0.23 , 106 0.73
		//==============================================//
		if(Flag_sudu==1)		Target_Velocity=28;
		else 				Target_Velocity=56;
		if(1==Flag_qian)   	Movement=-Target_Velocity/Flag_sudu;
		else if(1==Flag_hou) Movement=Target_Velocity/Flag_sudu;
		else Movement=0;
	//==============================================//
	Encoder_Least =(encoder_left+encoder_right)-0;                    //获取最新速度偏差=测量速度（左右编码器之和）-目标速度（此处为0）
	Encoder *= 0.8;		                                              //一阶低通滤波器
	Encoder += Encoder_Least*0.2;	                                  //一阶低通滤波器
	Encoder_Integral +=Encoder;                                       //积分出位移，积分时间：10ms
	Encoder_Integral=Encoder_Integral-Movement;                       //接收遥控器数据，控制前进后退
	if(Encoder_Integral>5000)  	Encoder_Integral=5000;             //积分限幅
	if(Encoder_Integral<-5000)	Encoder_Integral=-5000;              //积分限幅
	Velocity=(int)(Encoder*kp+Encoder_Integral*ki);                          //速度控制
	if(Turn_Off(Angle_Balance)==1)   Encoder_Integral=0;    //电机关闭后清除积分
	return Velocity;
}


//###########################################################################
// $ 函数名称:  turn(int encoder_left,int encoder_right,float gyro)        $
// $ 函数功能:  转向环控制量                                                                                                                           $
//###########################################################################
int turn(int encoder_left,int encoder_right,float gyro)
{
	static float Turn_Target=0,Turn,Encoder_Temp=0,Turn_Convert=0.9,Turn_Count=0;
	float Turn_Amplitude=44/Flag_sudu,Kp=42,Kd=0;
	//===============遥控左右旋转部分========================//
	if(1==Flag_left||1==Flag_right)				//根据旋转前的速度调整速度的起始速度
	{
		if(++Turn_Count==1)
		Encoder_Temp=myabs(encoder_left+encoder_right);
		Turn_Convert=Turn_rate/Encoder_Temp;
		if(Turn_Convert<0.6) Turn_Convert=0.6;
		if(Turn_Convert>5)	Turn_Convert=4; //改了，原来是3
	}
	else
	{
		Turn_Convert=0.9;
		Turn_Count=0;
		Encoder_Temp=0;
	}
	if(1==Flag_left)			Turn_Target-=Turn_Convert;
	else if(1==Flag_right) 	Turn_Target+=Turn_Convert;
	else Turn_Target=0;

	if(Turn_Target>Turn_Amplitude)	Turn_Target=Turn_Amplitude;//转向速度限幅
	if(Turn_Target<-Turn_Amplitude)	Turn_Target=-Turn_Amplitude;
	if(Flag_qian==1||Flag_hou==1)  Kd=0.5;
	else Kd=0;		//转向的时候取消陀螺仪的纠正
	//==================转向PD控制器==========================//
	Turn=-Turn_Target*Kp-gyro*Kd;
	return Turn;
}



//###########################################################################
// $ 函数名称:  Set_Pwm(int moto1,int moto2)        $
// $ 函数功能:  对EPwm1和EPwm2的比较寄存器分别赋值                                     $
//###########################################################################
void Set_Pwm(int moto1,int moto2)
{
	if(moto1>0)  PWM1A=2000,PWM1B=2000-myabs(moto1);
	else 	     PWM1A=2000-myabs(moto1),PWM1B=2000;

	if(moto2>0)  PWM2A=2000-myabs(moto2),PWM2B=2000;
	else 	     PWM2A=2000,PWM2B=2000-myabs(moto2);
}


//###########################################################################
// $ 函数名称:  Xianfu_Pwm(void)                   $
// $ 函数功能:  对EPwm1和EPwm2的比较寄存器值进行限制大小                     $
//###########################################################################
void Xianfu_Pwm(void)
{
	int Amplitude=2000;
    if(Moto1<-Amplitude) Moto1=-Amplitude;
    if(Moto1>Amplitude)  Moto1=Amplitude;
	if(Moto2<-Amplitude) Moto2=-Amplitude;
	if(Moto2>Amplitude)  Moto2=Amplitude;
}


//###########################################################################
// $ 函数名称:  myabs(int a)                   $
// $ 函数功能:  取绝对值函数                                                                                 $
//###########################################################################
int myabs(int a)
{
	  int temp;
	  if(a<0)  temp=-a;
	  else temp=a;
	  return temp;
}


//###########################################################################
// $ 函数名称:  void InitLEDGPIO(void)     $
// $ 函数功能:  初始化LED的GPIO引脚         $
//###########################################################################
void InitLEDGPIO(void)
{
   EALLOW;
   GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;   // 配置为GPIO功能
   GpioCtrlRegs.GPADIR.bit.GPIO12 = 1;   // 配置为输出方向
   GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 0;   // 配置为GPIO功能
   GpioCtrlRegs.GPADIR.bit.GPIO18 = 1;   // 配置为输出方向
   EDIS;
}


//###########################################################################
// $ 函数名称:  void LEDOverTurn(void)     $
// $ 函数功能:  LED灯翻转           $
//###########################################################################
void LEDOverTurn(void)
{
   GpioDataRegs.GPATOGGLE.bit.GPIO12 = 1;   //GPIO5电平翻转
   GpioDataRegs.GPATOGGLE.bit.GPIO18 = 1;   //GPIO6电平翻转
}


//###########################################################################
// $ 函数名称:  void Key(void)    $
// $ 函数功能:  按键检测                                            $
//###########################################################################
void Key(void)
{
	Uint8 val;
	val=click();
	if(val==1)  Flag_Stop=!Flag_Stop;
}



//###########################################################################
// $ 函数名称:  Turn_Off(float angle)    $
// $ 函数功能:  倾角保护               $
//###########################################################################
Uint8 Turn_Off(float angle)
{
    Uint8 temp;
	if(angle<-40||angle>40||1==Flag_Stop)//倾角大于40度关闭电机
	{
      temp=1;
      PWM1A=0;
      PWM1B=0;
      PWM2A=0;
      PWM2B=0;
    }
	else
      temp=0;
    return temp;
}
