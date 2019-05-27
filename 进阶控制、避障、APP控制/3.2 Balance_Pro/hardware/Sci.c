#include "DSP28x_Project.h"     // ͷ�ļ�

#include "Sci.h"     // ͷ�ļ�

Uint8 data_mode[8];
Uint8 data_stop[3]={0x59,0x59,0x59};//ֹͣ��������
Uint8 data_start[3]={0x58,0x58,0x58};//������������

//###########################################################################
// $ ��������:  void scib_init()    $
// $ ��������:  ��ʼ��scibģ��                                 $
//###########################################################################
void scia_init()
{
	InitSciaGpio();   // SCIB���ų�ʼ��
    SciaRegs.SCICCR.all =0x0007;   // 1λֹͣλ������żУ��λ��8������λ
    SciaRegs.SCICTL1.all =0x0003;   // ʹ��TX��RX���ر�˯��ģʽ���رս��վ���
    SciaRegs.SCICTL2.all =0x0003;   // ʹ�ܽ����жϣ���ֹ�����ж�
    SciaRegs.SCICTL2.bit.TXINTENA =1;   // ʹ��TXRDY�ж�
    SciaRegs.SCICTL2.bit.RXBKINTENA =1;   // ʹ��RXRDY/BRKDT�ж�
    SciaRegs.SCIHBAUD    =0x0001;
    SciaRegs.SCILBAUD    =0x0003;   // 9600 ������  @LSPCLK = 22.5MHz (90 MHz SYSCLK)����Ϊ80MHZ��Ϊ103
    SciaRegs.SCICTL1.all =0x0023;   //��λSCIA
}


//###########################################################################
// $ ��������:  interrupt void scib_rx_isr(void)  $
// $ ��������:  Scib�жϺ���                                 $
//###########################################################################
interrupt void scia_rx_isr(void)
{
	Uint16 ReceivedChar;
	ReceivedChar = SciaRegs.SCIRXBUF.all;

	if(data_mode[0]==0x58 && data_mode[1]==0x58 && data_mode[2]==0x58)
	{
		Flag_Stop=0;			//�������٣��������
		data_mode[0]=0;data_mode[1]=0;data_mode[2]=0;
	}
	if(data_mode[0]==0x59 && data_mode[1]==0x59 && data_mode[2]==0x59)
	{
		Flag_Stop=1;			//�������٣��رյ��
		data_mode[0]=0;data_mode[1]=0;data_mode[2]=0;
	}
	if(ReceivedChar==0x59)  Flag_sudu=2;//���ٵ�
	if(ReceivedChar==0x58)	Flag_sudu=1;//����

	if(ReceivedChar>10)
	{
		if(ReceivedChar==0x5A)		Flag_qian=0,Flag_hou=0,Flag_left=0,Flag_right=0; //break
		else if(ReceivedChar==0x41)	Flag_qian=1,Flag_hou=0,Flag_left=0,Flag_right=0; //forward
		else if(ReceivedChar==0x45)	Flag_qian=0,Flag_hou=1,Flag_left=0,Flag_right=0; //backward
		else if(ReceivedChar==0x42||ReceivedChar==0x43||ReceivedChar==0x44)
									Flag_qian=0,Flag_hou=0,Flag_left=0,Flag_right=1; //right
		else if(ReceivedChar==0x46||ReceivedChar==0x47||ReceivedChar==0x48)
									Flag_qian=0,Flag_hou=0,Flag_left=1,Flag_right=0;	//left
		else 						Flag_qian=0,Flag_hou=0,Flag_left=0,Flag_right=0; //break
	}

	data_mode[2]=data_mode[1];
	data_mode[1]=data_mode[0];

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}
