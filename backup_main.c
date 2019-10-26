// #include "2440addr.h"
// #include "device_driver.h"
// #include "macro.h"

// /* 5:5:5:I Color Definition */

// #define BLACK	0x0000
// #define WHITE	0xfffe
// #define BLUE	0x003e
// #define GREEN	0x07c0
// #define RED		0xf800

// #include <stdlib.h>

// #define IDX_MASK 0x3
// #define DIGIT_MASK 0x3fff;
// #define NO_OF_BMP (3) // 사진 로드될 때 여기 바꿔주기

// #include "./Images/img01.h"
// #include "./Images/img02.h"
// #include "./Images/img03.h"
// #include "./Images/bck01.h"

// const unsigned short * Disp[] = {img01, img02, img03};
// const unsigned short * DispB[] = {bck01,};
// static int buf_idx;
// extern volatile int Touch_pressed;
// extern volatile int Touch_x, Touch_y;
// typedef struct _photo
// {
//     unsigned addr; // addr은 블럭위치이다 -> Nand_Page_2_Addr 로 주소추출 필요
//     U8 h, w, off;
// }P;
// P img[NO_OF_BMP];


// void Camera_Polling_Test(void);
// void Camera_Interrupt_Test(void);

// static void (*Func_Arr[])(void) =
// {
// 	Camera_Polling_Test,
// 	Camera_Interrupt_Test
// };

// extern char * Cam_Frame_Buf;


// void Main(void)
// {
// 	int r;

// 	Nand_Init();
// 	Uart_Init(115200);
// 	Lcd_Graphic_Init();
// 	Key_ISR_Init();
// 	Key_ISR_Enable(1);
//     Custom_Layer_Draw_Display_Init(); // Layer mode setting

// 	Uart_Printf("Camera Interface Test\n");

// 	CAM_Camera_Init();
// 	CAM_Module_Init();
// 	CAM_Sensor_Enable(1);

// 	Uart_Printf("ID= %x%x\n", CAM_IIC_Read(0),CAM_IIC_Read(1));
// 	CAM_Sensor_Enable(0);

//     Touch_ISR_Enable(1);

// 	Cam_Frame_Buf = (char *)Free_Memory_BASE;

// 	for(;;)
// 	{
//         if(Touch_pressed)
//         {
// 			Uart_Printf("Pressed: X=%u,Y=%u\n", Touch_x, Touch_y);
// 			// int r = Touch_Get_Data();
//             // int ypos, xpos;
//             // ypos = r & DIGIT_MASK;
//             // xpos = (r>>16) & DIGIT_MASK;
//             if(Touch_x >= 730) Uart_Printf("Sticker Get Pressed\n");

//             // Sticker
//             if(Touch_x >= 730 && Touch_y >= 750) Custom_Draw_Sticker_Clicked(0);
//             else if(Touch_x >= 730 && Touch_y >= 500) Custom_Draw_Sticker_Clicked(1);
//             else if(Touch_x >= 730 && Touch_y >= 350) Custom_Draw_Sticker_Clicked(2);
//             else if(Touch_x >= 730 && Touch_y >= 100) Lcd_Select_Display_Frame_Buffer(3); // 되돌리기
// 		}
        

        
//         Custom_Display_Sticker_Clicked();

//         // 기존코드
//         /*
// 		Lcd_Control_Brightness(5);
// 		Lcd_Select_Buffer_Mode(LAYER_MODE);
// 	 	Lcd_Select_Draw_Frame_Buffer(0);
// 	 	Lcd_Select_Display_Frame_Buffer(0);
// 		Lcd_Set_Trans_Mode(0);
// 		Lcd_Set_Shape_Mode(0,0);
// 		Lcd_Set_Shape_Back_Image_Mode(0, (void *)0);
// 	 	Lcd_Clr_Screen(RED);
// 		Lcd_Printf(10,10,GREEN,RED,1,1,"Willtek 임베디드 전문가 그룹");

