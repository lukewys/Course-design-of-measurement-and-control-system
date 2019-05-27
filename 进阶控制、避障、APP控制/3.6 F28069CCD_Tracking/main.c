//!
//!$˵������ʵ��������CCDѭ��ģ��ʵ��ƽ�⳵��ѭ������                        $
//###########################################################################
//$ file        main.c               $
//$ author      HeDong               $
//$ version     V1.0                 $
//$ project     F28069CCDXunJi        $
//###########################################################################

#include "DSP28x_Project.h"     // ͷ�ļ�

__interrupt void TIMER0_ISR(void);   // TIMER0_ISR()��������


Uint16 *ttx,*dyw;
float Angle_Balance=0,Gyro_Balance=0,Gyro_Turn=0;  //ƽ����ǡ�ƽ��������
int Balance_Pwm=0,Velocity_Pwm=0,Turn_Pwm=0;
int Moto1=0,Moto2=0;
int Encoder_Left=0,Encoder_Right=0;   // �����ֱ�����
//$  CCD�����ر���
Uint16 ADV[128]={0};   // CCD���ش洢
Uint16 CCD_Z=0;
Uint16 CCD_Yuzhi=0;   // ��ֵ��ֵ
Uint16 ZZ=0;   // ���������ֵ
Uint8 Flag_Stop=1;

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
   InitPWM1(2000);   // EPwm1ģ���ʼ��
   InitPWM2(2000);   // EPwm2ģ���ʼ��
   InitEQep1Gpio();   // QEP1�������ų�ʼ��
   InitEQep2Gpio();   // QEP2�������ų�ʼ��
   POSSPEED_Init();   // QEP1��QEP2ģ���ʼ��

   EALLOW;
   PieVectTable.TINT0=&TIMER0_ISR;   // TINT0���ж�ӳ��
   EDIS;

   InitCpuTimers();   //��ʼ��TIMER0
   ConfigCpuTimer(&CpuTimer0,80,5000);   // ����Ϊ5ms����������

   Init_I2CA();   // ��ʼ��IICģ��

   MPU6050_Init();   // MPU6050��ʼ��
   DELAY_US(10000);   // ��ʱ

   InitCCDGPIO();   // GPIO8��GPIO9�ĳ�ʼ��

   Init_Adc_CCD();   // ADCģ���ʼ��

   IER |=M_INT1;   // ����CPU���ж�
   PieCtrlRegs.PIEIER1.bit.INTx7  =1;   // ����Timer0��PIE�ж�

   EINT;   // ʹ��ȫ���ж�
   ERTM;   // ʹ��ȫ���ж�DBGM

   for(;;)
   {
	  // LEDOverTurn();   // LED����˸

	 //	DELAY_US(500000);   // ��ʱ

   }

}

//###########################################################################
// $ ��������:  TIMER0_ISR(void)            $
// $ ��������:  Timer0���жϷ�����                                            $
//###########################################################################
__interrupt void TIMER0_ISR(void)
{
	static Uint8 flag_10ms=0;   // ��������
   // static Uint8 flag_20ms=0;
	flag_10ms=!flag_10ms;   // ȡ��
	 if(flag_10ms==1)  // 10ms��ȡһ��CCD��ֵ
	 {
		 //flag_20ms=!flag_20ms;
		 Read_CCD();   // ��ȡCCD����
	 }
	 //if(flag_20ms==1)  // 10ms��ȡһ��CCD��ֵ
	 //	 {
	 		 //flag_20ms=1;
	 //		 Read_CCD();   // ��ȡCCD����
	 //	 }
	Find_CCD_Zhongzhi();   // �����м�����ֵ
	Encoder_Left=Read_Encoder(1);   // ��ȡ���ֱ���������
	Encoder_Right=Read_Encoder(2);   // ��ȡ���ֱ���������
	Key();   // ������ͣ�ź�
	MPU6050_Read();   // ��ȡMPU6050����
	Get_Angle();   // ������
	Balance_Pwm =balance(Angle_Balance,Gyro_Balance);   // ����ƽ�������

	Velocity_Pwm=velocity(Encoder_Left,Encoder_Right);    // �����ٶȻ�������
	Turn_Pwm=turn_CCD(ZZ,Gyro_Turn);      // ����ת�򻷿�����

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
     float Bias,kp=300,kd=0.84;//500��1.04��300,0.84
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
    static float Velocity=0,Encoder_Least=0,Encoder=0,Movement=-6;
	static float Encoder_Integral=0;
	float kp=96,ki=0.83;

	Encoder_Least =(encoder_left+encoder_right)-0;  //��ȡ�����ٶ�ƫ��=�����ٶȣ����ұ�����֮�ͣ�-Ŀ���ٶȣ��˴�Ϊ0��
	Encoder *= 0.8;		            //һ�׵�ͨ�˲���
	Encoder += Encoder_Least*0.2;	//һ�׵�ͨ�˲���
	Encoder_Integral +=Encoder;     //���ֳ�λ��
	Encoder_Integral=Encoder_Integral-Movement;        //����ң�������ݣ�����ǰ������
	if(Encoder_Integral>5000)  	Encoder_Integral=5000; //�����޷�
	if(Encoder_Integral<-5000)	Encoder_Integral=-5000;       //
	Velocity=(int)(Encoder*kp+Encoder_Integral*ki);                          //�ٶȿ���
	if(Turn_Off(Angle_Balance)==1)   Encoder_Integral=0;    //����رպ��������
	return Velocity;
}


//###########################################################################
// $ ��������:  int turn_CCD(Uint16 CCD,float gyro)       $
// $ ��������:  ת�򻷿�����                                                                                                                           $
//###########################################################################
int turn_CCD(Uint16 CCD,float gyro)
{
	static float Turn=0;   //��������
	static int Bias,KP=17,KD=0.1;    //CCDת�򻷿��Ƶ�PD����

	if(CCD_Yuzhi>310)
		Turn=1800;
	else{
	Bias=CCD-64;   //����������λ��ƫ��

	Turn=-Bias*KP-gyro*KD;    //����ת�������
}
	return Turn;    //����ת�������
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
   GpioDataRegs.GPATOGGLE.bit.GPIO12 = 1;   //GPIO12��ƽ��ת
   GpioDataRegs.GPATOGGLE.bit.GPIO18 = 1;   //GPIO18��ƽ��ת
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
