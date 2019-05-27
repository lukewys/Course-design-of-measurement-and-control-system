//!
//!$说明：本实验利用TMS320F28069的ECap3和EPwm1进行实验，系统时钟设置为80MHz     $
//###########################################################################
//$ file        main.c         $
//$ author      HeDong         $
//$ version     V1.0           $
//$ project     F28069GPIO_LED $
//###########################################################################

#include "DSP28x_Project.h"     // 头文件

void InitECapture3();   // InitECapture3()函数声明
__interrupt void Ecap3_isr(void);   // Ecap3_isr()函数声明
void InitPWM1(Uint16 prd);   // InitPWM1() 函数声明

//Flash烧写相关变量
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
extern Uint16 RamfuncsLoadSize;

float ZhanKongBi;
Uint16 PWMa=1000;

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

   //Flash烧写相关语句
   //memcpy(&RamfuncsRunStart,&RamfuncsLoadStart,(Uint32)&RamfuncsLoadSize);
  //InitFlash();

   EALLOW;
   PieVectTable.ECAP3_INT=&Ecap3_isr;   // ECAP3_INT的中断映射
   EDIS;

   InitPWM1(2000);	 // 初始化EPwm1,设置周期寄存器值为2000

   InitECap3Gpio();   // ECap3引脚初始化
   InitECapture3();   // ECap3模块初始化

   IER |=M_INT4;   // 允许ECap3的cpu中断
   PieCtrlRegs.PIEIER4.bit.INTx3 =1;   // 允许ECap3的PIE中断

   EINT;   // 使能全局中断
   ERTM;   // 使能全局中断DBGM

   for(;;)
   {

	   EPwm1Regs.CMPA.half.CMPA = PWMa;     // 设定比较寄存器A的值
   }

}

//###########################################################################
// $ 函数名称:  void InitECapture3()    $
// $ 函数功能:  初始化ECap3模块           $
//###########################################################################
void InitECapture3()
{
   ECap3Regs.ECEINT.all = 0x0000;          //禁止所有捕获中断
   ECap3Regs.ECCLR.all = 0xFFFF;           //  清除所有捕获中断标志
   ECap3Regs.ECCTL1.bit.CAPLDEN = 0;    // 禁止CAP1-CAP4寄存器装载
   ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0;        //计数器停止
   ECap3Regs.ECCTL2.bit.CONT_ONESHT = 0;      // 连续模式
   ECap3Regs.ECCTL2.bit.STOP_WRAP = 2;      //在CAP3的捕获事件后计数器正常运行
   ECap3Regs.ECCTL1.bit.CAP1POL = 0;          // 上升沿捕获
   ECap3Regs.ECCTL1.bit.CAP2POL = 1;          // 下降沿捕获
   ECap3Regs.ECCTL1.bit.CAP3POL = 0;          //上升沿捕获
   ECap3Regs.ECCTL1.bit.CAP4POL = 1;          //下降沿捕获
   ECap3Regs.ECCTL1.bit.CTRRST3 = 1;    //在CAP3捕获后重置计数器值
   ECap3Regs.ECCTL2.bit.SYNCI_EN = 1;         //同步输入
   ECap3Regs.ECCTL2.bit.SYNCO_SEL = 0;        //同步输出
   ECap3Regs.ECCTL1.bit.CAPLDEN = 1;          //使能CAP1-4寄存器加载
   ECap3Regs.ECCTL2.bit.TSCTRSTOP = 1;        // 开始计数
   ECap3Regs.ECCTL2.bit.REARM = 1;            // 单次重加载
   ECap3Regs.ECEINT.bit.CEVT3 = 1;            //  捕获事件3中断使能
}

//###########################################################################
// $ 函数名称:  __interrupt void Ecap3_isr(void)     $
// $ 函数功能:  计算 变量ZhanKongBi的值           $
//###########################################################################
__interrupt void Ecap3_isr(void)
{

   ZhanKongBi=((float)(ECap3Regs.CAP2-ECap3Regs.CAP1))/((float)(ECap3Regs.CAP3-ECap3Regs.CAP1));   //计算占空比
   ECap3Regs.ECCLR.bit.CEVT3 = 1;   // 清除事件3中断标志
   ECap3Regs.ECCLR.bit.INT = 1;   // 清除ECap3中断标志
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;   // 清除PIE组4中断
}

//###########################################################################
// $ 函数名称:  void InitPWM1(Uint16 prd)    $
// $ 函数功能:  初始化EPwm1模块                           $
//###########################################################################
void InitPWM1(Uint16 prd)
{
   InitEPwm1Gpio();   // 初始化EPwm1的功能引脚

   EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;   // 向上计数
   EPwm1Regs.TBPRD = prd;   // 设定周期寄存器值，2000
   EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;   // 禁止装载相位寄存器的值
   EPwm1Regs.TBPHS.half.TBPHS = 0x0000;   // 相位为 0
   EPwm1Regs.TBCTR = 0x0000;     // 时基计数清0
   EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0x05;   //高速时基分频0X05对应   10分频
   EPwm1Regs.TBCTL.bit.CLKDIV = 0x02;   // 时基时钟分频为0X02对应4分频

   EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;   // 影子装载模式
   EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;   // 影子装载模式
   EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;   // 装载条件选择为在CTR=ZERO时候装载
   EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   // 装载条件选择为在CTR=ZERO时候装载

   EPwm1Regs.CMPA.half.CMPA = 1000;     // 设定比较寄存器A的值
   EPwm1Regs.CMPB =1000;   // 设定比较寄存器B的值

   EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;    // 当时基计数值等于0的时候，EPwm1A输出高
   EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;   // 当时基计数值等于CMPA寄存器的值时，EPwm1A输出低

   EPwm1Regs.AQCTLB.bit.ZRO = AQ_SET;     // 当时基计数值等于0的时候，EPwm1B输出低
   EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;     // 当时基计数值等于CMPA寄存器的值时，EPwm1B输出高

}
