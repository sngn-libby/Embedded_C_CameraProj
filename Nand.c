/**************** K9F1G08 NAND flash *******************/
// 2K/Page
// 1block=64page (128KB)
// 1024block=1Gb
/*****************************************************/
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "option.h"
#include "2440addr.h"
#include "device_driver.h"
#include "remap.h"

#define GBOX_1_TEST		0
#define GBOX_1					1
#define GBOX_2					1
#define BOARD_SEL			GBOX_2

// REMAP Check Functions 

unsigned int Check_Remap_Address(unsigned int addr, int* err);
unsigned int Check_Remap_Block(unsigned int block);
unsigned int Trace_Nand_Remap(unsigned int start, unsigned int val);

unsigned int Check_Remap_Address(unsigned int addr, int* err)
{
	unsigned int block, page, offset, new_block;

	Nand_Addr_2_Page(addr, &block, &page, &offset);
	*err = 0;
	new_block = Check_Remap_Block(block);
	if(new_block == REMAP_TABLE_DISABLE)
	{
		*err = NAND_REMAP_FAIL;
		return 0;
	}
	if(new_block == block) return addr;
	else return Nand_Page_2_Addr(new_block, page, offset);
}

unsigned int Check_Remap_Block(unsigned int block)
{
	int val = 0;
	int addr;
	
	addr = Nand_Page_2_Addr(NAND_REMAP_TABLE_BLOCK, 0, 0);
	addr = addr + 4096 + (block*2);	
	Format_Nand_Read_Bytes(addr, &val, 2);

	switch (val)
	{
		case 0xFFFF:
			val = block;
			break;
		case 0x0:
			val = Trace_Nand_Remap(2,block);
			break;
		default:
			break;
	}
	if(val == REMAP_TABLE_DISABLE)
	{
		block = 0;
	}
	block = val;

	return block;
}

unsigned int Trace_Nand_Remap(unsigned int start, unsigned int val)
{
	unsigned int addr;
	int block = 0;
	
	addr = Nand_Page_2_Addr(NAND_REMAP_TABLE_BLOCK, 0, 0);
	addr = addr+(2048*start)+(val*2);	
	Format_Nand_Read_Bytes(addr, &block, 2);	 
	switch(block)
	{
		case 0x0000:
			return Trace_Nand_Remap(start+1, val);
			break;
		case 0xFFFF:
			return 	REMAP_TABLE_DISABLE;		
		default:
			return block;
	}
}

// Nand Controller Control Macro

#if BOARD_SEL
	#define NF_nFCE_L()					{rNFCONT&=~(1<<1);}
	#define NF_nFCE_H()					{rNFCONT|=(1<<1);}
#else
	#define NF_nFCE_L()					{rGPADAT&=~(1<<16);}
	#define NF_nFCE_H()					{rGPADAT|=(1<<16);}
#endif

#define NF_RSTECC()					{rNFCONT|=(1<<4);}

#define NF_MECC_UNLOCK()         	{rNFCONT&=~(1<<5);}
#define NF_MECC_LOCK()         		{rNFCONT|=(1<<5);}

#define NF_SECC_UNLOCK()         	{rNFCONT&=~(1<<6);}
#define NF_SECC_LOCK()         		{rNFCONT|=(1<<6);}

#define NF_CLEAR_RB()    			{rNFSTAT |= (1<<2);}
#define NF_DETECT_RB()    			{while(!(rNFSTAT&(1<<2)));}

#define NF_CMD(cmd)					{rNFCMD=cmd;}
#define NF_ADDR(addr)				{rNFADDR=addr;}	

#define NF_RDDATA() 				(rNFDATA)
#define NF_RDDATA8() 				(rNFDATA8)
#define NF_RDDATA16() 				(*(volatile unsigned short *)0x4E000010)

#define NF_WRDATA(data) 			{rNFDATA=data;}
#define NF_WRDATA8(data) 			{rNFDATA8=data;}
#define NF_WRDATA16(data)			{(*(volatile unsigned short *)0x4E000010)=data;}

#define	NF_STATUS_FAIL()			(NF_RDDATA() & 0x01)
#define	NF_STATUS_READY()			(NF_RDDATA() & 0x40)
#define	NF_STATUS_NAND_PROTECTED()	(!(NF_RDDATA() & 0x80))
#define	NF_READ_STATUS()			(NF_RDDATA() & 0xff)
#define NF_STATUS_MECC_ERROR()		((rNFESTAT0 & 0x3) != 0x0)
#define NF_STATUS_SECC_ERROR()		((rNFESTAT0 & 0xc) != 0x0)
#define	NF_ILLEGAL_ACCESS()			(rNFSTAT&0x8)

// 100MHz HCLK

#define TACLS     0 // 0-clk(0ns)  => >=0ns
#define TWRPH0    2 // 2-clk(20ns) => >=12ns 
#define TWRPH1    1 // 2-clk(10ns) => >=10ns, TWRPH0+TWRPH1 >=25ns

// functions
unsigned int Nand_Page_2_Addr(unsigned int block, unsigned int page, unsigned int offset);
void Nand_Addr_2_Page(unsigned int addr, unsigned int * block, unsigned int * page, unsigned int * offset);

