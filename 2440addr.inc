@====================================================================
@ File Name	: 2440addr.inc
@====================================================================

@=================
@ Memory control
@=================
	.equ	BWSCON		,	 0x48000000		@Bus width & wait status
	.equ	BANKCON0	,	 0x48000004		@Boot ROM control
	.equ	BANKCON1	,	 0x48000008		@BANK1 control
	.equ	BANKCON2	,	 0x4800000c		@BANK2 control
	.equ	BANKCON3	,	 0x48000010		@BANK3 control
	.equ	BANKCON4	,	 0x48000014		@BANK4 control
	.equ	BANKCON5	,	 0x48000018		@BANK5 control
	.equ	BANKCON6	,	 0x4800001c		@BANK6 control
	.equ	BANKCON7	,	 0x48000020		@BANK7 control
	.equ	REFRESH		,	 0x48000024		@DRAM/SDRAM	refresh
	.equ	BANKSIZE	,	 0x48000028		@Flexible Bank Size
	.equ	MRSRB6		,	 0x4800002c		@Mode register set for SDRAM Bank6
	.equ	MRSRB7		,	 0x48000030		@Mode register set for SDRAM Bank7

@=================
@ WATCH	DOG	TIMER
@=================
	.equ	WTCON	,	 0x53000000			@Watch-dog timer mode
	.equ	WTDAT	,	 0x53000004			@Watch-dog timer data
	.equ	WTCNT	,	 0x53000008			@Eatch-dog timer count

@==========================
@ CLOCK	& POWER	MANAGEMENT
@==========================
	.equ	LOCKTIME	,	 0x4c000000		@PLL lock time counter
	.equ	MPLLCON		,	 0x4c000004		@MPLL Control
	.equ	UPLLCON		,	 0x4c000008		@UPLL Control
	.equ	CLKCON		,	 0x4c00000c		@Clock generator control
	.equ	CLKSLOW		,	 0x4c000010		@Slow clock	control
	.equ	CLKDIVN		,	 0x4c000014		@Clock divider control
	.equ	CAMDIVN		,	 0x4c000018	 @Camera clocl control
	.equ	MSLCON		,	 0x560000cc		@Memory	sleep control register
	
@=================
@ INTERRUPT
@=================
	.equ	SRCPND		,	 0x4a000000	   @Interrupt r,est status
	.equ	INTMOD		,	 0x4a000004	   @Interrupt mode control
	.equ	INTMSK		,	 0x4a000008	   @Interrupt mask control
	.equ	PRIORITY	,	 0x4a00000c	   @IRQ	priority control		   
	.equ	INTPND		,	 0x4a000010	  @Interrupt r,est status
	.equ	INTOFFSET	,	 0x4a000014	   @Interruot r,est source offset
	.equ	SUSSRCPND	,	 0x4a000018	   @Sub	source pending
	.equ	INTSUBMSK	,	 0x4a00001c	   @Interrupt sub mask

@=================
@ I/O PORT for LED
@=================
	.equ	GPBCON		,	 0x56000010		@Port B	control
	.equ	GPBDAT		,	 0x56000014		@Port B	data
	.equ	GPECON		,	 0x56000040		@Port E	control
	.equ	GPEDAT		,	 0x56000044		@Port E	data
	.equ	GPEUP		,	 0x56000048		@Pull-up control E
	.equ	GPFCON		,	 0x56000050		@Port F	control
	.equ	GPFDAT		,	 0x56000054		@Port F	data
	.equ	GPFUP		,	 0x56000058		@Pull-up control F
	.equ	GPGCON		,	 0x56000060		@Port G	control
	.equ	GPGDAT		,	 0x56000064		@Port G	data
	.equ	GPGUP		,	 0x56000068		@Pull-up control G
	.equ	GPHCON		,	 0x56000070		@Port H	control
	.equ	GPHDAT		,	 0x56000074		@Port H	data
	.equ	GPHUP		,	 0x56000078		@Pull-up control H

@=================
@Miscellaneous register
@=================
	.equ	MISCCR		,	 0x56000080		@Miscellaneous control
	.equ	DCKCON		,	 0x56000084		@DCLK0/1 control
	.equ	EXTINT0		,	 0x56000088		@External interrupt	control	register 0
	.equ	EXTINT1		,	 0x5600008c		@External interrupt	control	register 1
	.equ	EXTINT2		,	 0x56000090		@External interrupt	control	register 2
	.equ	EINTFLT0	,	 0x56000094		@Reserved
	.equ	EINTFLT1	,	 0x56000098		@Reserved
	.equ	EINTFLT2	,	 0x5600009c		@External interrupt	filter control register	2
	.equ	EINTFLT3	,	 0x560000a0		@External interrupt	filter control register	3
	.equ	EINTMASK	,	 0x560000a4		@External interrupt	mask
	.equ	EINTPEND	,	 0x560000a8		@External interrupt	pending
	.equ	GSTATUS0	,	 0x560000ac		@External pin status
	.equ	GSTATUS1	,	 0x560000b0		@Chip ID(0x32440000)
	.equ	GSTATUS2	,	 0x560000b4		@Reset type
	.equ	GSTATUS3	,	 0x560000b8		@Saved data0(32-bit) before	entering POWER_OFF mode
	.equ	GSTATUS4	,	 0x560000bc		@Saved data1(32-bit) before	entering POWER_OFF mode


