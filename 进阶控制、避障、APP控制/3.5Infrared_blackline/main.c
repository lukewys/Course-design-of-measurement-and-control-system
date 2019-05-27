//!
//!$说明：本实验结合CC2541蓝牙模块实现手机APP控制平衡车的前进，后退及转弯                  $
//###########################################################################
//$ file        main.c                  $
//$ author      HeDong                  $
//$ version     V1.0                    $
//$ project     F28069APPControl        $
//###########################################################################
#include "DSP28x_Project.h"     // 头文件
#define CCD_SI_SET    GpioDataRegs.GPASET.bit.GPIO7  //  GPIO 7输出高
#define CCD_CLK_SET   GpioDataRegs.GPASET.bit.GPIO6    // GPIO 6输出高
#define CCD_SI_CLR    GpioDataRegs.GPACLEAR.bit.GPIO7   //  GPIO 7输出低
#define CCD_CLK_CLR   GpioDataRegs.GPACLEAR.bit.GPIO6   //  GPIO 6输出低

__interrupt void TIMER0_ISR(void);   // TIMER0_ISR()函数声明
void Dly_us(void);
void Init_Infrared_GPIO(void);
void check_infrared(void);
float Angle_Balance=0,Gyro_Balance=0,Gyro_Turn=0;  //平衡倾角、平衡陀螺仪
int Balance_Pwm=0,Velocity_Pwm=0,Turn_Pwm=0;
int Moto1=0,Moto2=0;
int Encoder_Left=0,Encoder_Right=0;   // 左右轮编码器
float speed;
Uint32 distan=0;   // 超声波测距相关变量
Uint32 cap_val=0;
Uint8 Bi_zhang=0;
Uint8 Flag_Stop=1;
Uint8 Flag_qian=0,Flag_hou=0,Flag_left=0,Flag_right=0,Flag_sudu=2;//蓝牙遥控相关变量
Uint16 turn_rate=50;
int speed_table[];  //速度向量表
int dir;
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

   Init_KEY_GPIO();   // Key GPIO初始化
   Init_Infrared_GPIO();    //光电初始化
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

   speed=0;  //初始速度为零
   DELAY_US(2000000);
   speed=10;  //进行循迹时候的速度等于10
   for(;;)
   {
	    //LEDOverTurn();   // LED灯闪烁
		//DELAY_US(500000);   // 延时
       //turn_rate=turn_table();
	}
}
//###########################################################################
// $ 函数名称:  void turn_table(void)            $
// $ 函数功能:  调节速度函数,返回turn_rate                                           $
//###########################################################################
void check_infrared()  //非线性变换~~~~~~
{  Flag_qian=1;
   Flag_left=0;
	if(R2==0&&R1==0&&L1==0&&L2==0)       //向后转
	{dir=0;
	 Flag_left=1;
	 Flag_qian=0;}
	else if(R2==1&&R1==0&&L1==0&&L2==0)   //右转大弯
	{dir=2;}
	else if(R1==1&&L1==0&&L2==0)          //右转小弯
	{dir=1;}
	else if(R2==0&&R1==0&&L1==1)         //左转大弯
	{dir=-1;}
	else if(R2==0&&R1==0&&L1==0&&L2==1)   //左转小弯
	{dir=-2;}
	else    //其余全部直行，包括十字路口
	{   dir=5;
	    Flag_left=0;}
}
//###########################################################################
// $ 函数名称:  velocity(int encoder_left,int encoder_right)     $
// $ 函数功能:  速度环PID控制函数                                                                                             $
//###########################################################################
int velocity(int encoder_left,int encoder_right)
{
	static float Velocity=0,Encoder_Least=0,Encoder=0,Movement=0;
	static float Encoder_Integral=0;
	float kp=106,ki=0.73;//kp=46,ki=0.23;
	Movement=-speed;
	if(dir==0) Movement=0;   //速度为零，掉头
	Encoder_Least =(encoder_left+encoder_right)-0; //获取最新速度偏差=测量速度（左右编码器之和）-目标速度（此处为 0）
	Encoder *= 0.8; //一阶低通滤波器
	Encoder += Encoder_Least*0.2; //一阶低通滤波器
	Encoder_Integral +=Encoder; //积分出位移
	Encoder_Integral=Encoder_Integral-Movement; //接收遥控器数据，控制前进后退                                 //积分出位移，积分时间：10ms
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
	static float turn_big=400,turn_small=150,Kd_xunji=0.27;
	   //===============遥控左右旋转部分========================//
		if(1==Flag_left||1==Flag_right)				//根据旋转前的速度调整速度的起始速度
		{
			if(++Turn_Count==1)
			Encoder_Temp=myabs(encoder_left+encoder_right);
			Turn_Convert=turn_rate/Encoder_Temp;
			if(Turn_Convert<0.6) Turn_Convert=0.6;
			if(Turn_Convert>4)	Turn_Convert=4;
		}
		else
		{   Turn_Convert=0.9;
			Turn_Count=0;
			Encoder_Temp=0;
		}
		if(1==Flag_left)			Turn_Target-=Turn_Convert;
		else if(1==Flag_right) 	Turn_Target+=Turn_Convert;
		else Turn_Target=0;

		if(Turn_Target>Turn_Amplitude)	Turn_Target=Turn_Amplitude;//转向速度限幅
		if(Turn_Target<-Turn_Amplitude)	Turn_Target=-Turn_Amplitude;
		if(Flag_qian==1||Flag_hou==1)   Kd=0.5;
		else Kd=0;		//转向的时候取消陀螺仪的纠正
	//===================决策方法一===============================//
		if(dir==5||dir==0)   //前进或者掉头
		{	Turn=-Turn_Target*Kp-gyro*Kd;
		}
		else    //转向
		{	if(dir==2) 		 Turn_Target=-turn_big;
			else if(dir==-2) Turn_Target=turn_big;
			else if(dir==1)  Turn_Target=-turn_small;
			else if(dir==-1) Turn_Target=turn_small;
			Turn=Turn_Target-gyro*Kd_xunji;           //CCD产生的偏差和命令同时
		}
    //===================决策方法二===================================//
	/*	if(dir==0)   //掉头
		{	Turn=-Turn_Target*Kp-gyro*Kd;
		}
		else
		{	//转向
			if(dir==2) 		 {Turn_Target=-turn_big;}
			else if(dir==-2) {Turn_Target=turn_big;}
			else if(dir==1)  {Turn_Target=-turn_small;}
			else if(dir==-1) {Turn_Target=turn_small;}
			//前进
			else if(dir==5)  {Turn_Target=0;}

		Turn=Turn_Target-gyro*Kd_xunji;           //CCD产生的偏差和命令同时
		}
		*/
    //=========================================================//
	return Turn;
}
//###########################################################################
// $ 函数名称:  balance(float Angle,float Gyro)     $
// $ 函数功能:  直立平衡PID控制函数                                                                               $
//###########################################################################
int balance(float Angle,float Gyro)    //pd控制
{
     float Bias,kp=350,kd=0.88;//72 0.24
	 int balance;
	 Bias=Angle-ZHONGZHI;       //===求出平衡的角度中值 和机械相关
	 balance=(int)(kp*Bias+Gyro*kd);   //===求出平衡控制的电机PWM PD控制
	 return balance;
}
//###########################################################################
// $ 函数名称:  TIMER0_ISR(void)            $
// $ 函数功能:  Timer0的中断服务函数                                            $
//###########################################################################
__interrupt void TIMER0_ISR(void)
{
	Encoder_Left=Read_Encoder(1);   // 读取左轮编码器数据
	Encoder_Right=Read_Encoder(2);   // 读取右轮编码器数据
    check_infrared();
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

void Dly_us(void)
{
int ii;
for(ii=0;ii<10;ii++);//假设一条FOR语句执行周期为8个，则8*10*12.5ns=1us
}
//###########################################################################
// $ 函数名称:  void Init_Infrared_GPIO(void)    $
// $ 函数功能:  初始化光电的GPIO引脚         $
//###########################################################################
void Init_Infrared_GPIO(void)
{
   EALLOW;
   GpioCtrlRegs.GPBPUD.bit.GPIO44 = 0;   // 使能上拉
   //GpioCtrlRegs.GPBQSEL1.bit.GPIO44 = 0;   // 同步系统时钟
   GpioCtrlRegs.GPBMUX1.bit.GPIO44=0;   // 配置为GPIO功能
   GpioCtrlRegs.GPBDIR.bit.GPIO44=0;   // 配置为输入方向

   GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;   // 使能上拉
    //GpioCtrlRegs.GPAQSEL1.bit.GPIO7 = 0;   // 同步系统时钟
    GpioCtrlRegs.GPAMUX1.bit.GPIO7=0;   // 配置为GPIO功能
    GpioCtrlRegs.GPADIR.bit.GPIO7=0;   // 配置为输入方向

    GpioCtrlRegs.GPAPUD.bit.GPIO8 = 0;   // 使能上拉
     //GpioCtrlRegs.GPAQSEL1.bit.GPIO8 = 0;   // 同步系统时钟
     GpioCtrlRegs.GPAMUX1.bit.GPIO8=0;   // 配置为GPIO功能
     GpioCtrlRegs.GPADIR.bit.GPIO8=0;   // 配置为输入方向

     GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;   // 使能上拉
      //GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 0;   // 同步系统时钟
      GpioCtrlRegs.GPAMUX2.bit.GPIO16=0;   // 配置为GPIO功能
      GpioCtrlRegs.GPADIR.bit.GPIO16=0;   // 配置为输入方向

   EDIS;
}

