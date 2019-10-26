#include "2440addr.h"
#include "macro.h"
#include "option.h"
#include "device_driver.h"
#include <stdlib.h>

#define DEVICE_PRINT				1

void WM8973_IIC_Write(int add,int data);
void WM8973_Init(void);

#define WM8973_IIC_WR_ADDR()		rIICDS=0x34
#define WM8973_IIC_WR_DATA(x)		rIICDS=(x)
#define WM8973_IIC_WR_START()		rIICSTAT=0xf0
#define WM8973_IIC_WR_STOP()		rIICSTAT=0xd0

#define WM8973_IIC_Pending_Clear()	 Macro_Clear_Bit(rIICCON, 4)
#define WM8973_IIC_Wait_INTR()	 	 while(Macro_Check_Bit_Clear(rIICCON, 4))
#define WM8973_IIC_Wait_STOP()	 	 while(Macro_Check_Bit_Set(rIICSTAT, 5))

void WM8973_IIC_Write(int addr, int data)
{
  	Macro_Write_Block(rGPECON, 0xf, 0xa, 28);
	rIICCON = (1<<7)+(0<<6)+(1<<5)+(0<<4)+(8<<0);

	WM8973_IIC_WR_ADDR();
	WM8973_IIC_WR_START();
	WM8973_IIC_Wait_INTR();
	
	WM8973_IIC_WR_DATA((addr<<1) + ((data>>8)&0x1));
	WM8973_IIC_Pending_Clear();
	WM8973_IIC_Wait_INTR();

	WM8973_IIC_WR_DATA(data & 0xff);
	WM8973_IIC_Pending_Clear();
	WM8973_IIC_Wait_INTR();
			
	WM8973_IIC_WR_STOP();
	WM8973_IIC_Pending_Clear();
	WM8973_IIC_Wait_STOP();
}

static const int WM8973_INIT[ ][2] =
{
	{0x0f,0x000}, 	// RESET
	{0x19,0x0c0}, 	// Vmid, Vref ON
	{0x1A,0x180},	// DAC ON
	{0x1A,0x1e2},	// DAC, OUT1(HP), 3(GND) ON
	{0x00,0x097},	// L-IN PGA Mute
	{0x01,0x097},	// R-IN PGA Mute
	{0x02,0x17f},	// LOUT1(HP) Volume => max (0x7f~0x60, 0x00: mute)
	{0x03,0x17f},	// ROUT1(HP) Volume => max (0x7f~0x60, 0x00: mute)
	{0x07,0x002},	// 16bit, IIS
	{0x08,0x000},	// Sample Rate: Slave Mode(don't care)
	{0x28,0x17f},	// LOUT2(SP) Volume => max (0x7f~0x60, 0x00: mute)
	{0x29,0x17f},	// ROUT2(SP) Volume => max (0x7f~0x60, 0x00: mute)
	{0x0A,0x1ff},	// DACL VOL => 0xff(max) ~ 0x00(min)
	{0x0B,0x1ff},	// DACR VOL => 0xff(max) ~ 0x00(min)
	{0x22,0x170},	// Lmixer = DACL, Vol = 0x0(max) ~ 0x7(min)
	{0x25,0x170},	// Rmixer = DACR, Vol = 0x0(max) ~ 0x7(min)
	{0x23,0x070},	// 
	{0x24,0x070},	// 
	{0x0C,0x000},	// Bass => 0xf(Bypass) ~ 0x0(MAX) 
	{0x0D,0x00e},	// Treble => 0xf(Bypass) ~ 0x0(MAX)
	{0x10,0x00f},	// 3D Effect => off
	{0x2A,0x07f},	// MONO Out OFF
	{0x18,0x010},	// ROUT2(SP) Invert, OUT3(HP-GND)
	{0x11,0x07b},	// ALC1 => Set Default
	{0x12,0x000},	// ALC2 => Set Default
	{0x13,0x032},	// ALC3 => Set Default
	{0x14,0x000},	// Noise Gate => Set Default		
	{0x15,0x0c3},	// ADCL => Set Default
	{0x16,0x0c3},	// ADCR => Set Default
	{0x17,0x0c0},	// Set Default
	{0x1f,0x000},	// Set Default
	{0x20,0x000},	// Set Default
	{0x21,0x000},	// Set Default	
	{0x26,0x000},	// 
	{0x27,0x000},	// 
	{0x05,0x04},  	// DAC Soft Mute On
};

