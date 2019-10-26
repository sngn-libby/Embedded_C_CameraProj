typedef unsigned int 		U32;
typedef signed int			S32;
typedef unsigned short		U16;
typedef short int			S16;
typedef unsigned char		U8;
typedef signed char 		S8;
typedef unsigned long long 	ULL;
typedef unsigned char SOCKET;

typedef	struct _ST_SRCC
{
	unsigned int INC: 1;
	unsigned int LOC: 1;
}ST_SRCC;

typedef	struct _ST_DSTC
{
	unsigned int INC: 1;
	unsigned int LOC: 1;
	unsigned int CHK_INT: 1;
}ST_DSTC;

typedef	struct _ST_DCON
{
	unsigned int TC: 20;
	unsigned int DSZ: 2;
	unsigned int RELOAD: 1;
	unsigned int SWHW_SEL: 1;
	unsigned int HWSRCSEL: 3;
	unsigned int SERVMODE: 1;
	unsigned int TSZ: 1;
	unsigned int INT: 1;
	unsigned int SYNC: 1;
	unsigned int DMD_HS: 1;
}ST_DCON;

typedef union _SRCC
{
	unsigned int udata;
	ST_SRCC	st;
}UNI_SRCC;

typedef union _DSTC
{
	unsigned int udata;
	ST_DSTC	st;
}UNI_DSTC;

typedef union _DCON
{
	unsigned int udata;
	ST_DCON	st;
}UNI_DCON;

typedef struct
{
	int day;
	int year;
	int mon;
	int date;
	int hour;
	int min;
	int sec;
}RTC_TIME;

typedef struct
{
	unsigned int Play_file_size;
	unsigned int Play_sample_freq;
	unsigned short Play_bit_per_sample;
}IIS_WAV;

// Heap.c
extern void * Get_Heap_Limit(void);
extern void * Get_Heap_Base(void);
extern unsigned int Get_Heap_Size(void);

// Led.c
extern void Led_Init(void);
extern void Led_Display(int disp);

// Uart.c
extern void Uart_Init(int baud);
extern void Uart_Printf(char *fmt,...);
extern char Uart_Get_Char(void);
extern char Uart_Get_Pressed(void);
extern int Uart_GetIntNum(void);
extern void Uart_GetString(char *string);
extern void Uart_ISR_Enable(int rx, int tx, int err);

// Key.c
extern void Key_Poll_Init(void);
extern int Key_Get_Pressed(void);
extern int Key_Get_Pressed_Multi(void);
extern void Key_Wait_Key_Released(void);
extern int Key_Wait_Key_Pressed(void);
extern void Key_Wait_Key_Released_Multi(void);
extern int Key_Wait_Key_Pressed_Multi(void);
extern void Key_Push_ISR_Init(void);
extern void Key_Push_ISR_Enable(int en);
extern void Key_ISR_Init(void);
extern void Key_ISR_Enable(int en);

// Timer.c
extern void Timer0_Stop_Watch_Run(void);
extern unsigned int Timer0_Stop_Watch_Stop(void);
extern int Timer0_Stop_Watch_Run_Unlimited(unsigned int max);
extern unsigned int Timer0_Stop_Watch_Stop_Unlimited(void);
extern void Timer0_Delay(int time);
extern void Timer0_Repeat(int time);
extern int Timer0_Check_Timeout(void);
extern void Timer0_Stop(void);
extern void Timer0_Set_Time(int time);
extern void Timer0_Delay_ISR_Enable(int en, int time);
extern void Timer0_Repeat_ISR_Enable(int en, int time);
extern void Timer1_Stopwatch_Run(void);
extern int Timer1_Stopwatch_Stop(void);
extern void Timer4_Init(void);
extern void Timer4_Delay(unsigned int msec);

// Buzzer.c
extern void Timer3_Buzzer_Beep(int tone, int duration);
extern void Timer3_Buzzer_Init(void);

// DMA.c
extern void DMA_Start(int ch, void * sa, void * da, UNI_SRCC srcc, UNI_DSTC dstc, UNI_DCON dcon);
extern void DMA_Stop(int ch);
extern void DMA_Timer_Select(int dma, int ch);
extern void DMA_Uart0_Ch0_Select(int dma, int rx_tx);
extern void DMA_Uart1_Ch1_Select(int dma, int rx_tx);
extern void DMA_Uart2_Ch3_Select(int dma, int rx_tx);

