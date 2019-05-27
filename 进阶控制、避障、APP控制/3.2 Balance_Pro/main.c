//!
//!$˵������ʵ����CC2541����ģ��ʵ���ֻ�APP����ƽ�⳵��ǰ�������˼�ת��                  $
//###########################################################################
//$ file        main.c                  $
//$ author      HeDong                  $
//$ version     V1.0                    $
//$ project     F28069APPControl        $
//###########################################################################

#include "DSP28x_Project.h"     // ͷ�ļ�

__interrupt void TIMER0_ISR(void);   // TIMER0_ISR()��������

float Angle_Balance=0,Gyro_Balance=0,Gyro_Turn=0;  //ƽ����ǡ�ƽ��������
int Balance_Pwm=0,Velocity_Pwm=0,Turn_Pwm=0;
int Moto1=0,Moto2=0;
int Encoder_Left=0,Encoder_Right=0;   // �����ֱ�����
Uint32 distan=0;   // �����������ر���
Uint32 cap_val=0;
Uint8 Bi_zhang=0;
Uint8 Flag_Stop=1;
Uint8 Flag_qian=0,Flag_hou=0,Flag_left=0,Flag_right=0,Flag_sudu=2,Turn_rate=50;//����ң����ر���


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

   InitLEDGPIO();   // LED GPIO��ʼ��
   Init_KEY_GPIO();   // Key GPIO��ʼ��
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
	   LEDOverTurn();   // LED����˸

		DELAY_US(500000);   // ��ʱ

   }
*/
}

//###########################################################################
// $ ��������:  TIMER0_ISR(void)            $
// $ ��������:  Timer0���жϷ�����                                            $
//###########################################################################
__interrupt void TIMER0_ISR(void)
{
	Encoder_Left=Read_Encoder(1);   // ��ȡ���ֱ���������
	Encoder_Right=Read_Encoder(2);   // ��ȡ���ֱ���������
	ReadDistance();   // �������
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
// $ ��������:  balance(float Angle,float Gyro)     $
// $ ��������:  ֱ��ƽ��PID���ƺ���                                                                               $
//###########################################################################
int balance(float Angle,float Gyro)
{
     float Bias,kp=350,kd=0.88;//72 0.24
	 int balance;
	 Bias=Angle-ZHONGZHI;       //===���ƽ��ĽǶ���ֵ �ͻ�е���
	 balance=(int)(kp*Bias+Gyro*kd);   //===���ƽ����Ƶĵ��PWM PD����
	 return balance;
}


//###########################################################################
// $ ��������:  velocity(int encoder_left,int encoder_right)     $
// $ ��������:  �ٶȻ�PID���ƺ���                                                                                             $
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
	Encoder_Least =(encoder_left+encoder_right)-0;                    //��ȡ�����ٶ�ƫ��=�����ٶȣ����ұ�����֮�ͣ�-Ŀ���ٶȣ��˴�Ϊ0��
	Encoder *= 0.8;		                                              //һ�׵�ͨ�˲���
	Encoder += Encoder_Least*0.2;	                                  //һ�׵�ͨ�˲���
	Encoder_Integral +=Encoder;                                       //���ֳ�λ�ƣ�����ʱ�䣺10ms
	Encoder_Integral=Encoder_Integral-Movement;                       //����ң�������ݣ�����ǰ������
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
	//===============ң��������ת����========================//
	if(1==Flag_left||1==Flag_right)				//������תǰ���ٶȵ����ٶȵ���ʼ�ٶ�
	{
		if(++Turn_Count==1)
		Encoder_Temp=myabs(encoder_left+encoder_right);
		Turn_Convert=Turn_rate/Encoder_Temp;
		if(Turn_Convert<0.6) Turn_Convert=0.6;
		if(Turn_Convert>5)	Turn_Convert=4; //���ˣ�ԭ����3
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

	if(Turn_Target>Turn_Amplitude)	Turn_Target=Turn_Amplitude;//ת���ٶ��޷�
	if(Turn_Target<-Turn_Amplitude)	Turn_Target=-Turn_Amplitude;
	if(Flag_qian==1||Flag_hou==1)  Kd=0.5;
	else Kd=0;		//ת���ʱ��ȡ�������ǵľ���
	//==================ת��PD������==========================//
	Turn=-Turn_Target*Kp-gyro*Kd;
	return Turn;
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
// $ ��������:  void InitLEDGPIO(void)     $
// $ ��������:  ��ʼ��LED��GPIO����         $
//###########################################################################
void InitLEDGPIO(void)
{
   EALLOW;
   GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;   // ����ΪGPIO����
   GpioCtrlRegs.GPADIR.bit.GPIO12 = 1;   // ����Ϊ�������
   GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 0;   // ����ΪGPIO����
   GpioCtrlRegs.GPADIR.bit.GPIO18 = 1;   // ����Ϊ�������
   EDIS;
}


//###########################################################################
// $ ��������:  void LEDOverTurn(void)     $
// $ ��������:  LED�Ʒ�ת           $
//###########################################################################
void LEDOverTurn(void)
{
   GpioDataRegs.GPATOGGLE.bit.GPIO12 = 1;   //GPIO5��ƽ��ת
   GpioDataRegs.GPATOGGLE.bit.GPIO18 = 1;   //GPIO6��ƽ��ת
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