// 		Uart_Printf("==================================================\n");
// 		Uart_Printf("GBOX WT2440L Camera I/F Test     (c)Willtek Corp. \n");
// 		Uart_Printf("==================================================\n");
// 		Uart_Printf("[1] Camera Polling Test                           \n");
// 		Uart_Printf("[2] Camera Interrupt Test                         \n");
// 		Uart_Printf("==================================================\n");
// 		Uart_Printf(">> ");

// 		r = Uart_GetIntNum()-1;
// 		Uart_Get_Pressed();

// 		if((unsigned int)r >= (sizeof(Func_Arr)/sizeof(Func_Arr[0])))
// 		{
// 			Uart_Printf("Wrong Number!\n");
// 			continue;
// 		}

// 		Func_Arr[r]();

// 		Uart_Printf("Test Finished: Press any key to continue \n");
// 		Uart_Get_Char();
//         */
// 	}
// }

// extern volatile int Key_value;

// int Cam_Width  = 320;
// int Cam_Height = 240;
// int Cam_Exp_Mode = 0;


// static int frm = 0;

// void Toggle_Frame(void)
// {
// 	if(frm) frm = 0;
// 	else frm = 1;
// }

// void Camera_Polling_Test(void)
// {
// 	unsigned short *q;

// 	Uart_Printf("\n\nCamera Test (Polling)\n");

// 	Lcd_Select_Draw_Frame_Buffer(frm);
// 	Lcd_Select_Display_Frame_Buffer(frm);
// 	Lcd_Clr_Screen(BLACK);

// 	{
// 		Uart_Printf("Camera test: Press any key for next test\n");
// 		Uart_Printf("UP: Pause\n");
// 		Uart_Printf("DOWN: Play\n");

// 		CAM_Interrupt_Enable(0);
// 		Key_ISR_Enable(1);

// 		Cam_Width  = 320;
// 		Cam_Height = 240;

// 		CAM_Capture_Run(0,0,0);
// 		CAM_Capture_Run(1, Cam_Width, Cam_Height);
// 		CAM_XY_Flip(1,0);

// 		Lcd_Set_Shape_Mode(1, WHITE);

// 		do
// 		{
// 			if((q = CAM_Check_Image_Ready())!=0)
// 			{
// 				Lcd_Select_Draw_Frame_Buffer(frm);
// 				Lcd_Draw_Cam_Image(0, 0, (void *)q, Cam_Width, Cam_Height);
// 				//Lcd_Printf(10,10,GREEN,RED,1,1,"(주)윌텍 교육사업부");
//                 Lcd_Set_Shape_Mode(1, WHITE);
//                 Custom_Draw_Sticker_Clicked();
// 				Lcd_Select_Display_Frame_Buffer(frm);
// 				Toggle_Frame();
// 			}

// 			if(Key_value)
// 			{
// 				if(Key_value == 1) CAM_Capture_Pause();
// 				else if(Key_value == 3) CAM_Capture_Restart();
// 				Key_value = 0;
// 			}

// 		}while(!Uart_Get_Pressed());

// 		Key_ISR_Enable(0);
// 		Lcd_Set_Shape_Mode(0,0);
// 	}
// }


// void Camera_Interrupt_Test(void)
// {
// 	unsigned short *q;

//   	Uart_Printf("\n\nCamera Test (Interrupt)\n");

// 	{
// 		Uart_Printf("Camera test: Press any key for next test\n");
// 		Uart_Printf("UP: Pause\n");
// 		Uart_Printf("DOWN: Play\n");

// 		Cam_Width = 320;
// 		Cam_Height= 240;

// 		CAM_Set_Capture_Status(0);
// 		CAM_Capture_Run(0,0,0);
// 		CAM_Capture_Run(1, Cam_Width, Cam_Height);
// 		CAM_XY_Flip(0,1);

// 		Key_ISR_Enable(1);
// 		CAM_Interrupt_Enable(1);