void Nand_Init(void);
void Nand_Reset(void);
int Nand_Check_Device(void);

unsigned int Nand_Erase_Block(unsigned int block);
unsigned int Nand_Verify_Erased_Page(unsigned int block, unsigned int page);
unsigned int Nand_Verify_Erased_Block(unsigned int block);

unsigned int Nand_Write_Block(unsigned int block,U8 *buffer);
unsigned int Nand_Read_Block(unsigned int block,U8 *buffer);
unsigned int Nand_Verify_Block(unsigned int block,U8 *buffer);

unsigned int Nand_Write_Page(unsigned int block,unsigned int page,U8 *buffer);
unsigned int Nand_Read_Page(unsigned int block,unsigned int page,U8 *buffer);
unsigned int Nand_Verify_Page(unsigned int block,unsigned int page,U8 *buffer);

unsigned int Nand_Write_Bytes(unsigned int addr, int data, int byte);
unsigned int Nand_Read_Bytes(unsigned int addr, int *data, int byte);
unsigned int Nand_Verify_Bytes(unsigned int addr, int *data, int byte);

unsigned int Nand_Write_Small_Area(unsigned int addr, U8 *buf, unsigned int size);
unsigned int Nand_Read_Small_Area(unsigned int addr, U8 *buf, unsigned int size);
unsigned int Nand_Verify_Small_Area(unsigned int addr, U8 *buf, unsigned int size);
unsigned int Nand_Verify_Erased_Small_Area(unsigned int addr, unsigned int size);

unsigned int Nand_Write(unsigned int addr, U8 *buf, unsigned int size);
unsigned int Nand_Read(unsigned int addr, U8 *buf, unsigned int size);
unsigned int Nand_Verify(unsigned int addr, U8 *buf, unsigned int size);
unsigned int Nand_Verify_Erased(unsigned int addr, unsigned int size);

int NF8_Read_nByte_SA(int addr, int *data, int byte);

/*	포맷용 NAND함수 - remap table을 참조 하지 않는다. 	*/
unsigned int Format_Nand_Write_Block(unsigned int block, U8 *buffer);
unsigned int Format_Nand_Verify_Block(unsigned int block,U8 *buffer);
unsigned int Format_Nand_Erase_Block(unsigned int block);
unsigned int Format_Nand_Verify_Erased_Block(unsigned int block);
unsigned int Format_Nand_Read_Block(unsigned int block,U8 *buffer);
unsigned int Format_Nand_Read_Bytes(unsigned int addr, int *data, int byte);
unsigned int Format_Nand_Read_Page(unsigned int block,unsigned int page,U8 *buffer);
unsigned int Format_Nand_Verify_Erased_Page(unsigned int block, unsigned int page);
unsigned int Format_Nand_Verify_Page(unsigned int block,unsigned int page,U8 *buffer);
unsigned int Format_Nand_Write_Page(unsigned int block,unsigned int page,U8 *buffer);

unsigned int Nand_Page_2_Addr(unsigned int block, unsigned int page, unsigned int offset)
{
	return ((block & 0x3ff)<<17)+((page & 0x3f)<<11)+(offset & 0x7ff);
}

void Nand_Addr_2_Page(unsigned int addr, unsigned int * block, unsigned int * page, unsigned int * offset)
{
	*block = (addr >> 17) & 0x3ff;
	*page = (addr >> 11) & 0x3f;
	*offset = addr & 0x7ff;
}

void Nand_Init(void)
{
	rGPACON = (rGPACON &~(0x1<<16));
	rGPADAT|=(1<<16);
	rGPACON = (rGPACON &~(0x3f<<17)) | (0x3f<<17);
	rNFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4)|(1<<3)|(1<<2)|(0<<1)|(0<<0);	
	rNFCONT = (0<<13)|(0<<12)|(0<<10)|(0<<9)|(0<<8)|(1<<6)|(1<<5)|(1<<4)|(1<<1)|(1<<0);
	Nand_Reset();
}

void Nand_Reset(void)
{
	NF_nFCE_L();
	NF_CLEAR_RB();
	NF_CMD(0xFF);	
	NF_DETECT_RB();
	NF_nFCE_H();
}

int Nand_Check_Device(void)
{
	int id;
  
	NF_nFCE_L();
	NF_CMD(0x90);
	NF_ADDR(0x0);

	id=NF_RDDATA8();		// Maker code 0xec => negrect
	id=NF_RDDATA8()<<24;	// 2nd code Devide code (2442 내부 NAND => 0xa1), K9F1G(0xF1)
	id|=NF_RDDATA8()<<16;	// 3rd code (0x80, 0x00)
	id|=NF_RDDATA8()<<8;	// 4th code (0x15, 0x95)
	id|=NF_RDDATA8();		// 5th code (0x40)
	NF_nFCE_H();

	return id;
}

