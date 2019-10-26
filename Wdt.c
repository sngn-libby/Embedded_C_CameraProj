#include "2440addr.h"
#include "option.h"
#include "macro.h"

void WDT_Start(int start, int msec)
{
	Macro_Clear_Bit(rWTCON ,5);

	if(start)
	{
		rWTCON |= (192 <<8)|(3 <<3);
		rWTCNT = msec * 2;
		rWTDAT = msec * 2;
		Macro_Set_Bit(rWTCON ,5);
	}
}

void WDT_ISR_Enable(int enable, int msec)
{
	WDT_Start(0, 0);

	if(enable)
	{
		rSUBSRCPND = 1<<13;
		rSRCPND = 1<<9;
		rINTPND =  1<<9;

		WDT_Start(1, msec);
		Macro_Clear_Bit(rINTMSK, 9);
		Macro_Clear_Bit(rINTSUBMSK, 13);
		Macro_Set_Bit(rWTCON ,2);
	}

	else
	{
		Macro_Clear_Bit(rWTCON ,2);		
		Macro_Set_Bit(rINTMSK, 9);
		Macro_Set_Bit(rINTSUBMSK, 13);
	}
}

void WDT_Clear(unsigned int msec)
{
	rWTCNT = msec * 2;
	rWTDAT = msec * 2;
}

void WDT_Reset_Enable(int enable)
{	
	enable? (Macro_Set_Bit(rWTCON ,0)) : (Macro_Clear_Bit(rWTCON ,0));
}
