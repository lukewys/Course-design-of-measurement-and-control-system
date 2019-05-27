//!
//!$˵������ʵ����CC2541����ģ��ʵ���ֻ�APP����ƽ�⳵��ǰ�������˼�ת��                  $
//###########################################################################
//$ file        main.c                  $
//$ author      HeDong                  $
//$ version     V1.0                    $
//$ project     F28069APPControl        $
//###########################################################################
#include "DSP28x_Project.h"     // ͷ�ļ�
#define CCD_SI_SET    GpioDataRegs.GPASET.bit.GPIO7  //  GPIO 7�����
#define CCD_CLK_SET   GpioDataRegs.GPASET.bit.GPIO6    // GPIO 6�����
#define CCD_SI_CLR    GpioDataRegs.GPACLEAR.bit.GPIO7   //  GPIO 7�����
#define CCD_CLK_CLR   GpioDataRegs.GPACLEAR.bit.GPIO6   //  GPIO 6�����

__interrupt void TIMER0_ISR(void);   // TIMER0_ISR()��������
void Dly_us(void);
void Init_Infrared_GPIO(void);
void check_infrared(void);
float Angle_Balance=0,Gyro_Balance=0,Gyro_Turn=0;  //ƽ����ǡ�ƽ��������
int Balance_Pwm=0,Velocity_Pwm=0,Turn_Pwm=0;
int Moto1=0,Moto2=0;
int Encoder_Left=0,Encoder_Right=0;   // �����ֱ�����
float speed;
Uint32 distan=0;   // �����������ر���
Uint32 cap_val=0;
Uint8 Bi_zhang=0;
Uint8 Flag_Stop=1;
Uint8 Flag_qian=0,Flag_hou=0,Flag_left=0,Flag_right=0,Flag_sudu=2;//����ң����ر���
Uint16 turn_rate=50;
int speed_table[];  //�ٶ�������
int dir;
//Flash��д��ر���
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
extern Uint16 RamfuncsLoadSize;