unsigned int Nand_Erase_Block(unsigned int block)
{
	int r;
	r = Check_Remap_Block(block);
	if(r == REMAP_TABLE_DISABLE)	return NAND_REMAP_FAIL;
	block = r;	
	
	block = (block<<6);
	
	NF_nFCE_L();
	NF_CMD(0x70);   
	
	if(NF_STATUS_NAND_PROTECTED()) 
	{
		NF_nFCE_H();
		return NAND_PROTECTED;
	}
	
	NF_CMD(0x60);  
	
	NF_ADDR(block & 0xff);	
	NF_ADDR((block>>8) & 0xff);   
	
	NF_CLEAR_RB();
	NF_CMD(0xd0);	
	
	if(NF_ILLEGAL_ACCESS()) 
	{
		NF_nFCE_H();	
		return NAND_LOCKED;
	}
	
	NF_DETECT_RB();
	NF_CMD(0x70);   
	
	if(NF_STATUS_FAIL()) 
	{	
		NF_nFCE_H();
		return NAND_BAD_BLOCK;
	}
	
	NF_nFCE_H();
	return NAND_SUCCESS;
}

unsigned int Nand_Verify_Erased_Page(unsigned int block, unsigned int page)
{
	int i,r;
	unsigned int temp1;
	
	r = Check_Remap_Block(block);
	if(r == REMAP_TABLE_DISABLE)	return NAND_REMAP_FAIL;
	block = r;	
	    
	block = (block<<6)+page;

	rNFMECCD0 = 0;
	rNFMECCD1 = 0;
	
	NF_nFCE_L();
	NF_CMD(0x00);	
	NF_ADDR(0); 	
	NF_ADDR(0);		
	NF_ADDR((block)&0xff);	
	NF_ADDR((block>>8)&0xff);
	NF_CLEAR_RB();
	NF_CMD(0x30);	// 2nd command
	NF_DETECT_RB();

	// Data + SE
	for(i=0;i<((BYTE_PER_PAGE+SE_PER_PAGE)/4);i++) 
	{
		temp1 = NF_RDDATA();
		if(temp1 != 0xffffffff)
		{
			NF_nFCE_H();
			return NAND_VERIFY_FAIL;
		}
	}

	NF_nFCE_H();
	return NAND_SUCCESS;
}

unsigned int Nand_Verify_Erased_Block(unsigned int block)
{
	int i, res,r;
	
	
	r = Check_Remap_Block(block);
	if(r == REMAP_TABLE_DISABLE)	return NAND_REMAP_FAIL;
	block = r;
	
	for(i=0; i<PAGE_PER_BLOCK; i++)
	{
		if((res = Nand_Verify_Erased_Page(block, i)) != NAND_SUCCESS)
		{
			return res;
			break;
		}
	}
	
	return NAND_SUCCESS;
}

unsigned int Nand_Write_Block(unsigned int block, U8 *buffer)
{
	int i, r;
	
	r = Check_Remap_Block(block);
	if(r == REMAP_TABLE_DISABLE)	return NAND_REMAP_FAIL;
	block = r;
	
	for(i =0 ; i<PAGE_PER_BLOCK; i++)
	{
		r = Nand_Write_Page(block, i, buffer+(i*BYTE_PER_PAGE));
		if(r != NAND_SUCCESS) return r;
	}
	
	return NAND_SUCCESS;
}

unsigned int Nand_Read_Block(unsigned int block,U8 *buffer)
{
	int i, r;
	
	r = Check_Remap_Block(block);
	if(r == REMAP_TABLE_DISABLE)	return NAND_REMAP_FAIL;
	
	block = r;
	
	for(i =0 ; i<PAGE_PER_BLOCK; i++)
	{
		r = Nand_Read_Page(block, i, buffer+(i*BYTE_PER_PAGE));
		if(r != NAND_SUCCESS) return r;
	}
	
	return NAND_SUCCESS;
}
	
unsigned int Nand_Verify_Block(unsigned int block,U8 *buffer)
{
	int i, r;
	
	r = Check_Remap_Block(block);
	if(r == REMAP_TABLE_DISABLE)	return NAND_REMAP_FAIL;
	block = r;
	
	for(i =0 ; i<PAGE_PER_BLOCK; i++)
	{
		r = Nand_Verify_Page(block, i, buffer+(i*BYTE_PER_PAGE));
		if(r != NAND_SUCCESS) return r;
	}
	
	return NAND_SUCCESS;
}

unsigned int Nand_Write_Page(unsigned int block,unsigned int page,U8 *buffer)
{
	int i, r;
	
	r = Check_Remap_Block(block);
	if(r == REMAP_TABLE_DISABLE)	return NAND_REMAP_FAIL;
	block = r;
	
	block = (block<<6)+page;

	NF_nFCE_L();
	NF_CMD(0x70);   

	if(NF_STATUS_NAND_PROTECTED()) 
	{
		NF_nFCE_H();
		return NAND_PROTECTED;
	}
	
	NF_CMD(0x80);   
	NF_ADDR(0);			    
	NF_ADDR(0);			    
	NF_ADDR(block&0xff);	   
	NF_ADDR((block>>8)&0xff);  

	for(i=0;i<BYTE_PER_PAGE;i++)
	{
		NF_WRDATA8(*buffer++);
    }

 	NF_CLEAR_RB();
	NF_CMD(0x10);

	if(NF_ILLEGAL_ACCESS()) 
	{
		NF_nFCE_H();	
		return NAND_LOCKED;
	}
	
	NF_DETECT_RB();
	NF_CMD(0x70);

    if(NF_STATUS_FAIL()) 
    {	
		NF_nFCE_H();
		return NAND_BAD_PAGE;
	}

	NF_nFCE_H();
	return NAND_SUCCESS;
}

