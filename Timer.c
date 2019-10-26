#include "2440addr.h"
#include "macro.h"
#include "option.h"
#include "device_driver.h"

#define TIMER0_MAX	0xffff
#define TIMER0_TICK	20
#define TIMER0_PULSE_FOR_1MS (1000/TIMER0_TICK)

void Timer0_Stop_Watch_Run(void)
{
	Macro_Write_Block(rTCFG0, 0xff, PCLK/(50000 * 8)-1, 0);
	Macro_Write_Block(rTCFG1, 0xf, 2, 0);
	rTCNTB0 = 0xffff;
	Macro_Write_Block(rTCON,0x3, 2, 0);
	Macro_Write_Block(rTCON,0x3, 1, 0);
}

unsigned int Timer0_Stop_Watch_Stop(void)
{
	Macro_Clear_Bit(rTCON, 0);
	return (TIMER0_MAX - rTCNTO0) * TIMER0_TICK;
}

int Timer0_Stop_Watch_Run_Unlimited(unsigned int max_msec)
{
	int i;
	int n;
	int prescaler;

	for(i = 0, n = 2; n <= 16; n*=2, i++)
	{
		prescaler = (int)(((double)PCLK * (double)max_msec)/(1000.0 * 65535.0 * n));

		if(prescaler <= 256)
		{
			Macro_Write_Block(rTCFG0, 0xff, prescaler - 1, 0);
			Macro_Write_Block(rTCFG1, 0xf, i, 0);
			rTCNTB0 = 0xffff;
			Macro_Write_Block(rTCON,0x3, 2, 0);
			Macro_Write_Block(rTCON,0x3, 1, 0);
			return 1;
		}
	}

	return 0;
}

unsigned int Timer0_Stop_Watch_Stop_Unlimited(void)
{
	int mux[] = {2,4,8,16};

	Macro_Clear_Bit(rTCON, 0);
	return (int)((1000000.0 * (0xffff - rTCNTO0) * mux[Macro_Extract_Area(rTCFG1, 0xf, 0)] * (Macro_Extract_Area(rTCFG0, 0xff, 0) + 1) / (double)PCLK) + 0.5);
}

/* 시간 제약이 약 1.25초 정도인 함수 */

#if 0

void Timer0_Delay(int time)
{
	Macro_Write_Block(rTCFG0, 0xff, PCLK/(50000 * 8)-1, 0);
	Macro_Write_Block(rTCFG1, 0xf, 2, 0);
	rTCNTB0 = (time * TIMER0_PULSE_FOR_1MS) - 1;
	rSRCPND = BIT_TIMER0;
	Macro_Write_Block(rTCON,0x3, 2, 0);
	Macro_Write_Block(rTCON,0x3, 1, 0);
	while(Macro_Check_Bit_Clear(rSRCPND, TIMER0));
	rSRCPND = 1<<10;
}

#else

/* 시간 제약이 없는 함수 */

void Timer0_Delay(int time)
{
	int i;
	unsigned int temp = time * 50;

	Macro_Write_Block(rTCFG0, 0xff, PCLK/(50000 * 8)-1, 0);
	Macro_Write_Block(rTCFG1, 0xf, 2, 0);

	for(i=0; i<(temp/0x10000); i++)
	{
		rTCNTB0 = 0xFFFF;
		rSRCPND = BIT_TIMER0;
		Macro_Write_Block(rTCON,0x3, 2, 0);
		Macro_Write_Block(rTCON,0x3, 1, 0);
		
		while(Macro_Check_Bit_Clear(rSRCPND, TIMER0));
		rSRCPND = 1<<10;;
	}
	
	if((temp % 0x10000) - 1)
	{
		rTCNTB0 = (temp % 0x10000) - 1;
		rSRCPND = BIT_TIMER0;
		Macro_Write_Block(rTCON,0x3, 2, 0);
		Macro_Write_Block(rTCON,0x3, 1, 0);
		
		while(Macro_Check_Bit_Clear(rSRCPND, TIMER0));
		rSRCPND = 1<<10;;
	}
}

#endif

void Timer0_Repeat(int time)
{
	Macro_Write_Block(rTCFG0, 0xff, PCLK/(50000 * 8)-1, 0);
	Macro_Write_Block(rTCFG1, 0xf, 2, 0);
	rTCNTB0 = (time * TIMER0_PULSE_FOR_1MS) - 1;
	rSRCPND = 1<<10;;
	Macro_Write_Block(rTCON,0xF, 0x2, 0);
	Macro_Write_Block(rTCON,0xF, 0x9, 0);
}

