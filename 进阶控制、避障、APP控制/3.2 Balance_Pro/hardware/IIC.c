#include "DSP28x_Project.h"
#include "IIC.h"


//###########################################################################
// $ 函数名称:    void Init_I2CA(void)                            $
// $ 函数功能:    IIC模块初始化，设置IIC模块的功能引脚以及设置IIC的工作方式                             $
//###########################################################################
void Init_I2CA(void)
{
	I2caRegs.I2CMDR.all = 0x0000;	// 复位IIC

	EALLOW;
	GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;    // 使能(SDAA)上拉
	GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;	   //  使能 (SCLA)上拉
	GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;  // 同步 (SDAA)
	GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;  // 同步 (SCLA)
	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;   // 配置 GPIO32为 SDAA
	GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;   // 配置GPIO33 为SCLA
	EDIS;

	// 预分频——时钟模块的频率
	I2caRegs.I2CPSC.all = 7;	   // 预分频	IIC模块时钟需设置为7-12MHz，本实验设置为 (80/8 = 10MHz)
	I2caRegs.I2CCLKL = 10;   //时钟低电平时间值
	I2caRegs.I2CCLKH = 5;	//时钟高电平时间值

	I2caRegs.I2CMDR.all = 0x0020;	// IIC准备就绪

}


//###########################################################################
// $ 函数名称:    Uint16	I2C_xrdy()                        $
// $ 函数功能:    IIC模块发送准备OK                              $
//###########################################################################
Uint16	I2C_xrdy()
{
	Uint16	t;
	t = I2caRegs.I2CSTR.bit.XRDY;   // IIC模块发送准备OK
	return t;
}


//###########################################################################
// $ 函数名称:    Uint16	I2C_rrdy()                      $
// $ 函数功能:    IIC模块接收准备OK                              $
//###########################################################################
Uint16	I2C_rrdy()
{
	Uint16	t;
	t = I2caRegs.I2CSTR.bit.RRDY;   //  IIC模块接收准备OK
	return t;
}


//###########################################################################
// $ 函数名称:    Uint16 WriteData( Uint16	SlaveAddress, Uint16 RomAddress,Uint16 number, Uint8 *Wdata)        $
// $ 函数功能:    IIC写数据                                                       $
//###########################################################################
Uint16 WriteData( Uint16 SlaveAddress, Uint16 RomAddress,Uint16 number, Uint8 *Wdata)
{
   Uint16 i;
   if (I2caRegs.I2CSTR.bit.BB == 1)
   {
      return I2C_BUS_BUSY_ERROR;   //返回总线忙错误状态
   }
   while(!I2C_xrdy());      //等待数据发送就绪，XRDY=1,表明发送寄存器已经准备好接受新的数据
   I2caRegs.I2CSAR = SlaveAddress&0xff;    //设备从地址
   I2caRegs.I2CCNT = number + 1;   //需要发送的字节数
   I2caRegs.I2CDXR = RomAddress&0xff;    //第一个发送字节为发送数据的目标寄存器地址
   I2caRegs.I2CMDR.all = 0x6E20;    //发送起始信号，内部数据计数器减到0时，发送停止信号，主机发送模式，使能IIC模式，
   for (i=0; i<number; i++)
   {
      while(!I2C_xrdy());   //等待数据发送就绪，发送下一个数据
      I2caRegs.I2CDXR = *Wdata&0xFF;
      Wdata++;
	  if (I2caRegs.I2CSTR.bit.NACK == 1)    //送到无应答信号返回错误
   		  return	I2C_BUS_BUSY_ERROR;
   }
   return I2C_SUCCESS;         //发送成功
}


//###########################################################################
// $ 函数名称:    Uint16 ReadData( Uint16 SlaveAddress, Uint16 RomAddress,Uint16 number, Uint16  *RamAddr)       $
// $ 函数功能:    IIC读数据                                                       $
//###########################################################################
Uint16 ReadData( Uint16 SlaveAddress, Uint16 RomAddress,Uint16 number, Uint16  *RamAddr)
{
   Uint16  i,Temp;

   if (I2caRegs.I2CSTR.bit.BB == 1)  //返回总线忙错误状态
   {
       return I2C_BUS_BUSY_ERROR;
   }
   while(!I2C_xrdy());
   I2caRegs.I2CSAR = SlaveAddress&0xff;      //设备从地址
   I2caRegs.I2CCNT = 1;        //发送一个字节为要读取数据的寄存器地址
   I2caRegs.I2CDXR = RomAddress&0xff;
   I2caRegs.I2CMDR.all = 0x6620;   //主机发送模式，自动清除停止位不产生停止信号
   if (I2caRegs.I2CSTR.bit.NACK == 1)    //送到无应答信号返回错误
   		return	I2C_BUS_BUSY_ERROR;
   DELAY_US(50);
   while(!I2C_xrdy());					//调试过程中经常会卡在这一步
   I2caRegs.I2CSAR = SlaveAddress&0xff;
   I2caRegs.I2CCNT = number;
   I2caRegs.I2CMDR.all = 0x6C20;   //主机接受模式，包含有停止信号
   if (I2caRegs.I2CSTR.bit.NACK == 1)
   		return	I2C_BUS_BUSY_ERROR;
   for(i=0;i<number;i++)
   {
         while(!I2C_rrdy());
   	  Temp = I2caRegs.I2CDRR;
	  if (I2caRegs.I2CSTR.bit.NACK == 1)
   		  return	I2C_BUS_BUSY_ERROR;
   	  *RamAddr = Temp;
   	  RamAddr++;
   }
   return I2C_SUCCESS;    //接受数据成功
}


//###########################################################################
// $ 函数名称:    IICwriteBit(Uint16 slaveaddress, Uint16 regaddress, Uint8 bitNum, Uint8 data)       $
// $ 函数功能:    IIC向寄存器的某一位写数据                                                     $
//###########################################################################
void IICwriteBit(Uint16 slaveaddress, Uint16 regaddress, Uint8 bitNum, Uint8 data)
{
	Uint16 a;
	Uint8 b;
	DELAY_US(50);
	ReadData(slaveaddress,regaddress,1,&a);
	b=(Uint8 )(a&0xff);
	b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
	DELAY_US(50);
	WriteData(slaveaddress,regaddress,1,&b);
}


//###########################################################################
// $ 函数名称:   IICwriteBits(Uint16 slaveaddress,Uint16 regaddress,Uint8 bitStart,Uint8 length,Uint8 data)      $
// $ 函数功能:    IIC向寄存器的某几位写数据                                                     $
//###########################################################################
void IICwriteBits(Uint16 slaveaddress,Uint16 regaddress,Uint8 bitStart,Uint8 length,Uint8 data)
{

	Uint8 b,mask;
	Uint16 a;
	DELAY_US(50);
	ReadData(slaveaddress,regaddress,1,&a);
	b=(Uint8 )(a&0xff);
	mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
	data <<= (8 - length);
	data >>= (7 - bitStart);
	b &= mask;
	b |= data;
	DELAY_US(50);
	WriteData(slaveaddress,regaddress,1, &b);
}