unsigned int Nand_Read_Page(unsigned int block,unsigned int page,U8 *buffer)
{
   	int i,r;
	
	r = Check_Remap_Block(block);
	if(r == REMAP_TABLE_DISABLE)	return NAND_REMAP_FAIL;
	block = r;

	block = (block<<6)+page;

	rNFMECCD0 = 0;
	rNFMECCD1 = 0;
	
	NF_nFCE_L();
	NF_CMD(0x00);	
	NF_ADDR(0); 	
	NF_ADDR(0);		
	NF_ADDR((block)&0xff);	
	NF_ADDR((block>>8)&0xff);
	NF_CLEAR_RB();
	NF_CMD(0x30);	
	NF_DETECT_RB();

	for(i=0;i<BYTE_PER_PAGE;i++)
	{
		*buffer=NF_RDDATA8();
		buffer++;
	}
	
	NF_nFCE_H();
	return NAND_SUCCESS;
}

unsigned int Nand_Verify_Page(unsigned int block,unsigned int page,U8 *buffer)
{
   	int i,r;
	unsigned char temp0, temp1;
	
	r = Check_Remap_Block(block);
	if(r == REMAP_TABLE_DISABLE)	return NAND_REMAP_FAIL;
	block = r;
	    
	block = (block<<6)+page;

	NF_nFCE_L();
	NF_CMD(0x00);	
	NF_ADDR(0); 	
	NF_ADDR(0);		
	NF_ADDR((block)&0xff);	
	NF_ADDR((block>>8)&0xff);
	NF_CLEAR_RB();
	NF_CMD(0x30);
	NF_DETECT_RB();

	for(i=0;i<BYTE_PER_PAGE;i++)
	{
		temp0 = *buffer;
		temp1 = NF_RDDATA8();

		buffer++;

		if(temp0 != temp1)
		{
			NF_nFCE_H();
			return NAND_VERIFY_FAIL;
		}
	}

	NF_nFCE_H();
	return NAND_SUCCESS;
}

unsigned int Nand_Write_Bytes(unsigned int addr, int data, int byte)
{
	int r, err;

	r = Check_Remap_Address(addr, &err);
	
	if(err)	return NAND_REMAP_FAIL;
	addr = r;
	
	switch(byte)
	{
		case 2: if(addr%2) return NAND_BAD_ALIGN; break;
		case 4: if(addr%4) return NAND_BAD_ALIGN; break;
	}

	NF_nFCE_L();
	NF_CMD(0x70);   

	if(NF_STATUS_NAND_PROTECTED()) 
	{
		NF_nFCE_H();
		return NAND_PROTECTED;
	}
	
	NF_CMD(0x80);   
	NF_ADDR(addr);
	NF_ADDR((addr >> 8) & 0x7);
	NF_ADDR(addr >> 11);	   
	NF_ADDR(addr >> 19);  

	switch(byte)
	{
		case 1: NF_WRDATA8((U8)data); 	break;
		case 2: NF_WRDATA16((U16)data); break;
		case 4: NF_WRDATA((unsigned int)data); break;
	}

 	NF_CLEAR_RB();
	NF_CMD(0x10);

	if(NF_ILLEGAL_ACCESS()) 
	{
		NF_nFCE_H();	
		return NAND_LOCKED;
	}
	
	NF_DETECT_RB();
	NF_CMD(0x70);

    if(NF_STATUS_FAIL()) 
    {	
		NF_nFCE_H();
		return NAND_ACCESS_FAIL;
	}

	NF_nFCE_H();
	return NAND_SUCCESS;
}

unsigned int Nand_Read_Bytes(unsigned int addr, int *data, int byte)
{
	int r,err;
	
	r = Check_Remap_Address(addr,&err);
	if(err)	return NAND_REMAP_FAIL;
	addr = r;
	
	switch(byte)
	{
		case 2: if(addr%2) return NAND_BAD_ADDRESS; break;
		case 4: if(addr%4) return NAND_BAD_ADDRESS; break;
	}
		
	NF_nFCE_L();
	NF_CMD(0x00);	
	NF_ADDR(addr);
	NF_ADDR((addr >> 8) & 0x7);
	NF_ADDR(addr >> 11);	   
	NF_ADDR(addr >> 19);  
	NF_CLEAR_RB();
	NF_CMD(0x30);	
	NF_DETECT_RB();

	switch(byte)
	{
		case 1: *data = NF_RDDATA8() & 0xff; 	break;
		case 2: *data = NF_RDDATA16() & 0xffff; break;
		case 4: *data = NF_RDDATA(); break;
	}

	NF_nFCE_H();
	return NAND_SUCCESS;
}

