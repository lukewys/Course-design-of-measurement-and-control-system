//!
//!$˵������ʵ������TMS320F28069��ECap3�ͳ�����ģ��HC-SR04����ʵ�飬ϵͳʱ������Ϊ80MHz     $
//###########################################################################
//$ file        main.c         $
//$ author      HeDong         $
//$ version     V1.0           $
//$ project     F28069ECap     $
//###########################################################################

#include "DSP28x_Project.h"     // ͷ�ļ�

void Init_ECap3(void);   // Init_ECap3()��������
__interrupt void Ecap3_isr(void);   // Ecap3_isr()��������
void ReadDistance(void);   // ReadDistance() ��������

//Flash��д��ر���
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
extern Uint16 RamfuncsLoadSize;

Uint32 cap_val=0,Distan=0;

//###########################################################################
// $ ��������:  int main(void)     $
//###########################################################################
int main(void)
{
   InitSysCtrl();   //��ʼ��ϵͳʱ�ӣ�80MHz

   DINT;   // ��������ж�

   InitPieCtrl();   // ��ʼ��PIE���ƼĴ���

   IER = 0x0000;   // ��ֹCPU�ж�
   IFR = 0x0000;   // ���CPU�жϱ�־

   InitPieVectTable();   // ��ʼ��PIE�жϱ�

   //Flash��д������
   //memcpy(&RamfuncsRunStart,&RamfuncsLoadStart,(Uint32)&RamfuncsLoadSize);
  //InitFlash();

   EALLOW;
   PieVectTable.ECAP3_INT=&Ecap3_isr;   // ECAP3_INT���ж�ӳ��
   EDIS;

   Init_ECap3();   // ECap3ģ���ʼ��

   IER |=M_INT4;   // ����ECap3��cpu�ж�
   PieCtrlRegs.PIEIER4.bit.INTx3 =1;   // ����ECap3��PIE�ж�

   EINT;   // ʹ��ȫ���ж�
   ERTM;   // ʹ��ȫ���ж�DBGM

   for(;;)
   {
	   ReadDistance();   // �������

	   DELAY_US(2000);   // ��ʱ2ms

   }

}

//###########################################################################
// $ ��������:  void Init_ECap3(void)    $
// $ ��������:  ��ʼ��ECap3ģ��           $
//###########################################################################
void Init_ECap3(void)
{
	InitECap3Gpio();

	EALLOW;
	GpioCtrlRegs.GPAMUX2.bit.GPIO23 =0;	  // ����GPIO23ΪTrig����
	GpioCtrlRegs.GPADIR.bit.GPIO23 =1;     // ���
	EDIS;

	ECap3Regs.ECEINT.all = 0x0000;    // �ر����е�CAP�ж�
	ECap3Regs.ECCLR.all = 0xFFFF;     // ������е�CAP�жϱ�־λ
	ECap3Regs.ECCTL1.bit.CAPLDEN = 0;     // ��ֹCAP1-CAP4�Ĵ���װ��
	ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0;   // ֹͣ����

	ECap3Regs.TSCTR = 0;    // ��������
	ECap3Regs.CTRPHS = 0;	// ������λ�Ĵ�������

	ECap3Regs.ECCTL2.all = 0x0092;	 // ���ÿ��ƼĴ���2

	ECap3Regs.ECCTL1.all = 0xC14C;	 // ���ÿ��ƼĴ���1

	ECap3Regs.ECEINT.all = 0x0004;	// �����ж�ʹ�ܼĴ���


}


//###########################################################################
// $ ��������:  __interrupt void Ecap3_isr(void)     $
// $ ��������:  ����ߵ�ƽ������������         $
//###########################################################################
__interrupt void Ecap3_isr(void)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4; // ���CAP3��PIE�жϱ�־
	ECap3Regs.ECCLR.bit.INT = 1;	  // ���ECap3�жϱ�־λ
	ECap3Regs.ECCLR.bit.CEVT2 = 1;	  // ����¼�2��־λ
	cap_val=(int32)ECap3Regs.CAP2 - (int32)ECap3Regs.CAP1;   // ��������
}

//###########################################################################
// $ ��������:  void ReadDistance(void)    $
// $ ��������:  �������                       $
//###########################################################################
void ReadDistance(void)
{
	GpioDataRegs.GPASET.bit.GPIO23=1;   //Trig��������ߵ�ƽ
	DELAY_US(15);   //����10us����ʵ���15us
	GpioDataRegs.GPACLEAR.bit.GPIO23=1;   // Trig���������
	Distan=cap_val*170/80000;   // ������룬�˴���λΪmm
}
