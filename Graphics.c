#include "2440addr.h"
#include "option.h"
#include "macro.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "device_driver.h"

//////////////////// GBOX LCD SPI Control ////////////////////////

#define LCD_SPI0_CLK		(20000000)
#define LCD_SPI0_ENABLE() 	Macro_Clear_Bit(rGPGDAT, 2)
#define LCD_SPI0_DISABLE()	Macro_Set_Bit(rGPGDAT, 2)
#define LCD_WRITE			(0<<0)
#define LCD_REG				(0<<1)
#define LCD_DATA			(1<<1)
#define LCD_ID				(0x1c<<2)
#define SPI0_WAIT_SEND()	while(!(rSPSTA0 & 1))

void SPI0_LCD_Init(void);
void SPI0_LCD_Write(int reg, int data);

void SPI0_LCD_Init(void)
{
	double d;

	Macro_Write_Block(rGPECON, 0xf, 0xa, 24);
	Macro_Write_Block(rGPGCON, 0x3, 0x1, 4);
	LCD_SPI0_DISABLE();

	rSPCON0 = (0<<5) + (1<<4) + (1<<3) + (0<<2) + (0<<1) + (0<<0);
	d = PCLK/(2 * LCD_SPI0_CLK);
	rSPPRE0 = (d == 0)? ((int)d -1) : ((int)d);
	rSPPIN0 = (0<<2) + (0<<0);
}

void SPI0_LCD_Write(int reg, int data)
{
	LCD_SPI0_ENABLE();
	rSPTDAT0 = LCD_ID + LCD_REG + LCD_WRITE;
	SPI0_WAIT_SEND();
	rSPTDAT0 = (reg>>8) & 0xff;
	SPI0_WAIT_SEND();
	rSPTDAT0 = reg & 0xff;
	SPI0_WAIT_SEND();
	LCD_SPI0_DISABLE();

	LCD_SPI0_ENABLE();
	rSPTDAT0 = LCD_ID + LCD_DATA + LCD_WRITE;
	SPI0_WAIT_SEND();
	rSPTDAT0 = (data>>8) & 0xff;
	SPI0_WAIT_SEND();
	rSPTDAT0 = data & 0xff;
	SPI0_WAIT_SEND();
	LCD_SPI0_DISABLE();
}

//////////////////// GBOX LCD Customization ////////////////////////
void Custom_Layer_Draw_Display_Init(void)
{
	Lcd_Select_Buffer_Mode(LAYER_MODE);
}




//////////////////// GBOX LCD Configuration ////////////////////////

#define rLPCSEL (*(volatile unsigned int *)0x4d000060)

#define HOZVAL			(LCD_XSIZE-1)
#define LINEVAL			(LCD_YSIZE-1)

#define VBPD_030561		((15-1)&0xff)
#define VFPD_030561		((4-1)&0xff)
#define VSPW_030561		((3-1)&0x3f)
#define HBPD_030561		((114-1)&0x7f)
#define HFPD_030561		((60-1)&0xff)
#define HSPW_030561 	((90-1)&0xff)
#define CLKVAL			1

void Lcd_Init(void);
void Lcd_Port_Init(void);
void Lcd_Power_Enable(int en);
void Lcd_Reset(int reset);
void Lcd_Envid_On_Off(int onoff);
void Lcd_Set_Address(unsigned int fp);

void Lcd_Reset(int reset)
{
   if(reset) Macro_Clear_Bit(rGPCDAT, 0);
   else Macro_Set_Bit(rGPCDAT, 0);
}

void Lcd_Port_Init(void)
{
	Macro_Write_Block(rGPCCON, 0x3, 0x1, 0);
	Macro_Clear_Bit(rGPCDAT, 0);
	Macro_Write_Block(rGPGCON, 0x3, 0x3, 8);
	rGPCUP=0xffffffff;
	Macro_Write_Block(rGPCCON, 0xff, 0xaa, 2);
	rGPDUP=0xffffffff;
	Macro_Write_Block(rGPDCON, 0xfff, 0xaaa, 20);
}

void Lcd_Envid_On_Off(int onoff)
{
	(onoff) ? (rLCDCON1 |= 1) : (rLCDCON1 &= ~0x1);
}

void Lcd_Power_Enable(int en)
{
    rLCDCON5= (rLCDCON5&(~(1<<3)))|(en<<3);
}

void Lcd_Init(void)
{
	rLCDCON1=(CLKVAL<<8)|(3<<5)|(13<<1)|0;
	rLCDCON2=(VBPD_030561<<24)|((LINEVAL)<<14)|(VFPD_030561<<6)|(VSPW_030561);
	rLCDCON3=(HBPD_030561<<19)|(HOZVAL<<8)|(HFPD_030561);
	rLCDCON4=(HSPW_030561);
	rLCDCON5=(0<<12)|(1<<10)|(1<<9)|(1<<8);
	rLCDINTMSK|=(3);
	rLPCSEL &=(~7);
}