//###########################################################################
// $ ��������:  int main(void)     $
//###########################################################################
int main(void)
{
   InitSysCtrl();   // ��ʼ��ϵͳʱ�ӣ�80MHz
   DINT;   // ��������ж�
   InitPieCtrl();   // ��ʼ��PIE���ƼĴ���
   IER = 0x0000;   // ��ֹCPU�ж�
   IFR = 0x0000;   // ���CPU�жϱ�־
   InitPieVectTable();   // ��ʼ��PIE�жϱ�

   //Flash��д������
   memcpy(&RamfuncsRunStart,&RamfuncsLoadStart,(Uint32)&RamfuncsLoadSize);
   InitFlash();

   Init_KEY_GPIO();   // Key GPIO��ʼ��
   Init_Infrared_GPIO();    //����ʼ��
   /*********moto����***********/
   InitPWM1(2000);   // EPwm1ģ���ʼ��
   InitPWM2(2000);   // EPwm2ģ���ʼ��
   /*********qep����***********/
   InitEQep1Gpio();   // QEP1�������ų�ʼ��
   InitEQep2Gpio();   // QEP2�������ų�ʼ��
   POSSPEED_Init();   // QEP1��QEP2ģ���ʼ��
   /*********��ʱ������***********/
   EALLOW;
   PieVectTable.TINT0=&TIMER0_ISR;   // TINT0���ж�ӳ��

   EDIS;

   InitCpuTimers();   //��ʼ��TIMER0
   ConfigCpuTimer(&CpuTimer0,80,5000);   // ����Ϊ5ms����������

	/********ECAP����***********/
   EALLOW;
   PieVectTable.ECAP3_INT=&Ecap3_isr;
   EDIS;
   Init_ECap3();   // Ecap3ģ���ʼ��
   /*********SCI����***********/
	EALLOW;
	PieVectTable.SCIRXINTA = &scia_rx_isr;
	EDIS;
	scia_init();
	/********IIC����***********/
   Init_I2CA();   // ��ʼ��IICģ��
   MPU6050_Init();   // MPU6050��ʼ��
   DELAY_US(10000);   // ��ʱ
   /********************/
   IER |=M_INT1;   // ����CPU���ж�
   PieCtrlRegs.PIEIER1.bit.INTx7  =1;   // ����Timer0��PIE�ж�

   IER |=M_INT4;    // ����CPU���ж�
   PieCtrlRegs.PIEIER4.bit.INTx3 =1;   // ��ECap3��PIE�ж�

   IER |=M_INT9;   // ����CPU���ж�
   PieCtrlRegs.PIEIER9.bit.INTx1 = 1;   // ��Scia�Ľ���PIE�ж�

   EINT;   // ʹ��ȫ���ж�
   ERTM;   // ʹ��ȫ���ж�DBGM

   speed=0;  //��ʼ�ٶ�Ϊ��
   DELAY_US(2000000);
   speed=10;  //����ѭ��ʱ����ٶȵ���10
   for(;;)
   {
	    //LEDOverTurn();   // LED����˸
		//DELAY_US(500000);   // ��ʱ
       //turn_rate=turn_table();
	}
}
//###########################################################################
// $ ��������:  void turn_table(void)            $
// $ ��������:  �����ٶȺ���,����turn_rate                                           $
//###########################################################################
void check_infrared()  //�����Ա任~~~~~~
{  Flag_qian=1;
   Flag_left=0;
	if(R2==0&&R1==0&&L1==0&&L2==0)       //���ת
	{dir=0;
	 Flag_left=1;
	 Flag_qian=0;}
	else if(R2==1&&R1==0&&L1==0&&L2==0)   //��ת����
	{dir=2;}
	else if(R1==1&&L1==0&&L2==0)          //��תС��
	{dir=1;}
	else if(R2==0&&R1==0&&L1==1)         //��ת����
	{dir=-1;}
	else if(R2==0&&R1==0&&L1==0&&L2==1)   //��תС��
	{dir=-2;}
	else    //����ȫ��ֱ�У�����ʮ��·��
	{   dir=5;
	    Flag_left=0;}
}
//###########################################################################
// $ ��������:  velocity(int encoder_left,int encoder_right)     $
// $ ��������:  �ٶȻ�PID���ƺ���                                                                                             $
//###########################################################################
int velocity(int encoder_left,int encoder_right)
{
	static float Velocity=0,Encoder_Least=0,Encoder=0,Movement=0;
	static float Encoder_Integral=0;
	float kp=106,ki=0.73;//kp=46,ki=0.23;
	Movement=-speed;
	if(dir==0) Movement=0;   //�ٶ�Ϊ�㣬��ͷ
	Encoder_Least =(encoder_left+encoder_right)-0; //��ȡ�����ٶ�ƫ��=�����ٶȣ����ұ�����֮�ͣ�-Ŀ���ٶȣ��˴�Ϊ 0��
	Encoder *= 0.8; //һ�׵�ͨ�˲���
	Encoder += Encoder_Least*0.2; //һ�׵�ͨ�˲���
	Encoder_Integral +=Encoder; //���ֳ�λ��
	Encoder_Integral=Encoder_Integral-Movement; //����ң�������ݣ�����ǰ������                                 //���ֳ�λ�ƣ�����ʱ�䣺10ms
	if(Encoder_Integral>5000)  	Encoder_Integral=5000;             //�����޷�
	if(Encoder_Integral<-5000)	Encoder_Integral=-5000;              //�����޷�
	Velocity=(int)(Encoder*kp+Encoder_Integral*ki);                          //�ٶȿ���
	if(Turn_Off(Angle_Balance)==1)   Encoder_Integral=0;    //����رպ��������
	return Velocity;
}
//###########################################################################
// $ ��������:  turn(int encoder_left,int encoder_right,float gyro)        $
// $ ��������:  ת�򻷿�����                                                                                                                           $
//###########################################################################
int turn(int encoder_left,int encoder_right,float gyro)
{
	static float Turn_Target=0,Turn,Encoder_Temp=0,Turn_Convert=0.9,Turn_Count=0;
	float Turn_Amplitude=44/Flag_sudu,Kp=42,Kd=0;
	static float turn_big=400,turn_small=150,Kd_xunji=0.27;
	   //===============ң��������ת����========================//
		if(1==Flag_left||1==Flag_right)				//������תǰ���ٶȵ����ٶȵ���ʼ�ٶ�
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

		if(Turn_Target>Turn_Amplitude)	Turn_Target=Turn_Amplitude;//ת���ٶ��޷�
		if(Turn_Target<-Turn_Amplitude)	Turn_Target=-Turn_Amplitude;
		if(Flag_qian==1||Flag_hou==1)   Kd=0.5;
		else Kd=0;		//ת���ʱ��ȡ�������ǵľ���
	//===================���߷���һ===============================//
		if(dir==5||dir==0)   //ǰ�����ߵ�ͷ
		{	Turn=-Turn_Target*Kp-gyro*Kd;
		}
		else    //ת��
		{	if(dir==2) 		 Turn_Target=-turn_big;
			else if(dir==-2) Turn_Target=turn_big;
			else if(dir==1)  Turn_Target=-turn_small;
			else if(dir==-1) Turn_Target=turn_small;
			Turn=Turn_Target-gyro*Kd_xunji;           //CCD������ƫ�������ͬʱ
		}
    //===================���߷�����===================================//
	/*	if(dir==0)   //��ͷ
		{	Turn=-Turn_Target*Kp-gyro*Kd;
		}
		else
		{	//ת��
			if(dir==2) 		 {Turn_Target=-turn_big;}
			else if(dir==-2) {Turn_Target=turn_big;}
			else if(dir==1)  {Turn_Target=-turn_small;}
			else if(dir==-1) {Turn_Target=turn_small;}
			//ǰ��
			else if(dir==5)  {Turn_Target=0;}

		Turn=Turn_Target-gyro*Kd_xunji;           //CCD������ƫ�������ͬʱ
		}
		*/
    //=========================================================//
	return Turn;
}
//###########################################################################
// $ ��������:  balance(float Angle,float Gyro)     $
// $ ��������:  ֱ��ƽ��PID���ƺ���                                                                               $
//###########################################################################
int balance(float Angle,float Gyro)    //pd����
{
     float Bias,kp=350,kd=0.88;//72 0.24
	 int balance;
	 Bias=Angle-ZHONGZHI;       //===���ƽ��ĽǶ���ֵ �ͻ�е���
	 balance=(int)(kp*Bias+Gyro*kd);   //===���ƽ����Ƶĵ��PWM PD����
	 return balance;
}
//###########################################################################
// $ ��������:  TIMER0_ISR(void)            $
// $ ��������:  Timer0���жϷ�����                                            $
//###########################################################################
__interrupt void TIMER0_ISR(void)
{
	Encoder_Left=Read_Encoder(1);   // ��ȡ���ֱ���������
	Encoder_Right=Read_Encoder(2);   // ��ȡ���ֱ���������
    check_infrared();
	Key();   // ������ͣ�ź�
	MPU6050_Read();   // ��ȡMPU6050����
	Get_Angle();   // ������
	Balance_Pwm =balance(Angle_Balance,Gyro_Balance);   // ����ƽ�������
	Velocity_Pwm=velocity(Encoder_Left,Encoder_Right);    // �����ٶȻ�������
	Turn_Pwm=turn(Encoder_Left,Encoder_Right,Gyro_Turn);   // ����ת�򻷿�����
    Moto1=Balance_Pwm+Velocity_Pwm-Turn_Pwm;
	Moto2=Balance_Pwm+Velocity_Pwm+Turn_Pwm;
	Xianfu_Pwm();   // �޸�
	if(Turn_Off(Angle_Balance)==0)   // ��Ǳ���
	Set_Pwm(Moto1,Moto2);   // ����PWM��ֵ
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;    // ����TIMER0��PIE�ж�
}
//###########################################################################
// $ ��������:  Set_Pwm(int moto1,int moto2)        $
// $ ��������:  ��EPwm1��EPwm2�ıȽϼĴ����ֱ�ֵ                                     $
//###########################################################################
void Set_Pwm(int moto1,int moto2)
{
	if(moto1>0)  PWM1A=2000,PWM1B=2000-myabs(moto1);
	else 	     PWM1A=2000-myabs(moto1),PWM1B=2000;

	if(moto2>0)  PWM2A=2000-myabs(moto2),PWM2B=2000;
	else 	     PWM2A=2000,PWM2B=2000-myabs(moto2);
}
//###########################################################################
// $ ��������:  Xianfu_Pwm(void)                   $
// $ ��������:  ��EPwm1��EPwm2�ıȽϼĴ���ֵ�������ƴ�С                     $
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
// $ ��������:  myabs(int a)                   $
// $ ��������:  ȡ����ֵ����                                                                                 $
//###########################################################################
int myabs(int a)
{
	  int temp;
	  if(a<0)  temp=-a;
	  else temp=a;
	  return temp;
}

