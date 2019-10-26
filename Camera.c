#include "2440addr.h"
#include "option.h"
#include "macro.h"
#include "device_driver.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define CAM_SRC_HSIZE	320
#define CAM_SRC_VSIZE	240

#define PRE_HSIZE		320
#define PRE_VSIZE		240

void CAM_IIC_Set_Output(void);
void CAM_IIC_Set_Input(void);
void CAM_IIC_Out_Clock(int d);
void CAM_IIC_Out_Data(int d);
int CAM_IIC_In_Data(void);
void CAM_IIC_Delay(void);
void CAM_IIC_Start(void);
void CAM_IIC_Stop(void);
void CAM_IIC_Out_Bit(int d);
int CAM_IIC_In_Bit(void);
void CAM_IIC_Out_Byte(int d);
int CAM_IIC_In_Byte(void);
int CAM_IIC_Read(int add);
void CAM_IIC_Write(int add,int data);

void CAM_IIC_Set_Output(void)
{
	rGPCCON = (rGPCCON & ~((unsigned int) 0xf<<22)) | ((unsigned int) 0x5<<22);
	rGPCUP  = (rGPCUP  & ~(0x3<<11)) | (0x0<<11);
}

void CAM_IIC_Set_Input(void)
{
	rGPCCON = (rGPCCON & ~((unsigned int) 0xf<<22)) | ((unsigned int) 0x4<<22);
	rGPCUP  = (rGPCUP  & ~(0x3<<11)) | (0x0<<11);
}

void CAM_IIC_Out_Clock(int d)
{
	rGPCDAT = (rGPCDAT & ~(0x1<<12)) | ((d & 0x1)<<12);
}

void CAM_IIC_Out_Data(int d)
{
	rGPCDAT = (rGPCDAT & ~(0x1<<11)) | ((d & 0x1)<<11);
}

int CAM_IIC_In_Data(void)
{
	if(	(rGPCDAT & (0x1<<11)) != 0) return 1;
	return 0;
}

void CAM_IIC_Delay(void)
{
	int i;
	for(i=0;i<2000;i++);
}

void CAM_IIC_Start(void)
{
	CAM_IIC_Set_Output();
	CAM_IIC_Out_Clock(1);
	CAM_IIC_Delay();
	CAM_IIC_Out_Data(1);
	CAM_IIC_Delay();
	CAM_IIC_Out_Data(0);
	CAM_IIC_Delay();
}

void CAM_IIC_Stop(void)
{
	CAM_IIC_Out_Clock(0);
	CAM_IIC_Delay();
	CAM_IIC_Set_Output();
	CAM_IIC_Out_Data(0);
	CAM_IIC_Delay();
	CAM_IIC_Out_Clock(1);
	CAM_IIC_Delay();
	CAM_IIC_Out_Data(1);
	CAM_IIC_Delay();
}

void CAM_IIC_Out_Bit(int d)
{
	CAM_IIC_Out_Clock(0);
	CAM_IIC_Delay();
	CAM_IIC_Set_Output();
	CAM_IIC_Out_Data(d);
	CAM_IIC_Delay();
	CAM_IIC_Out_Clock(1);
	CAM_IIC_Delay();
}

int CAM_IIC_In_Bit(void)
{
	int d;
	CAM_IIC_Out_Clock(0);
	CAM_IIC_Delay();
	CAM_IIC_Set_Input();
	CAM_IIC_Out_Clock(1);
	CAM_IIC_Delay();
	d=CAM_IIC_In_Data();
	return(d);
}

void CAM_IIC_Out_Byte(int d)
{
    int i;

	for (i=0; i<8; i++)
	{
		if ((d&0x80)==0)
		{
			CAM_IIC_Out_Bit(0);
		}
		else
		{
			CAM_IIC_Out_Bit(1);
		}
		d=d<<1;
	}
}

int CAM_IIC_In_Byte(void)
{
    int d,i,d0,dd[8];

	d0=0;
	for (i=0; i<8; i++)
	{
		d=CAM_IIC_In_Bit();
		dd[i]=d;
		d0=d0*2+d;
	}
   	return(d0);
}

