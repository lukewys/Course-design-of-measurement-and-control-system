
//###########################################################################
//
// FILE:   DSP28x_Project.h
//
// TITLE:  DSP28x Project Headerfile and Examples Include File
//
//###########################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#ifndef DSP28x_PROJECT_H
#define DSP28x_PROJECT_H

#include "F2806x_Cla_typedefs.h"// F2806x CLA Type definitions
#include "F2806x_Device.h"      // F2806x Headerfile Include File
#include "F2806x_Examples.h"   	// F2806x Examples Include File
#include <math.h>

#include "EPwm.h"    // EPwm相关头文件
#include "IIC.h"    // IIC相关头文件
#include "MPU6050.h"   // MPU6050相关头文件
#include "QEP.h"   // QEP相关头文件
#include "Ecap.h"   // QEP相关头文件
#include "Key.h"   //按键相关头文件
#include "Sci.h"   // 串口相关头文件


//$  函数声明
int balance(float Angle,float Gyro);
int velocity(int encoder_left,int encoder_right);
int turn(int encoder_left,int encoder_right,float gyro);
void Set_Pwm(int moto1,int moto2);
void Xianfu_Pwm(void);
int myabs(int a);
void InitLEDGPIO(void);
void LEDOverTurn(void);
Uint8 Turn_Off(float angle);
void Key(void);

#define Encoder1	EQep1Regs.QPOSCNT
#define Encoder2	EQep2Regs.QPOSCNT

#define PWM1A  	EPwm1Regs.CMPA.half.CMPA
#define PWM1B  	EPwm1Regs.CMPB
#define PWM2A  	EPwm2Regs.CMPA.half.CMPA
#define PWM2B  	EPwm2Regs.CMPB

#define R2 GpioDataRegs.GPADAT.bit.GPIO7
#define R1 GpioDataRegs.GPBDAT.bit.GPIO44
#define L1 GpioDataRegs.GPADAT.bit.GPIO8
#define L2 GpioDataRegs.GPADAT.bit.GPIO16

#define PI 3.14159265
#define ZHONGZHI 0


extern float Angle_Balance,Gyro_Balance,Gyro_Turn;
extern Uint32 distan;
extern Uint32 cap_val;
extern Uint8 Flag_Stop;
extern Uint8 Flag_qian,Flag_hou,Flag_left,Flag_right,Flag_sudu;//蓝牙遥控相关变量
#endif  // end of DSP28x_PROJECT_H definition

