/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file uart.c
 * 
 *  Handles the hardware UART interface
 *
 * \version $Id: uart.c 334 2010-07-23 19:11:46Z vt $
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


#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "uart.h"
#include "main.h"

#if (BOOTLOADER_RESET_UART == 1)
#if (BOOTLOADER_RESET_WDT == 1)
#include <avr/wdt.h>
#endif
#include <avr/interrupt.h>
#endif

#define UART_BAUD             9600L

// calculate real baud rate:
#define UBRR_VAL              ((F_CPU+UART_BAUD*8)/(UART_BAUD*16)-1)            // round
#define BAUD_REAL             (F_CPU/(16*(UBRR_VAL+1)))                         // real baudrate
#define BAUD_ERROR            ((BAUD_REAL*1000)/UART_BAUD)                      // error in promille

#if ((BAUD_ERROR < 990) || (BAUD_ERROR > 1010))
#  error Error of baud rate of RS232 UART is more than 1%. That is too high!
#endif

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Initialize  UART
 *  @details  Initializes UART
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
uart_init (void)
{
#if (BOOTLOADER_RESET_UART == 1)
  UCSR0B |= (1<<TXEN0)|(1<<RXEN0)|(1<<RXCIE0);                                  // activate UART0 TX,RX,RXINT
#else
  UCSR0B |= (1<<TXEN0);                                                         // activate UART0 TX
#endif
  UBRR0H = UBRR_VAL >> 8;                                                       // store baudrate (upper byte)
  UBRR0L = UBRR_VAL & 0xFF;                                                     // store baudrate (lower byte)
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Send character
 *  @details  Sends character
 *  @param    ch character to be transmitted
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
uart_putc (unsigned char ch)
{
  while (!(UCSR0A & (1<<UDRE0)))
  {
    ;
  }

  UDR0 = ch;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Send string
 *  @details  Sends '\0'-terminated string
 *  @param    s string to be transmitted
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
uart_puts (const char * s)
{
  while (*s)
  {
    uart_putc (*s++);
  }
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Send string (stored in program memory)
 *  @details  Sends String stored in program memory (e.g. flash memory)
 *  @param    s program memory string to be transmitted
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
uart_puts_p (const char * progmem_s)
{
  char ch;

  while ((ch = pgm_read_byte (progmem_s++)) != '\0')
  {
    uart_putc (ch);
  }
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  RX Interrupt on UART
 *  @details  jump to the bootloader if 'R' was received via WDT or direct jump
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
#if (BOOTLOADER_RESET_UART == 1)
ISR(USART_RX_vect){
  if(UDR0=='R')                                                                 //Reset-Signal für BT-Bootloader bekommen?
  {
#if (BOOTLOADER_RESET_WDT == 1)                                                 // Reset via Watchdog
    //cli();                                                                      // disable Interrupts - not neccessary because interupt context
    wdt_enable(WDTO_15MS);                                                      // enable Watchdog Timer with lowest Timeout Value
    while(1);                                                                   // will never end until Watchdog Reset
#else
    asm volatile ("jmp 0x3800");                                                // jump directly to chip45boot2 bootloader
#endif
  }
}
#endif