//###########################################################################
// $ ��������:  void Key(void)    $
// $ ��������:  �������                                            $
//###########################################################################
void Key(void)
{
	Uint8 val;
	val=click();
	if(val==1)  Flag_Stop=!Flag_Stop;
}
//###########################################################################
// $ ��������:  Turn_Off(float angle)    $
// $ ��������:  ��Ǳ���               $
//###########################################################################
Uint8 Turn_Off(float angle)
{
    Uint8 temp;
	if(angle<-40||angle>40||1==Flag_Stop)//��Ǵ���40�ȹرյ��
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
for(ii=0;ii<10;ii++);//����һ��FOR���ִ������Ϊ8������8*10*12.5ns=1us
}
//###########################################################################
// $ ��������:  void Init_Infrared_GPIO(void)    $
// $ ��������:  ��ʼ������GPIO����         $
//###########################################################################
void Init_Infrared_GPIO(void)
{
   EALLOW;
   GpioCtrlRegs.GPBPUD.bit.GPIO44 = 0;   // ʹ������
   //GpioCtrlRegs.GPBQSEL1.bit.GPIO44 = 0;   // ͬ��ϵͳʱ��
   GpioCtrlRegs.GPBMUX1.bit.GPIO44=0;   // ����ΪGPIO����
   GpioCtrlRegs.GPBDIR.bit.GPIO44=0;   // ����Ϊ���뷽��

   GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;   // ʹ������
    //GpioCtrlRegs.GPAQSEL1.bit.GPIO7 = 0;   // ͬ��ϵͳʱ��
    GpioCtrlRegs.GPAMUX1.bit.GPIO7=0;   // ����ΪGPIO����
    GpioCtrlRegs.GPADIR.bit.GPIO7=0;   // ����Ϊ���뷽��

    GpioCtrlRegs.GPAPUD.bit.GPIO8 = 0;   // ʹ������
     //GpioCtrlRegs.GPAQSEL1.bit.GPIO8 = 0;   // ͬ��ϵͳʱ��
     GpioCtrlRegs.GPAMUX1.bit.GPIO8=0;   // ����ΪGPIO����
     GpioCtrlRegs.GPADIR.bit.GPIO8=0;   // ����Ϊ���뷽��

     GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;   // ʹ������
      //GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 0;   // ͬ��ϵͳʱ��
      GpioCtrlRegs.GPAMUX2.bit.GPIO16=0;   // ����ΪGPIO����
      GpioCtrlRegs.GPADIR.bit.GPIO16=0;   // ����Ϊ���뷽��

   EDIS;
}

