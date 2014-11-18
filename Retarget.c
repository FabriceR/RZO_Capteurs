/*----------------------------------------------------------------------------
 * Name:    Retarget.c
 * Purpose: 'Retarget' layer for target-dependent low level functions
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stm32f10x.h>

#include <stdio.h>
#include <rt_misc.h>
#include "lcd.h"
#include "Retarget.h"

#pragma import(__use_no_semihosting_swi)

FILE __stdout={-1};
FILE __stdin={-2};
FILE __debug={-3};

volatile int32_t ITM_RxBuffer;         

int fputc(int c, FILE *f) 
{
	if (f->handle==-1)
	{
		if ((c<' ') || (c>'~'))
		{
			lcd_putchar(' ');
		}
		else
		{
			lcd_putchar(c);
		}
	}
	else if (f->handle == -3)
	{
		ITM_SendChar(c);
	}
	
	return c;
}


int fgetc(FILE *f) {
int c=0;
	
	if (f->handle == -3)
	{
		c = ITM_ReceiveChar();
	}
	
  return (c);
}


int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}


void _ttywrch(int c) {
  lcd_putchar(c);
}


void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}
