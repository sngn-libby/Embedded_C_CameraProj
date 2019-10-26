#include "2440addr.h"
#include "device_driver.h"
#include "macro.h"

#define TEST_PASSED 0
#define INIT_NAND 0

/* 5:5:5:I Color Definition */

#define BLACK	0x0000
#define WHITE	0xfffe
#define BLUE	0x003e
#define GREEN	0x07c0
#define RED		0xf800
#define CHROMAKEY 0x46FF

#include <stdlib.h>

#define PIDX_BLOCK (600)
#define DCNT_BLOCK (605)
#define DARR_BLOCK (610)
#define IDX_MASK 0x3
#define DIGIT_MASK 0x3fff
#define MIN_ADDR 0xc8000000
#define NO_OF_BMP (3) // 사진 로드될 때 여기 바꿔주기
#define LED_ON 0xf
#define LED_OFF 0x0
#define NO_ZOOM 2

#include "./Images/img01.h"
#include "./Images/img02.h"
#include "./Images/img03.h"
#include "./Images/bck01.h" // Sponge bop
#include "./Images/bck02.h" // Cloud
#include "./Images/bck03.h"

#include "./Images/sticker02.h"
#include "./Images/sticker03.h"
#include "./Images/sticker04.h"
#include "./Images/sticker05.h"
#include "./Images/sticker06.h"

const unsigned short * Disp[] = {img01, img02, img03};
const unsigned short * Sticker[] = {sticker02, sticker03, sticker04, sticker05, sticker06};
const unsigned short * DispB[] = {bck02, bck01};
const unsigned short Strpt[][2] = {{0, 0}, {160, 0}, {0, 120}, {160, 120}};
<<<<<<< HEAD
<<<<<<< HEAD
const unsigned short TT[3] = {0, 3, 5}, ZT[3] = {0, 2, 4};
=======
const unsigned short TT[3] = {0, 6, 10};
>>>>>>> 191025_Embedded_Proj_backup
=======
const unsigned short TT[3] = {0, 6, 10}, ZT[3] = {0, 2, 4};
unsigned short Deleted[200];
static int dcnt;
>>>>>>> 191025_Embedded_Proj_backup2
static int buf_idx;
static int pidx;
static int led = 0xf;
static int pidx_addr, deleted_addr, dcnt_addr;
static int sticker_x = 0, sticker_y = 10;
int sticker_flag = 0;
static int pencil_flag = 0, timer_flag = 0, zoom_flag = 0;
extern volatile int Touch_pressed;
extern volatile int Touch_x, Touch_y;
extern volatile int color_flag;
extern volatile int Timer0_time_out;
typedef struct _photo
{
    unsigned addr; // addr은 블럭위치이다 -> Nand_Page_2_Addr 로 주소추출 필요
    U8 h, w, off;
}P;
P img[NO_OF_BMP];
unsigned short background[240][320];



void Camera_Interrupt_Test(void);
void Save_Modified_2_Nand(void);
void Back_Load_Nand2Mem(int i);
void Custom_Touch_Action(void);
void Custom_Touch_Action_Paused(int i);
void Custom_View_Gallery(void);
void Custom_Draw_Sticker_Clicked(int mode);
void Custom_Display_Photo(int mode, int i);


static void (*Func_Arr[])(void) =
{
	Camera_Interrupt_Test
};

extern char * Cam_Frame_Buf;

