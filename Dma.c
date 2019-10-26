#include "device_driver.h"
#include "2440addr.h"
#include "option.h"
#include "macro.h"

volatile unsigned int * DISRC[] = {&rDISRC0, &rDISRC1, &rDISRC2, &rDISRC3};
volatile unsigned int * DIDST[] = {&rDIDST0, &rDIDST1, &rDIDST2, &rDIDST3};
volatile unsigned int * DISRCC[] = {&rDISRCC0, &rDISRCC1, &rDISRCC2, &rDISRCC3};
volatile unsigned int * DIDSTC[] = {&rDIDSTC0, &rDIDSTC1, &rDIDSTC2, &rDIDSTC3};
volatile unsigned int * DCON[] = {&rDCON0, &rDCON1, &rDCON2, &rDCON3};
volatile unsigned int * DSTAT[] = {&rDSTAT0, &rDSTAT1, &rDSTAT2, &rDSTAT3};
volatile unsigned int * DCSRC[] = {&rDCSRC0, &rDCSRC1, &rDCSRC2, &rDCSRC3};
volatile unsigned int * DCDST[] = {&rDCDST0, &rDCDST1, &rDCDST2, &rDCDST3};
volatile unsigned int * DMASKTRIG[] = {&rDMASKTRIG0, &rDMASKTRIG1, &rDMASKTRIG2, &rDMASKTRIG3};

void DMA_Start(int ch, void * sa, void * da, UNI_SRCC srcc, UNI_DSTC dstc, UNI_DCON dcon)
{
	Macro_Set_Bit(*DMASKTRIG[ch], 2);
	Macro_Clear_Bit(*DMASKTRIG[ch], 1);

	*DISRC[ch]  = (unsigned int)sa;
	*DIDST[ch]  = (unsigned int)da;

	*DISRCC[ch] = srcc.udata;
	*DIDSTC[ch] = dstc.udata;
	*DCON[ch] = dcon.udata;

	rSRCPND = 1<<(17+ch);
	rINTPND = 1<<(17+ch);
	Macro_Clear_Bit(rINTMSK, (17+ch));

	Macro_Clear_Bit(*DMASKTRIG[ch], 2);
	Macro_Set_Bit(*DMASKTRIG[ch], 1);
	Macro_Set_Bit(*DMASKTRIG[ch], 0);
}

void DMA_Stop(int ch)
{
	Macro_Set_Bit(rINTMSK, (17+ch));
	Macro_Set_Bit(*DMASKTRIG[ch], 2);
}

void DMA_Timer_Select(int dma, int ch)
{
	if(dma)
	{
		Macro_Write_Block(rTCFG1, 0xf, ch+1 , 20);
	}

	else
	{
		Macro_Clear_Area(rTCFG1, 0xf, 20);
	}
}

void DMA_Uart0_Ch0_Select(int dma, int rx_tx)
{
	int bit = rx_tx ? 2 : 0;

	if(dma) Macro_Write_Block(rUCON0, 0x3, 2, bit);
	else Macro_Write_Block(rUCON0, 0x3, 1, bit);
}

void DMA_Uart1_Ch1_Select(int dma, int rx_tx)
{
	int bit = rx_tx ? 2 : 0;

	if(dma) Macro_Write_Block(rUCON1, 0x3, 3, bit);
	else Macro_Write_Block(rUCON1, 0x3, 1, bit);
}

void DMA_Uart2_Ch3_Select(int dma, int rx_tx)
{
	int bit = rx_tx ? 2 : 0;

	if(dma) Macro_Write_Block(rUCON2, 0x3, 2, bit);
	else Macro_Write_Block(rUCON2, 0x3, 1, bit);
}