//////////////////// GBOX Graphic Frame Buffer Configuration ////////////////////////

unsigned int Frame_Mode = LAYER_MODE;
unsigned int Virtual_X = LCD_XSIZE;
unsigned int Virtual_Y = LCD_YSIZE;
unsigned int color_flag = 0;

typedef unsigned int (* LAYER_FRAME)[LCD_XSIZE];
typedef unsigned int (* VIRTUAL_FRAME)[LCD_XSIZE*2];

LAYER_FRAME Fb_Layer = (LAYER_FRAME)LCD_FB00;
VIRTUAL_FRAME Fb_Frame = (VIRTUAL_FRAME)LCD_FB0;

unsigned int Selected_frame = 0;
unsigned int Display_frame = 0;

#define OFFSIZE		((Virtual_X	- LCD_XSIZE) << 1)
#define PAGEWIDTH	(LCD_XSIZE << 1)

void Lcd_Set_Address(unsigned int fp)
{
	rLCDSADDR1 = fp >> 1;
	rLCDSADDR2 = (rLCDSADDR1 & 0x1fffff) + (PAGEWIDTH+OFFSIZE) * LCD_YSIZE;
	rLCDSADDR3 = (OFFSIZE << 11) + PAGEWIDTH;
}

void Lcd_Select_Buffer_Mode(unsigned int mode)
{
	if(mode == LAYER_MODE)
	{
		Virtual_X = LCD_XSIZE;
		Virtual_Y = LCD_YSIZE;
	 	Frame_Mode = LAYER_MODE;
		Lcd_Set_Address(LCD_FB00);
	}
	else
	{
		Virtual_X = LCD_XSIZE*2;
		Virtual_Y = LCD_YSIZE*2;
	 	Frame_Mode = VIRTUAL_MODE;
		Lcd_Set_Address(LCD_FB0);
	}
}

static int Back_x = 0, Back_y = 0;

void Lcd_Set_Virtual_Display_Start(int x, int y)
{
	Lcd_Set_Address((Display_frame ? LCD_FB1 : LCD_FB0) + ((y * Virtual_X) + (x*3)) * 4);

	Back_x = x;
	Back_y = y;
}

void Lcd_Select_Draw_Frame_Buffer(unsigned int id)
{
	if(Frame_Mode == LAYER_MODE)
	{
	   	Fb_Layer = (LAYER_FRAME)(LCD_FB00+LCD_SIZE*id);
	   	Selected_frame = id;
	}
	else
	{
		Fb_Frame = (VIRTUAL_FRAME)(id?LCD_FB1:LCD_FB0);
		Selected_frame = id;
	}
}

void Lcd_Select_Display_Frame_Buffer(unsigned int id)
{
	if(Frame_Mode == LAYER_MODE)
	{
	   	Lcd_Set_Address(LCD_FB0+LCD_SIZE*id);
	   	Display_frame = id;
	}
	else
	{
		Lcd_Set_Virtual_Display_Start(Back_x, Back_y);
		Display_frame = id;
	}
}

//////////////////// GBOX Graphic Primitive Functions ////////////////////////

void Lcd_Graphic_Init(void)
{
	Lcd_Port_Init();
	Lcd_Init();
	Lcd_Reset(0);
	Lcd_Envid_On_Off(0);

   	Lcd_Set_Address(LCD_FB0);

	SPI0_LCD_Init();
	SPI0_LCD_Write(0x0001,0x7300);
	SPI0_LCD_Write(0x0002,0x0200);
	SPI0_LCD_Write(0x0003,0x6364);
	SPI0_LCD_Write(0x0004,0x04cf);
	SPI0_LCD_Write(0x0005,0xbcd4);
	SPI0_LCD_Write(0x000a,0x4008);
	SPI0_LCD_Write(0x000b,0xd400);
	SPI0_LCD_Write(0x000d,0x3229);
	SPI0_LCD_Write(0x000e,0x3200);
	SPI0_LCD_Write(0x000f,0x0000);
	SPI0_LCD_Write(0x0016,0x9f80);
	SPI0_LCD_Write(0x0017,0x3fff);
	SPI0_LCD_Write(0x001e,0x0052);

	Lcd_Clr_Screen(0);
	Lcd_Envid_On_Off(1);
	Lcd_Power_Enable(1);
}

void Lcd_Control_Brightness(int bright)
{
	int value;

	if(bright > 10) bright = 10;
	else if(bright < 0) bright = 0;

	value = ((bright * 5 + 0x30) << 8) + 0x08;

	SPI0_LCD_Write(0x000a,value);
}

//////////////////// GBOX Graphic Primitive Functions ////////////////////////

unsigned int Bit_per_pixel = BPP_16;

