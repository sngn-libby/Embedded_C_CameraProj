#include "2440addr.h"
#include "device_driver.h"
#include "macro.h"

// ISR Functions declaration

void Invalid_ISR(void) __attribute__ ((interrupt ("IRQ")));
void EINT0_ISR(void) __attribute__ ((interrupt ("IRQ")));
void EINT4_7_ISR(void) __attribute__ ((interrupt ("IRQ")));
void UART0_ISR(void) __attribute__ ((interrupt ("IRQ")));
void TIMER0_ISR(void) __attribute__ ((interrupt ("IRQ")));
void DMA0_ISR(void) __attribute__ ((interrupt ("IRQ")));
void DMA1_ISR(void) __attribute__ ((interrupt ("IRQ")));
void DMA2_ISR(void) __attribute__ ((interrupt ("IRQ")));
void DMA3_ISR(void) __attribute__ ((interrupt ("IRQ")));
void RTC_ISR(void) __attribute__ ((interrupt ("IRQ")));
void RTC_Tick_ISR(void) __attribute__ ((interrupt ("IRQ")));
void WDT_ISR(void) __attribute__ ((interrupt ("IRQ")));
void ADC_ISR(void) __attribute__ ((interrupt ("IRQ")));
void CAM_ISR(void) __attribute__ ((interrupt ("IRQ")));

typedef void (*VFP)(void);

const VFP ISR_Vectors[ ] =
{
	EINT0_ISR, 					// ISR_EINT0
	Invalid_ISR, 				// ISR_EINT1
	Invalid_ISR, 				// ISR_EINT2
	Invalid_ISR, 				// ISR_EINT3
	EINT4_7_ISR, 				// ISR_EINT4_7
	Invalid_ISR, 				// ISR_EINT8_23
	CAM_ISR, 					// ISR_CAM
	Invalid_ISR, 				// ISR_BAT_FLT
	RTC_Tick_ISR, 				// ISR_TICK
	WDT_ISR, 					// ISR_WDT_AC97
	TIMER0_ISR, 				// ISR_TIMER0
	Invalid_ISR, 				// ISR_TIMER1
	Invalid_ISR, 				// ISR_TIMER2
	Invalid_ISR, 				// ISR_TIMER3
	Invalid_ISR, 				// ISR_TIMER4
	Invalid_ISR, 				// ISR_UART2
	Invalid_ISR, 				// ISR_LCD
	DMA0_ISR, 					// ISR_DMA0
	DMA1_ISR, 					// ISR_DMA1
	DMA2_ISR, 					// ISR_DMA2
	DMA3_ISR, 					// ISR_DMA3
	Invalid_ISR, 				// ISR_SDI
	Invalid_ISR, 				// ISR_SPI0
	Invalid_ISR, 				// ISR_UART1
	Invalid_ISR, 				// ISR_NFCON
	Invalid_ISR, 				// ISR_USBD
	Invalid_ISR, 				// ISR_USBH
	Invalid_ISR, 				// ISR_IIC
	UART0_ISR, 					// ISR_UART0
	Invalid_ISR, 				// ISR_SPI1
	RTC_ISR,	 				// ISR_RTC
	ADC_ISR  					// ISR_ADC
};

void CAM_ISR(void)
{
	rSUBSRCPND = 1<<12;
	rSRCPND = 1<<6;
	rINTPND = 1<<6;
	CAM_Set_Capture_Status(2);
}

volatile int Adc_complete = 0;
volatile unsigned int Adc_value = 0;
volatile int Touch_pressed = 0;

void ADC_ISR(void)
{
	if(Macro_Check_Bit_Set(rSUBSRCPND, 10))
	{
		Adc_complete = 1;
		Adc_value = Macro_Extract_Area(rADCDAT0, 0x3ff, 0);
		rSUBSRCPND = 1<<10;
	}

	else
	{
		if(Macro_Check_Bit_Set(rADCUPDN, 1))
		{
			Touch_pressed = 0;
			rADCTSC=(0<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(0<<3)|(0<<2)|(3);
			rADCDLY=0xffff;
		}

		else if(Macro_Check_Bit_Set(rADCUPDN, 0))
		{
			Touch_pressed = 1;
			Touch_Get_Position();

			rADCTSC=(1<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(0<<3)|(0<<2)|(3);
			rADCDLY=100;
		}

		Macro_Clear_Area(rADCUPDN, 3, 0);
		rSUBSRCPND = 1<<9;
	}

	rSRCPND = 1<<31;
	rINTPND =  1<<31;
}

volatile int Wdt_timeout = 0;

void WDT_ISR(void)
{
	rSUBSRCPND = 1<<13;
	rSRCPND = 1<<9;
	rINTPND =  1<<9;
	Wdt_timeout = 1;
}

volatile int RTC_tick = 0;

void RTC_Tick_ISR(void)
{
	rSRCPND = 1<<8;
	rINTPND = 1<<8;
	RTC_tick = 1;
}

volatile int RTC_alarm = 0;

void RTC_ISR(void)
{
	rSRCPND = 1<<30;
	rINTPND = 1<<30;
	RTC_alarm = 1;
}

volatile int DMA_complete[] = {0,0,0,0};

void DMA0_ISR(void)
{
	rSRCPND = 1<<17;
	rINTPND = 1<<17;
	DMA_complete[0] = 1;
	DMA_Stop(0);
}

void DMA1_ISR(void)
{
	rSRCPND = 1<<18;
	rINTPND = 1<<18;
	DMA_complete[1] = 1;
	DMA_Stop(1);
}

void DMA2_ISR(void)
{
	rSRCPND = 1<<19;
	rINTPND = 1<<19;
	DMA_complete[2] = 1;
	DMA_Stop(2);
}

void DMA3_ISR(void)
{
	rSRCPND = 1<<20;
	rINTPND = 1<<20;
	DMA_complete[3] = 1;
	DMA_Stop(3);
}

volatile int Timer0_time_out = 0;

void TIMER0_ISR(void)
{
	rSRCPND = 1<<10;
	rINTPND = 1<<10;

	Timer0_time_out = 1;
}

volatile int Rx_data_in = 0;
volatile int Rx_data = 0;

void UART0_ISR(void)
{
	rSUBSRCPND = 1<<0;
	rSRCPND = 1<<28;
	rINTPND = 1<<28;

	Rx_data = rURXH0;
	Rx_data_in = 1;
}

volatile int Key_push = 0;

void EINT0_ISR(void)
{
	Key_push = 1;

	rSRCPND = 1<<0;
	rINTPND = 1<<0;
}

volatile int Key_value = 0;

void EINT4_7_ISR(void)
{
	Macro_Write_Block(rGPFCON, 0xff, 0x0, 8);
	Key_value = Key_Get_Pressed();
	Key_ISR_Init();

	rEINTPEND = 0xf << 4;
	rSRCPND = 1<<4;
	rINTPND = 1<<4;
}

void Invalid_ISR(void)
{
	Uart_Printf("Invalid IRQ Requested...\n");
	for(;;);
}

void FIQ_Handler(void)
{
	Uart_Printf("FIQ-Exception!\n");
	for(;;);
}

void Undef_Handler(void)
{
	Uart_Printf("UND-Exception!\n");
	for(;;);
}

void Pabort_Handler(void)
{
	Uart_Printf("PABT-Exception!\n");
	for(;;);
}

void Dabort_Handler(void)
{
	Uart_Printf("DABT-Exception!\n");
	for(;;);
}

void SWI_Handler(void)
{
	Uart_Printf("SWI-Exception!\n");
	for(;;);
}


