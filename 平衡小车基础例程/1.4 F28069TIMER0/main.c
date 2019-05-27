//!
//!$˵������ʵ������TIMER0���жϿ���LED�Ƶ�����     $
//###########################################################################
//$ file        main.c         $
//$ author      HeDong         $
//$ version     V1.0           $
//$ project     F28069TIMER0 $
//###########################################################################

#include "DSP28x_Project.h"     // ͷ�ļ�

void InitLEDGPIO(void);   // InitLEDGPIO()��������
void LEDOverTurn(void);   // LEDOverTurn()��������
__interrupt void TIMER0_ISR(void);   // TIMER0_ISR()��������

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

   EALLOW;
   PieVectTable.TINT0=&TIMER0_ISR;   // TINT0���ж�ӳ��
   EDIS;

   InitCpuTimers();   //��ʼ��TIMER0
   ConfigCpuTimer(&CpuTimer0,90,1000000);   // ����Ϊ1s����������

   IER |=M_INT1;   // ����CPU���ж�
   PieCtrlRegs.PIEIER1.bit.INTx7  =1;   // ����Timer0��PIE�ж�

   EINT;   // ʹ��ȫ���ж�
   ERTM;   // ʹ��ȫ���ж�DBGM

   for(;;)
   {

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
   GpioDataRegs.GPATOGGLE.bit.GPIO12 = 1;   //GPIO12��ƽ��ת
   GpioDataRegs.GPATOGGLE.bit.GPIO18 = 1;   //GPIO18��ƽ��ת
}

//###########################################################################
// $ ��������:  __interrupt void TIMER0_ISR(void)     $
// $ ��������:  TIMER0���жϺ�������LED�Ƶķ�ת          $
//###########################################################################
__interrupt void TIMER0_ISR(void)
{
	LEDOverTurn();   // LED�Ʒ�ת

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   //����TIMER0��PIE�ж�
}
