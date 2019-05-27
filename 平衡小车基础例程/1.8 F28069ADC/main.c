//!
//!$说明：本实验利用ADC的ADCINA0通道进行对电池电压的实时检测     $
//###########################################################################
//$ file        main.c         $
//$ author      HeDong         $
//$ version     V1.0           $
//$ project     F28069ADC      $
//###########################################################################

#include "DSP28x_Project.h"     // 头文件

void Init_Adc_Volt(void);   //Init_Adc_Volt()函数声明

//Flash烧写相关变量
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
extern Uint16 RamfuncsLoadSize;

float Volt;   // 电池电压变量
//###########################################################################
// $ 函数名称:  int main(void)     $
//###########################################################################
int main(void)
{
   int temp;   // 定义变量

   InitSysCtrl();   // 初始化系统时钟，90MHz

   DINT;   // 清除所有中断

   InitPieCtrl();   // 初始化PIE控制寄存器

   IER = 0x0000;   // 禁止CPU中断
   IFR = 0x0000;   // 清除CPU中断标志

   InitPieVectTable();   // 初始化PIE中断表

   //Flash烧写相关语句
   //memcpy(&RamfuncsRunStart,&RamfuncsLoadStart,(Uint32)&RamfuncsLoadSize);
   //InitFlash();

   InitAdc();
   AdcOffsetSelfCal();

   Init_Adc_Volt();   //初始化ADC的工作方式

   EINT;   // 使能全局中断
   ERTM;   // 使能全局中断DBGM

   for(;;)
   {
	   AdcRegs.ADCSOCFRC1.bit.SOC1 = 1;  //SOC1开始转换
	   temp = AdcResult.ADCRESULT1;   //获取SOC1结果寄存器的值
	   Volt=(float)(temp*3.3*11/4096); //计算电池电压

   }

}

//###########################################################################
// $ 函数名称:  void Init_Adc_Volt(void)   $
// $ 函数功能:  初始化ADC的工作方式         $
//###########################################################################
void Init_Adc_Volt(void)
{
   EALLOW;
   AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 0;   //允许覆盖采样转换
   AdcRegs.ADCSOC1CTL.bit.CHSEL  = 0;   // 设置SOC1采样通道ADCINA0
   AdcRegs.ADCSOC1CTL.bit.ACQPS  = 7;  // 设置SOC1采样窗大小
   EDIS;
}



