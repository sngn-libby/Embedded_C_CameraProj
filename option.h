//=============================================================================
// File Name : option.h
//=============================================================================

#define	FCLK		399650000		// 399.650MHz (400MHz)

#define	HCLK		(FCLK/4)
#define	PCLK		(FCLK/8)

/*************************************/
/* Memory Configuration (SDRAM 32MB) */
/*************************************/

#define CODE_START			0x30000000				// 8MB
#define CODE_LIMIT			(0x30800000-4)

#define HEAP_BASE_ADDRESS	0x30800000				// 10MB
#define HEAP_END_ADDRESS	(0x31200000-4)

#define Free_Memory_BASE	0x31200000				// 4MB
#define Free_Memory_LIMIT	(0x31600000-4)

#define END_OF_STACK		0x31600000				// 2MB-16KB
#define BASE_OF_STACK		(0x31800000-0x4000)

#define _MMUTT_STARTADDRESS	(0x31800000-0x4000)		// 16KB
#define _MMUTT_ENDADDRESS	0x31800000

// LCD Frame Buffer (For Layer Mode)
#define LCD_FB00			0x31800000				// 1MB
#define LCD_FB01			0x31900000 				// 1MB
#define LCD_FB02			0x31A00000 				// 1MB
#define LCD_FB03			0x31B00000 				// 1MB

// LCD Frame Buffer (For Virtual Mode)
#define LCD_FB0				0x31800000				// 4MB
#define LCD_FB1				0x31C00000				// 4MB

#define NON_CACHE_AREA		LCD_FB0					// FB영역 8MB NCB 처리

#define END_OF_SDRAM		0x32000000			   	// 32MB