unsigned int Trans_mode = 0;
unsigned int Shape_mode = 0;
unsigned int Shape_mode_color = 0;
unsigned int Shape_back_image_mode = 0;
unsigned short (* Back_Image_Layer)[320] = (void *)0;
unsigned short (* Back_Image_Frame)[640] = (void *)0;

void Lcd_Wait_Blank(void)
{
	while(rLCDCON5 & 1<<16);
}

void Lcd_Set_Trans_Mode(int mode)
{
	if(mode) Trans_mode = 1;
	else Trans_mode = 0;
}

void Custom_Lcd_Set_Shape_Mode(int mode, int color)
{

}

void Lcd_Set_Shape_Mode(int mode, int color)
{
	if(mode)
	{
		Shape_mode = 1;
		Shape_mode_color = color;
	}
	else
	{
		Shape_mode = 0;
	}
}

void Lcd_Set_Shape_Back_Image_Mode(int mode, void * img)
{
	if(mode)
	{
		Shape_back_image_mode = 1;

		if(Frame_Mode == LAYER_MODE)
		{
			Back_Image_Layer = img;
		}
		else
		{
			Back_Image_Frame = img;
		}
	}
	else
	{
		Shape_back_image_mode = 0;
	}
}

void Lcd_Put_Pixel_Area(int x, int y, int xsize, int ysize, int color)
{
	int i,j;

	for(i=x; i<x+xsize; i++)
	{
		for(j=y; j<y+ysize; j++)
		{
			Lcd_Put_Pixel(i, j, color);
		}
	}
}

void Lcd_Put_Pixel(int x,int y,int color)
{
	unsigned int sr, sg, sb, mask, r, g, b;

	if(Bit_per_pixel == BPP_16)
	{
		sr = 8, sg = 13, sb = 18;
		mask = 0x00f80000;
	}
	else
	{
		sr = 0, sg = 8, sb = 16;
		mask = 0x00ff0000;
	}
	
	if(!Shape_mode || ((Shape_mode_color & 0xfffe)!= (color & 0xfffe)))
	{
		r = (color << sr) & mask;
		g = (color << sg) & mask;
		b = (color << sb) & mask;

		if(Frame_Mode == LAYER_MODE)
		{
			if(!Trans_mode)
			{
				Fb_Layer[y][3*x]   = r;
				Fb_Layer[y][3*x+1] = g;
				Fb_Layer[y][3*x+2] = b;
			}
			else
			{
				Fb_Layer[y][3*x]   |= r;
				Fb_Layer[y][3*x+1] |= g;
				Fb_Layer[y][3*x+2] |= b;
			}
		}
		else
		{
			if(!Trans_mode)
			{
				Fb_Frame[y][3*x]   = r;
				Fb_Frame[y][3*x+1] = g;
				Fb_Frame[y][3*x+2] = b;
			}
			else
			{
				Fb_Frame[y][3*x]   |= r;
				Fb_Frame[y][3*x+1] |= g;
				Fb_Frame[y][3*x+2] |= b;
			}
		}
	}

	else if(Shape_back_image_mode)
	{
		if(Frame_Mode == LAYER_MODE)
		{
			unsigned short pix = Back_Image_Layer[y][x];

			r = (pix << sr) & mask;
			g = (pix << sg) & mask;
			b = (pix << sb) & mask;

			Fb_Layer[y][3*x]   = r;
			Fb_Layer[y][3*x+1] = g;
			Fb_Layer[y][3*x+2] = b;
		}

		else
		{
			unsigned short pix = Back_Image_Frame[y][x];

			r = (pix << sr) & mask;
			g = (pix << sg) & mask;
			b = (pix << sb) & mask;

			Fb_Frame[y][3*x]   = r;
			Fb_Frame[y][3*x+1] = g;
			Fb_Frame[y][3*x+2] = b;
		}
	}
}

int Lcd_Get_Pixel_24bpp(int x,int y)
{
	unsigned int r, g, b;

	if(Frame_Mode == LAYER_MODE)
	{
		r = Fb_Layer[y][3*x] & 0x00ff0000;
		g = Fb_Layer[y][3*x+1] & 0x00ff0000;
		b = Fb_Layer[y][3*x+2] & 0x00ff0000;
	}

	else
	{
		r = Fb_Frame[y][3*x] & 0x00ff0000;
		g = Fb_Frame[y][3*x+1] & 0x00ff0000;
		b = Fb_Frame[y][3*x+2] & 0x00ff0000;
	}

	return r | (g >> 8) | (b >> 16);
}