#define DMA_LOC_AHB			0
#define DMA_LOC_APB			1
#define DMA_ADDR_INC		0
#define DMA_ADDR_FIX		1
#define DMA_INT_TC			0
#define DMA_INT_RELOAD		1
#define DMA_DEMAND			0
#define DMA_HANDSHAKE		1
#define DMA_SYNC_PCLK		0
#define DMA_SYNC_HCLK		1
#define DMA_INT_DIS			0
#define DMA_INT_EN			1
#define DMA_TSZ_1UNIT		0
#define DMA_TSZ_4UNIT		1
#define DMA_SVC_SINGLE		0
#define DMA_SVC_WHOLE		1
#define DMA_TRIGGER_SW		0
#define DMA_TRIGGER_HW		1
#define DMA_RELOAD_ON		0
#define DMA_RELOAD_OFF		1
#define DMA_DSZ_1B			0
#define DMA_DSZ_2B			1
#define DMA_DSZ_4B			2
#define DMA_HWSRC_CH0_XDREQ0	0
#define DMA_HWSRC_CH1_XDREQ1	0
#define DMA_HWSRC_CH2_I2SSDO	0
#define DMA_HWSRC_CH3_UART2		0
#define DMA_HWSRC_CH0_UART0		1
#define DMA_HWSRC_CH1_UART1		1
#define DMA_HWSRC_CH2_I2SSDI	1
#define DMA_HWSRC_CH3_SDI		1
#define DMA_HWSRC_CH0_SDI		2
#define DMA_HWSRC_CH1_I2SSDI	2
#define DMA_HWSRC_CH2_SDI		2
#define DMA_HWSRC_CH3_SPI1		2
#define DMA_HWSRC_CH0_TIMER		3
#define DMA_HWSRC_CH1_SPI0		3
#define DMA_HWSRC_CH2_TIMER		3
#define DMA_HWSRC_CH3_TIMER		3
#define DMA_HWSRC_CH0_EP1		4
#define DMA_HWSRC_CH1_EP2		4
#define DMA_HWSRC_CH2_EP3		4
#define DMA_HWSRC_CH3_EP4		4
#define DMA_HWSRC_CH0_I2SSDO	5
#define DMA_HWSRC_CH1_PCMOUT	5
#define DMA_HWSRC_CH2_PCMIN		5
#define DMA_HWSRC_CH3_MICIN		5
#define DMA_HWSRC_CH0_PCMIN		6
#define DMA_HWSRC_CH1_SDI		6
#define DMA_HWSRC_CH2_MICIN		6
#define DMA_HWSRC_CH3_PCMOUT	6

// RTC.c
extern void RTC_Set_Time(RTC_TIME *time);
extern void RTC_Get_Time(RTC_TIME *time);
extern void RTC_Set_Alarm(RTC_TIME *time);
extern void RTC_Get_Alarm(RTC_TIME *time);
extern void RTC_Set_Alarm_Enable(int en, RTC_TIME *time);
extern void RTC_Tick_ISR_Enable(int en, int msec);

// WDT.c
extern void WDT_ISR_Enable(int enable, int msec);
extern void WDT_Start(int start, int msec);
extern void WDT_Clear(unsigned int msec);
extern void WDT_Reset_Enable(int enable);

// SPI1 & Acc Sensor
extern int SPI1_Acc_Init(void);
extern int SPI1_ACC_Read_ID(void);
extern int SPI1_ACC_Read_Status(void);
extern int SPI1_ACC_Read_XYZ(void);
extern int SPI1_ACC_Read_X(void);
extern int SPI1_ACC_Read_Y(void);
extern int SPI1_ACC_Read_Z(void);

// Graphics.c
extern void Lcd_Graphic_Init(void);
extern void Lcd_Clr_Screen(int color);
extern void Lcd_Draw_BMP(int x, int y, const unsigned short int *fp);
extern void Lcd_Draw_Bar(int x1, int y1, int x2, int y2, int color);

// ADC.c
extern unsigned int ADC_Start(int ch);
extern void ADC_ISR_Init(int en, int ch);
extern unsigned int Touch_Get_Data(void);
extern void Touch_ISR_Enable(int enable);
extern void Touch_Get_Position(void);
extern void Touch_Set_Wait_For_Release(void);

// IIS_Sound.c
extern void Sound_Init(void);
extern void Sound_Control_Speaker_Volume(int vol);
extern void Sound_Control_Headphone_Volume(int vol);
extern void Sound_Control_Soft_Mute(int mute);
extern void Sound_Control_Output_Select(int mode);
extern void Sound_Set_Sampling_Rate(int rate);
extern void Sound_Set_Tempo(int up);
extern void Sound_Set_Mode(int tx_rx_mode, int bps);
extern void Sound_Play_Pause(int pause);
extern void Sound_IIS_Start(void);
extern void Sound_Stop_Sound(void);
extern void Sound_Get_WAV_Info(IIS_WAV * sound, void * p);