int Timer0_Check_Timeout(void)
{
	int r = Macro_Check_Bit_Set(rSRCPND, 10);
	
	if(r) rSRCPND = 1<<10;
	return r;
}

void Timer0_Stop(void)
{
	Macro_Clear_Bit(rTCON, 0);
}

void Timer0_Set_Time(int time)
{
	rTCNTB0 = (time * TIMER0_PULSE_FOR_1MS) - 1;
}

void Timer0_Delay_ISR_Enable(int en, int time)
{
	if(en)
	{
		Macro_Write_Block(rTCFG0, 0xff, PCLK/(50000 * 8)-1, 0);
		Macro_Write_Block(rTCFG1, 0xf, 2, 0);
		rTCNTB0 = (time * TIMER0_PULSE_FOR_1MS) - 1;
		rSRCPND = BIT_TIMER0;
		Macro_Write_Block(rTCON,0x3, 2, 0);

		rSRCPND = 1<<10;
		rINTPND = 1<<10;
		Macro_Clear_Bit(rINTMSK, 10);

		Macro_Write_Block(rTCON,0x3, 1, 0);
	}

	else
	{
		Macro_Clear_Bit(rTCON, 0);
		Macro_Set_Bit(rINTMSK, 10);
	}
}

void Timer0_Repeat_ISR_Enable(int en, int time)
{
	if(en)
	{
		Macro_Write_Block(rTCFG0, 0xff, PCLK/(50000 * 8)-1, 0);
		Macro_Write_Block(rTCFG1, 0xf, 2, 0);
		rTCNTB0 = (time * TIMER0_PULSE_FOR_1MS) - 1;
		rSRCPND = BIT_TIMER0;
		Macro_Write_Block(rTCON,0xF, 0x2, 0);

		rSRCPND = 1<<10;
		rINTPND = 1<<10;
		Macro_Clear_Bit(rINTMSK, 10);

		Macro_Write_Block(rTCON,0xF, 0x9, 0);
	}

	else
	{
		Macro_Clear_Bit(rTCON, 0);
		Macro_Set_Bit(rINTMSK, 10);
	}
}

#define TIMER1_MAX	0xffff
#define TIMER1_TICK	5

void Timer1_Stopwatch_Run(void)
{
	rTCNTB1 = TIMER1_MAX;

	Macro_Write_Block(rTCFG0, 0xff, PCLK/(50000 * 8)-1, 0);
	Macro_Write_Block(rTCFG1, 0xf, 0, 4);
	Macro_Write_Block(rTCON,0x3, 2, 8);
	Macro_Write_Block(rTCON,0x3, 1, 8);
}

int Timer1_Stopwatch_Stop(void)
{
	Macro_Clear_Bit(rTCON, 8);
	return (TIMER1_MAX - rTCNTO1) * TIMER1_TICK;
}

void Timer4_Init(void)
{
	Macro_Write_Block(rTCFG0, 0xff, 0, 8); 	// PRS1 = 0
	Macro_Write_Block(rTCFG1, 0xf, 3, 16);  // MUX4 = 1/16
	Macro_Clear_Area(rTCFG1, 0xf, 20);		// DMA - No
}

void Timer4_Delay(unsigned int msec)
{
	// 분주는 1/16사용, PCLK는 399.65MHz/6
	// Tick time = 16/(PCLK/1000)
	// 1msec에 필요한 tick의 수 = 1msec/tick_time = (PCLK/1000)/16

	int i;
	unsigned int temp = msec * ((PCLK/1000)/16);

	for(i = 0; i<(temp/0x10000); i++)
	{
		rTCNTB4 = 0xffff;

		Macro_Write_Block(rTCON, 0x7, 0x2, 20);
		Macro_Write_Block(rTCON, 0x7, 0x1, 20);
		while(Macro_Check_Bit_Clear(rSRCPND, 14));
		rSRCPND = BIT_TIMER4;
	}

	rTCNTB4 = temp%0x10000;

	Macro_Write_Block(rTCON, 0x7, 0x2, 20);
	Macro_Write_Block(rTCON, 0x7, 0x1, 20);
	while(Macro_Check_Bit_Clear(rSRCPND, 14));
	rSRCPND = BIT_TIMER4;

	// timer stop
	Macro_Clear_Bit(rTCON, 20);
}