int Custom_Lcd_Get_Pixel_16bpp(int x, int y)
{
	// if(Bit_per_pixel != BPP_24) return;
	// 24bpp : r | (g >> 8) | (b >> 16)
	// 16bpp : (r >> 8) | (g >> 13) | (b >> 19)
	unsigned int r, g, b;

	if(Frame_Mode == LAYER_MODE)
	{
		r = Fb_Layer[y][3*x] & 0x00f80000;
		g = Fb_Layer[y][3*x+1] & 0x00fc0000;
		b = Fb_Layer[y][3*x+2] & 0x00f80000;
	}

	else
	{
		r = Fb_Frame[y][3*x] & 0x00f80000;
		g = Fb_Frame[y][3*x+1] & 0x00fc0000;
		b = Fb_Frame[y][3*x+2] & 0x00f80000;
	}

	return (r >> 8) | (g >> 13) | (b >> 19);
}

void Lcd_Save_Backup_24bpp(int x, int y, int xsize, int ysize, unsigned int *fp)
{
	int xx, yy;

	for(yy=0;yy<ysize;yy++)
	{
		for(xx=0;xx<xsize;xx++)
		{
			fp[yy*xsize+xx]=Lcd_Get_Pixel_24bpp(xx+x,yy+y);
		}
	}
}

void Lcd_Draw_Backup_24bpp(int x, int y, int xsize, int ysize, unsigned int *fp)
{
	register int xx, yy;
	unsigned int mode;

	mode = Bit_per_pixel;
	Bit_per_pixel= BPP_24;

	for(yy=0;yy<ysize;yy++)
	{
		for(xx=0;xx<xsize;xx++)
		{
			Lcd_Put_Pixel(xx+x,yy+y,(int)fp[yy*xsize+xx]);
		}
	}

	Bit_per_pixel = mode;
}

void Lcd_Draw_BMP_Tile(int x, int y, const unsigned short int *fp, int x1, int y1, int xsize, int ysize)
{
	register int width = fp[0];
	register int xx, yy;

	for(yy=0;yy<ysize;yy++)
	{
		for(xx=0;xx<xsize;xx++)
		{
			Lcd_Put_Pixel(xx+x,yy+y,(int)fp[(yy+y1)*width+(xx+x1)+2]);
		}
	}
}

void Lcd_Draw_Image_Tile(int x, int y, const unsigned short int *fp, int width, int height, int x1, int y1, int xsize, int ysize)
{
	register int xx, yy;

	for(yy=0;yy<ysize;yy++)
	{
		for(xx=0;xx<xsize;xx++)
		{
			Lcd_Put_Pixel(xx+x,yy+y,(int)fp[(yy+y1)*width+(xx+x1)+2]);
		}
	}
}

void Lcd_Clr_Screen(int color)
{
	int i,j;

	for(j=0;j<Virtual_Y;j++)
	{
		for(i=0;i<(Virtual_X/3);i++)
		{
     		Lcd_Put_Pixel(i, j, color);
		}
	}
}

///////////////////////// BMP Display ///////////////////////////////

void Lcd_Draw_BMP_File_24bpp(int x, int y, void *fp)
{
	// 24bpp. 비압축 모드만 지원
	int xx, yy, p;
	unsigned char * t;

	unsigned char * raw;
	unsigned int w;
	unsigned int h;
	unsigned int pad;
	unsigned int mode;

	mode = Bit_per_pixel;
	Bit_per_pixel= BPP_24;

	t = (unsigned char *)((unsigned int)fp + 0xA);
	raw = (unsigned char *)(t[0]+(t[1]<<8)+(t[2]<<16)+(t[3]<<24)+(unsigned int)fp);

	t = (unsigned char *)((unsigned int)fp + 0x12);
	w = (unsigned int)(t[0]+(t[1]<<8)+(t[2]<<16)+(t[3]<<24));

	t = (unsigned char *)((unsigned int)fp + 0x16);
	h = (unsigned int)(t[0]+(t[1]<<8)+(t[2]<<16)+(t[3]<<24));

	pad = (4-(w*3)%4)%4;

	// Uart_Printf("fp=%#x, raw=%#x, w=%d, h=%d\n", fp, raw, w, h);

	for(yy=(h-1);yy>=0;yy--)
	{
		for(xx=0;xx<w;xx++)
		{
			p=(int)((raw[0]<<0)+(raw[1]<<8)+(raw[2]<<16));
			Lcd_Put_Pixel(xx+x,yy+y,p);
			raw += 3;
		}
		raw = (unsigned char *)((unsigned int)raw + pad);
	}

	Bit_per_pixel = mode;
}

void Lcd_Get_Info_BMP(int * x, int  * y, const unsigned short int *fp)
{
	*x =(int)fp[0];
	*y =(int)fp[1];
}

void Lcd_Draw_BMP(int x, int y, const unsigned short int *fp)
{
	register int width = fp[0], height = fp[1];
	register int xx, yy;
	
	for(yy=0;yy<height;yy++)
	{
		for(xx=0;xx<width;xx++)
		{
			Lcd_Put_Pixel(xx+x,yy+y,(int)fp[yy*width+xx+2]);
		} 
	}
}