unsigned int Nand_Verify_Bytes(unsigned int addr, int *data, int byte)
{
	int temp0 = 0, temp1 = 0, r, err = 0;
	
	r = Check_Remap_Address(addr, &err);
	if(err)	return NAND_REMAP_FAIL;
	addr = r;
	
	switch(byte)
	{
		case 2: if(addr%2) return NAND_BAD_ADDRESS; break;
		case 4: if(addr%4) return NAND_BAD_ADDRESS; break;
	}
		
	NF_nFCE_L();
	NF_CMD(0x00);	
	NF_ADDR(addr);
	NF_ADDR((addr >> 8) & 0x7);
	NF_ADDR(addr >> 11);	   
	NF_ADDR(addr >> 19);  
	NF_CLEAR_RB();
	NF_CMD(0x30);	
	NF_DETECT_RB();

	switch(byte)
	{
		case 1: temp0 = NF_RDDATA8() & 0xff; 	temp1 = *data & 0xff;	break;
		case 2: temp0 = NF_RDDATA16() & 0xffff; temp1 = *data & 0xffff;	break;
		case 4: temp0 = NF_RDDATA();			temp1 = *data; break;
	}

	NF_nFCE_H();
		
	if(temp0 != temp1)
	{
		return NAND_VERIFY_FAIL;	
	}

	return NAND_SUCCESS;
}

unsigned int Nand_Write_Small_Area(unsigned int addr, U8 *buf, unsigned int size)
{
	int i, r, err;
	
	// All data must be positioned within same page
	
	r = Check_Remap_Address(addr, &err);
	if(err)	return NAND_REMAP_FAIL;
	addr = r;
	
	if((addr / BYTE_PER_PAGE) != ((addr + size - 1) / BYTE_PER_PAGE))
	{
		return NAND_PAGE_OVER;
	}

	NF_nFCE_L();
	NF_CMD(0x70);   

	if(NF_STATUS_NAND_PROTECTED()) 
	{
		NF_nFCE_H();
		return NAND_PROTECTED;
	}
	
	NF_CMD(0x80);
	NF_ADDR(addr);
	NF_ADDR((addr >> 8) & 0x7);
	NF_ADDR(addr >> 11);	   
	NF_ADDR(addr >> 19);  

	for(i=0; i<((unsigned int)buf % 4); i++)
	{
		NF_WRDATA8(*buf++);
		size--;
	}

	for(i=0; i<(size / 4); i++)
	{
		NF_WRDATA(*(unsigned int *)buf);
		buf += 4;
	}
	
	for(i=0; i<(size % 4); i++)
	{
		NF_WRDATA8(*buf++);
	}

 	NF_CLEAR_RB();
	NF_CMD(0x10);	

	if(NF_ILLEGAL_ACCESS()) 
	{
		NF_nFCE_H();	
		return NAND_LOCKED;
	}
	
	NF_DETECT_RB();
	NF_CMD(0x70);  

    if(NF_STATUS_FAIL()) 
    {	
		NF_nFCE_H();
		return NAND_ACCESS_FAIL;
	}

	NF_nFCE_H();
	return NAND_SUCCESS;
}

unsigned int Nand_Read_Small_Area(unsigned int addr, U8 *buf, unsigned int size)
{
	int i, r, err;
	
	// All data must be positioned within same page

	r = Check_Remap_Address(addr, &err);

	if(err)	return NAND_REMAP_FAIL;
	addr = r;
	
	if((addr / BYTE_PER_PAGE) != ((addr + size - 1) / BYTE_PER_PAGE))
	{
		return NAND_PAGE_OVER;
	}
	
	NF_nFCE_L();
	NF_CMD(0x00);	
	NF_ADDR(addr);
	NF_ADDR((addr >> 8) & 0x7);
	NF_ADDR(addr >> 11);	   
	NF_ADDR(addr >> 19);  
	NF_CLEAR_RB();
	NF_CMD(0x30);	
	NF_DETECT_RB();

	for(i=0; i<((unsigned int)buf % 4); i++)
	{
		*buf++ = NF_RDDATA8();
		size--;
	}

	for(i=0; i<(size / 4); i++)
	{
		*(unsigned int *)buf = NF_RDDATA();	
		buf += 4;
	}
	
	for(i=0; i<(size % 4); i++)
	{
		*buf++ = NF_RDDATA8();
	}

	NF_nFCE_H();
	return NAND_SUCCESS;	
}