void WM8973_Init(void)
{
	int i;
	
	for(i=0; i<(sizeof(WM8973_INIT)/sizeof(WM8973_INIT[0])); i++)
	{
		WM8973_IIC_Write(WM8973_INIT[i][0], WM8973_INIT[i][1]);
	}
}

void Sound_Control_Output_Select(int mode)
{
	// [0]: All Off, [1]: SPKR, [2]:H-P, [3]: BOTH
	static unsigned short out_mode[4] = {0, 0x198, 0x1e2, 0x1fe};

	WM8973_IIC_Write(0x1A,out_mode[mode]);
}

void Sound_Control_Headphone_Volume(int vol)
{
	// 0:Mute, 1:Min, 9:Max
	if(vol == 0) 
	{
		WM8973_IIC_Write(0x02,0x00);	// LOUT1(HP) Volume => max (0x7f~0x60, 0x00: mute)
		WM8973_IIC_Write(0x03,0x00);	// ROUT1(HP) Volume => max (0x7f~0x60, 0x00: mute)	
		WM8973_IIC_Write(0x0a,0x100);	// LDAC MUTE
		WM8973_IIC_Write(0x0b,0x100);	// RDAC MUTE
	}	

	else 
	{
		WM8973_IIC_Write(0x0a,0x1FF);	// LDAC 0db
		WM8973_IIC_Write(0x0b,0x1FF);	// RDAC 0db
		WM8973_IIC_Write(0x02,(0x060+(0x7f-0x60)/9*vol));	// LOUT1(HP) Volume => max (0x7f~0x60, 0x00: mute)
		WM8973_IIC_Write(0x03,(0x160+(0x7f-0x60)/9*vol));	// ROUT1(HP) Volume => max (0x7f~0x60, 0x00: mute)	
	}
}

void Sound_Control_Speaker_Volume(int vol)
{
	// 0:Mute, 1:Min, 9:Max
	if(vol == 0) 
	{
		WM8973_IIC_Write(0x28,0x00);	// LOUT2(SP) Volume => max (0x7f~0x60, 0x00: mute)
		WM8973_IIC_Write(0x29,0x00);	// ROUT2(SP) Volume => max (0x7f~0x60, 0x00: mute)
		WM8973_IIC_Write(0x0a,0x100);	// LDAC MUTE
		WM8973_IIC_Write(0x0b,0x100);	// RDAC MUTE
	}	

	else 
	{
		WM8973_IIC_Write(0x0a,0x1FF);	// LDAC 0db
		WM8973_IIC_Write(0x0b,0x1FF);	// RDAC 0db
		WM8973_IIC_Write(0x28,(0x060+(0x7f-0x60)/9*vol));	// LOUT1(HP) Volume => max (0x7f~0x60, 0x00: mute)
		WM8973_IIC_Write(0x29,(0x160+(0x7f-0x60)/9*vol));	// ROUT1(HP) Volume => max (0x7f~0x60, 0x00: mute)	
	}
}

void Sound_Control_Soft_Mute(int mute)
{
	if(mute)
	{
		WM8973_IIC_Write(0x05,0x04);
	}
	else
	{
		WM8973_IIC_Write(0x05,0x00);
	}
}

