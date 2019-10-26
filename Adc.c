#include "2440addr.h"
#include "option.h"
#include "macro.h"
#include "device_driver.h"

unsigned int ADC_Start(int ch)
{
	rADCCON = (1<<14)|(39<<6)|(0<<2)|(0<<1)|(0<<0);
	rADCDLY = 0xffff;	
	Macro_Write_Block(rADCCON, 0x7, ch & 0x7, 3);
	Macro_Clear_Bit(rADCCON, 2);
	Macro_Set_Bit(rADCCON, 0);

	while(Macro_Check_Bit_Set(rADCCON, 0));
	while(Macro_Check_Bit_Clear(rADCCON, 15));

	return Macro_Extract_Area(rADCDAT0, 0x3ff, 0);
}

void ADC_ISR_Init(int en, int ch)
{
	if(en)
	{
		rADCDLY = 0xff;
		rADCCON = (1<<14)|(39<<6)|((ch & 0x7)<<3)|(0<<2)|(1<<1)|(0<<0);
		rSUBSRCPND = 1<<10;
		rSRCPND = 1<<31;
		rINTPND = 1<<31;
		Macro_Clear_Bit(rINTMSK, 31);
		Macro_Clear_Bit(rINTSUBMSK, 10);
		en = rADCDAT0;
	}

	else
	{
		rADCCON = 0;
		Macro_Set_Bit(rINTMSK, 31);
		Macro_Set_Bit(rINTSUBMSK, 10);
	}
}

unsigned int Touch_Get_Data(void)
{
	rADCCON = (1<<14)|(39<<6)|(0<<2)|(0<<1)|(0<<0);
	rADCDLY = 0xffff;
	rADCTSC = (1<<3)+(1<<2)+(0<<0);

	Macro_Set_Bit(rADCCON, 0);
	while(Macro_Check_Bit_Set(rADCCON, 0));
	while(Macro_Check_Bit_Clear(rADCCON, 15));

	return ((rADCDAT0 & 0x3ff) << 16) + (rADCDAT1 & 0x3ff);
}

void Touch_ISR_Enable(int enable)
{
	if(enable)
	{
		rADCCON = (1<<14)|(39<<6)|(0<<2)|(0<<1)|(0<<0);
		rADCDLY = 0xffff;
		rADCTSC=(0<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(0<<3)|(0<<2)|(3);
		rSUBSRCPND = 1<<9;
		rSRCPND = 1<<31;
		rINTPND =  1<<31;
		Macro_Clear_Bit(rINTMSK, 31);
		Macro_Clear_Bit(rINTSUBMSK, 9);
	}

	else
	{
		Macro_Set_Bit(rINTMSK, 31);
		Macro_Set_Bit(rINTSUBMSK, 9);
	}
}

#define XM	83
#define YM	893
#define XP	940
#define YP	118

volatile int Touch_x = 0, Touch_y = 0;

void Touch_Get_Position(void)
{
	rADCTSC = (1<<3)+(1<<2)+(0<<0);
	Macro_Clear_Bit(rADCCON, 2);
	Macro_Set_Bit(rADCCON, 0);
	while(Macro_Check_Bit_Set(rADCCON, 0));
	while(Macro_Check_Bit_Clear(rADCCON, 15));

	Touch_x = ((rADCDAT0 & 0x3ff)-XM)*(320-10)/(XP-XM)+5;
	Touch_y = 235-((rADCDAT1 & 0x3ff)-YP)*(240-10)/(YM-YP);

	if(Touch_x<0) Touch_x=0;
	if(Touch_x>=320) Touch_x=320-1;
	if(Touch_y<0) Touch_y=0;
	if(Touch_y>=240) Touch_y=240-1;
}

void Touch_Set_Wait_For_Release(void)
{
	volatile int i;

	rADCTSC=(1<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(0<<3)|(0<<2)|(3);
	rADCDLY=100;
	for(i=0; i<0x1000; i++);
}