unsigned int Nand_Verify_Small_Area(unsigned int addr, U8 *buf, unsigned int size)
{
	int i, r, err;
	
	// All data must be positioned within same page
	
	r = Check_Remap_Address(addr, &err);
	if(err)	return NAND_REMAP_FAIL;
	addr = r;
	
	if((addr / BYTE_PER_PAGE) != ((addr + size - 1) / BYTE_PER_PAGE))
	{
		return NAND_PAGE_OVER;
	}

	NF_nFCE_L();
	NF_CMD(0x00);	
	NF_ADDR(addr);
	NF_ADDR((addr >> 8) & 0x7);
	NF_ADDR(addr >> 11);	   
	NF_ADDR(addr >> 19);  
	NF_CLEAR_RB();
	NF_CMD(0x30);
	NF_DETECT_RB();

	for(i=0; i<((unsigned int)buf % 4); i++)
	{
		if(*buf++ != NF_RDDATA8())
		{
			NF_nFCE_H();
			return NAND_VERIFY_FAIL;
		}
		size--;
	}

	for(i=0; i<(size / 4); i++)
	{
		if(*(unsigned int *)buf != NF_RDDATA())
		{
			NF_nFCE_H();
			return NAND_VERIFY_FAIL;
		}
		buf += 4;
	}
	
	for(i=0; i<(size % 4); i++)
	{
		if(*buf++ != NF_RDDATA8())
		{
			NF_nFCE_H();
			return NAND_VERIFY_FAIL;
		}
	}

	NF_nFCE_H();
	return NAND_SUCCESS;	
}

unsigned int Nand_Verify_Erased_Small_Area(unsigned int addr, unsigned int size)
{
	int i, r, err;
	
	// All data must be positioned within same page
	
	r = Check_Remap_Address(addr, &err);
	if(err)	return NAND_REMAP_FAIL;
	addr = r;
	
	if((addr / BYTE_PER_PAGE) != ((addr + size - 1) / BYTE_PER_PAGE))
	{
		return NAND_PAGE_OVER;
	}

	NF_nFCE_L();
	NF_CMD(0x00);	
	NF_ADDR(addr);
	NF_ADDR((addr >> 8) & 0x7);
	NF_ADDR(addr >> 11);	   
	NF_ADDR(addr >> 19);  
	NF_CLEAR_RB();
	NF_CMD(0x30);
	NF_DETECT_RB();

	for(i=0; i<size; i++)
	{
		if(0xff != NF_RDDATA8())
		{
			NF_nFCE_H();
			return NAND_VERIFY_FAIL;
		}
	}

	NF_nFCE_H();
	return NAND_SUCCESS;	
}

unsigned int Nand_Write(unsigned int addr, U8 *buf, unsigned int size)
{
	int i;
	unsigned int res;
	unsigned int temp;
	
	temp = BYTE_PER_PAGE - addr % BYTE_PER_PAGE;
	
	if(temp > size)
	{
		res = Nand_Write_Small_Area(addr, buf, size);
		return res;
	}
	else if(temp != 0)
	{
		res = Nand_Write_Small_Area(addr, buf, temp);
		if(res != NAND_SUCCESS) return res;
		buf += temp; size -= temp; addr += temp;
	}
	
	temp = size;
	
	for(i=0; i<(temp / BYTE_PER_PAGE); i++)
	{
		res = Nand_Write_Page((addr>>17)&0x3ff, (addr>>11)&0x3f, buf);
		if(res != NAND_SUCCESS) return res;
		buf += BYTE_PER_PAGE; size -= BYTE_PER_PAGE; addr += BYTE_PER_PAGE;
	}
	
	if(size > 0)
	{
		res = Nand_Write_Small_Area(addr, buf, size % BYTE_PER_PAGE);
		return res;
	}
	
	return NAND_SUCCESS;
}

unsigned int Nand_Read(unsigned int addr, U8 *buf, unsigned int size)
{
	int i;
	unsigned int res;
	unsigned int temp;
	
	temp = BYTE_PER_PAGE - addr % BYTE_PER_PAGE;
	
	if(temp >= size)
	{
		res = Nand_Read_Small_Area(addr, buf, size);
		return res;
	}
	else if(temp != 0)
	{
		res = Nand_Read_Small_Area(addr, buf, temp);
		if(res != NAND_SUCCESS) return res;
		buf += temp; size -= temp; addr += temp;		
	}

	temp = size;
	
	for(i=0; i<(temp / BYTE_PER_PAGE); i++)
	{
		res = Nand_Read_Page((addr>>17)&0x3ff, (addr>>11)&0x3f, buf);
		if(res != NAND_SUCCESS) return res;
		buf += BYTE_PER_PAGE;
		size -= BYTE_PER_PAGE;
		addr += BYTE_PER_PAGE;
	}

	if(size > 0)
	{
		res = Nand_Read_Small_Area(addr, buf, size % BYTE_PER_PAGE);
		return res;
	}

	return NAND_SUCCESS;	
}

unsigned int Nand_Verify(unsigned int addr, U8 *buf, unsigned int size)
{
	int i;
	unsigned int res;
	unsigned int temp;
	
	temp = BYTE_PER_PAGE - addr % BYTE_PER_PAGE;
	
	if(temp > size)
	{
		res = Nand_Verify_Small_Area(addr, buf, size);
		return res;
	}
	else if(temp != 0)
	{
		res = Nand_Verify_Small_Area(addr, buf, temp);
		if(res != NAND_SUCCESS) return res;
		buf += temp; size -= temp; addr += temp;
	}
	
	temp = size;
	
	for(i=0; i<(temp / BYTE_PER_PAGE); i++)
	{
		res = Nand_Verify_Page((addr>>17)&0x3ff, (addr>>11)&0x3f, buf);
		if(res != NAND_SUCCESS) return res;
		buf += BYTE_PER_PAGE; size -= BYTE_PER_PAGE; addr += BYTE_PER_PAGE;
	}

	if(size > 0)
	{	
		res = Nand_Verify_Small_Area(addr, buf, size % BYTE_PER_PAGE);
		return res;
	}
	
	return NAND_SUCCESS;
}

