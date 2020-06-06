/* Metrowerks ARM Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation. All rights reserved. 
 * 
 * $Date: 2004/05/26 18:47:41 $ 
 * $Revision: 1.8 $ 
 */ 


/*
 *	startup.c	-	entry-point for ARM programs
 *
 */
#include <string.h>
#include <stdlib.h>
#include "Runtime_configuration.h"

static char *argv[] = { 0 };
extern char __SP_INIT[];
extern void __call_static_initializers(void);
extern int main(int, char **);

extern void _fp_init(void);

extern void __init_registers();
extern void __init_hardware();
extern void __init_user();

#if SUPPORT_ROM_TO_RAM
extern void __copy_rom_sections_to_ram(void);
extern char __S_romp[];
#endif

static void zero_fill_bss(void)
{
	extern char __START_BSS[];
	extern char __END_BSS[];
	
	memset(__START_BSS, 0, (__END_BSS - __START_BSS));
}

#ifdef __thumb // Thumb version
static void thumb_startup(void);
asm void thumb_startup(void)
{
		// Setup registers
		bl 	__init_registers


		// setup hardware
		bl __init_hardware
	
		//	setup the stack before we attempt anything else
		lda		r0,__SP_INIT
		mov		sp,r0
		sub		sp,#4
		mov		r0,#0
		mvn		r0,r0
		str		r0,[sp,#0]
		add		sp,#4
	
		//	zero-fill the .bss section
		bl		zero_fill_bss

#if SUPPORT_ROM_TO_RAM
		lda 	r0, __S_romp
		cmp		r0,#0
		beq		over
		bl		__copy_rom_sections_to_ram
over:	
#endif

		
		//      initialize the floating-point library
		bl      _fp_init
	
		//	call C++ static initializers
		bl		__call_static_initializers
		
		// initializations before main, user specific
		bl 		__init_user

		
		//	call main(argc, &argv)
		mov		a1,#0
		lda		a2,argv
		bl		main
		
		//	exit the program
		mov		a1,#0
		bl		exit
		
		//	should never get here
@1:		b		@1
}
#endif

#pragma thumb off
void __startup(void);
asm void __startup(void)
{
#ifndef __thumb // ARM version

		// Setup registers
		bl 	__init_registers


		// setup hardware
		bl __init_hardware
		
		// ----- access to memory is save now
		
		//	setup the stack before we attempt anything else
		lda		sp,__SP_INIT
		mvn		r0,#0
		str		r0,[sp,#-4]!
	
	
		// ----- stack is set up, you can call C functions now

		//	zero-fill the .bss section
		bl		zero_fill_bss

#if SUPPORT_ROM_TO_RAM
		lda 	r0, __S_romp
		cmp		r0,#0
		blne	__copy_rom_sections_to_ram	
#endif

		
		//      initialize the floating-point library
		bl      _fp_init
	
		//	call C++ static initializers
		bl		__call_static_initializers
		
		// initializations before main, user specific
		bl 		__init_user
		
		//	call main(argc, &argv)
		mov		a1,#0
		lda		a2,argv
		bl		main
		
		//	exit the program
		mov		a1,#0
		bl		exit
		
		//	should never get here
@1:		b		@1

#else // Thumb version

		//	we cannot start in Thumb mode, so call thumb_startup
		//	via interworking branch
		lda		r0,thumb_startup
		bx 		r0

#endif
}
#pragma thumb reset 

