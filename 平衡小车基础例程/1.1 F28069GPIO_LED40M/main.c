//!
//!$˵������ʵ��ͨ������TMS320F28069��ϵͳʱ��Ϊ40MHz����LED�Ƶ�����     $
//###########################################################################
//$ file        main.c             $
//$ author      HeDong             $
//$ version     V1.0               $
//$ project     F28069GPIO_LED40M  $
//###########################################################################

#include "DSP28x_Project.h"     // ͷ�ļ�

void InitLEDGPIO(void);   //InitLEDGPIO()��������
void LEDOverTurn(void);   //LEDOverTurn()��������

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
   InitSysCtrl();   //��ʼ��ϵͳʱ�ӣ�90MHz

   DINT;   // ��������ж�

   InitPieCtrl();   // ��ʼ��PIE���ƼĴ���

   IER = 0x0000;   // ��ֹCPU�ж�
   IFR = 0x0000;   // ���CPU�жϱ�־

   InitPieVectTable();   // ��ʼ��PIE�жϱ�

   //Flash��д������
   //memcpy(&RamfuncsRunStart,&RamfuncsLoadStart,(Uint32)&RamfuncsLoadSize);
   //InitFlash();

   InitLEDGPIO();   //��ʼ��GPIO5��GPIO6


   EINT;   // ʹ��ȫ���ж�
   ERTM;   // ʹ��ȫ���ж�DBGM

   for(;;)
   {
	   LEDOverTurn(); // LED�Ʒ�ת
	   DELAY_US(2000000); //��ʱ(�˴�����2s�����2s��)
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