void Lcd_Draw_Image(int x, int y, const unsigned short int *fp, int width, int height)
{
	// BIN 파일 자체에 해상도 정보가 없는 경우 사용
	register int xx, yy;

	for(yy=0;yy<height;yy++)
	{
		for(xx=0;xx<width;xx++)
		{
			Lcd_Put_Pixel(xx+x,yy+y,(int)fp[yy*width+xx]);
		}
	}
}

//////////////////// GBOX Camera Display Related Library ////////////////////////

extern unsigned short background[240][320];
extern int sticker_flag;
void Lcd_Put_Cam_Pixel(int x,int y,unsigned int color)
{
	// 16bpp(5:6:5) mode => 5:5:5:I format
	int r, g, b;

	r = (color << 8) & (0xf80000);
	g = (color << 13) & (0xfc0000);
	b = (color << 19) & (0xf80000);

	if( sticker_flag == 2 && (r<0xa00000 && g<0xa00000 && b>0x600000) )
	{
		Fb_Layer[y][3*x]   = (background[y][3*x] << 8) & (0xf80000);
		Fb_Layer[y][3*x+1] = (background[y][3*x] << 13) & (0xf80000);
		Fb_Layer[y][3*x+2] = (background[y][3*x] << 19) & (0xf80000);
	}
	else if(Frame_Mode == LAYER_MODE)
	{
		Fb_Layer[y][3*x]   = (color << 8) & (0xf80000);
		Fb_Layer[y][3*x+1] = (color << 13) & (0xfc0000);
		Fb_Layer[y][3*x+2] = (color << 19) & (0xf80000);
	}
	else
	{
		Fb_Frame[y][3*x]   = (color << 8) & (0xf80000);
		Fb_Frame[y][3*x+1] = (color << 13) & (0xfc0000);
		Fb_Frame[y][3*x+2] = (color << 19) & (0xf80000);
	}
}

void Lcd_Put_Cam_Pixel_Grey(int x, int y, unsigned int color)
{
	int r, g, b, yy;

	r = (color << 8) & (0xf80000);
	g = (color << 13) & (0xfc0000);
	b = (color << 19) & (0xf80000);

	r >>= 16; g >>= 16; b >>= 16;

	yy = (0.257 * r) + (0.504 * g) + (0.098 * b) +16;
	if(yy>255) yy = 255; else if(yy<0) yy = 0;

	if(Frame_Mode == LAYER_MODE) 
	{
		Fb_Layer[y][3*x] = Fb_Layer[y][3*x+1] = Fb_Layer[y][3*x+2] = yy<<16;
	}
	else 
	{
		Fb_Frame[y][3*x] = Fb_Frame[y][3*x+1] = Fb_Frame[y][3*x+2] = yy<<16;
	}
}

void Lcd_Draw_Cam_Image(int x, int y, const unsigned short *fp, int width, int height)
{
	register int xx, yy;

	for(yy=0;yy<height;yy++)
	{
		for(xx=0;xx<width;xx++)
		{
			if(!color_flag&0x1) Lcd_Put_Cam_Pixel(xx+x,yy+y,(int)fp[yy*width+xx]);
			else if(color_flag&0x1 >= 1) Lcd_Put_Cam_Pixel_Grey(xx+x, yy+y, (int)fp[yy*width+xx]);
		}
	}
}

<<<<<<< HEAD
extern const unsigned short ZT[3];

