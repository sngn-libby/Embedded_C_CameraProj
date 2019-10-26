#include "2440addr.h"
#include "macro.h"
#include "option.h"
#include "device_driver.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Uart0 (Debug Port) 

void Uart_Init(int baud)
{
	// PORT GPIO initial
	Macro_Write_Block(rGPHCON, 0xf, 0xa, 4);

	rUFCON0 = 0x0;
	rUMCON0 = 0x0;
	rULCON0 = 0x3;
	rUCON0  = 0x245;		
	rUBRDIV0= ((unsigned int)(PCLK/16./baud+0.5)-1 );
}

void Uart_Send_Byte(char data)
{
	if(data == '\n')
	{
		while(Macro_Check_Bit_Clear(rUTRSTAT0,1));
	 	Uart_Send_Byte('\r');
	}
	while(Macro_Check_Bit_Clear(rUTRSTAT0,1));
	WrUTXH0(data);
}               

void Uart_Send_String(char *pt)
{
	while(*pt) Uart_Send_Byte(*pt++);
}

void Uart_Printf(char *fmt,...)
{
    va_list ap;
    char string[256];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    Uart_Send_String(string);
    va_end(ap);
}

char Uart_Get_Char(void)
{
	while(!(rUTRSTAT0 & 0x1));
	return RdURXH0();
}

char Uart_Get_Pressed(void)
{
	if(rUTRSTAT0 & 0x1) return RdURXH0();
	else return 0;
}

void Uart_GetString(char *string)
{
    char *string2 = string;
    char c;
    while((c = Uart_Get_Char())!='\r')
    {
        if(c=='\b')
        {
            if( (int)string2 < (int)string )
            {
                Uart_Printf("\b \b");
                string--;
            }
        }
        else
        {
            *string++ = c;
            Uart_Send_Byte(c);
        }
    }
    *string='\0';
    Uart_Send_Byte('\n');
}

int Uart_GetIntNum(void)
{
    char str[30];
    char *string = str;
    int base     = 10;
    int minus    = 0;
    int result   = 0;
    int lastIndex;
    int i;

    Uart_GetString(string);

    if(string[0]=='-')
    {
        minus = 1;
        string++;
    }

    if(string[0]=='0' && (string[1]=='x' || string[1]=='X'))
    {
        base    = 16;
        string += 2;
    }

    lastIndex = strlen(string) - 1;

    if(lastIndex<0)
        return -1;

    if(string[lastIndex]=='h' || string[lastIndex]=='H' )
    {
        base = 16;
        string[lastIndex] = 0;
        lastIndex--;
    }

    if(base==10)
    {
        result = atoi(string);
        result = minus ? (-1*result):result;
    }
    else
    {
        for(i=0;i<=lastIndex;i++)
        {
            if(isalpha(string[i]))
            {
                if(isupper(string[i]))
                    result = (result<<4) + string[i] - 'A' + 10;
                else
                    result = (result<<4) + string[i] - 'a' + 10;
            }
            else
                result = (result<<4) + string[i] - '0';
        }
        result = minus ? (-1*result):result;
    }
    return result;
}

void Uart_ISR_Enable(int rx, int tx, int err)
{
	(rx)  ? Macro_Clear_Bit(rINTSUBMSK, 0) : Macro_Set_Bit(rINTSUBMSK, 0);
	(tx)  ? Macro_Clear_Bit(rINTSUBMSK, 1) : Macro_Set_Bit(rINTSUBMSK, 1);
	(err) ? Macro_Clear_Bit(rINTSUBMSK, 2) : Macro_Set_Bit(rINTSUBMSK, 2);

	if(rx || tx || err)
	{
		rSUBSRCPND = 0x7<<0;
		rSRCPND = 1<<28;
		rINTPND = 1<<28;
		Macro_Clear_Bit(rINTMSK, 28);
	}

	else
	{
		Macro_Set_Bit(rINTMSK, 28);
	}
}
