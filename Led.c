#include "2440addr.h"
#include "macro.h"
#include "device_driver.h"

void Led_Init(void)
{
	Macro_Write_Block(rGPBCON, 0xff, 0x55, 14);
	Macro_Set_Area(rGPBDAT, 0xf, 7);	
}

void Led_Display(int disp)
{
	Macro_Write_Block(rGPBDAT, 0xf, (~disp & 0xf), 7);
}

