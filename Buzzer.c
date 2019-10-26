#include "2440addr.h"
#include "macro.h"
#include "device_driver.h"
#include "option.h"

void Timer3_Buzzer_Init(void)
{
	Macro_Write_Block(rTCFG0, 0xff, 0, 8);
	Macro_Write_Block(rTCFG1, 0xf, 2, 12);
	Macro_Clear_Bit(rGPBDAT, 3);
	Macro_Write_Block(rGPBCON, 0x3, 2, 6);
}

#define BASE 			500
#define TIMER3_TICK		(PCLK/8)

const static unsigned int Tone_value[] = {261,277,293,311,329,349,369,391,415,440,466,493,523,554,587,622,659,698,739,783,830,880,932,987};
const static unsigned int Note_delay[] = {BASE/4, BASE/2, BASE, BASE*2, BASE*4};

void Timer3_Buzzer_Beep(int tone, int duration)
{
	rTCNTB3 = TIMER3_TICK/Tone_value[tone]- 1;
	rTCMPB3 = rTCNTB3/2;

	Macro_Write_Block(rTCON, 0xf, 0xe, 16);
	Macro_Write_Block(rTCON, 0xf, 0xd, 16);
	Timer0_Delay(Note_delay[duration]);
	Macro_Clear_Bit(rTCON, 16);
}
