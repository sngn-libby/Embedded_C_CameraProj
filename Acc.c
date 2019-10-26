#include "2440addr.h"
#include "macro.h"
#include "device_driver.h"

#define ACC_SPI1_CLK		(10000000)
#define ACC_ENABLE() 		Macro_Clear_Bit(rGPGDAT, 3)
#define ACC_DISABLE()		Macro_Set_Bit(rGPGDAT, 3)

#define ACC_READ			(1<<7)
#define ACC_WRITE			(0<<7)
#define ACC_INC				(1<<6)
#define ACC_FIX				(0<<6)

#define SPI1_DUMMY_DATA		(0xff)
#define SPI1_WAIT_SEND()	while(!(rSPSTA1 & 1))

int SPI1_Acc_Read(int reg);
void SPI1_Acc_Write(int reg, int data);

int SPI1_Acc_Init(void);
int SPI1_ACC_Read_ID(void);
int SPI1_ACC_Read_Status(void);
int SPI1_ACC_Read_XYZ(void);
int SPI1_ACC_Read_X(void);
int SPI1_ACC_Read_Y(void);
int SPI1_ACC_Read_Z(void);

int SPI1_Acc_Init(void)
{
	double d;

	Macro_Write_Block(rGPGCON, 0x3F3, 0x3F1, 6);
	ACC_DISABLE();
	rSPCON1 = (0<<5)|(1<<4)|(1<<3)|(0<<2)|(0<<1)|(0<<0);
	d = (double)PCLK/(2. * 10000000.) + 0.5;
	rSPPRE1 = ((U32)d == 0)? ((U32)d) : ((U32)d - 1);
	rSPPIN1 = (0<<2)|(0<<0);
	SPI1_Acc_Write(0x20,0xE7); 
	SPI1_Acc_Write(0x21,0x00);

	return SPI1_ACC_Read_ID();
}

int SPI1_Acc_Read(int reg)
{
	ACC_ENABLE();
	rSPTDAT1 = ACC_READ | reg;
	SPI1_WAIT_SEND();
	rSPTDAT1 = SPI1_DUMMY_DATA;
	SPI1_WAIT_SEND();
	ACC_DISABLE();
	return rSPRDAT1;
}

void SPI1_Acc_Write(int reg, int data)
{
	ACC_ENABLE();
	rSPTDAT1 = ACC_WRITE | reg;
	SPI1_WAIT_SEND();
	rSPTDAT1 = data;
	SPI1_WAIT_SEND();
	ACC_DISABLE();
}

int SPI1_ACC_Read_ID(void)
{
	return SPI1_Acc_Read(0xf);
}

int SPI1_ACC_Read_Status(void)
{
	return SPI1_Acc_Read(0x27);
}

int SPI1_ACC_Read_XYZ(void)
{
	int r = 0;

	r = (SPI1_Acc_Read(0x29) & 0xff) << 16;
	r |= (SPI1_Acc_Read(0x2B) & 0xff) << 8;	
	r |= (SPI1_Acc_Read(0x2D) & 0xff);

	return r;
}

int SPI1_ACC_Read_X(void)
{
	return SPI1_Acc_Read(0x29);
}

int SPI1_ACC_Read_Y(void)
{
	return SPI1_Acc_Read(0x2B);
}

int SPI1_ACC_Read_Z(void)
{
	return SPI1_Acc_Read(0x2D);
}