void Main(void)
{
	int r;

	Nand_Init();
	Uart_Init(115200);
	Lcd_Graphic_Init();
	Key_ISR_Init();
    Led_Init();

	Uart_Printf("Camera Interface Test\n");

	CAM_Camera_Init();
	CAM_Module_Init();
	CAM_Sensor_Enable(1);


	Uart_Printf("ID= %x%x\n", CAM_IIC_Read(0),CAM_IIC_Read(1));
	CAM_Sensor_Enable(0);

	Cam_Frame_Buf = (char *)Free_Memory_BASE;

	for(;;)
	{
        Lcd_Control_Brightness(5);
        Lcd_Select_Buffer_Mode(LAYER_MODE);
        Lcd_Select_Draw_Frame_Buffer(0);
        Lcd_Select_Display_Frame_Buffer(0);
        Lcd_Set_Trans_Mode(0);
        Lcd_Set_Shape_Mode(0,0);
        Lcd_Set_Shape_Back_Image_Mode(0, (void *)0);

        Func_Arr[0]();

		Uart_Printf("Test Finished: Press any key to continue \n");
		Uart_Get_Char();
	}
}

extern volatile int Key_value;

int Cam_Width  = 320;
int Cam_Height = 240;
int Cam_Exp_Mode = 0;
<<<<<<< HEAD
int Z_Cam_Width = 360; // 400
int Z_Cam_Height = 270; // 300
int Z_Sx = -20, Z_Sy = -15;
=======
unsigned short remain = 0;
<<<<<<< HEAD
>>>>>>> 191025_Embedded_Proj_backup
=======
int Z_Cam_Width = 360; // 400
int Z_Cam_Height = 270; // 300
int Z_Sx = -20, Z_Sy = -15;

>>>>>>> 191025_Embedded_Proj_backup2


static int frm = 0;

void Toggle_Frame(void)
{
	if(frm) frm = 0;
	else frm = 1;
}


void Camera_Interrupt_Test(void)
{
    int i, j;

  	Uart_Printf("\n\nCamera Test (Interrupt)\n");


	{
		Uart_Printf("Camera test: Press any key for next test\n");
		Uart_Printf("UP: Pause\n");
		Uart_Printf("DOWN: Play\n");

		Cam_Width = 320;
		Cam_Height= 240;

		CAM_Set_Capture_Status(0);
		CAM_Capture_Run(0,0,0);
		CAM_Capture_Run(1, Cam_Width, Cam_Height);
		CAM_XY_Flip(1,0);

        Back_Load_Nand2Mem(0);

		Key_ISR_Enable(1);
		CAM_Interrupt_Enable(1);
        Touch_ISR_Enable(1);
        Custom_Layer_Draw_Display_Init();
        
<<<<<<< HEAD
<<<<<<< HEAD

        zoom_flag = sticker_flag = pencil_flag = color_flag = 0;
=======
        sticker_flag = pencil_flag = color_flag = 0;
>>>>>>> 191025_Embedded_Proj_backup
=======
        zoom_flag = sticker_flag = pencil_flag = color_flag = 0;
>>>>>>> 191025_Embedded_Proj_backup2

        {
            Lcd_Clr_Screen(0xFFD4DF);
            Lcd_Set_Shape_Mode(1, WHITE);
            Lcd_Draw_BMP(0, 0, bck03);
            Lcd_Set_Shape_Mode(0, 0);

            Uart_Printf("==================================================\n");
            Uart_Printf("Touch the Screen to Start                         \n");
            Uart_Printf("==================================================\n");

            while(1)
            {
                if(Touch_pressed)
                {
                    Uart_Printf("<<< Start the Camera >>>\n");
                    Uart_Get_Pressed();
                    while(Touch_pressed);
                    break;
                }
            }
        }

// -----------------------------------------------------------------------------------------//

		do
		{
			if(CAM_Get_Capture_Status() == 2)
			{
                if(sticker_flag < 2) Custom_Display_Photo(0, 0);
                Custom_Draw_Sticker_Clicked(0); // sticker_flag == 2 일 때, Chroma-Key 모드로 자동
				Lcd_Select_Display_Frame_Buffer(frm);
				Toggle_Frame();
			}

            if(rCIIMGCPT == 0)
            {
                if(Touch_pressed) 
                {
                    if(Touch_x >= 260 && Touch_y > 100 && Touch_y <= 150)
                    {
                        Led_Display(led = LED_OFF);
                        CAM_Capture_Restart();
                        while(Touch_pressed);
                    }
                }
                else if(Key_Get_Pressed() == 1)
                {
                    Led_Display(led = LED_OFF);
                    CAM_Capture_Restart();
                    Uart_Printf("key - restart!\n");
                    Key_Wait_Key_Released();
                }
            }
            else if(rCIIMGCPT != 0 && Touch_pressed)
            {
                Custom_Touch_Action();
            }
			else if(Key_Get_Pressed()) // save images to NAND Flash
			{
				if(rCIIMGCPT && Key_value == 1) 
                {
                    Save_Modified_2_Nand();
                    CAM_Capture_Pause();
                    Uart_Printf("key - writed!\n");
                }
                else if(Key_value == 2) // Album
                {
                    Lcd_Clr_Screen(BLACK);
                    if(pidx==0) continue;
                    Custom_View_Gallery();
                }
                else if(Key_value == 5) // Timer
                {
                    if(++timer_flag == 3) timer_flag = 0;
                    Uart_Printf("<Timer> set %d\n", TT[timer_flag]/2);
                }
                else if(Key_value == 7) // Zoom
                {
<<<<<<< HEAD
                    if(++zoom_flag == 3) zoom_flag = 0;
=======
                    if(++zoom_flag == NO_ZOOM) zoom_flag = 0;
>>>>>>> 191025_Embedded_Proj_backup2
                    
                    if(zoom_flag == 1) Uart_Printf("<Zoom> set - Zoom-In\n");
                    else if(zoom_flag == 2) Uart_Printf("<Zoom> set - Zoom-out\n");
                }
				
                Key_Wait_Key_Released();
			}

            if(remain && Timer0_time_out)
            {
                if(!((remain--)%2)) Uart_Printf("auto Timer %d - %d left\n", TT[timer_flag], remain);
                Timer0_time_out = 0;
                Led_Display(led = ~led);
                if(remain == 0) // save photo!
                {
                    Led_Display(LED_OFF); // 제대로 꺼지는지 체크
                    Save_Modified_2_Nand();
                    CAM_Capture_Pause();
                }
                Timer0_Delay_ISR_Enable(1, 1000);
            }

		}while(!Uart_Get_Pressed());

		CAM_Capture_Run(0,0,0);
		CAM_Capture_Run(0, Cam_Width, Cam_Height);
		CAM_Interrupt_Enable(0);
		Key_ISR_Enable(0);
        Touch_ISR_Enable(0);
	}
}


