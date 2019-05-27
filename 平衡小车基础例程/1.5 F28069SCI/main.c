//!
//!$说明：本实验利用TMS320F28069的SCIB实现串口通信     $
//波特率96008 位数据位，1 位停止位，无奇偶校验的工作方式
//###########################################################################
//$ file        main.c         $
//$ author      HeDong         $
//$ version     V1.0           $
//$ project     F28069SCI      $
//###########################################################################

#include "DSP28x_Project.h"     // 头文件

void scib_echoback_init();   // scib_echoback_init函数声明
void scib_xmit( int a);    // scib_xmit 函数声明
void scib_msg(  char * msg);   // scib_msg函数声明

//Flash烧写相关变量
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
extern Uint16 RamfuncsLoadSize;

//###########################################################################
// $ 函数名称:  int main(void)     $
//###########################################################################
int main(void)
{
   Uint16 ReceiveChar;   // 变量定义
   char *msg;   // 变量定义

   InitSysCtrl();   //初始化系统时钟，90MHz

   DINT;   // 清除所有中断

   InitPieCtrl();   // 初始化PIE控制寄存器

   IER = 0x0000;   // 禁止CPU中断
   IFR = 0x0000;   // 清除CPU中断标志

   InitPieVectTable();   // 初始化PIE中断表

   //Flash烧写相关语句
   memcpy(&RamfuncsRunStart,&RamfuncsLoadStart,(Uint32)&RamfuncsLoadSize);
   InitFlash();

   InitScibGpio();   // SCIB引脚初始化
   scib_echoback_init();   // 初始化scib模块

   EINT;   // 使能全局中断
   ERTM;   // 使能全局中断DBGM

   msg="\r\n欢迎使用合动智能平衡车!  \0";
   scib_msg(msg);
   msg= "\r\n请输入一个字符,DSP将会将该字符回传给串口终端!\n\0";
   scib_msg(msg);
   for(;;)
   {
      if(ScibRegs.SCIRXST.bit.RXRDY==1)
      {
    	  ReceiveChar=ScibRegs.SCIRXBUF.all;    // 将接收到的字符发送给变量
    	  msg=" 您输入的字符是: \0";
    	  scib_msg(msg);   // 发送字符串
    	  scib_xmit(ReceiveChar);   //发送接收的字符
    	  scib_msg("\r\n");
      }
   }
}

//###########################################################################
// $ 函数名称:  void scib_echoback_init()     $
// $ 函数功能:  初始化scib模块        $
//###########################################################################
void scib_echoback_init()
{
    ScibRegs.SCICCR.all =0x0007;   // 1位停止位，无奇偶校验位，8个数据位
    ScibRegs.SCICTL1.all =0x0003;   // 使能TX，RX，关闭睡眠模式，关闭接收纠错
    ScibRegs.SCICTL2.all =0x0003;   // 使能接收中断，禁止发送中断
    ScibRegs.SCICTL2.bit.TXINTENA =1;   // 使能TXRDY中断
    ScibRegs.SCICTL2.bit.RXBKINTENA =1;   // 使能RXRDY/BRKDT中断
    ScibRegs.SCIHBAUD    =0x0001;
    ScibRegs.SCILBAUD    =0x0024;   // 9600 波特率  @LSPCLK = 22.5MHz (90 MHz SYSCLK)
    ScibRegs.SCICTL1.all =0x0023;   //复位SCIB
}

//###########################################################################
// $ 函数名称:  void scib_xmit( int a)     $
// $ 函数功能:  scib发送单个数据        $
//###########################################################################
void scib_xmit( int a)
{
   while (ScibRegs.SCICTL2.bit.TXEMPTY == 0)  // 查询是否发送完毕
   {

   }

   ScibRegs.SCITXBUF=a;   //发送数据a
}

//###########################################################################
// $ 函数名称:  void scib_msg(  char * msg)     $
// $ 函数功能:  scib发送数组数据        $
//###########################################################################
void scib_msg(  char * msg)
{
   int i=0;
   while(msg[i] != '\0')   //判断数组是否结束
   {
    scib_xmit(msg[i]);    //调用前面发送单个字母的函数
    i++;
   }
}




