#include "DSP28x_Project.h"     // ͷ�ļ�

#include "EPwm.h"



//###########################################################################
// $ ��������:  void InitPWM1(Uint16 prd)    $
// $ ��������:  ��ʼ��EPwm1ģ��                           $
//###########################################################################
void InitPWM1(Uint16 prd)
{
   InitEPwm1Gpio();   // ��ʼ��EPwm1��������

   EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;  // ���ϼ���
   EPwm1Regs.TBPRD = prd;     // �趨���ڼĴ���ֵ��2000
   EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // ��ֹװ����λ�Ĵ�����ֵ
   EPwm1Regs.TBPHS.half.TBPHS = 0x0000;       // ��λΪ 0
   EPwm1Regs.TBCTR = 0x0000;                  // ʱ��������0
   EPwm1Regs.TBCTL.bit.HSPCLKDIV =1;   // ����ʱ����Ƶ0X05��Ӧ 10��Ƶ
   EPwm1Regs.TBCTL.bit.CLKDIV =1;  //ʱ��ʱ�ӷ�ƵΪ0X02��Ӧ4��Ƶ

   EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;   // Ӱ��װ��ģʽ
   EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;   // Ӱ��װ��ģʽ
   EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;   // װ������ѡ��Ϊ��CTR=ZEROʱ��װ��
   EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   // װ������ѡ��Ϊ��CTR=ZEROʱ��װ��

   EPwm1Regs.CMPA.half.CMPA = 0;    // �趨�ȽϼĴ���A��ֵ
   EPwm1Regs.CMPB =0;     // �趨�ȽϼĴ���B��ֵ

   EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET ;    // ��ʱ������ֵ����0��ʱ��EPwm1A�����
   EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR ;  // ��ʱ������ֵ����CMPA�Ĵ�����ֵʱ��EPwm1A�����

   EPwm1Regs.AQCTLB.bit.ZRO =AQ_SET;    // ��ʱ������ֵ����0��ʱ��EPwm1B�����
   EPwm1Regs.AQCTLB.bit.CBU =AQ_CLEAR;     // ��ʱ������ֵ����CMPA�Ĵ�����ֵʱ��EPwm1B�����

}

//###########################################################################
// $ ��������:  void InitPWM2(Uint16 prd)    $
// $ ��������:  ��ʼ��EPwm2ģ��                           $
//###########################################################################
void InitPWM2(Uint16 prd)
{
	InitEPwm2Gpio();   // ��ʼ��EPwm2��������

   EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;  // ���ϼ���
   EPwm2Regs.TBPRD = prd;       // �趨���ڼĴ���ֵ��2000
   EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;     // ��ֹװ����λ�Ĵ�����ֵ
   EPwm2Regs.TBPHS.half.TBPHS = 0x0000;       // ��λΪ 0
   EPwm2Regs.TBCTR = 0x0000;                   // ʱ��������0
   EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;  // ����ʱ����Ƶ0X05��Ӧ 10��Ƶ
   EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV2;    //ʱ��ʱ�ӷ�ƵΪ0X02��Ӧ4��Ƶ

   EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;   // Ӱ��װ��ģʽ
   EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;   // Ӱ��װ��ģʽ
   EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;   // װ������ѡ��Ϊ��CTR=ZEROʱ��װ��
   EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   // װ������ѡ��Ϊ��CTR=ZEROʱ��װ��

   EPwm2Regs.CMPA.half.CMPA = 0;    // �趨�ȽϼĴ���A��ֵ
   EPwm2Regs.CMPB =0;      // �趨�ȽϼĴ���B��ֵ

   EPwm2Regs.AQCTLA.bit.ZRO = AQ_SET;     // ��ʱ������ֵ����0��ʱ��EPwm1A�����
   EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;    // ��ʱ������ֵ����CMPA�Ĵ�����ֵʱ��EPwm1A�����

   EPwm2Regs.AQCTLB.bit.ZRO = AQ_SET;     // ��ʱ������ֵ����0��ʱ��EPwm1B�����
   EPwm2Regs.AQCTLB.bit.CBU = AQ_CLEAR;     // ��ʱ������ֵ����CMPA�Ĵ�����ֵʱ��EPwm1B�����

}