void Save_Modified_2_Nand(void)
{
    unsigned short shot[Cam_Height*Cam_Width];
    int address;
    register int xx, yy;
    int tmp;

    for(yy=0;yy<Cam_Height;yy++)
	{
		for(xx=0;xx<Cam_Width;xx++)
		{
            shot[yy*Cam_Width+xx] = Custom_Lcd_Get_Pixel_16bpp(xx, yy);
        }
    }

    Nand_Erase_Block(110+(pidx)*2);
    Nand_Erase_Block(110+(pidx)*2+1);
    Nand_Erase_Block(PIDX_BLOCK);

    address = Nand_Page_2_Addr(110 + (pidx++)*2, 0, 0);
    if(address <= MIN_ADDR && address >= (1<<27)) return;
    Nand_Write(address, (U8 *)shot, Cam_Width*Cam_Height*2);

    Nand_Write(pidx_addr, (U8*)&pidx, 4);
    Uart_Printf("Writed!! : ");
    Uart_Printf("Total Photo - %d\n", pidx);
}


void Back_Load_Nand2Mem(int i)
{
    int tmpidx;
    unsigned short *p;
    register int yy, xx;
    int address;

    Lcd_Wait_Blank();
    Lcd_Clr_Screen(BLACK);
    
    for(yy = 0; yy<Cam_Height; yy++)
    {
        for(xx = 0; xx<Cam_Width; xx++)
        {
            background[yy][xx] = DispB[i][yy*Cam_Width + xx];
        }
    }
    
    pidx_addr = Nand_Page_2_Addr(PIDX_BLOCK, 0, 0);
    #if INIT_NAND
    Nand_Erase_Block(PIDX_BLOCK);
    Nand_Write(pidx_addr, (U8*)&pidx, 4);
    #endif
    Nand_Read(pidx_addr, (U8*)&tmpidx, 4);
    if(pidx_addr >= (1<<27)) return;
    pidx = tmpidx;

    #if INIT_NAND
    for(yy=0; yy<100; yy++) Nand_Erase_Block(DARR_BLOCK + yy);
    address = Nand_Page_2_Addr(DARR_BLOCK, 0, 0);
    if(address >= (1<<26)) return;
    Nand_Write(address, (U8*)Deleted, 200*2);
    #endif

    #if TEST_PASSED
    deleted_addr = Nand_Page_2_Addr(DARR_BLOCK, 0, 0);
    Nand_Read(deleted_addr, (U8*)p, 200*2);
    if(p >= (1<<27)) return;
    for(xx=0; xx<200; xx++) 
    {
        Deleted[xx] = p[xx]; //(p[xx] != 1 ? 0 : 1);
        Uart_Printf("Deleted %d - %d\n", xx, Deleted[xx]);
    }

    Uart_Printf("pidx : %d, dcnt : %d\n", pidx, dcnt);
    Uart_Printf("<<init completed>> Saved Photo cnt : %d\n", pidx);
    #endif
}



