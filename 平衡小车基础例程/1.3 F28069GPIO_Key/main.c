//!
//!$˵������ʵ�����ð���LED�Ƶ�����     $
//###########################################################################
//$ file        main.c          $
//$ author      HeDong          $
//$ version     V1.0            $
//$ project     F28069GPIO_Key  $
//###########################################################################

#include "DSP28x_Project.h"     // ͷ�ļ�

void InitLEDGPIO(void);   // InitLEDGPIO()��������
void LEDOverTurn(void);   // LEDOverTurn()��������
void Init_KEY_GPIO(void);   // Init_KEY_GPIO��������
void click(void);   // click()��������

//Flash��д��ر���
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
extern Uint16 RamfuncsLoadSize;

Uint8 flag_key=1;   // �������ɿ���־

//###########################################################################
// $ ��������:  int main(void)     $
//###########################################################################
int main(void)
{
	   InitSysCtrl();   //��ʼ��ϵͳʱ�ӣ�90MHz

	   DINT;   // ��������ж�

	   InitPieCtrl();   // ��ʼ��PIE���ƼĴ���

	   IER = 0x0000;   // ��ֹCPU�ж�
	   IFR = 0x0000;   // ���CPU�жϱ�־

	   InitPieVectTable();   // ��ʼ��PIE�жϱ�

	  // Flash��д������
	  //memcpy(&RamfuncsRunStart,&RamfuncsLoadStart,(Uint32)&RamfuncsLoadSize);
	  //InitFlash();

	   InitLEDGPIO();   // ��ʼ��LED GPIO5��GPIO6
	   Init_KEY_GPIO();   // ��ʼ������  GPIO4

	   EINT;   // ʹ��ȫ���ж�
	   ERTM;   // ʹ��ȫ���ж�DBGM

	   for(;;)
	   {
		   click();   // ��ȡ����״̬
		   if(flag_key==0)   //��������
		   {
			   LEDOverTurn(); // LED�Ʒ�ת
			   DELAY_US(500000); // ��ʱ0.5s����Ӧϵͳʱ��90MHz
		   }
		   else if(flag_key==1)   //����û�а���
		   {
			   LEDOverTurn(); // LED�Ʒ�ת
			   DELAY_US(2000000); // ��ʱ2s����Ӧϵͳʱ��90MHz
		   }
	   }
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
// $ ��������:  void Init_KEY_GPIO(void)     $
// $ ��������:  ��ʼ������GPIO����         $
//###########################################################################
void Init_KEY_GPIO(void)
{
   EALLOW;
   GpioCtrlRegs.GPAPUD.bit.GPIO4 = 0;   // ʹ������
   GpioCtrlRegs.GPAQSEL1.bit.GPIO4 = 0;   // ͬ��ϵͳʱ��
   GpioCtrlRegs.GPAMUX1.bit.GPIO4=0;   // ����ΪGPIO����
   GpioCtrlRegs.GPADIR.bit.GPIO4=0;   // ����Ϊ���뷽��
   EDIS;
}

//###########################################################################
// $ ��������:  void click(void)     $
// $ ��������:  ��ⰴ��S2�Ƿ񱻰��¹�       $
//###########################################################################
void click(void)
{
	if(GpioDataRegs.GPADAT.bit.GPIO4==0)   //��ȡGPIO4���ݼĴ�����ֵ
	{
		flag_key=0;   // �������±�־
	}
	else if(GpioDataRegs.GPADAT.bit.GPIO4==1)
	{
		flag_key=1;   //�����ɿ���־
	}
}
