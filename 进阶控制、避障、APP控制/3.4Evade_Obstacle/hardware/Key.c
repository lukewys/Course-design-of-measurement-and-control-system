#include "DSP28x_Project.h"     // 头文件

#include "Key.h"

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
// $ 函数名称:  Uint8 click(void)    $
// $ 函数功能:  按键起止按钮检测                                $
//###########################################################################
Uint8 click(void)
{
			static Uint8 flag_key=1;//按键按松开标志
			if(flag_key&&(GpioDataRegs.GPADAT.bit.GPIO4==0))
			{
			flag_key=0;
			return 1;	// 按键按下
			}
			else if(1==GpioDataRegs.GPADAT.bit.GPIO4)			flag_key=1;
			return 0;//无按键按下
}
