#include "DSP28x_Project.h"
#include "QEP.h"


//###########################################################################
// $ 函数名称:  POSSPEED_Init(void)        $
// $ 函数功能:  EQep1和EQep2的模块初始化                                            $
//###########################################################################
void  POSSPEED_Init(void)
{
    EQep1Regs.QUPRD=800000;      // QEP1时间频率 100Hz 系统时钟 80 MHz
    EQep1Regs.QDECCTL.bit.QSRC=00;    // QEP1 计数模式
    EQep1Regs.QEPCTL.bit.FREE_SOFT=2;   //自由运行
    EQep1Regs.QEPCTL.bit.PCRM=00;   // QEP1位置计数器在索引事件复位
    EQep1Regs.QEPCTL.bit.UTE=1;     //使能单位定时器
    EQep1Regs.QEPCTL.bit.QCLM=1;    // 锁存计数单元值
    EQep1Regs.QPOSMAX=0xffffffff;    //初始化最大位置值
    EQep1Regs.QEPCTL.bit.QPEN=1;     // QEP1 模块使能
    EQep1Regs.QCAPCTL.bit.UPPS=5;    // 单位位置32分频
    EQep1Regs.QCAPCTL.bit.CCPS=6;    //CAP捕获128分频
    EQep1Regs.QCAPCTL.bit.CEN=1;     // QEP 捕获使能

    EQep2Regs.QUPRD=800000;     // QEP2时间频率 100Hz 系统时钟 80 MHz
    EQep2Regs.QDECCTL.bit.QSRC=00;   // QEP2 计数模式
    EQep2Regs.QEPCTL.bit.FREE_SOFT=2;    //自由运行
    EQep2Regs.QEPCTL.bit.PCRM=00;   // QEP2位置计数器在索引事件复位
    EQep2Regs.QEPCTL.bit.UTE=1;     //使能单位定时器
    EQep2Regs.QEPCTL.bit.QCLM=1;    // 锁存计数单元值
    EQep2Regs.QPOSMAX=0xffffffff;   //初始化最大位置值
    EQep2Regs.QEPCTL.bit.QPEN=1;     // QEP2 模块使能
    EQep2Regs.QCAPCTL.bit.UPPS=5;    // 单位位置32分频
    EQep2Regs.QCAPCTL.bit.CCPS=6;    //CAP捕获128分频
    EQep2Regs.QCAPCTL.bit.CEN=1;      // QEP 捕获使能

}


//###########################################################################
// $ 函数名称:  Read_Encoder(char TIMX)              $
// $ 函数功能:  EQep1和EQep2的位置寄存器数值读取                                            $
//###########################################################################
int Read_Encoder(char TIMX)
{
    int Encoder;
   switch(TIMX)
	 {
	   case 1:  Encoder= (unsigned int)Encoder1; Encoder1=0;break;   // 读取EQep1的位置寄存器的值，然后清零
	   case 2:  Encoder= (unsigned int)Encoder2; Encoder2=0;break;   // 读取EQep2的位置寄存器的值，然后清零
	 default:  Encoder=0;
	 }
		return Encoder;
}
