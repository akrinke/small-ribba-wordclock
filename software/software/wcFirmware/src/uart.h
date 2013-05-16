/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file uart.h
 * 
 *  Handles the hardware UART interface
 *
 * \version $Id: uart.h 285 2010-03-24 21:43:24Z vt $
 * 
 * \author Copyright (c) 2010 Frank Meyer - frank(at)fli4l.de
* 
 * \remarks
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 */
 /*-----------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef _WC_UART_H_
#define _WC_UART_H_

#include <avr/pgmspace.h>

#ifdef __cplusplus
extern "C"
{
#endif


/**
 *  Initialize  UART
 *  @details  Initializes UART
 */
extern void                   uart_init(void);

/**
 *  Send character
 *  @details  Sends character
 *  @param    ch  character to be transmitted
 */
extern void                   uart_putc (unsigned char ch);

/**
 *  Send string
 *  @details  Sends '\\0'-terminated string
 *  @param    s string to be transmitted
 */
extern void                   uart_puts (const char * s);

/**
 *  Send string (stored in program memory)
 *  @details  Sends String stored in program memory (e.g. flash memory)
 *  @param    s program memory string to be transmitted
 */
extern void                   uart_puts_p(const char *s );

/**
 * Macro to automatically put a string constant into program memory
 */
#define uart_puts_P(__s)      uart_puts_p(PSTR(__s))

#ifdef __cplusplus
}
#endif


#endif // _WC_UART_H_

