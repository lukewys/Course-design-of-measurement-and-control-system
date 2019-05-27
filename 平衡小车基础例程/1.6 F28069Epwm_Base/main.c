//!
//!$说明：本实验利用TMS320F28069的EPwm1模块产生PWM波，设置系统时钟为80MHz     $
//###########################################################################
//$ file        main.c         $
//$ author      HeDong         $
//$ version     V1.0           $
//$ project     F28069GPIO_LED $
//###########################################################################

#include "DSP28x_Project.h"     // 头文件

void InitPWM1(Uint16 prd);   // InitPWM1() 函数声明

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
   InitSysCtrl();   //初始化系统时钟，80MHz

   DINT;   // 清除所有中断

   InitPieCtrl();   // 初始化PIE控制寄存器

   IER = 0x0000;   // 禁止CPU中断
   IFR = 0x0000;   // 清除CPU中断标志

   InitPieVectTable();   // 初始化PIE中断表

   // Flash烧写相关语句
   // memcpy(&RamfuncsRunStart,&RamfuncsLoadStart,(Uint32)&RamfuncsLoadSize);
   // InitFlash();

   InitPWM1(2000);	 // 初始化EPwm1,设置周期寄存器值为2000

   EINT;   // 使能全局中断
   ERTM;   // 使能全局中断DBGM

   for(;;)
   {

   }


}

//###########################################################################
// $ 函数名称:  void InitPWM1(Uint16 prd)    $
// $ 函数功能:  初始化EPwm1模块                           $
//###########################################################################
void InitPWM1(Uint16 prd)
{
	InitEPwm1Gpio();
   // Setup TBCLK
   EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // 向上计数
   EPwm1Regs.TBPRD = prd;       // 设定周期寄存器值，2000
   EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // 禁止装载相位寄存器的值
   EPwm1Regs.TBPHS.half.TBPHS = 0x0000;       // 相位为 0
   EPwm1Regs.TBCTR = 0x0000;                  // 时基计数清0
   EPwm1Regs.TBCTL.bit.HSPCLKDIV =5;   // 高速时基分频0X05对应 10分频
   EPwm1Regs.TBCTL.bit.CLKDIV =2;  //时基时钟分频为0X02对应4分频

   EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;   // 影子装载模式
   EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;   // 影子装载模式
   EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;   // 装载条件选择为在CTR=ZERO时候装载
   EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   // 装载条件选择为在CTR=ZERO时候装载

   EPwm1Regs.CMPA.half.CMPA = 1000;    // 设定比较寄存器A的值
   EPwm1Regs.CMPB =1000;     // 设定比较寄存器B的值

   EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET ;    // 当时基计数值等于0的时候，EPwm1A输出高
   EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR ;  // 当时基计数值等于CMPA寄存器的值时，EPwm1A输出低

   EPwm1Regs.AQCTLB.bit.ZRO =AQ_CLEAR ;    // 当时基计数值等于0的时候，EPwm1B输出低
   EPwm1Regs.AQCTLB.bit.CBU =AQ_SET ;     // 当时基计数值等于CMPA寄存器的值时，EPwm1B输出高

}