unsigned int Nand_Verify_Erased(unsigned int addr, unsigned int size)
{
	int i;
	unsigned int res;
	unsigned int temp;
	
	temp = BYTE_PER_PAGE - addr % BYTE_PER_PAGE;
	
	if(temp > size)
	{
		res = Nand_Verify_Erased_Small_Area(addr, size);
		return res;
	}
	else if(temp != 0)
	{
		res = Nand_Verify_Erased_Small_Area(addr, temp);
		if(res != NAND_SUCCESS) return res;
		size -= temp; addr += temp;
	}
	
	temp = size;
	
	for(i=0; i<(temp / BYTE_PER_PAGE); i++)
	{
		res = Nand_Verify_Erased_Page((addr>>17)&0x3ff, (addr>>11)&0x3f);
		if(res != NAND_SUCCESS) return res;
		size -= BYTE_PER_PAGE; addr += BYTE_PER_PAGE;
	}

	if(size > 0)
	{	
		res = Nand_Verify_Erased_Small_Area(addr, size % BYTE_PER_PAGE);
		return res;
	}
	
	return NAND_SUCCESS;
}


int NF8_Read_nByte_SA(int addr, int *data, int byte)
{
   	switch(byte)
	{
		case 2: if(addr%2) return NAND_BAD_ADDRESS; break;
		case 4: if(addr%4) return NAND_BAD_ADDRESS; break;
	}
		
	NF_nFCE_L();
	NF_CMD(0x00);	
	NF_ADDR(addr);
	NF_ADDR(((addr >> 8) & 0x7) | 0x8 );
	NF_ADDR(addr >> 11);	   
	NF_ADDR(addr >> 19);  
	NF_CLEAR_RB();
	NF_CMD(0x30);	// 2nd command
	NF_DETECT_RB();

	switch(byte)
	{
		case 1: *data = NF_RDDATA8() & 0xff; break;
		case 2: *data = NF_RDDATA16() & 0xffff; break;
		case 4: *data = NF_RDDATA(); break;
	}

	NF_nFCE_H();
	return NAND_SUCCESS;
}

unsigned int Format_Nand_Write_Block(unsigned int block, U8 *buffer)
{
	int i, r;
	

	for(i =0 ; i<PAGE_PER_BLOCK; i++)
	{
		r = Format_Nand_Write_Page(block, i, buffer+(i*BYTE_PER_PAGE));
		if(r != NAND_SUCCESS) return r;
	}
	
	return NAND_SUCCESS;
}

unsigned int Format_Nand_Verify_Block(unsigned int block,U8 *buffer)
{
	int i, r;
	
	for(i =0 ; i<PAGE_PER_BLOCK; i++)
	{
		r = Format_Nand_Verify_Page(block, i, buffer+(i*BYTE_PER_PAGE));
		if(r != NAND_SUCCESS) return r;
	}
	
	return NAND_SUCCESS;
}

unsigned int Format_Nand_Erase_Block(unsigned int block)
{
	
	block = (block<<6);
	
	NF_nFCE_L();
	NF_CMD(0x70);   
	
	if(NF_STATUS_NAND_PROTECTED()) 
	{
		NF_nFCE_H();
		return NAND_PROTECTED;
	}
	
	NF_CMD(0x60);  
	
	NF_ADDR(block & 0xff);	
	NF_ADDR((block>>8) & 0xff);   
	
	NF_CLEAR_RB();
	NF_CMD(0xd0);	
	
	if(NF_ILLEGAL_ACCESS()) 
	{
		NF_nFCE_H();	
		return NAND_LOCKED;
	}
	
	NF_DETECT_RB();
	NF_CMD(0x70);   
	
	if(NF_STATUS_FAIL()) 
	{	
		NF_nFCE_H();
		return NAND_BAD_BLOCK;
	}
	
	NF_nFCE_H();
	return NAND_SUCCESS;
}

unsigned int Format_Nand_Verify_Erased_Block(unsigned int block)
{
	int i, res;
	
	for(i=0; i<PAGE_PER_BLOCK; i++)
	{
		if((res = Format_Nand_Verify_Erased_Page(block, i)) != NAND_SUCCESS)
		{
			return res;
			break;
		}
	}
	
	return NAND_SUCCESS;
}

unsigned int Format_Nand_Read_Block(unsigned int block,U8 *buffer)
{
	int i, r;
	

	for(i =0 ; i<PAGE_PER_BLOCK; i++)
	{
		r = Format_Nand_Read_Page(block, i, buffer+(i*BYTE_PER_PAGE));
		if(r != NAND_SUCCESS) return r;
	}
	
	return NAND_SUCCESS;
}