int CAM_IIC_Read(int add)
{
	int i;

	CAM_IIC_Start();
	CAM_IIC_Out_Byte(0xDC); 	//Device Address + Write(0)
	i=CAM_IIC_In_Bit(); 		//ack

	CAM_IIC_Out_Byte(add);
	CAM_IIC_In_Bit(); 			//ack
	CAM_IIC_Stop();

	CAM_IIC_Start();
	CAM_IIC_Out_Byte(0xDD); 	//Device Address + Read(1)
	CAM_IIC_In_Bit(); 			//ack

	i=CAM_IIC_In_Byte();

	CAM_IIC_Out_Bit(1); 		//ack no more data
	CAM_IIC_Stop();

	return(i);
}

void CAM_IIC_Write(int add,int data)
{
	int i;

	CAM_IIC_Start();

	CAM_IIC_Out_Byte(0xDC); 	//Device Address + Write(0)
	i=CAM_IIC_In_Bit(); 		//ack

	CAM_IIC_Out_Byte(add);
	CAM_IIC_In_Bit(); 			//ack

	CAM_IIC_Out_Byte(data);
	CAM_IIC_In_Bit(); 			//ack

	CAM_IIC_Stop();
}

void CAM_Port_Init(void)
{
	// Mack UCLK => 48MHz, CAMCLKOUT => 24MHz
	rLOCKTIME = 0xffffffff;
	rUPLLCON = 0x3c042;
	rLOCKTIME = 0xffffffff;
	rCAMDIVN = (0<<12)+(0<<9)+(0<<8)+(1<<4)+(0<<0);

	// CAM_DATA[7-0] = GPJ[7:0]
	// CAMPCLK = GPJ8, CAMVSYNC = GPJ9, CAMHREF = GPJ10, CAMCLKOUT = GPJ11, CAMRESET = GPJ12, special
	Macro_Write_Block(rGPJCON, 0x3ffffff, 0x2aaaaaa, 0);
}

void CAM_If_Reset(void)
{
	int i;

	Macro_Set_Bit(rCIGCTRL, 31);
	for(i=0; i<0x100000; i++);//need 10ms
	Macro_Clear_Bit(rCIGCTRL, 31);
}

void CAM_Sensor_Enable(int en)
{
	int i;

	if(en)
	{
		Macro_Clear_Bit(rCIGCTRL, 30);
		for(i=0; i<0x100000; i++);
		Macro_Set_Bit(rCIGCTRL, 30);
	}
	else
	{
		Macro_Clear_Bit(rCIGCTRL, 30);
		for(i=0; i<0x100000; i++);
	}
}

void CalculatePrescalerRatioShift(U32 SrcSize, U32 DstSize, U32 *ratio,U32 *shift)
{
	if(SrcSize >= (64*DstSize))
	{
		while(1);
	}
	else if(SrcSize >= (32*DstSize))
	{
		*ratio = 32;
		*shift = 5;
	}
	else if(SrcSize >= (16*DstSize))
	{
		*ratio = 16;
		*shift = 4;
	}
	else if(SrcSize >= (8*DstSize))
	{
		*ratio = 8;
		*shift = 3;
	}
	else if(SrcSize >= (4*DstSize))
	{
		*ratio = 4;
		*shift = 2;
	}
	else if(SrcSize >= (2*DstSize))
	{
		*ratio = 2;
		*shift = 1;
	}
	else
	{
		*ratio = 1;
		*shift = 0;
	}    	
}

static int Capture_status = 0;	// 0: Ready, 1: During Capture, 2: Capture Finished
char * Cam_Frame_Buf;
int Width = 320, Height = 240;
int LineSize, FrameSize;