#define IIS_RX_ONLY			1
#define IIS_TX_ONLY			2
#define IIS_TX_RX			3

// NAND.c
extern unsigned int Nand_Page_2_Addr(unsigned int block, unsigned int page, unsigned int offset);
extern void Nand_Addr_2_Page(unsigned int addr, unsigned int * block, unsigned int * page, unsigned int * offset);

extern void Nand_Init(void);
extern void Nand_Reset(void);
extern int	Nand_Check_Device(void);

extern unsigned int Nand_Erase_Block(unsigned int block);
extern unsigned int Nand_Verify_Erased_Page(unsigned int block, unsigned int page);
extern unsigned int Nand_Verify_Erased_Block(unsigned int block);

extern unsigned int Nand_Write_Block(unsigned int block,U8 *buffer);
extern unsigned int Nand_Read_Block(unsigned int block,U8 *buffer);
extern unsigned int Nand_Verify_Block(unsigned int block,U8 *buffer);

extern unsigned int Nand_Write_Page(unsigned int block,unsigned int page,U8 *buffer);
extern unsigned int Nand_Read_Page(unsigned int block,unsigned int page,U8 *buffer);
extern unsigned int Nand_Verify_Page(unsigned int block,unsigned int page,U8 *buffer);

extern unsigned int Nand_Write_Small_Area(unsigned int addr, U8 *buf, unsigned int size);
extern unsigned int Nand_Read_Small_Area(unsigned int addr, U8 *buf, unsigned int size);
extern unsigned int Nand_Verify_Small_Area(unsigned int addr, U8 *buf, unsigned int size);
extern unsigned int Nand_Verify_Erased_Small_Area(unsigned int addr, unsigned int size);

extern unsigned int Nand_Write(unsigned int addr, U8 *buf, unsigned int size);
extern unsigned int Nand_Read(unsigned int addr, U8 *buf, unsigned int size);
extern unsigned int Nand_Verify(unsigned int addr, U8 *buf, unsigned int size);
extern unsigned int Nand_Verify_Erased(unsigned int addr, unsigned int size);

// for 1Gb NAND Flash
#define BYTE_PER_PAGE		2048
#define SE_PER_PAGE			64
#define	PAGE_PER_BLOCK		64
#define	BYTE_PER_BLOCK		(PAGE_PER_BLOCK * BYTE_PER_PAGE)
#define	SE_PER_BLOCK		(PAGE_PER_BLOCK * SE_PER_PAGE)
#define	BLOCK_PER_DEVICE	1024
#define	NAND_GOOD_BLOCK		0xff

#define NAND_SUCCESS		0
#define NAND_PROTECTED		1
#define NAND_LOCKED			2
#define NAND_BAD_BLOCK		3
#define NAND_VERIFY_FAIL	4
#define NAND_BAD_PAGE		5
#define NAND_BAD_MECC		6
#define NAND_BAD_SECC		7
#define NAND_BAD_ADDRESS	8
#define NAND_BAD_ALIGN		9
#define	NAND_PAGE_OVER		10
#define NAND_REMAP_FAIL		11
#define NAND_ACCESS_FAIL	12
#define NAND_USER_DATA_FULL	13
#define NAND_RESTORE_FAIL	14
#define NAND_FAIL			126
#define NAND_ABORT			127

// Graphics.c
#define LCD_XSIZE 		(960)
#define LCD_YSIZE 		(240)
#define LCD_LAYER_SIZE	(LCD_YSIZE*LCD_XSIZE/3)
#define LCD_SIZE 		(LCD_YSIZE*LCD_XSIZE*4)

#define LAYER_MODE		0
#define VIRTUAL_MODE	1

extern unsigned int Virtual_X;
extern unsigned int Virtual_Y;

#define BPP_16	0
#define BPP_24	1

extern unsigned int Bit_per_pixel;
extern unsigned int Selected_frame;
extern unsigned int Display_frame;

extern unsigned int Trans_mode;
extern unsigned int Shape_mode;
extern unsigned int Shape_mode_color;

extern void Lcd_Control_Brightness(int bright);

extern void Lcd_Set_Trans_Mode(int mode);
extern void Lcd_Set_Shape_Mode(int mode, int color);
extern void Lcd_Set_Shape_Back_Image_Mode(int mode, void * img);

extern void Lcd_Wait_Blank(void);
extern void Lcd_Select_Buffer_Mode(unsigned int mode);
extern void Lcd_Set_Virtual_Display_Start(int x, int y);
extern void Lcd_Select_Draw_Frame_Buffer(unsigned int id);
extern void Lcd_Select_Display_Frame_Buffer(unsigned int id);

