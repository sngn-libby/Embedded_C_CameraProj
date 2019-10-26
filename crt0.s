	.include "2440addr.inc"
	.include "Option.inc"

	.code	32
	.text

	.global	__start
__start:

@--------------------------------------------------
@ Exception Vector Configuration
@--------------------------------------------------

	b		ResetHandler
	b		Undef_Handler
	b		SWI_Handler
	b		Pabort_Handler
	b		Dabort_Handler
	b		.
	b		HandlerIRQ
	b		FIQ_Handler

@ IRQ Handler

	.extern ISR_Vectors

	.global HandlerIRQ
HandlerIRQ:
	sub		sp,sp,#4
	stmfd	sp!,{r8-r9}
	ldr		r9,=INTOFFSET
	ldr		r9,[r9]
	ldr		r8,=ISR_Vectors
	ldr		r8,[r8, r9,lsl #2]
	str		r8,[sp,#8]
	ldmfd	sp!,{r8-r9,pc}

@--------------------------------------------------
@ Reset Handler Routine
@--------------------------------------------------

ResetHandler:
	ldr		r0,=WTCON				
	ldr		r1,=0x0         
	str		r1,[r0]

	ldr		r0,=INTSUBMSK
	ldr		r1,=0x7fff				
	str		r1,[r0]
	
	ldr		r0,=INTMSK
	ldr		r1,=0xffffffff			
	str		r1,[r0]

	ldr		r0,=LOCKTIME
	ldr		r1,=0xffffffff			
	str		r1,[r0]

	ldr		r0,=CAMDIVN
	ldr		r1,=0x0 			
	str		r1,[r0]

	ldr		r0,=CLKDIVN

	ldr		r1,=0x5 			
	str		r1,[r0]

	ldr		r0,=MPLLCON
   	ldr		r1,= 0x6e031 		@ 399.65MHz(About 400MHz) @ 16.9344MHz
	str		r1,[r0]
	
	ldr  	r0,=0xc000107e		@ Async mode, I,D Cache ON
		
	mcr  	p15,0,r0,c1,c0,0	
	
@@@@@@@@@@@@@@@ Memory Configuration @@@@@@@@@@@@@@@@@

    ldr		r0,=SMRDATA
	ldr		r1,=BWSCON			
	add		r2, r0, #52			
1:
	ldr		r3, [r0], #4     	
	str		r3, [r1], #4  		
	cmp		r2, r0
	bne		1b

@@@@@@@@@@@@@@@ C Runtime Startup @@@@@@@@@@@@@@@@@

	.extern __ZI_BASE__
	.extern __ZI_LIMIT__

	ldr		r0, =__ZI_BASE__
	ldr		r1, =__ZI_LIMIT__
	mov		r2, #0x0
1:
	cmp		r0, r1
	strlo	r2, [r0], #4
	blo		1b

@@@@@@@@@@@@@@@@ Stack Mounting @@@@@@@@@@@@@@@@@

	msr		cpsr_c, #(IRQ_BIT|FIQ_BIT|IRQ_MODE)
	ldr		sp, =IRQ_STACK_BASE

	msr		cpsr_c, #(IRQ_BIT|FIQ_BIT|SVC_MODE)
	ldr		sp, =SVC_STACK_BASE

	msr		cpsr_c, #(IRQ_BIT|FIQ_BIT|FIQ_MODE)
	ldr		sp, =FIQ_STACK_BASE

	msr		cpsr_c, #(IRQ_BIT|FIQ_BIT|UNDEF_MODE)
	ldr		sp, =UNDEF_STACK_BASE

	msr		cpsr_c, #(IRQ_BIT|FIQ_BIT|ABORT_MODE)
	ldr		sp, =ABORT_STACK_BASE

	msr		cpsr_c, #(SYS_MODE)
	ldr		sp, =SYS_STACK_BASE

@@@@@@@@@@@@@@@@ Call Main @@@@@@@@@@@@@@@@@

	.extern Run_Time_Config
	.extern MMU_Init
	.extern Main

	bl	Run_Time_Config
	bl	MMU_Init
   	bl	Main	

   	b		.

	@ 133MHz, Winbond BGA SDRAM (W9825G2DB), 32MB(8M*4B)

SMRDATA:

	.word 	0x02000000	@ BWSCON
	.word 	0x0
	.word 	0x00007ff0	@for BANK1 w5100
	.word 	0x0
	.word 	0x0
	.word 	0x0
	.word 	0x0
	.word 	0x00018001	@ BANKCON6 100MHz
	.word 	0x0
	.word 	0x008401E7 	@ REFRESH 100MHz
	.word 	0x90
 	.word 	0x20		@ MRSRB6 100MHz
 	.word 	0x30
 	 	
  .end