void CAM_Init(void)
{
	U32 pre_hor_ratio, h_shift, main_hor_ration;
	U32 pre_ver_ratio, v_shitf, main_ver_ration;
	
	CalculatePrescalerRatioShift(CAM_SRC_HSIZE, PRE_HSIZE, &pre_hor_ratio, &h_shift);
	CalculatePrescalerRatioShift(CAM_SRC_VSIZE, PRE_VSIZE, &pre_ver_ratio, &v_shitf);

	main_hor_ration=(CAM_SRC_HSIZE<<8)/(PRE_HSIZE<<h_shift);
	main_ver_ration=(CAM_SRC_VSIZE<<8)/(PRE_VSIZE<<v_shitf);

	rCISRCFMT = ((U32)1<<31) + (0<<30) + (2<<14) + (CAM_SRC_HSIZE<<16) + (CAM_SRC_VSIZE<<0);
	
	rCIWDOFST = 0x00;

	rCICOTRGFMT = (PRE_HSIZE<<16) + (PRE_VSIZE<<0);
	// x-mirror(3), y-mirror(2), 180(1)
	rCIPRTRGFMT = (PRE_HSIZE<<16) + (PRE_VSIZE<<0) + (3<<14);

	rCICOCTRL = (4<<19) + (4<<14) + (4<<9) + (4<<4);
	rCIPRCTRL = (4<<19) + (4<<14);

	rCICOSCPRERATIO = (1<<28)|(1<<16)|(1);
	rCIPRSCPRERATIO = (1<<28)|(1<<16)|(1);

	rCICOSCPREDST = ((CAM_SRC_HSIZE/pre_hor_ratio)<<16) + ((CAM_SRC_VSIZE/pre_ver_ratio)<<0);
	rCIPRSCPREDST = ((CAM_SRC_HSIZE/pre_hor_ratio)<<16) + ((CAM_SRC_VSIZE/pre_ver_ratio)<<0);

	rCICOSCCTRL = ((U32)0<<31) + (1<<29) + (1<<15) + (main_hor_ration<<16) + (main_ver_ration<<0); // ??

	rCIPRSCCTRL = ((U32)1<<31) + (0<<30) + (1<<28) + (main_hor_ration<<16) + (0<<15)+(main_ver_ration<<0); // ??

	rCICOTAREA = PRE_HSIZE*PRE_VSIZE;
	rCIPRTAREA = PRE_HSIZE*PRE_VSIZE;
}

static const int CAM_MODULE_INIT[ ][2] =
{
	{0x00,0x10},
	{0x01,0x30},
	{0x02,0xFF},
	{0x03,0xFF},
	{0x04,0x02},
	{0x05,0x80},
	{0x06,0x01},
	{0x07,0xE0},
	{0x08,0x00},
	{0x09,0xA0},
	{0x0A,0x00},
	{0x0B,0x78},
	{0x0C,0x01},
	{0x0D,0x40},
	{0x0E,0x00},
	{0x0F,0xF0},
	{0x10,0xFF},
	{0x11,0xFF},
	{0x12,0x03},
	{0x13,0x02},
	{0x14,0xFF},
	{0x15,0x03},
	{0x16,0x58},
	{0x17,0x40},
	{0x18,0x64},
	{0x19,0x40},
	{0x1A,0x01},
	{0x1B,0x7B},
	{0x1C,0x80},
	{0x1D,0x1B},
	{0x1E,0x83},
	{0x1F,0x00},
	{0x20,0x04},
	{0x21,0x90},
	{0x22,0x00},
	{0x23,0x00},
	{0x24,0xB0},
	{0x25,0x00},
	{0x26,0x94},
	{0x27,0x58},
	{0x28,0x00},
	{0x29,0x00},
	{0x2A,0x00},
	{0x2B,0x03},
	{0x2C,0x00},
	{0x2D,0x1C},
	{0x2E,0x46},
	{0x2F,0x75},
	{0x30,0x96},
	{0x31,0xB1},
	{0x32,0xC8},
	{0x33,0xDF},
	{0x34,0xFF},
	{0x35,0x06},
	{0x36,0x1E},
	{0x37,0x12},
	{0x38,0x02},
	{0x39,0xAA},
	{0x3A,0x53},
	{0x3B,0x37},
	{0x3C,0xD5},
	{0x3D,0xF2},
	{0x3E,0x0B},
	{0x3F,0x04},
	{0x40,0x90},
	{0x41,0x03},
	{0x42,0x14},
	{0x43,0x30},
	{0x44,0x46},
	{0x45,0xFF},
	{0x46,0xFF},
	{0x47,0x80},
	{0x48,0x80},
	{0x49,0x40},
	{0x4A,0x42},
	{0x4B,0x02},
	{0x4C,0xEF},
	{0x4D,0x08},
	{0x4E,0xCD},
	{0x4F,0x00},
	{0x50,0xD5},
	{0x51,0x00},
	{0x52,0xA0},
	{0x53,0x01},
	{0x54,0xAA},
	{0x55,0x01},
	{0x56,0x40},
	{0x57,0x00},
	{0x58,0x01},
	{0x59,0x1B},
	{0x5A,0x40},
	{0x5B,0x00},
	{0x5C,0x30},
	{0x5D,0x03},
	{0x5E,0x00},
	{0x5F,0x00},
	{0x60,0x80},
	{0x61,0xFF},
	{0x62,0x40},
	{0x63,0x38},
	{0x64,0x38},
	{0x65,0x37},
	{0x66,0x55},
	{0x67,0x7D},
	{0x68,0x9B},
	{0x69,0x00},
	{0x6A,0xFF},
	{0x6B,0x08},
	{0x6C,0xFF},
	{0x6D,0x00},
	{0x6E,0x00},
	{0x6F,0x01},
	{0x70,0x00},
	{0x71,0x80},
	{0x72,0x10},
	{0x73,0x10},
	{0x74,0x80},
	{0x75,0xEB},
	{0x76,0x03},
	{0x77,0x02},
	{0x78,0x14},
	{0x79,0x00},
	{0x7A,0x92},
	{0x7B,0x79},
	{0x7C,0x79},
	{0x7D,0x79},
	{0x7E,0x03},
	{0x7F,0x02}
};

