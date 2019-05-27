#include "DSP28x_Project.h"     // 头文件

#include "Ecap.h"     // 头文件

//###########################################################################
// $ 函数名称:  void Init_ECap3(void)    $
// $ 函数功能:  初始化ECap3模块           $
//###########################################################################
void Init_ECap3(void)
{
	InitECap3Gpio();

	EALLOW;
	GpioCtrlRegs.GPAMUX2.bit.GPIO27 =0;	  // 配置GPIO27为Trig引脚
	GpioCtrlRegs.GPADIR.bit.GPIO27 =1;     // 输出
	EDIS;

	ECap3Regs.ECEINT.all = 0x0000;    // 关闭所有的CAP中断
	ECap3Regs.ECCLR.all = 0xFFFF;     // 清除所有的CAP中断标志位
	ECap3Regs.ECCTL1.bit.CAPLDEN = 0;     // 禁止CAP1-CAP4寄存器装载
	ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0;   // 停止计数

	ECap3Regs.TSCTR = 0;    // 计数清零
	ECap3Regs.CTRPHS = 0;	// 计数相位寄存器清零

	ECap3Regs.ECCTL2.all = 0x0092;	 // 配置控制寄存器2

	ECap3Regs.ECCTL1.all = 0xC14C;	 // 配置控制寄存器1

	ECap3Regs.ECEINT.all = 0x0004;	// 配置中断使能寄存器


}


//###########################################################################
// $ 函数名称:  __interrupt void Ecap3_isr(void)     $
// $ 函数功能:  计算高电平持续的脉冲数         $
//###########################################################################
__interrupt void Ecap3_isr(void)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4; // 清除CAP3的PIE中断标志
	ECap3Regs.ECCLR.bit.INT = 1;	  // 清除ECap3中断标志位
	ECap3Regs.ECCLR.bit.CEVT2 = 1;	  // 清除事件2标志位
	cap_val=(int32)ECap3Regs.CAP2 - (int32)ECap3Regs.CAP1;   // 计算脉冲
}

//###########################################################################
// $ 函数名称:  void ReadDistance(void)    $
// $ 函数功能:  计算距离                       $
//###########################################################################
void ReadDistance(void)
{
	GpioDataRegs.GPASET.bit.GPIO27=1;   //Trig引脚输出高电平
	DELAY_US(15);   //至少10us，本实验给15us
	GpioDataRegs.GPACLEAR.bit.GPIO27=1;   // Trig引脚输出低
	distan=cap_val*170/80000;   // 计算距离，此处单位为mm
}
