#include "DSP28x_Project.h"
#include "QEP.h"


//###########################################################################
// $ ��������:  POSSPEED_Init(void)        $
// $ ��������:  EQep1��EQep2��ģ���ʼ��                                            $
//###########################################################################
void  POSSPEED_Init(void)
{
    EQep1Regs.QUPRD=800000;      // QEP1ʱ��Ƶ�� 100Hz ϵͳʱ�� 80 MHz
    EQep1Regs.QDECCTL.bit.QSRC=00;    // QEP1 ����ģʽ
    EQep1Regs.QEPCTL.bit.FREE_SOFT=2;   //��������
    EQep1Regs.QEPCTL.bit.PCRM=00;   // QEP1λ�ü������������¼���λ
    EQep1Regs.QEPCTL.bit.UTE=1;     //ʹ�ܵ�λ��ʱ��
    EQep1Regs.QEPCTL.bit.QCLM=1;    // ���������Ԫֵ
    EQep1Regs.QPOSMAX=0xffffffff;    //��ʼ�����λ��ֵ
    EQep1Regs.QEPCTL.bit.QPEN=1;     // QEP1 ģ��ʹ��
    EQep1Regs.QCAPCTL.bit.UPPS=5;    // ��λλ��32��Ƶ
    EQep1Regs.QCAPCTL.bit.CCPS=6;    //CAP����128��Ƶ
    EQep1Regs.QCAPCTL.bit.CEN=1;     // QEP ����ʹ��

    EQep2Regs.QUPRD=800000;     // QEP2ʱ��Ƶ�� 100Hz ϵͳʱ�� 80 MHz
    EQep2Regs.QDECCTL.bit.QSRC=00;   // QEP2 ����ģʽ
    EQep2Regs.QEPCTL.bit.FREE_SOFT=2;    //��������
    EQep2Regs.QEPCTL.bit.PCRM=00;   // QEP2λ�ü������������¼���λ
    EQep2Regs.QEPCTL.bit.UTE=1;     //ʹ�ܵ�λ��ʱ��
    EQep2Regs.QEPCTL.bit.QCLM=1;    // ���������Ԫֵ
    EQep2Regs.QPOSMAX=0xffffffff;   //��ʼ�����λ��ֵ
    EQep2Regs.QEPCTL.bit.QPEN=1;     // QEP2 ģ��ʹ��
    EQep2Regs.QCAPCTL.bit.UPPS=5;    // ��λλ��32��Ƶ
    EQep2Regs.QCAPCTL.bit.CCPS=6;    //CAP����128��Ƶ
    EQep2Regs.QCAPCTL.bit.CEN=1;      // QEP ����ʹ��

}


//###########################################################################
// $ ��������:  Read_Encoder(char TIMX)              $
// $ ��������:  EQep1��EQep2��λ�üĴ�����ֵ��ȡ                                            $
//###########################################################################
int Read_Encoder(char TIMX)
{
    int Encoder;
   switch(TIMX)
	 {
	   case 1:  Encoder= (unsigned int)Encoder1; Encoder1=0;break;   // ��ȡEQep1��λ�üĴ�����ֵ��Ȼ������
	   case 2:  Encoder= (unsigned int)Encoder2; Encoder2=0;break;   // ��ȡEQep2��λ�üĴ�����ֵ��Ȼ������
	 default:  Encoder=0;
	 }
		return Encoder;
}