void Custom_Touch_Action(void)
{
    if(Touch_x > 0 && Touch_x < 200)  // mono-filter
    {
        color_flag^=0x1;
        Uart_Printf("color changed -> %d\n", color_flag);
    }
    else if(Touch_x >= 260 && Touch_y > 100 && Touch_y <= 150) // take photo (with Timer)
    {
        remain = TT[timer_flag];
        led = LED_OFF; //-> main에서 NAND저장합니다
        if(remain) 
        {
            Led_Display(led = ~led);
            Timer0_Delay_ISR_Enable(1, 1000);
        }
        else if(remain == 0)
        {
            Led_Display(led = ~led);
            Save_Modified_2_Nand();
            CAM_Capture_Pause();
        }
    }
    else if(Touch_x >= 260 && Touch_y > 150) // Album
    {
        Lcd_Clr_Screen(BLACK);
        if(pidx==0) return;
        Custom_View_Gallery();
    }
    else if(Touch_x >= 260 && Touch_y > 0 && Touch_y <= 70) // sticker
    {
        if(++sticker_flag == 3) sticker_flag = 0;
        Uart_Printf("sticker %d status\n", sticker_flag);
    }

    while(Touch_pressed);
    Uart_Printf("touch_released\n");
}

void Custom_Display_Photo(int mode, int i)
{
    if(mode == 1) // album_mode
    {
        int address;
        unsigned short p[Cam_Height*Cam_Width];

        address = 110 + i*2;
        address = Nand_Page_2_Addr(address, 0, 0);
        Nand_Read(address, (void *)p, LCD_LAYER_SIZE*2);
        Lcd_Draw_Cam_Image(0, 0, p, Cam_Width, Cam_Height);
    }
    else if(mode == 0) // camera mode
    {
        unsigned short *q;

        CAM_Set_Capture_Status(0);
        q = CAM_Get_Image_Address();
        Lcd_Select_Draw_Frame_Buffer(frm);
<<<<<<< HEAD
        if(zoom_flag == 0) Lcd_Draw_Cam_Image(0, 0, (void *)q, Cam_Width, Cam_Height);
        else if(zoom_flag == 1) Custom_Lcd_Draw_Cam_Image_Zoomed(80, 60, q, Cam_Width, Cam_Height); // 확대
        else if(zoom_flag == 2) Lcd_Draw_Cam_Image_Large(0, 0, q, Cam_Width, Cam_Height, 20, 15, 280, 210); // 축소
=======

        if(zoom_flag == 0) Lcd_Draw_Cam_Image(0, 0, (void *)q, Cam_Width, Cam_Height);
        else if(zoom_flag == 1) Custom_Lcd_Draw_Cam_Image_Zoomed(80, 60, q, Cam_Width, Cam_Height, ZT[1]); // 확대
        //else if(zoom_flag == 2) Lcd_Draw_Cam_Image_Large(0, 0, q, Cam_Width, Cam_Height, 20, 15, 280, 210); // 축소
>>>>>>> 191025_Embedded_Proj_backup2
    }
    
}