unsigned int Format_Nand_Read_Bytes(unsigned int addr, int *data, int byte)
{
	switch(byte)
	{
		case 2: if(addr%2) return NAND_BAD_ADDRESS; break;
		case 4: if(addr%4) return NAND_BAD_ADDRESS; break;
	}
		
	NF_nFCE_L();
	NF_CMD(0x00);	
	NF_ADDR(addr);
	NF_ADDR((addr >> 8) & 0x7);
	NF_ADDR(addr >> 11);	   
	NF_ADDR(addr >> 19);  
	NF_CLEAR_RB();
	NF_CMD(0x30);	
	NF_DETECT_RB();

	switch(byte)
	{
		case 1: *data = NF_RDDATA8() & 0xff; 	break;
		case 2: *data = NF_RDDATA16() & 0xffff; break;
		case 4: *data = NF_RDDATA(); break;
	}

	NF_nFCE_H();
	return NAND_SUCCESS;
}

unsigned int Format_Nand_Read_Page(unsigned int block,unsigned int page,U8 *buffer)
{
   	int i;
	unsigned int *buf_ptr = (unsigned int *)buffer;
	
	block = (block<<6)+page;

	rNFMECCD0 = 0;
	rNFMECCD1 = 0;
	
	NF_nFCE_L();
	NF_CMD(0x00);	
	NF_ADDR(0); 	
	NF_ADDR(0);		
	NF_ADDR((block)&0xff);	
	NF_ADDR((block>>8)&0xff);
	NF_CLEAR_RB();
	NF_CMD(0x30);	
	NF_DETECT_RB();

	for(i=0;i<(BYTE_PER_PAGE/4);i++) 
	{
		*buf_ptr=NF_RDDATA();
		buf_ptr++;		
	}
	
	NF_nFCE_H();
	return NAND_SUCCESS;
}

unsigned int Format_Nand_Verify_Erased_Page(unsigned int block, unsigned int page)
{
	int i;
	unsigned int temp1;
	
	block = (block<<6)+page;

	rNFMECCD0 = 0;
	rNFMECCD1 = 0;
	
	NF_nFCE_L();
	NF_CMD(0x00);	
	NF_ADDR(0); 	
	NF_ADDR(0);		
	NF_ADDR((block)&0xff);	
	NF_ADDR((block>>8)&0xff);
	NF_CLEAR_RB();
	NF_CMD(0x30);	// 2nd command
	NF_DETECT_RB();

	// Data + SE
	for(i=0;i<((BYTE_PER_PAGE+SE_PER_PAGE)/4);i++) 
	{
		temp1 = NF_RDDATA();
		if(temp1 != 0xffffffff)
		{
			NF_nFCE_H();
			return NAND_VERIFY_FAIL;
		}
	}

	NF_nFCE_H();
	return NAND_SUCCESS;
}

unsigned int Format_Nand_Verify_Page(unsigned int block,unsigned int page,U8 *buffer)
{
   	int i;
	unsigned int temp0, temp1;
	unsigned int *buf_ptr= (unsigned int *)buffer;
	    
	block = (block<<6)+page;

	NF_nFCE_L();
	NF_CMD(0x00);	
	NF_ADDR(0); 	
	NF_ADDR(0);		
	NF_ADDR((block)&0xff);	
	NF_ADDR((block>>8)&0xff);
	NF_CLEAR_RB();
	NF_CMD(0x30);
	NF_DETECT_RB();

	for(i=0;i<(BYTE_PER_PAGE/4);i++) 
	{
		temp0 = *buf_ptr;
		temp1 = NF_RDDATA();

		buf_ptr++;

		if(temp0 != temp1)
		{
			NF_nFCE_H();
			return NAND_VERIFY_FAIL;
		}
	}

	NF_nFCE_H();
	return NAND_SUCCESS;
}

unsigned int Format_Nand_Write_Page(unsigned int block,unsigned int page,U8 *buffer)
{
	int i;
	unsigned int *buf_ptr= (unsigned int *)buffer;
	
	block = (block<<6)+page;

	NF_nFCE_L();
	NF_CMD(0x70);   

	if(NF_STATUS_NAND_PROTECTED()) 
	{
		NF_nFCE_H();
		return NAND_PROTECTED;
	}
	
	NF_CMD(0x80);   
	NF_ADDR(0);			    
	NF_ADDR(0);			    
	NF_ADDR(block&0xff);	   
	NF_ADDR((block>>8)&0xff);  

	for(i=0;i<(BYTE_PER_PAGE/4);i++) 
	{
		NF_WRDATA(*buf_ptr++);
    }

 	NF_CLEAR_RB();
	NF_CMD(0x10);

	if(NF_ILLEGAL_ACCESS()) 
	{
		NF_nFCE_H();	
		return NAND_LOCKED;
	}
	
	NF_DETECT_RB();
	NF_CMD(0x70);

    if(NF_STATUS_FAIL()) 
    {	
		NF_nFCE_H();
		return NAND_BAD_PAGE;
	}

	NF_nFCE_H();
	return NAND_SUCCESS;
}
