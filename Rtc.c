#include "2440addr.h"
#include "macro.h"
#include "device_driver.h"

void RTC_Set_Time(RTC_TIME *time)
{
	Macro_Set_Bit(rRTCCON, 0);

	rBCDDAY = time->day;     	
	rBCDYEAR = (unsigned char)(((time->year/10)<<4)|(time->year%10));
	rBCDMON = (unsigned char)(((time->mon/10)<<4)|(time->mon%10));
	rBCDDATE = (unsigned char)(((time->date/10)<<4)|(time->date%10));
	rBCDHOUR = (unsigned char)(((time->hour/10)<<4)|(time->hour%10));
	rBCDMIN = (unsigned char)(((time->min/10)<<4)|(time->min%10));
	rBCDSEC = (unsigned char)(((time->sec/10)<<4)|(time->sec%10));
	
	Macro_Clear_Bit(rRTCCON, 0);
}

void RTC_Get_Time(RTC_TIME *time)
{
	RTC_TIME a;

	a.day = (int)rBCDDAY;
	a.year = (int)rBCDYEAR;
	a.mon = (int)rBCDMON;
	a.date = (int)rBCDDATE;
	a.hour = (int)rBCDHOUR;
	a.min = (int)rBCDMIN;
	a.sec = (int)rBCDSEC;

	if(!a.sec)
	{
		a.day = (int)rBCDDAY;
		a.year = (int)rBCDYEAR;
		a.mon = (int)rBCDMON;
		a.date = (int)rBCDDATE;
		a.hour = (int)rBCDHOUR;
		a.min = (int)rBCDMIN;
	}

	time->day = a.day;
	time->year = ((a.year >> 4) & 0xf) * 10 + (a.year & 0xf);
	time->mon = ((a.mon >> 4) & 0xf) * 10 + (a.mon & 0xf);
	time->date = ((a.date >> 4) & 0xf) * 10 + (a.date & 0xf);
	time->hour = ((a.hour >> 4) & 0xf) * 10 + (a.hour & 0xf);
	time->min = ((a.min >> 4) & 0xf) * 10 + (a.min & 0xf);
	time->sec = ((a.sec >> 4) & 0xf) * 10 + (a.sec & 0xf);
}

void RTC_Set_Alarm_Enable(int en, RTC_TIME *time)
{
	unsigned int x = (1<<6);

	if(en)
	{
		if(time->year) x |= 1<<5;
		if(time->mon) x |= 1<<4;
		if(time->date) x |= 1<<3;
		if(time->hour) x |= 1<<2;
		if(time->min) x |= 1<<1;
		if(time->sec) x |= 1<<0;

		Macro_Set_Bit(rRTCCON, 0);
		rRTCALM = x;
		Macro_Clear_Bit(rRTCCON, 0);

		rSRCPND = 1<<30;
		rINTPND = 1<<30;
		Macro_Clear_Bit(rINTMSK, 30);
	}

	else
	{
		Macro_Set_Bit(rINTMSK, 30);
	}
}

void RTC_Set_Alarm(RTC_TIME *time)
{
	Macro_Set_Bit(rRTCCON, 0);

	rALMYEAR = (unsigned char)(((time->year/10)<<4)|(time->year%10));
	rALMMON = (unsigned char)(((time->mon/10)<<4)|(time->mon%10));
	rALMDATE = (unsigned char)(((time->date/10)<<4)|(time->date%10));
	rALMHOUR = (unsigned char)(((time->hour/10)<<4)|(time->hour%10));
	rALMMIN = (unsigned char)(((time->min/10)<<4)|(time->min%10));
	rALMSEC = (unsigned char)(((time->sec/10)<<4)|(time->sec%10));

	Macro_Clear_Bit(rRTCCON, 0);
}

void RTC_Get_Alarm(RTC_TIME *time)
{
	time->year = ((rALMYEAR >> 4) & 0xf) * 10 + (rALMYEAR & 0xf);
	time->mon = ((rALMMON >> 4) & 0xf) * 10 + (rALMMON & 0xf);
	time->date = ((rALMDATE >> 4) & 0xf) * 10 + (rALMDATE & 0xf);
	time->hour = ((rALMHOUR >> 4) & 0xf) * 10 + (rALMHOUR & 0xf);
	time->min = ((rALMMIN >> 4) & 0xf) * 10 + (rALMMIN & 0xf);
	time->sec = ((rALMSEC >> 4) & 0xf) * 10 + (rALMSEC & 0xf);
}

void RTC_Tick_ISR_Enable(int en, int msec)
{
	Macro_Set_Bit(rRTCCON, 0);

	if(en)
	{
		Macro_Write_Block(rTICNT, 0x7f, (128.0/1000.0)*msec ,0);
		rSRCPND = 1<<8;
		rINTPND = 1<<8;
		Macro_Clear_Bit(rINTMSK, 8);
		Macro_Set_Bit(rTICNT, 7);
	}

	else
	{
		Macro_Clear_Bit(rTICNT, 7);
		Macro_Set_Bit(rINTMSK, 8);
	}

	Macro_Clear_Bit(rRTCCON, 0);
}