void Custom_Touch_Action_Paused(int i)
{
    if(Touch_pressed) // setting
    {
        if(Touch_x >= 260 && Touch_y <= 100) // sticker mode (without Chroma-Key mode)
        {
            ++sticker_flag;
            Uart_Printf("sticker %d status\n", sticker_flag);
            if(sticker_flag == 3)
            {
                Custom_Display_Photo(1, i);
                Custom_Draw_Sticker_Clicked(1);
                sticker_flag = 0;
            } 
            else if(sticker_flag == 1)
            {
                Custom_Draw_Sticker_Clicked(1);
                sticker_flag++;
            }
        }
        else if(Touch_x >= 260 && Touch_y > 100 && Touch_y <= 150) // pencil mode
        {
            pencil_flag = ++pencil_flag & 0x3; // 1, 2, 3, 0
            Uart_Printf("pencil %d selected\n", pencil_flag);
        }
        else if(pencil_flag!=0 && Touch_x < 250) // drag&drop
        {
            for(;;)
            {
                Touch_Get_Position();
                
                if(Touch_x < 250)
                {
                    if(pencil_flag == 0x1) Lcd_Put_Pixel_Area(Touch_x, Touch_y, 2, 2, BLACK);
                    else if(pencil_flag == 0x2) Lcd_Put_Pixel_Area(Touch_x, Touch_y, 4, 4, BLACK);
                    else if(pencil_flag >= 0x3) Lcd_Put_Pixel_Area(Touch_x, Touch_y, 8, 8, BLACK);
                    Uart_Printf("X=%d Y=%d drawed\n", Touch_x, Touch_y);
                }

                Touch_Set_Wait_For_Release();
                Uart_Printf("drawed going on?\n");
                if(!Touch_pressed) break;
            }
        }
        else if(Touch_x >= 260 && Touch_y > 150) // save the photo
        {
            int j;
            Save_Modified_2_Nand();
            for(j=1; j<=0x1000; j++);
        }
        
        while(Touch_pressed);
    }
}


