//!
//!$˵������ʵ������TMS320F28069��ECap3��EPwm1����ʵ�飬ϵͳʱ������Ϊ80MHz     $
//###########################################################################
//$ file        main.c         $
//$ author      HeDong         $
//$ version     V1.0           $
//$ project     F28069GPIO_LED $
//###########################################################################

#include "DSP28x_Project.h"     // ͷ�ļ�

void InitECapture3();   // InitECapture3()��������
__interrupt void Ecap3_isr(void);   // Ecap3_isr()��������
void InitPWM1(Uint16 prd);   // InitPWM1() ��������

//Flash��д��ر���
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
extern Uint16 RamfuncsLoadSize;

float ZhanKongBi;
Uint16 PWMa=1000;

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

   InitPWM1(2000);	 // ��ʼ��EPwm1,�������ڼĴ���ֵΪ2000

   InitECap3Gpio();   // ECap3���ų�ʼ��
   InitECapture3();   // ECap3ģ���ʼ��

   IER |=M_INT4;   // ����ECap3��cpu�ж�
   PieCtrlRegs.PIEIER4.bit.INTx3 =1;   // ����ECap3��PIE�ж�

   EINT;   // ʹ��ȫ���ж�
   ERTM;   // ʹ��ȫ���ж�DBGM

   for(;;)
   {

	   EPwm1Regs.CMPA.half.CMPA = PWMa;     // �趨�ȽϼĴ���A��ֵ
   }

}

//###########################################################################
// $ ��������:  void InitECapture3()    $
// $ ��������:  ��ʼ��ECap3ģ��           $
//###########################################################################
void InitECapture3()
{
   ECap3Regs.ECEINT.all = 0x0000;          //��ֹ���в����ж�
   ECap3Regs.ECCLR.all = 0xFFFF;           //  ������в����жϱ�־
   ECap3Regs.ECCTL1.bit.CAPLDEN = 0;    // ��ֹCAP1-CAP4�Ĵ���װ��
   ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0;        //������ֹͣ
   ECap3Regs.ECCTL2.bit.CONT_ONESHT = 0;      // ����ģʽ
   ECap3Regs.ECCTL2.bit.STOP_WRAP = 2;      //��CAP3�Ĳ����¼����������������
   ECap3Regs.ECCTL1.bit.CAP1POL = 0;          // �����ز���
   ECap3Regs.ECCTL1.bit.CAP2POL = 1;          // �½��ز���
   ECap3Regs.ECCTL1.bit.CAP3POL = 0;          //�����ز���
   ECap3Regs.ECCTL1.bit.CAP4POL = 1;          //�½��ز���
   ECap3Regs.ECCTL1.bit.CTRRST3 = 1;    //��CAP3��������ü�����ֵ
   ECap3Regs.ECCTL2.bit.SYNCI_EN = 1;         //ͬ������
   ECap3Regs.ECCTL2.bit.SYNCO_SEL = 0;        //ͬ�����
   ECap3Regs.ECCTL1.bit.CAPLDEN = 1;          //ʹ��CAP1-4�Ĵ�������
   ECap3Regs.ECCTL2.bit.TSCTRSTOP = 1;        // ��ʼ����
   ECap3Regs.ECCTL2.bit.REARM = 1;            // �����ؼ���
   ECap3Regs.ECEINT.bit.CEVT3 = 1;            //  �����¼�3�ж�ʹ��
}

//###########################################################################
// $ ��������:  __interrupt void Ecap3_isr(void)     $
// $ ��������:  ���� ����ZhanKongBi��ֵ           $
//###########################################################################
__interrupt void Ecap3_isr(void)
{

   ZhanKongBi=((float)(ECap3Regs.CAP2-ECap3Regs.CAP1))/((float)(ECap3Regs.CAP3-ECap3Regs.CAP1));   //����ռ�ձ�
   ECap3Regs.ECCLR.bit.CEVT3 = 1;   // ����¼�3�жϱ�־
   ECap3Regs.ECCLR.bit.INT = 1;   // ���ECap3�жϱ�־
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;   // ���PIE��4�ж�
}

//###########################################################################
// $ ��������:  void InitPWM1(Uint16 prd)    $
// $ ��������:  ��ʼ��EPwm1ģ��                           $
//###########################################################################
void InitPWM1(Uint16 prd)
{
   InitEPwm1Gpio();   // ��ʼ��EPwm1�Ĺ�������

   EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;   // ���ϼ���
   EPwm1Regs.TBPRD = prd;   // �趨���ڼĴ���ֵ��2000
   EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;   // ��ֹװ����λ�Ĵ�����ֵ
   EPwm1Regs.TBPHS.half.TBPHS = 0x0000;   // ��λΪ 0
   EPwm1Regs.TBCTR = 0x0000;     // ʱ��������0
   EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0x05;   //����ʱ����Ƶ0X05��Ӧ   10��Ƶ
   EPwm1Regs.TBCTL.bit.CLKDIV = 0x02;   // ʱ��ʱ�ӷ�ƵΪ0X02��Ӧ4��Ƶ

   EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;   // Ӱ��װ��ģʽ
   EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;   // Ӱ��װ��ģʽ
   EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;   // װ������ѡ��Ϊ��CTR=ZEROʱ��װ��
   EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   // װ������ѡ��Ϊ��CTR=ZEROʱ��װ��

   EPwm1Regs.CMPA.half.CMPA = 1000;     // �趨�ȽϼĴ���A��ֵ
   EPwm1Regs.CMPB =1000;   // �趨�ȽϼĴ���B��ֵ

   EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;    // ��ʱ������ֵ����0��ʱ��EPwm1A�����
   EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;   // ��ʱ������ֵ����CMPA�Ĵ�����ֵʱ��EPwm1A�����

   EPwm1Regs.AQCTLB.bit.ZRO = AQ_SET;     // ��ʱ������ֵ����0��ʱ��EPwm1B�����
   EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;     // ��ʱ������ֵ����CMPA�Ĵ�����ֵʱ��EPwm1B�����

}
