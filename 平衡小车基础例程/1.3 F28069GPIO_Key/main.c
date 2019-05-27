//!
//!$说明：本实验利用按键LED灯的亮灭     $
//###########################################################################
//$ file        main.c          $
//$ author      HeDong          $
//$ version     V1.0            $
//$ project     F28069GPIO_Key  $
//###########################################################################

#include "DSP28x_Project.h"     // 头文件

void InitLEDGPIO(void);   // InitLEDGPIO()函数声明
void LEDOverTurn(void);   // LEDOverTurn()函数声明
void Init_KEY_GPIO(void);   // Init_KEY_GPIO函数声明
void click(void);   // click()函数声明

//Flash烧写相关变量
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
extern Uint16 RamfuncsLoadSize;

Uint8 flag_key=1;   // 按键按松开标志

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

	  // Flash烧写相关语句
	  //memcpy(&RamfuncsRunStart,&RamfuncsLoadStart,(Uint32)&RamfuncsLoadSize);
	  //InitFlash();

	   InitLEDGPIO();   // 初始化LED GPIO5和GPIO6
	   Init_KEY_GPIO();   // 初始化按键  GPIO4

	   EINT;   // 使能全局中断
	   ERTM;   // 使能全局中断DBGM

	   for(;;)
	   {
		   click();   // 获取按键状态
		   if(flag_key==0)   //按键按下
		   {
			   LEDOverTurn(); // LED灯翻转
			   DELAY_US(500000); // 延时0.5s，对应系统时钟90MHz
		   }
		   else if(flag_key==1)   //按键没有按下
		   {
			   LEDOverTurn(); // LED灯翻转
			   DELAY_US(2000000); // 延时2s，对应系统时钟90MHz
		   }
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
   GpioDataRegs.GPATOGGLE.bit.GPIO12 = 1;   //GPIO5电平翻转
   GpioDataRegs.GPATOGGLE.bit.GPIO18 = 1;   //GPIO6电平翻转
}

//###########################################################################
// $ 函数名称:  void Init_KEY_GPIO(void)     $
// $ 函数功能:  初始化按键GPIO引脚         $
//###########################################################################
void Init_KEY_GPIO(void)
{
   EALLOW;
   GpioCtrlRegs.GPAPUD.bit.GPIO4 = 0;   // 使能上拉
   GpioCtrlRegs.GPAQSEL1.bit.GPIO4 = 0;   // 同步系统时钟
   GpioCtrlRegs.GPAMUX1.bit.GPIO4=0;   // 配置为GPIO功能
   GpioCtrlRegs.GPADIR.bit.GPIO4=0;   // 配置为输入方向
   EDIS;
}

//###########################################################################
// $ 函数名称:  void click(void)     $
// $ 函数功能:  检测按键S2是否被按下过       $
//###########################################################################
void click(void)
{
	if(GpioDataRegs.GPADAT.bit.GPIO4==0)   //读取GPIO4数据寄存器的值
	{
		flag_key=0;   // 按键按下标志
	}
	else if(GpioDataRegs.GPADAT.bit.GPIO4==1)
	{
		flag_key=1;   //按键松开标志
	}
}
