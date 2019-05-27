//!
//!$说明：本实验利用TIMER0的中断控制LED灯的亮灭     $
//###########################################################################
//$ file        main.c         $
//$ author      HeDong         $
//$ version     V1.0           $
//$ project     F28069TIMER0 $
//###########################################################################

#include "DSP28x_Project.h"     // 头文件

void InitLEDGPIO(void);   // InitLEDGPIO()函数声明
void LEDOverTurn(void);   // LEDOverTurn()函数声明
__interrupt void TIMER0_ISR(void);   // TIMER0_ISR()函数声明

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
   InitSysCtrl();   //初始化系统时钟，90MHz

   DINT;   // 清除所有中断

   InitPieCtrl();   // 初始化PIE控制寄存器

   IER = 0x0000;   // 禁止CPU中断
   IFR = 0x0000;   // 清除CPU中断标志

   InitPieVectTable();   // 初始化PIE中断表

   //Flash烧写相关语句
   //memcpy(&RamfuncsRunStart,&RamfuncsLoadStart,(Uint32)&RamfuncsLoadSize);
  //InitFlash();

   InitLEDGPIO();   //初始化GPIO5和GPIO6

   EALLOW;
   PieVectTable.TINT0=&TIMER0_ISR;   // TINT0的中断映射
   EDIS;

   InitCpuTimers();   //初始化TIMER0
   ConfigCpuTimer(&CpuTimer0,90,1000000);   // 设置为1s做控制周期

   IER |=M_INT1;   // 允许CPU级中断
   PieCtrlRegs.PIEIER1.bit.INTx7  =1;   // 允许Timer0的PIE中断

   EINT;   // 使能全局中断
   ERTM;   // 使能全局中断DBGM

   for(;;)
   {

   }

}

//###########################################################################
// $ 函数名称:  void InitLEDGPIO(void)     $
// $ 函数功能:  初始化LED的GPIO引脚         $
//###########################################################################
void InitLEDGPIO(void)
{
   EALLOW;
   GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;   // 配置为GPIO功能
   GpioCtrlRegs.GPADIR.bit.GPIO12 = 1;   // 配置为输出方向
   GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 0;   // 配置为GPIO功能
   GpioCtrlRegs.GPADIR.bit.GPIO18 = 1;   // 配置为输出方向
   EDIS;
}

//###########################################################################
// $ 函数名称:  void LEDOverTurn(void)     $
// $ 函数功能:  LED灯翻转           $
//###########################################################################
void LEDOverTurn(void)
{
   GpioDataRegs.GPATOGGLE.bit.GPIO12 = 1;   //GPIO12电平翻转
   GpioDataRegs.GPATOGGLE.bit.GPIO18 = 1;   //GPIO18电平翻转
}

//###########################################################################
// $ 函数名称:  __interrupt void TIMER0_ISR(void)     $
// $ 函数功能:  TIMER0的中断函数控制LED灯的翻转          $
//###########################################################################
__interrupt void TIMER0_ISR(void)
{
	LEDOverTurn();   // LED灯翻转

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   //允许TIMER0的PIE中断
}
