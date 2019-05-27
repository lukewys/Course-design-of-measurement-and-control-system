#include "DSP28x_Project.h"     // 头文件

#include "EPwm.h"



//###########################################################################
// $ 函数名称:  void InitPWM1(Uint16 prd)    $
// $ 函数功能:  初始化EPwm1模块                           $
//###########################################################################
void InitPWM1(Uint16 prd)
{
   InitEPwm1Gpio();   // 初始化EPwm1功能引脚

   EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;  // 向上计数
   EPwm1Regs.TBPRD = prd;     // 设定周期寄存器值，2000
   EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // 禁止装载相位寄存器的值
   EPwm1Regs.TBPHS.half.TBPHS = 0x0000;       // 相位为 0
   EPwm1Regs.TBCTR = 0x0000;                  // 时基计数清0
   EPwm1Regs.TBCTL.bit.HSPCLKDIV =1;   // 高速时基分频0X05对应 10分频
   EPwm1Regs.TBCTL.bit.CLKDIV =1;  //时基时钟分频为0X02对应4分频

   EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;   // 影子装载模式
   EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;   // 影子装载模式
   EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;   // 装载条件选择为在CTR=ZERO时候装载
   EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   // 装载条件选择为在CTR=ZERO时候装载

   EPwm1Regs.CMPA.half.CMPA = 0;    // 设定比较寄存器A的值
   EPwm1Regs.CMPB =0;     // 设定比较寄存器B的值

   EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET ;    // 当时基计数值等于0的时候，EPwm1A输出高
   EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR ;  // 当时基计数值等于CMPA寄存器的值时，EPwm1A输出低

   EPwm1Regs.AQCTLB.bit.ZRO =AQ_SET;    // 当时基计数值等于0的时候，EPwm1B输出低
   EPwm1Regs.AQCTLB.bit.CBU =AQ_CLEAR;     // 当时基计数值等于CMPA寄存器的值时，EPwm1B输出高

}

//###########################################################################
// $ 函数名称:  void InitPWM2(Uint16 prd)    $
// $ 函数功能:  初始化EPwm2模块                           $
//###########################################################################
void InitPWM2(Uint16 prd)
{
	InitEPwm2Gpio();   // 初始化EPwm2功能引脚

   EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;  // 向上计数
   EPwm2Regs.TBPRD = prd;       // 设定周期寄存器值，2000
   EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;     // 禁止装载相位寄存器的值
   EPwm2Regs.TBPHS.half.TBPHS = 0x0000;       // 相位为 0
   EPwm2Regs.TBCTR = 0x0000;                   // 时基计数清0
   EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;  // 高速时基分频0X05对应 10分频
   EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV2;    //时基时钟分频为0X02对应4分频

   EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;   // 影子装载模式
   EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;   // 影子装载模式
   EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;   // 装载条件选择为在CTR=ZERO时候装载
   EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   // 装载条件选择为在CTR=ZERO时候装载

   EPwm2Regs.CMPA.half.CMPA = 0;    // 设定比较寄存器A的值
   EPwm2Regs.CMPB =0;      // 设定比较寄存器B的值

   EPwm2Regs.AQCTLA.bit.ZRO = AQ_SET;     // 当时基计数值等于0的时候，EPwm1A输出高
   EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;    // 当时基计数值等于CMPA寄存器的值时，EPwm1A输出低

   EPwm2Regs.AQCTLB.bit.ZRO = AQ_SET;     // 当时基计数值等于0的时候，EPwm1B输出低
   EPwm2Regs.AQCTLB.bit.CBU = AQ_CLEAR;     // 当时基计数值等于CMPA寄存器的值时，EPwm1B输出高

}