void Custom_Lcd_Draw_Cam_Image_Zoomed(int offx, int offy, const unsigned short *fp, int width, int height)
{
	register int xx, yy, tt;
	if(((width-2*offx)/320) > 3) return;
	int t = ZT[320 / (width-2*offx)];
=======

void Custom_Lcd_Draw_Cam_Image_Zoomed(int offx, int offy, const unsigned short *fp, int width, int height, int t)
{
	register int xx, yy, tt;
>>>>>>> 191025_Embedded_Proj_backup2

	for(yy=0; yy<height; yy++)
	{
		for(xx=0; xx<width; xx++)
		{
			if(!color_flag&0x1) Lcd_Put_Cam_Pixel(xx,yy,(int)fp[(yy/t+offy)*width+(xx/t+offx)]);
			else if(color_flag&0x1 >= 1) Lcd_Put_Cam_Pixel_Grey(xx, yy, (int)fp[(yy/t+offy)*width+(xx/t+offx)]);
		}
	}
}

void Lcd_Draw_Cam_Image_Large(int x, int y, const unsigned short *fp, int width, int height, int startx, int starty, int w, int h)
{
	register int xx, yy;

	for(yy=0;yy<h;yy++)
	{
		for(xx=0;xx<w;xx++)
		{
			if(!color_flag&0x1) Lcd_Put_Cam_Pixel(xx+x,yy+y,(int)fp[(yy+starty)*width+xx+startx]);
			else if(color_flag&0x1 >= 1) Lcd_Put_Cam_Pixel_Grey(xx+x,yy+y,(int)fp[(yy+starty)*width+xx+startx]);
		}
	}
}

//////////////////// GBOX Graphics Library ////////////////////////

void Lcd_Draw_Line(int x1,int y1,int x2,int y2,int color)
{
	int dx,dy,e;
	dx=x2-x1;
	dy=y2-y1;

	if(dx>=0)
	{
		if(dy >= 0) 	// dy>=0
		{
			if(dx>=dy) 	// 1/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					Lcd_Put_Pixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}
					x1+=1;
					e+=dy;
				}
			}
			else		// 2/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					Lcd_Put_Pixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   	// dy<0
		{
			dy=-dy;   	// dy=abs(dy)

			if(dx>=dy) 	// 8/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					Lcd_Put_Pixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}
					x1+=1;
					e+=dy;
				}
			}
			else		// 7/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					Lcd_Put_Pixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}
					y1-=1;
					e+=dx;
				}
			}
		}
	}
	else //dx<0
	{
		dx=-dx;			//dx=abs(dx)
		if(dy >= 0) 	// dy>=0
		{
			if(dx>=dy) 	// 4/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					Lcd_Put_Pixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}
					x1-=1;
					e+=dy;
				}
			}
			else		// 3/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					Lcd_Put_Pixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   	// dy<0
		{
			dy=-dy;   	// dy=abs(dy)

			if(dx>=dy) 	// 5/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					Lcd_Put_Pixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}
					x1-=1;
					e+=dy;
				}
			}
			else		// 6/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					Lcd_Put_Pixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}
					y1-=1;
					e+=dx;
				}
			}
		}
	}
}

void Lcd_Draw_Hline(int y, int x1, int x2, int color)
{
     int i, xx1, xx2;

     if(x1<x2)
     {
     	xx1=x1;
     	xx2=x2;
     }
     else
     {
     	xx1=x2;
     	xx2=x1;
     }
     for(i=xx1;i<=xx2;i++)
     {
         Lcd_Put_Pixel(i,y,color);
     }
}

void Lcd_Draw_Vline(int x, int y1, int y2, int color)
{
     int i, yy1, yy2;

     if(y1<y2)
     {
     	yy1=y1;
     	yy2=y2;
     }
     else
     {
     	yy1=y2;
     	yy2=y1;
     }
     for(i=yy1;i<=yy2;i++)
     {
         Lcd_Put_Pixel(x,i,color);
     }
}

void Lcd_Draw_Rect(int x1, int y1, int x2, int y2, int color)
{
     int xx1, yy1, xx2, yy2;

     if(x1<x2)
     {
     	xx1=x1;
     	xx2=x2;
     }
     else
     {
     	xx1=x2;
     	xx2=x1;
     }

     if(y1<y2)
     {
     	yy1=y1;
     	yy2=y2;
     }
     else
     {
     	yy1=y2;
     	yy2=y1;
     }

     Lcd_Draw_Hline(yy1,xx1,xx2,color);
     Lcd_Draw_Hline(yy2,xx1,xx2,color);
     Lcd_Draw_Vline(xx1,yy1,yy2,color);
     Lcd_Draw_Vline(xx2,yy1,yy2,color);
}

void Lcd_Draw_Bar(int x1, int y1, int x2, int y2, int color)
{
     int i, j;
     int xx1, yy1, xx2, yy2;

     if(x1<x2)
     {
     	xx1=x1;
     	xx2=x2;
     }
     else
     {
     	xx1=x2;
     	xx2=x1;
     }

     if(y1<y2)
     {
     	yy1=y1;
     	yy2=y2;
     }
     else
     {
     	yy1=y2;
     	yy2=y1;
     }

     for(i=yy1;i<=yy2;i++)
     {
         for(j=xx1;j<=xx2;j++)
         {
             Lcd_Put_Pixel(j,i,color);
         }
     }
}

///////////////////////// Font Display functions ////////////////////////

static unsigned char _first[]={0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 };
static unsigned char _middle[]={0,0,0,1,2,3,4,5,0,0,6,7,8,9,10,11,0,0,12,13,14,15,16,17,0,0,18,19,20,21};
static unsigned char _last[]={0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,0,17,18,19,20,21,22,23,24,25,26,27};
static unsigned char cho[]={0,0,0,0,0,0,0,0,0,1,3,3,3,1,2,4,4,4,2,1,3,0};
static unsigned char cho2[]={0,5,5,5,5,5,5,5,5,6,7,7,7,6,6,7,7,7,6,6,7,5};
static unsigned char jong[]={0,0,2,0,2,1,2,1,2,3,0,2,1,3,3,1,2,1,3,3,1,1};

#include ".\Fonts\ENG8X16.H"
#include ".\Fonts\HAN16X16.H"
#include ".\Fonts\HANTABLE.H"

