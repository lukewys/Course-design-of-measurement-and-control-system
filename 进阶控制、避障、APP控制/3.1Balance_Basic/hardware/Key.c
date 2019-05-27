#include "DSP28x_Project.h"     // ͷ�ļ�

#include "Key.h"

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
// $ ��������:  Uint8 click(void)    $
// $ ��������:  ������ֹ��ť���                                $
//###########################################################################
Uint8 click(void)
{
			static Uint8 flag_key=1;//�������ɿ���־
			if(flag_key&&(GpioDataRegs.GPADAT.bit.GPIO4==0))
			{
			flag_key=0;
			return 1;	// ��������
			}
			else if(1==GpioDataRegs.GPADAT.bit.GPIO4)			flag_key=1;
			return 0;//�ް�������
}
