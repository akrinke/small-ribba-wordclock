/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file display_wc.c
 * 
 *  Implementation of the language-independent word clock display stuff
 *
 * \version $Id: display_wc.c 405 2011-11-24 20:39:00Z sm $
 * 
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

#include "main.h"
#include "display.h"
#include "shift.h"

#include "uart.h"

#if (WC_DISP_ENG == 1) || (WC_DISP_GER == 1) || (WC_DISP_GER3 == 1)


void display_init (void)
{
  shift24_init();
  DISPLAY_MIN1_DDR |= (1 << DISPLAY_MIN1_PIN);
  DISPLAY_MIN2_DDR |= (1 << DISPLAY_MIN2_PIN);
  DISPLAY_MIN3_DDR |= (1 << DISPLAY_MIN3_PIN);
  DISPLAY_MIN4_DDR |= (1 << DISPLAY_MIN4_PIN);

  DISPLAY_MIN1_PORT &= ~(1 << DISPLAY_MIN1_PIN);
  DISPLAY_MIN2_PORT &= ~(1 << DISPLAY_MIN2_PIN);
  DISPLAY_MIN3_PORT &= ~(1 << DISPLAY_MIN3_PIN);
  DISPLAY_MIN4_PORT &= ~(1 << DISPLAY_MIN4_PIN);

  DISPLAY_TIMER_ENABLE_INTS();
}


/**
 * writes the given data to display
 */
void display_outputData(DisplayState state)
{
  shift24_output(state);

  if( state & (1L<<DWP_min1)){
    DISPLAY_MIN1_PORT |=  (1 << DISPLAY_MIN1_PIN);
  }else{
    DISPLAY_MIN1_PORT &= ~(1 << DISPLAY_MIN1_PIN);
  }

  if( state & (1L<<DWP_min2)){
    DISPLAY_MIN2_PORT |=  (1 << DISPLAY_MIN2_PIN);
  }else{
    DISPLAY_MIN2_PORT &= ~(1 << DISPLAY_MIN2_PIN);
  }

  if( state & (1L<<DWP_min3)){
    DISPLAY_MIN3_PORT |=  (1 << DISPLAY_MIN3_PIN);
  }else{
    DISPLAY_MIN3_PORT &= ~(1 << DISPLAY_MIN3_PIN);
  }

  if( state & (1L<<DWP_min4)){
    DISPLAY_MIN4_PORT |=  (1 << DISPLAY_MIN4_PIN);
  }else{
    DISPLAY_MIN4_PORT &= ~(1 << DISPLAY_MIN4_PIN);
  }

#if (DISPLAY_LOG_STATE==1)
  {
    uint8_t i;
    uart_puts_P("Disp: ");
    for(i=0; i<32; ++i){
      uart_putc( '0' + (state&1));
      state >>= 1;
    }
    uart_putc('\n');
  }
#endif
}


void display_autoOffAnimStep1Hz(uint8_t g_animPreview)
{
   static uint8_t s_state = 0;
   ++s_state;
   s_state %=8;
   if(s_state &1)
   {
     DisplayState state = ((DisplayState) 1) << ((s_state>>1)+DWP_MIN_LEDS_BEGIN);
     if( g_animPreview ){
       state |= display_getNumberDisplayState(2);
     }
     display_fadeDisplayState( state );
   }else{
     display_fadeDisplayState(0);
   }
}


#endif
