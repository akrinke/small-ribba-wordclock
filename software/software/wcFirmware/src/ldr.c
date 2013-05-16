/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file ldr.c
 * 
 *  Implements the handling of he ldr brightness measurement.
 *  \details
 *      Written for ATMEGA88 @ 8 MHz. \n
 *
 * \version $Id: ldr.c 285 2010-03-24 21:43:24Z vt $
 * 
 * 
 * \author Copyright (c) 2010 René Harsch ( rene@harsch.net )
 * \author Copyright (c) 2010 Vlad Tepesch  
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
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "main.h"
#include "uart.h"
#include "ldr.h"
#include "base.h"

/** contains the last LDR_ARRAY_SIZE measurements */
static volatile uint8_t             array[LDR_ARRAY_SIZE];

/** 
 *  contains the sum of all elements of array 
 *  \details  This is redundant but saves the effort to calculate the sum
 *            of all buffer elements after every measurements
 */
static volatile uint16_t            curr_sum;



/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * ldr_init: initialize adc for ldr
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
ldr_init (void)
{
  volatile uint8_t result;
 
  ADMUX = 0                       // ADC0
        | (1<<REFS0)              // use Avcc
        | (1<<ADLAR);             // left justify result in ADCH
 
  ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS0);      // prescaler to 32 / in our case sampling frequence of 250kHz
                                                     // and activate ADC

  ADCSRA |= (1<<ADSC);                      // ADC start 
  while ( ADCSRA & (1<<ADSC) ) {
    ;     
  }

  result = ADCH;  // read out the value and init the summary array
  for (int i = 0; i < LDR_ARRAY_SIZE; array[i++] = result);

  curr_sum = result;              // also init the sum
  curr_sum *= LDR_ARRAY_SIZE;     //

# if (LDR_LOG == 1)
  {
    char buff[5];
    byteToStr(result, buff);
    uart_puts_P("LDR init: ");
    uart_puts(buff);
    uart_putc('\n');
  }
# endif

  ADCSRA |= (1<<ADSC);     // start next measurement (will be read in 1Hz Interupt)

  return;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * ldr_get_brightness: returns the average of brightness 
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
uint8_t
ldr_get_brightness (void)
{
  return ( 255-(curr_sum / LDR_ARRAY_SIZE)); 
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * ldr isr, called every 1 sec
 * recalculate our new summary and start a new conversion
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
ldr_ISR (void)
{
  static uint8_t   curr_index = 0;

  if ( (ADCSRA & (1<<ADSC)) == 0) {
    // read out last conversion and recalculating summary
    uint8_t measurement = ADCH;

# if (LDR_LOG == 1)
  {
    char buff[5];
    byteToStr(measurement, buff);
    uart_puts_P("LDR: ");
    uart_puts(buff);
    uart_putc('\n');
  }
# endif

    curr_sum -= array[curr_index];
    array[curr_index] = measurement;
    curr_sum += measurement;
    curr_index++;

    curr_index %= LDR_ARRAY_SIZE;

    // start next ADC converting
    ADCSRA |= (1<<ADSC); 
  } 

  return;
}