#define 	ENG_FONT_X 		8
#define 	ENG_FONT_Y 		16

#define COPY(A,B) 	for(loop=0;loop<32;loop++) *(B+loop)=*(A+loop);
#define OR(A,B) 	for(loop=0;loop<32;loop++) *(B+loop)|=*(A+loop);

void Lcd_Han_Putch(int x,int y,int color,int bkcolor, int data, int zx, int zy)
{
	unsigned int first,middle,last;
	unsigned int offset,loop;
	unsigned char xs,ys;
	unsigned char temp[32];
	unsigned char bitmask[]={128,64,32,16,8,4,2,1};

	first=(unsigned)((data>>8)&0x00ff);
	middle=(unsigned)(data&0x00ff);
	offset=(first-0xA1)*(0x5E)+(middle-0xA1);
	first=*(HanTable+offset*2);
	middle=*(HanTable+offset*2+1);
	data=(int)((first<<8)+middle);

	first=_first[(data>>10)&31];
	middle=_middle[(data>>5)&31];
	last=_last[(data)&31];

	if(last==0)
	{
		offset=(unsigned)(cho[middle]*640);
		offset+=first*32;
		COPY(han16x16+offset,temp);

		if(first==1||first==24) offset=5120;
		else offset=5120+704;
		offset+=middle*32;
		OR(han16x16+offset,temp);
	}
	else
	{
		offset=(unsigned)(cho2[middle]*640);
		offset+=first*32;
		COPY(han16x16+offset,temp);

		if(first==1||first==24) offset=5120+704*2;
		else offset=5120+704*3;
		offset+=middle*32;
		OR(han16x16+offset,temp);

		offset=(unsigned)(5120+2816+jong[middle]*896);
		offset+=last*32;
		OR(han16x16+offset,temp);
	}

	for(ys=0;ys<16;ys++)
	{
		for(xs=0;xs<8;xs++)
		{
			if(temp[ys*2]&bitmask[xs])
			{
				if( (zx==1)&&(zy==1) ) Lcd_Put_Pixel(x+xs,y+ys,color);
				else if( (zx==2)&&(zy==1) )
				{
					Lcd_Put_Pixel(x+2*xs,y+ys,color);
					Lcd_Put_Pixel(x+2*xs+1,y+ys,color);
				}
				else if( (zx==1)&&(zy==2) )
				{
					Lcd_Put_Pixel(x+xs,y+2*ys,color);
					Lcd_Put_Pixel(x+xs,y+2*ys+1,color);
				}
				else if( (zx==2)&&(zy==2) )
				{
					Lcd_Put_Pixel(x+2*xs,y+2*ys+1,color);
					Lcd_Put_Pixel(x+2*xs+1,y+2*ys,color);
					Lcd_Put_Pixel(x+2*xs,y+2*ys,color);
					Lcd_Put_Pixel(x+2*xs+1,y+2*ys+1,color);
				}
			}
			else
			{
				if( (zx==1)&&(zy==1) ) Lcd_Put_Pixel(x+xs,y+ys,bkcolor);
				else if( (zx==2)&&(zy==1) )
				{
					Lcd_Put_Pixel(x+2*xs,y+ys,bkcolor);
					Lcd_Put_Pixel(x+2*xs+1,y+ys,bkcolor);
				}
				else if( (zx==1)&&(zy==2) )
				{
					Lcd_Put_Pixel(x+xs,y+2*ys,bkcolor);
					Lcd_Put_Pixel(x+xs,y+2*ys+1,bkcolor);
				}
				else if( (zx==2)&&(zy==2) )
				{
					Lcd_Put_Pixel(x+2*xs,y+2*ys+1,bkcolor);
					Lcd_Put_Pixel(x+2*xs+1,y+2*ys,bkcolor);
					Lcd_Put_Pixel(x+2*xs,y+2*ys,bkcolor);
					Lcd_Put_Pixel(x+2*xs+1,y+2*ys+1,bkcolor);
				}
			}
		}

		for(xs=0;xs<8;xs++)
		{
			if(temp[ys*2+1]&bitmask[xs])
			{
				if( (zx==1)&&(zy==1) )
				Lcd_Put_Pixel(x+xs+8,y+ys,color);
				else if( (zx==2)&&(zy==1) ){
				Lcd_Put_Pixel(x+2*(xs+8),y+ys,color);
				Lcd_Put_Pixel(x+2*(xs+8)+1,y+ys,color);
				}
				else if( (zx==1)&&(zy==2) ){
				Lcd_Put_Pixel(x+(xs+8),y+2*ys,color);
				Lcd_Put_Pixel(x+(xs+8),y+2*ys+1,color);
				}
				else if( (zx==2)&&(zy==2) ){
				Lcd_Put_Pixel(x+2*(xs+8),y+2*ys+1,color);
				Lcd_Put_Pixel(x+2*(xs+8)+1,y+2*ys,color);
				Lcd_Put_Pixel(x+2*(xs+8),y+2*ys,color);
				Lcd_Put_Pixel(x+2*(xs+8)+1,y+2*ys+1,color);
				}
			}
			else
			{
				if( (zx==1)&&(zy==1) ) Lcd_Put_Pixel(x+xs+8,y+ys,bkcolor);
				else if( (zx==2)&&(zy==1) )
				{
					Lcd_Put_Pixel(x+2*(xs+8),y+ys,bkcolor);
					Lcd_Put_Pixel(x+2*(xs+8)+1,y+ys,bkcolor);
				}
				else if( (zx==1)&&(zy==2) )
				{
					Lcd_Put_Pixel(x+(xs+8),y+2*ys,bkcolor);
					Lcd_Put_Pixel(x+(xs+8),y+2*ys+1,bkcolor);
				}
				else if( (zx==2)&&(zy==2) )
				{
					Lcd_Put_Pixel(x+2*(xs+8),y+2*ys+1,bkcolor);
					Lcd_Put_Pixel(x+2*(xs+8)+1,y+2*ys,bkcolor);
					Lcd_Put_Pixel(x+2*(xs+8),y+2*ys,bkcolor);
					Lcd_Put_Pixel(x+2*(xs+8)+1,y+2*ys+1,bkcolor);
				}
			}
		}
	}
}