void Sound_Set_Sampling_Rate(int rate)
{
	switch(rate)
	{
		case 44100 : // fs=44.1KHz => CLK = 16.9344MHz(384fs)
			    	 rIISPSR = (2<<5) + 2; // PCLK/3 = 16.6MHz
			    	 break;
		case 32000 : // fs=32.0KHz => CLK = 12.2880MHz(384fs)
			    	 rIISPSR = (3<<5) + 3; // PCLK/4 = 12.5MHz 
			    	 break;
		case 22050 : // fs=22.05KHz => CLK = 8.46728MHz(384fs)
			    	 rIISPSR = (5<<5) + 5; // PCLK/6 = 8.3MHz 
			    	 break;
		case 11025 : // fs=11.025KHz => CLK = 4.2336MHz(384fs)
			    	 rIISPSR = (10<<5) + 10; // PCLK/11 = 4.5454MHz
			    	 break;
		case 8000 : // fs=8.0KHz => CLK = 3.072MHz(384fs)
			    	 rIISPSR = (15<<5) + 15; // PCLK/16 = 3.125.6MHz
			    	 break;
		default : return;			    	 
	}
}

void Sound_Set_Tempo(int up)
{
	int ct = Macro_Extract_Area(rIISPSR, 0x1f, 0);
	
	if(up)
	{
		ct--;
		if(ct == -1) ct = 0;
	}
	
	else
	{
		ct++;
		if(ct == 32) ct=31;
	}
	
	rIISPSR = (ct<<5) + ct;
	Uart_Printf("Step=[%d], Sampling Rate=%.4fKHz\n", ct, (PCLK/1000.)/(ct+1.)/384.);
}	

void Sound_Play_Pause(int pause)
{
	if(pause) rIISCON |= (1<<3);
	else rIISCON &=~ (1<<3);
}

void Sound_Stop_Sound(void)
{
	Sound_Control_Soft_Mute(1);	
	DMA_Stop(2);
	rIISFCON = 0;
	rIISCON = 0;
}

void Sound_Init(void)
{
   	rGPEUP  = (rGPEUP  & ~(0x1f))  | 0x1f;
  	rGPECON = (rGPECON & ~(0x3ff)) | 0x2aa;

	WM8973_Init();
	Sound_Stop_Sound();
}

void Sound_IIS_Start(void)
{
	Macro_Set_Bit(rIISCON, 0);
}

void Sound_Set_Mode(int tx_rx_mode, int bps)
{
	rIISCON = (1<<1);
	rIISMOD = (1<<2)|(1<<0);
    rIISFCON = 0;

	if(tx_rx_mode == 1)
	{
		Macro_Set_Bit(rIISCON, 4);
		Macro_Clear_Bit(rIISCON, 2);
		Macro_Write_Block(rIISMOD, 0x3, 1, 6);
		Macro_Set_Bit(rIISFCON, 14);
		Macro_Set_Bit(rIISFCON, 12);
	}

	else if(tx_rx_mode == 2)
	{
		Macro_Set_Bit(rIISCON, 5);
		Macro_Clear_Bit(rIISCON, 3);
		Macro_Write_Block(rIISMOD, 0x3, 2, 6);
		Macro_Set_Bit(rIISFCON, 15);
		Macro_Set_Bit(rIISFCON, 13);
	}

	else if(tx_rx_mode == 3)
	{
		Macro_Set_Area(rIISCON, 0x3, 4);
		Macro_Clear_Area(rIISCON, 0x3, 2);
		Macro_Write_Block(rIISMOD, 0x3, 3, 6);
		Macro_Set_Area(rIISFCON, 0xf, 12);
	}

	if(bps == 8)
	{
		Macro_Clear_Bit(rIISMOD, 3);
	}

	else if(bps == 16)
	{
		Macro_Set_Bit(rIISMOD, 3);
	}
}

void Sound_Get_WAV_Info(IIS_WAV * sound, void * p)
{
	unsigned char * q = p;

	sound->Play_sample_freq = *(unsigned int *)&q[24];
	sound->Play_bit_per_sample = *(unsigned short *)&q[34];
	sound->Play_file_size = *(unsigned int *)&q[40];
}