void CAM_Module_Init(void)
{
	int i;

	for(i=0; i<(sizeof(CAM_MODULE_INIT)/sizeof(CAM_MODULE_INIT[0])); i++)
	{
		CAM_IIC_Write(CAM_MODULE_INIT[i][0], CAM_MODULE_INIT[i][1]);
	}
}

void CAM_Camera_Init(void)
{
	CAM_Port_Init();
	CAM_Sensor_Enable(0);
	CAM_If_Reset();
	CAM_Init();
}

static unsigned int Cam_Buf[5];
const static unsigned int Cam_Frame_Number[4] = {2,3,0,1};

int CAM_Capture_Run(U32 run, int width, int height)
{
	if(run)
	{
		CAM_Sensor_Enable(1);
		CAM_Module_Init();

		if((width ==320 && height == 240))
		{
			CAM_IIC_Write(0x0C,0x01);
			CAM_IIC_Write(0x0D,0x40);
			CAM_IIC_Write(0x0E,0x00);
			CAM_IIC_Write(0x0F,0xF0);
		}
		else
		{
			return 0;
		}

		LineSize = width*sizeof(unsigned short);
		FrameSize = LineSize*height;

		rCIPRCLRSA1 = Cam_Buf[0] = (U32)Cam_Frame_Buf;
		rCIPRCLRSA2 = Cam_Buf[1] = (U32)(Cam_Frame_Buf+FrameSize);
		rCIPRCLRSA3 = Cam_Buf[2] = (U32)(Cam_Frame_Buf+FrameSize*2);
		rCIPRCLRSA4 = Cam_Buf[3] = (U32)(Cam_Frame_Buf+FrameSize*3);
		Cam_Buf[4] = (U32)(Cam_Frame_Buf+FrameSize*4);
		
		Macro_Set_Bit(rSUBSRCPND, SUB_CAM_P);
		rCIPRSCCTRL |= (1<<15);
		rCIIMGCPT |= ((unsigned int)1<<31)+(0<<30)+(1<<29);
		rCIWDOFST |= (1<<30)|(0xf<<12);
		rCIWDOFST &= ~((1<<30)|(0xf<<12));
		Capture_status = 1;
	}
	else
	{
		rCIPRSCCTRL &= ~(1<<15);
		rCIIMGCPT = 0;
		CAM_Sensor_Enable(0);
		Capture_status = 0;
	}

	return 1;
}