void Lcd_Eng_Putch(int x,int y,int color,int bkcolor,int data, int zx, int zy)
{
	unsigned offset,loop;
	unsigned char xs,ys;
	unsigned char temp[32];
	unsigned char bitmask[]={128,64,32,16,8,4,2,1};

	offset=(unsigned)(data*16);
	COPY(eng8x16+offset,temp);

	for(ys=0;ys<16;ys++)
	{
		for(xs=0;xs<8;xs++)
		{
			if(temp[ys]&bitmask[xs])
			{

				if( (zx==1)&&(zy==1) ) Lcd_Put_Pixel(x+xs,y+ys,color);
				else if( (zx==2)&&(zy==1) )
				{
					Lcd_Put_Pixel(x+2*xs,y+ys,color);
					Lcd_Put_Pixel(x+2*xs+1,y+ys,color);
				}
				else if( (zx==1)&&(zy==2) )
				{
					Lcd_Put_Pixel(x+xs,y+2*ys,color);
					Lcd_Put_Pixel(x+xs,y+2*ys+1,color);
				}
				else if( (zx==2)&&(zy==2) )
				{
					Lcd_Put_Pixel(x+2*xs,y+2*ys+1,color);
					Lcd_Put_Pixel(x+2*xs+1,y+2*ys,color);
					Lcd_Put_Pixel(x+2*xs,y+2*ys,color);
					Lcd_Put_Pixel(x+2*xs+1,y+2*ys+1,color);
				}
			}
			else
			{
				if( (zx==1)&&(zy==1) ) Lcd_Put_Pixel(x+xs,y+ys,bkcolor);
				else if( (zx==2)&&(zy==1) )
				{
					Lcd_Put_Pixel(x+2*xs,y+ys,bkcolor);
					Lcd_Put_Pixel(x+2*xs+1,y+ys,bkcolor);
				}
				else if( (zx==1)&&(zy==2) )
				{
					Lcd_Put_Pixel(x+xs,y+2*ys,bkcolor);
					Lcd_Put_Pixel(x+xs,y+2*ys+1,bkcolor);
				}
				else if( (zx==2)&&(zy==2) )
				{
					Lcd_Put_Pixel(x+2*xs,y+2*ys+1,bkcolor);
					Lcd_Put_Pixel(x+2*xs+1,y+2*ys,bkcolor);
					Lcd_Put_Pixel(x+2*xs,y+2*ys,bkcolor);
					Lcd_Put_Pixel(x+2*xs+1,y+2*ys+1,bkcolor);
				}
			}
		}
	}
}

void Lcd_Puts(int x, int y, int color, int bkcolor, char *str, int zx, int zy)
{
     unsigned data;

     while(*str)
     {
        data=*str++;
        if(data>=128)
        {
             data*=256;
             data|=*str++;
             Lcd_Han_Putch(x, y, color, bkcolor, (int)data, zx, zy);
             x+=zx*16;
        }
        else
        {
             Lcd_Eng_Putch(x, y, color, bkcolor, (int)data, zx, zy);
             x+=zx*ENG_FONT_X;
        }
     }
}

void Lcd_Printf(int x, int y, int color, int bkcolor, int zx, int zy, char *fmt,...)
{
	va_list ap;
	char string[256];

	va_start(ap,fmt);
	vsprintf(string,fmt,ap);
	Lcd_Puts(x, y, color, bkcolor, string, zx, zy);
	va_end(ap);
}

