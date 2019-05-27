#include "DSP28x_Project.h"
#ifndef IIC_H
#define IIC_H


//º¯ÊıÉùÃ÷
Uint16 	ReadData( Uint16 SlaveAddress, Uint16 RomAddress,Uint16 number, Uint16 *RamAddr);
Uint16 	WriteData( Uint16 SlaveAddress, Uint16 RomAddress,Uint16 number, Uint8 *Wdata);
void IICwriteBit(Uint16 slaveaddress, Uint16 regaddress, Uint8 bitNum, Uint8 data);
void IICwriteBits(Uint16 slaveaddress,Uint16 regaddress,Uint8 bitStart,Uint8 length,Uint8 data);
Uint16	I2C_xrdy();
Uint16	I2C_rrdy();
void Init_I2CA(void);


#endif
