//!
//!$˵������ʵ������ADC��ADCINA0ͨ�����жԵ�ص�ѹ��ʵʱ���     $
//###########################################################################
//$ file        main.c         $
//$ author      HeDong         $
//$ version     V1.0           $
//$ project     F28069ADC      $
//###########################################################################

#include "DSP28x_Project.h"     // ͷ�ļ�

void Init_Adc_Volt(void);   //Init_Adc_Volt()��������

//Flash��д��ر���
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
extern Uint16 RamfuncsLoadSize;

float Volt;   // ��ص�ѹ����
//###########################################################################
// $ ��������:  int main(void)     $
//###########################################################################
int main(void)
{
   int temp;   // �������

   InitSysCtrl();   // ��ʼ��ϵͳʱ�ӣ�90MHz

   DINT;   // ��������ж�

   InitPieCtrl();   // ��ʼ��PIE���ƼĴ���

   IER = 0x0000;   // ��ֹCPU�ж�
   IFR = 0x0000;   // ���CPU�жϱ�־

   InitPieVectTable();   // ��ʼ��PIE�жϱ�

   //Flash��д������
   //memcpy(&RamfuncsRunStart,&RamfuncsLoadStart,(Uint32)&RamfuncsLoadSize);
   //InitFlash();

   InitAdc();
   AdcOffsetSelfCal();

   Init_Adc_Volt();   //��ʼ��ADC�Ĺ�����ʽ

   EINT;   // ʹ��ȫ���ж�
   ERTM;   // ʹ��ȫ���ж�DBGM

   for(;;)
   {
	   AdcRegs.ADCSOCFRC1.bit.SOC1 = 1;  //SOC1��ʼת��
	   temp = AdcResult.ADCRESULT1;   //��ȡSOC1����Ĵ�����ֵ
	   Volt=(float)(temp*3.3*11/4096); //�����ص�ѹ

   }

}

//###########################################################################
// $ ��������:  void Init_Adc_Volt(void)   $
// $ ��������:  ��ʼ��ADC�Ĺ�����ʽ         $
//###########################################################################
void Init_Adc_Volt(void)
{
   EALLOW;
   AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 0;   //�����ǲ���ת��
   AdcRegs.ADCSOC1CTL.bit.CHSEL  = 0;   // ����SOC1����ͨ��ADCINA0
   AdcRegs.ADCSOC1CTL.bit.ACQPS  = 7;  // ����SOC1��������С
   EDIS;
}