void Custom_View_Gallery(void)
{
    int address;
    unsigned short p[240][320];
    int i,j, tmp, start_flag = 0;
    int cnt = 1;

    Lcd_Clr_Screen(BLACK);
<<<<<<< HEAD
    zoom_flag = pencil_flag = sticker_flag = color_flag = 0;
=======
    zoom_flag = sticker_flag = pencil_flag = color_flag = 0;
>>>>>>> 191025_Embedded_Proj_backup2

    for(i=0; i<pidx; i++)
    {
        #if TEST_PASSED
        Uart_Printf("Gallery %d - %d\n", i, Deleted[i]);
        if(Deleted[i]) continue;
        #endif
        Uart_Printf("<album> %d / %d\n", cnt++, pidx-dcnt);
        Custom_Display_Photo(1, i);
        Custom_Draw_Sticker_Clicked(1);

        while((tmp = Key_Get_Pressed()) == 0)
        {
            Custom_Touch_Action_Paused(i);
            if(Key_value == 3) // DN 키로만 빠져나갈 수 있다.
            {
<<<<<<< HEAD
                zoom_flag = pencil_flag = sticker_flag = color_flag = 0;
=======
                zoom_flag = sticker_flag = pencil_flag = color_flag = 0;
>>>>>>> 191025_Embedded_Proj_backup2
                Lcd_Clr_Screen(BLACK);
                CAM_Capture_Restart();
                return;
            } 
            #if TEST_PASSED
            else if(Key_value == 8) // delete photo
            {
                Deleted[i] = 1;

                #if TEST_PASSED
                for(j=0; j<100; j++) Nand_Erase_Block(DARR_BLOCK + j);
                Nand_Erase_Block(DCNT_BLOCK);

                address = Nand_Page_2_Addr(DARR_BLOCK, 0, 0);
                if(address >= (1<<26)) return;
                Nand_Write(address, (U8*)Deleted, 200*2);

                address = Nand_Page_2_Addr(DCNT_BLOCK, 0, 0);
                if(address >= (1<<26)) return;
                Nand_Write(address, (U8*)Deleted, 200*2);
                #endif
            }
            #endif
        }

        if(tmp == 3)
        {
            zoom_flag = sticker_flag = pencil_flag = color_flag = 0;
            Lcd_Clr_Screen(BLACK);
            CAM_Capture_Restart();
            return;
        } 
        if(tmp == 2) i = (i==0 ? pidx-2 : i-2);
        else if(tmp == 4) i = (i==pidx-1 ? -1 : i);
        else if(tmp == 8) // delete photo
        {
            Deleted[i] = 1;

            #if TEST_PASSED
            for(j=0; j<200; j++) Nand_Erase_Block(DARR_BLOCK + j);
            Nand_Erase_Block(DCNT_BLOCK);

            address = Nand_Page_2_Addr(DARR_BLOCK, 0, 0);
            if(address >= (1<<26)) return;
            Nand_Write(address, (U8*)Deleted, 200*2);

            address = Nand_Page_2_Addr(DCNT_BLOCK, 0, 0);
            if(address >= (1<<26)) return;
            Nand_Write(address, (U8*)Deleted, 200*2);
            #endif
        }

        Key_Wait_Key_Released();
    }
}


void Custom_Draw_Sticker_Clicked(int mode) // 화면 전환 (스티커 버퍼에 그리기)
{   
    int address;
    unsigned short p[240][320];
    U8 size;

    if(sticker_flag == 1) // Sticker --> check : 스티커 움직임 구현
    {
        Lcd_Set_Shape_Mode(1, WHITE);
        Lcd_Draw_BMP(sticker_x, sticker_y, img01);
        Lcd_Set_Shape_Mode(0, 0);
    }
    else if(sticker_flag == 2) // Chroma-Key
    {
        //Lcd_Set_Shape_Back_Image_Mode(1, DispB[0]);
        Lcd_Set_Shape_Mode(1, CHROMAKEY);
        Custom_Display_Photo(0, 0);
        Lcd_Set_Shape_Mode(0, 0);
        //Lcd_Set_Shape_Back_Image_Mode(0, 0);
    }

    if(mode == 0) // camera mode
    {
        Lcd_Put_Pixel_Area(250, 0, 70, LCD_YSIZE, BLACK);
                
        Lcd_Set_Shape_Mode(1, WHITE);
        Lcd_Draw_BMP(260, 10, Sticker[0]);
        Lcd_Draw_BMP(260, 85, Sticker[3]);
        Lcd_Draw_BMP(260, 160, Sticker[2]);
        Lcd_Set_Shape_Mode(0, 0);
    }
    else if(mode == 1) // album mode
    {
        Lcd_Put_Pixel_Area(250, 0, 70, LCD_YSIZE, BLACK);

        Lcd_Set_Shape_Mode(1, WHITE);
        Lcd_Draw_BMP(260, 10, Sticker[0]);
        Lcd_Draw_BMP(260, 85, Sticker[1]);
        Lcd_Draw_BMP(260, 180, Sticker[4]);
        Lcd_Set_Shape_Mode(0, 0);
    }
}