// 		do
// 		{
// 			if(CAM_Get_Capture_Status() == 2)
// 			{
// 				CAM_Set_Capture_Status(0);
// 				q = CAM_Get_Image_Address();
// 				Lcd_Select_Draw_Frame_Buffer(frm);
// 				Lcd_Draw_Cam_Image(0, 0, (void *)q, Cam_Width, Cam_Height);
// 				Lcd_Printf(10,10,GREEN,RED,1,1,"(주)윌텍 교육사업부");
// 				Lcd_Select_Display_Frame_Buffer(frm);
// 				Toggle_Frame();
// 			}

// 			if(Key_value)
// 			{
// 				if(Key_value == 1) CAM_Capture_Pause();
// 				else if(Key_value == 3) CAM_Capture_Restart();
// 				Key_value = 0;
// 			}

// 		}while(!Uart_Get_Pressed());

// 		CAM_Capture_Run(0,0,0);
// 		CAM_Capture_Run(0, Cam_Width, Cam_Height);
// 		CAM_Interrupt_Enable(0);
// 		Key_ISR_Enable(0);
// 	}
// }

// void Custom_Photo_Info_Setup(void)
// {
//     int i;

//     for(i=0; i<NO_OF_BMP; i++)
//     {
//         img[i].addr = 100 + (i)*5;
//         img[i].addr = Nand_Page_2_Addr(img[i].addr, 0, 0); // 체크 : 페이지번호, 오프셋 위치 설정 어떻게하지? (일단 그냥 0 써놈)
//         Nand_Read(img[i].addr, (U8 *)&img[i].off, 4);
//         Nand_Read(img[i].addr+4, (U8 *)&img[i].w, 2);
//         Nand_Read(img[i].addr+6, (U8 *)&img[i].h, 2);
//     }
// }

// void Custom_Draw_Sticker_Clicked(int pid) // 화면 전환 (스티커 버퍼에 그리기)
// {   
//     P ph;
//     ph.addr = img[pid].addr;
//     unsigned short p[240][320];

//     Uart_Printf("%d Sticker Selected, (size)150*200 in 2nd Layer\n", pid);
//     Lcd_Set_Shape_Mode(1, WHITE);
//     Nand_Read(ph.addr+ph.off, (U8 *)p, ph.w*ph.h*2);
//     Lcd_Draw_Image(50, 20, (void *)p, ph.w, ph.h);
//     Lcd_Set_Shape_Mode(0, 0);

//     //Lcd_Select_Draw_Frame_Buffer(3); // 이전으로 되돌리기 위해 (상태저장)
//     // Lcd_Set_Shape_Mode(0,0);
//     // Lcd_Draw_BMP(0, 0, bck01);
//     // Lcd_Set_Shape_Back_Image_Mode(1, (void *)img01);
//     // Lcd_Clr_Screen(BLACK);
//     // buf_idx = 0; // 다시 초기화 (여기 테스트 다시해보기 오류가능성있음)
// }

// void Custom_Display_Sticker_Clicked(int pid) // 버퍼에 그려진 스티커 사진 보여주기
// {
//     do
//     {
//         Lcd_Wait_Blank();
//         Lcd_Select_Display_Frame_Buffer(buf_idx++ & 0x1);
//         Lcd_Select_Draw_Frame_Buffer(buf_idx & 0x1); // 미리 그려놓기
//         Timer4_Delay(400);
//     } while (!Uart_Get_Pressed());
    
// }


// void Layer_Test(void)
// {
// 	int i;

// 	Uart_Printf("Layer Mode Test, 320*240 4 Layers\n");
// 	Lcd_Select_Buffer_Mode(LAYER_MODE);

// 	for(i=0; i<(sizeof(Disp)/sizeof(Disp[0])); i++) // 각 Layer에 Draw
// 	{
// 		Lcd_Select_Draw_Frame_Buffer(i);
// 		Lcd_Draw_BMP(0,0,Disp[i]);
// 	}

// 	for(i=0; i<(sizeof(Disp)/sizeof(Disp[0])); i++) // 각 Layer 순차 Display
// 	{
// 		Lcd_Select_Display_Frame_Buffer(i);
// 		Timer4_Delay(1000);
// 	}
// }