extern void Lcd_Graphic_Init(void);
extern void Lcd_Put_Pixel(int x,int y,int color);
extern void Lcd_Clr_Screen(int color);
extern void Lcd_Draw_BMP(int x, int y, const unsigned short int *fp);
extern void Lcd_Draw_Image(int x, int y, const unsigned short int *fp, int width, int height);
extern void Lcd_Get_Info_BMP(int * x, int  * y, const unsigned short int *fp);
extern void Lcd_Draw_Cam_Image(int x, int y, const unsigned short *fp, int width, int height);
extern void Lcd_Draw_Cam_Image_Large(int x, int y, const unsigned short *fp, int width, int height, int startx, int starty, int w, int h);

extern void Lcd_Draw_Line(int x1,int y1,int x2,int y2,int color);
extern void Lcd_Draw_Hline(int y, int x1, int x2, int color);
extern void Lcd_Draw_Vline(int x, int y1, int y2, int color);
extern void Lcd_Draw_Rect(int x1, int y1, int x2, int y2, int color);
extern void Lcd_Draw_Bar(int x1, int y1, int x2, int y2, int color);
extern void Lcd_Draw_BMP_File_24bpp(int x, int y, void *fp);

extern void Lcd_Puts(int x, int y, int color, int bkcolor, char *str, int zx, int zy);
extern void Lcd_Printf(int x, int y, int color, int bkcolor, int zx, int zy, char *fmt,...);

extern int Lcd_Get_Pixel_24bpp(int x,int y);
extern void Lcd_Draw_BMP_Tile(int x, int y, const unsigned short int *fp, int x1, int y1, int xsize, int ysize);
extern void Lcd_Draw_Image_Tile(int x, int y, const unsigned short int *fp, int width, int height, int x1, int y1, int xsize, int ysize);
extern void Lcd_Draw_Backup_24bpp(int x, int y, int xsize, int ysize, unsigned int *fp);
extern void Lcd_Save_Backup_24bpp(int x, int y, int xsize, int ysize, unsigned int *fp);

extern void Custom_Layer_Draw_Display_Init(void);
extern void Lcd_Put_Pixel_Area(int x, int y, int xsize, int ysize, int color);
extern int Custom_Lcd_Get_Pixel_16bpp(int x, int y);
extern void Lcd_Put_Cam_Pixel_Grey(int x, int y, unsigned int color);
extern void Custom_Layer_Draw_Display_Init(void);
<<<<<<< HEAD
extern void Custom_Lcd_Draw_Cam_Image_Zoomed(int offx, int offy, const unsigned short *fp, int width, int height);

=======
extern void Custom_Lcd_Draw_Cam_Image_Zoomed(int offx, int offy, const unsigned short *fp, int width, int height, int t);
>>>>>>> 191025_Embedded_Proj_backup2

// Camera.c
#define CAM_EFFECT_NONE			0
#define CAM_EFFECT_MONO			1
#define CAM_EFFECT_SEPIA		2
#define CAM_EFFECT_GREEN		3
#define CAM_EFFECT_AQUA			4
#define CAM_EFFECT_EMBOSSING	5
#define CAM_EFFECT_SKETCH		6
#define CAM_EFFECT_NEGATIVE		7

extern void CAM_Camera_Init(void);
extern int CAM_Capture_Run(U32 run, int width, int height);
extern void * CAM_Check_Image_Ready(void);
extern void CAM_Interrupt_Enable(int en);
extern void CAM_Capture_Pause(void);
extern void CAM_Capture_Restart(void);
extern void * CAM_Get_Image_Address(void);
extern void CAM_XY_Flip(int x, int y);
extern void CAM_Set_Capture_Status(int state);
extern int CAM_Get_Capture_Status(void);
extern void CAM_Set_Auto_White_Balance(int en);
extern void CAM_Set_Auto_Saturation(int en);
extern void CAM_Set_Auto_Exposure(int en);
extern int CAM_Set_Exposure(int ms);
extern int CAM_Get_Exposure(void);
extern int CAM_Set_Gain(int val);
extern int CAM_Get_Gain(void);
extern int CAM_Increase_Exposure(void);
extern int CAM_Decrease_Exposure(void);
extern int CAM_Increase_Gain(void);
extern int CAM_Decrease_Gain(void);
extern void CAM_Set_Auto_Brightness(int en);
extern int CAM_Set_Brightness(int val);
extern int CAM_Get_Brightness(void);
extern int CAM_Increase_Brightness(void);
extern int CAM_Decrease_Brightness(void);
extern void CAM_Set_Auto_Framerate(int en);
extern int CAM_IIC_Read(int addr);
extern void CAM_Sensor_Enable(int en);
extern void CAM_Set_Effect(int effect);
extern void CAM_Module_Init(void);

// Sticker.c
