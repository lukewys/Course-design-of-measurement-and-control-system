#include "DSP28x_Project.h"
#include "IIC.h"


//###########################################################################
// $ ��������:    void Init_I2CA(void)                            $
// $ ��������:    IICģ���ʼ��������IICģ��Ĺ��������Լ�����IIC�Ĺ�����ʽ                             $
//###########################################################################
void Init_I2CA(void)
{
	I2caRegs.I2CMDR.all = 0x0000;	// ��λIIC

	EALLOW;
	GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;    // ʹ��(SDAA)����
	GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;	   //  ʹ�� (SCLA)����
	GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;  // ͬ�� (SDAA)
	GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;  // ͬ�� (SCLA)
	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;   // ���� GPIO32Ϊ SDAA
	GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;   // ����GPIO33 ΪSCLA
	EDIS;

	// Ԥ��Ƶ����ʱ��ģ���Ƶ��
	I2caRegs.I2CPSC.all = 7;	   // Ԥ��Ƶ	IICģ��ʱ��������Ϊ7-12MHz����ʵ������Ϊ (80/8 = 10MHz)
	I2caRegs.I2CCLKL = 10;   //ʱ�ӵ͵�ƽʱ��ֵ
	I2caRegs.I2CCLKH = 5;	//ʱ�Ӹߵ�ƽʱ��ֵ

	I2caRegs.I2CMDR.all = 0x0020;	// IIC׼������

}


//###########################################################################
// $ ��������:    Uint16	I2C_xrdy()                        $
// $ ��������:    IICģ�鷢��׼��OK                              $
//###########################################################################
Uint16	I2C_xrdy()
{
	Uint16	t;
	t = I2caRegs.I2CSTR.bit.XRDY;   // IICģ�鷢��׼��OK
	return t;
}


//###########################################################################
// $ ��������:    Uint16	I2C_rrdy()                      $
// $ ��������:    IICģ�����׼��OK                              $
//###########################################################################
Uint16	I2C_rrdy()
{
	Uint16	t;
	t = I2caRegs.I2CSTR.bit.RRDY;   //  IICģ�����׼��OK
	return t;
}


//###########################################################################
// $ ��������:    Uint16 WriteData( Uint16	SlaveAddress, Uint16 RomAddress,Uint16 number, Uint8 *Wdata)        $
// $ ��������:    IICд����                                                       $
//###########################################################################
Uint16 WriteData( Uint16 SlaveAddress, Uint16 RomAddress,Uint16 number, Uint8 *Wdata)
{
   Uint16 i;
   if (I2caRegs.I2CSTR.bit.BB == 1)
   {
      return I2C_BUS_BUSY_ERROR;   //��������æ����״̬
   }
   while(!I2C_xrdy());      //�ȴ����ݷ��;�����XRDY=1,�������ͼĴ����Ѿ�׼���ý����µ�����
   I2caRegs.I2CSAR = SlaveAddress&0xff;    //�豸�ӵ�ַ
   I2caRegs.I2CCNT = number + 1;   //��Ҫ���͵��ֽ���
   I2caRegs.I2CDXR = RomAddress&0xff;    //��һ�������ֽ�Ϊ�������ݵ�Ŀ��Ĵ�����ַ
   I2caRegs.I2CMDR.all = 0x6E20;    //������ʼ�źţ��ڲ����ݼ���������0ʱ������ֹͣ�źţ���������ģʽ��ʹ��IICģʽ��
   for (i=0; i<number; i++)
   {
      while(!I2C_xrdy());   //�ȴ����ݷ��;�����������һ������
      I2caRegs.I2CDXR = *Wdata&0xFF;
      Wdata++;
	  if (I2caRegs.I2CSTR.bit.NACK == 1)    //�͵���Ӧ���źŷ��ش���
   		  return	I2C_BUS_BUSY_ERROR;
   }
   return I2C_SUCCESS;         //���ͳɹ�
}


//###########################################################################
// $ ��������:    Uint16 ReadData( Uint16 SlaveAddress, Uint16 RomAddress,Uint16 number, Uint16  *RamAddr)       $
// $ ��������:    IIC������                                                       $
//###########################################################################
Uint16 ReadData( Uint16 SlaveAddress, Uint16 RomAddress,Uint16 number, Uint16  *RamAddr)
{
   Uint16  i,Temp;

   if (I2caRegs.I2CSTR.bit.BB == 1)  //��������æ����״̬
   {
       return I2C_BUS_BUSY_ERROR;
   }
   while(!I2C_xrdy());
   I2caRegs.I2CSAR = SlaveAddress&0xff;      //�豸�ӵ�ַ
   I2caRegs.I2CCNT = 1;        //����һ���ֽ�ΪҪ��ȡ���ݵļĴ�����ַ
   I2caRegs.I2CDXR = RomAddress&0xff;
   I2caRegs.I2CMDR.all = 0x6620;   //��������ģʽ���Զ����ֹͣλ������ֹͣ�ź�
   if (I2caRegs.I2CSTR.bit.NACK == 1)    //�͵���Ӧ���źŷ��ش���
   		return	I2C_BUS_BUSY_ERROR;
   DELAY_US(50);
   while(!I2C_xrdy());					//���Թ����о����Ῠ����һ��
   I2caRegs.I2CSAR = SlaveAddress&0xff;
   I2caRegs.I2CCNT = number;
   I2caRegs.I2CMDR.all = 0x6C20;   //��������ģʽ��������ֹͣ�ź�
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
   return I2C_SUCCESS;    //�������ݳɹ�
}


//###########################################################################
// $ ��������:    IICwriteBit(Uint16 slaveaddress, Uint16 regaddress, Uint8 bitNum, Uint8 data)       $
// $ ��������:    IIC��Ĵ�����ĳһλд����                                                     $
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
// $ ��������:   IICwriteBits(Uint16 slaveaddress,Uint16 regaddress,Uint8 bitStart,Uint8 length,Uint8 data)      $
// $ ��������:    IIC��Ĵ�����ĳ��λд����                                                     $
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