void CAM_Capture_Stop(void)
{
	rCIPRSCCTRL &= ~(1<<15);
	rCIIMGCPT = 0;

	rCIWDOFST |= (1<<30)|(0xf<<12);
	rCIWDOFST &= ~((1<<30)|(0xf<<12));
	
	CAM_Sensor_Enable(0);
}

void CAM_XY_Flip(int x, int y)
{
	int i;

	if(x && y) i = 0xC0;
	else if(x) i = 0x80;
	else if(y) i = 0x40;
	else i = 0x00;

	CAM_IIC_Write(0x1E, i);
}	

void CAM_Capture_Restart(void)
{
	rCIPRSTATUS |= (1<<21);
	rCIPRSCCTRL |= (1<<15);
	rCIIMGCPT |= ((unsigned int)1<<31)+(0<<30)+(1<<29);
}

void CAM_Capture_Pause(void)
{
	rCIPRSTATUS &= ~(1<<21);
	rCIPRSCCTRL &= ~(1<<15);
	rCIIMGCPT = 0;

	rCIWDOFST |= (1<<30)|(0xf<<12);
	rCIWDOFST &= ~((1<<30)|(0xf<<12));

}

void * CAM_Check_Image_Ready(void)
{
	int frameCnt;
	// Return : Address of buffer, if NULL => Not Ready
	// Use only polling scheme
	if(Macro_Check_Bit_Set(rSUBSRCPND, SUB_CAM_P))
	{
		Macro_Set_Bit(rSUBSRCPND, SUB_CAM_P);	
	
		rCIWDOFST |= (1<<30)|(0xf<<12);   
		rCIWDOFST &= ~((1<<30)|(0xf<<12));

		frameCnt = (rCIPRSTATUS>>26)&0x03;
		if(frameCnt>1) frameCnt-=2;
		else frameCnt+=2;
		memcpy((void *)(rCIPRCLRSA1+PRE_HSIZE*PRE_VSIZE*sizeof(unsigned short)*4), (void *)(rCIPRCLRSA1+PRE_HSIZE*PRE_VSIZE*sizeof(unsigned short)*frameCnt), PRE_HSIZE*PRE_VSIZE*sizeof(unsigned short));
		return (void *)(rCIPRCLRSA1+PRE_HSIZE*PRE_VSIZE*sizeof(unsigned short)*4);
	}
	else
	{
		return (void *)0;
	}
}

void * CAM_Get_Image_Address(void)
{
	int frameCnt;
	unsigned int temp1, temp2;

	frameCnt = (rCIPRSTATUS>>26)&0x03;

	temp1 = Cam_Buf[Cam_Frame_Number[frameCnt]];
	temp2 = Cam_Buf[4];
	Cam_Buf[4] = temp1;
	Cam_Buf[Cam_Frame_Number[frameCnt]] = temp2;

	switch(frameCnt)
	{
		case 0: rCIPRCLRSA3 = temp2; break;
		case 1: rCIPRCLRSA4 = temp2; break;
		case 2: rCIPRCLRSA1 = temp2; break;
		case 3: rCIPRCLRSA2 = temp2; break;
	}

	return (void *)temp1;
}

void CAM_Interrupt_Enable(int en)
{
	// Use Only Interrupt Scheme
	if(en)
	{
		rSUBSRCPND = 1<<12;
		rSRCPND = 1<<6;
		rINTPND = 1<<6;
		Macro_Clear_Bit(rINTSUBMSK, 12);
		Macro_Clear_Bit(rINTMSK,6);
	}
	else
	{
		rINTSUBMSK |= (BIT_SUB_CAM_P); //INT CAMERA Port A ENABLE
		rINTMSK |= (BIT_CAM);
	}
}

int CAM_Get_Capture_Status(void)
{
	// 0: ready, 1: during capture, 2: capture finished
	// 함수가 호출되어 2가 리턴되면 0상태로 돌아감
	int r = Capture_status;
	
	if(Capture_status == 2)
	{
		Capture_status = 0;
	}

	return r;
}

void CAM_Set_Capture_Status(int state)
{
	Capture_status = state;
}


