#include "option.h"
#include "2440slib.h"

#define DESC_SEC	(0x2|(1<<4))
#define CB			(3<<2)  //cache_on, write_back
#define CNB			(2<<2)  //cache_on, write_through 
#define NCB        	(1<<2)  //cache_off,WR_BUF on
#define NCNB		(0<<2)  //cache_off,WR_BUF off
#define AP_RW		(3<<10) //supervisor=RW, user=RW
#define AP_RO		(2<<10) //supervisor=RW, user=RO

#define DOMAIN_FAULT	(0x0)
#define DOMAIN_CHK	(0x1) 
#define DOMAIN_NOTCHK	(0x3) 
#define DOMAIN0		(0x0<<5)
#define DOMAIN1		(0x1<<5)

#define DOMAIN0_ATTR	(DOMAIN_CHK<<0) 
#define DOMAIN1_ATTR	(DOMAIN_FAULT<<2) 

#define RW_CB		(AP_RW|DOMAIN0|CB|DESC_SEC)
#define RW_CNB		(AP_RW|DOMAIN0|CNB|DESC_SEC)
#define RW_NCB		(AP_RW|DOMAIN0|NCB|DESC_SEC)
#define RW_NCNB		(AP_RW|DOMAIN0|NCNB|DESC_SEC)
#define RW_FAULT	(AP_RW|DOMAIN1|NCNB|DESC_SEC)

void MMU_SetMTT(unsigned int vaddrStart,unsigned int vaddrEnd,unsigned int paddrStart,int attr);

void MMU_Init(void)
{
    int i,j;

    MMU_DisableDCache();
    MMU_DisableICache();

    for(i=0;i<64;i++) for(j=0;j<8;j++)
    {
    	MMU_CleanInvalidateDCacheIndex((unsigned int)((i<<26)|(j<<5)));
    }
    MMU_InvalidateICache();
    
    MMU_EnableICache(); 
    MMU_DisableMMU();
    MMU_InvalidateTLB();

    MMU_SetMTT(0x00000000,0x07ffffff,0x30000000,RW_NCNB);   				//bnak0 : NONE, EVT -> Virtual EVT mapping
	MMU_SetMTT(0x08000000,0x0fffffff,0x08000000,RW_NCNB);  				//bank1	: NONE
    MMU_SetMTT(0x10000000,0x17ffffff,0x10000000,RW_FAULT);  				//bank2 : NONE
    MMU_SetMTT(0x18000000,0x1fffffff,0x18000000,RW_FAULT);  				//bank3 : NONE
    MMU_SetMTT(0x20000000,0x27ffffff,0x20000000,RW_FAULT);  				//bank4 : NONE
    MMU_SetMTT(0x28000000,0x2fffffff,0x28000000,RW_FAULT);  				//bank5 : NONE
    MMU_SetMTT(CODE_START,HEAP_END_ADDRESS,CODE_START,RW_CB);				//Bank6, SDRAM1(18MB), section1 => RO,RW,ZI,Stack,HEAP
    MMU_SetMTT(Free_Memory_BASE,Free_Memory_LIMIT,Free_Memory_BASE,RW_NCB);	//Bank6, SDRAM2(4MB), section2 => Free Memory for DMA
    MMU_SetMTT(END_OF_STACK,_MMUTT_ENDADDRESS-1,END_OF_STACK,RW_CB);		//Bank6, SDRAM3(2MB),  section3 => Stack, MMU(16KB)
    MMU_SetMTT(LCD_FB0,END_OF_SDRAM-1,LCD_FB0,RW_NCB);						//Bank6, SDRAM4(8MB),  section4 => Non-Cache Frame-buffer(4MB+4MB)
    MMU_SetMTT(END_OF_SDRAM,0x38000000-1,END_OF_SDRAM,RW_FAULT);			//Bank6 : NONE
    MMU_SetMTT(0x38000000,0x3fffffff,0x38000000,RW_FAULT); 					//Bank7, Not Used
    MMU_SetMTT(0x40000000,0x5affffff,0x40000000,RW_NCNB);  					//SFR+StepSram
    MMU_SetMTT(0x5b000000,0xffffffff,0x5b000000,RW_FAULT); 					//not used
    
    MMU_SetTTBase(_MMUTT_STARTADDRESS);
    MMU_SetDomain(0x55555550|DOMAIN1_ATTR|DOMAIN0_ATTR); 					//DOMAIN1: no_access, DOMAIN0,2~15=client(AP is checked)
    MMU_SetProcessId(0x0);
    MMU_EnableAlignFault();
    	
    MMU_EnableMMU();
    MMU_EnableICache();
    MMU_EnableDCache(); //DCache should be turned on after MMU is turned on.
}    

void MMU_SetMTT(unsigned int vaddrStart,unsigned int vaddrEnd,unsigned int paddrStart,int attr)
{
    unsigned int *pTT;
    unsigned int i,nSec;
    pTT=(unsigned int *)_MMUTT_STARTADDRESS+(vaddrStart>>20);
    nSec=(vaddrEnd>>20)-(vaddrStart>>20);

    for(i=0;i<=nSec;i++)
    {
    	*pTT++=(unsigned int)(attr |(((paddrStart>>20)+i)<<20));
   	}
}
