#include "DSP28x_Project.h"     // 头文件

#include "Sci.h"     // 头文件

Uint8 data_mode[8];
Uint8 data_stop[3]={0x59,0x59,0x59};//停止数据样本
Uint8 data_start[3]={0x58,0x58,0x58};//启动数据样本

//###########################################################################
// $ 函数名称:  void scib_init()    $
// $ 函数功能:  初始化scib模块                                 $
//###########################################################################
void scia_init()
{
	InitSciaGpio();   // SCIB引脚初始化
    SciaRegs.SCICCR.all =0x0007;   // 1位停止位，无奇偶校验位，8个数据位
    SciaRegs.SCICTL1.all =0x0003;   // 使能TX，RX，关闭睡眠模式，关闭接收纠错
    SciaRegs.SCICTL2.all =0x0003;   // 使能接收中断，禁止发送中断
    SciaRegs.SCICTL2.bit.TXINTENA =1;   // 使能TXRDY中断
    SciaRegs.SCICTL2.bit.RXBKINTENA =1;   // 使能RXRDY/BRKDT中断
    SciaRegs.SCIHBAUD    =0x0001;
    SciaRegs.SCILBAUD    =0x0003;   // 9600 波特率  @LSPCLK = 22.5MHz (90 MHz SYSCLK)，当为80MHZ是为103
    SciaRegs.SCICTL1.all =0x0023;   //复位SCIA
}


//###########################################################################
// $ 函数名称:  interrupt void scib_rx_isr(void)  $
// $ 函数功能:  Scib中断函数                                 $
//###########################################################################
interrupt void scia_rx_isr(void)
{
	Uint16 ReceivedChar;
	ReceivedChar = SciaRegs.SCIRXBUF.all;

	if(data_mode[0]==0x58 && data_mode[1]==0x58 && data_mode[2]==0x58)
	{
		Flag_Stop=0;			//三击高速，启动电机
		data_mode[0]=0;data_mode[1]=0;data_mode[2]=0;
	}
	if(data_mode[0]==0x59 && data_mode[1]==0x59 && data_mode[2]==0x59)
	{
		Flag_Stop=1;			//三击低速，关闭电机
		data_mode[0]=0;data_mode[1]=0;data_mode[2]=0;
	}
	if(ReceivedChar==0x59)  Flag_sudu=2;//低速档
	if(ReceivedChar==0x58)	Flag_sudu=1;//高速

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
