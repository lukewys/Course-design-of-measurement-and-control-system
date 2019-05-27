#include "DSP28x_Project.h"     // ͷ�ļ�

#include "Ecap.h"     // ͷ�ļ�

//###########################################################################
// $ ��������:  void Init_ECap3(void)    $
// $ ��������:  ��ʼ��ECap3ģ��           $
//###########################################################################
void Init_ECap3(void)
{
	InitECap3Gpio();

	EALLOW;
	GpioCtrlRegs.GPAMUX2.bit.GPIO27 =0;	  // ����GPIO27ΪTrig����
	GpioCtrlRegs.GPADIR.bit.GPIO27 =1;     // ���
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
	GpioDataRegs.GPASET.bit.GPIO27=1;   //Trig��������ߵ�ƽ
	DELAY_US(15);   //����10us����ʵ���15us
	GpioDataRegs.GPACLEAR.bit.GPIO27=1;   // Trig���������
	distan=cap_val*170/80000;   // ������룬�˴���λΪmm
}
