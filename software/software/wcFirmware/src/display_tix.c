/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file display_tix.c
 * 
 *  This files implements the tix display
 *
 * \version $Id: display_tix.c 403 2011-11-13 00:08:32Z sm $
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
#include <stdlib.h>

#include "shift.h"
#include "main.h"
#include "display.h"
#include "uart.h"

#include "wceeprom.h"

#if (TIX_DISP == 1)




#include "display_tix_patterns.h"

void display_init (void)
{
  shift24_init();
  DISPLAY_HOUR11_DDR |= (1 << DISPLAY_HOUR11_PIN);
  DISPLAY_HOUR12_DDR |= (1 << DISPLAY_HOUR12_PIN);
  DISPLAY_HOUR13_DDR |= (1 << DISPLAY_HOUR13_PIN);

  DISPLAY_HOUR11_PORT &= ~(1 << DISPLAY_HOUR11_PIN);
  DISPLAY_HOUR12_PORT &= ~(1 << DISPLAY_HOUR12_PIN);
  DISPLAY_HOUR13_PORT &= ~(1 << DISPLAY_HOUR13_PIN);

  DISPLAY_TIMER_ENABLE_INTS();
  srand( 0 );
}


/**
 *  Fills the lower Bits of an 16bit Value with n random Bits.
 *  @param datalen   bit 0 - bit datalen will be filled
 *  @param nbits     the number of bits to distribute into result
 *  @return          16bit value that contains nbits 1s in the lower datalen bits
 */
static uint16_t randomBits(uint8_t datalen, uint8_t nbits)
{
  uint8_t  count = 0;
  uint16_t res   = 0;
  while( count != nbits ){
    uint8_t rnd = rand()%datalen;
    if( (res & (1<< rnd)) == 0)
    {
      res |= (1<< rnd);
      ++count;
    }
  }
  return res;
}



/**
 * writes the given data to display
 */
void display_outputData(DisplayState state)
{
  shift24_output(state);

  if( state & (1L<<DP_hour11)){
    DISPLAY_HOUR11_PORT |=  (1 << DISPLAY_HOUR11_PIN);
  }else{
    DISPLAY_HOUR11_PORT &= ~(1 << DISPLAY_HOUR11_PIN);
  }

  if( state & (1L<<DP_hour12)){
    DISPLAY_HOUR12_PORT |=  (1 << DISPLAY_HOUR12_PIN);
  }else{
    DISPLAY_HOUR12_PORT &= ~(1 << DISPLAY_HOUR12_PIN);
  }

  if( state & (1L<<DP_hour13)){
    DISPLAY_HOUR13_PORT |=  (1 << DISPLAY_HOUR13_PIN);
  }else{
    DISPLAY_HOUR13_PORT &= ~(1 << DISPLAY_HOUR13_PIN);
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

/**
 *  returns a displayState part for a number between 0-9
 */
uint16_t display_getNumber(uint8_t number)
{
  uint16_t res  = 0;


  if( number != 0 )
  {
    if( number == 9 ){
      res = C_NUMBERS9_FULL;
    }else{
      if( g_displayParams->mode >= tm_random){
        res = randomBits(9, number);
      }else{
        --number;
        res = c_numbers9[g_displayParams->mode][number];
      }
    }
  }

  return res;

};

/**
 *  returns a displayState part for upper minute parts (0-6)
 */
uint8_t display_getNumberHighMinute(uint8_t number)
{
  uint8_t  res8 = 0;

  if( number != 0 )
  {
    if( number>5 ){
      res8 = C_NUMBERS6_FULL;
    }else{
      if( g_displayParams->mode >= tm_random){
        res8 = randomBits(6, number);
      }else{
        --number;
        res8 = c_numbers6[g_displayParams->mode][number];
      }
    }
  }
  return res8;
};

/**
 *  returns a displayState part for upper hour parts (0-3)
 */
uint8_t display_getNumberHighHour(uint8_t number)
{
  uint8_t  res8 = 0;


  if( number != 0 )
  {
    if( number>2 ){
      res8 = C_NUMBERS3_FULL;
    }else{
      if( g_displayParams->mode >= tm_random){
        res8 = randomBits(3, number);
      }else{
        --number;
        res8 = c_numbers3[g_displayParams->mode][number];
      }
    }
  }
  return res8;
};




DisplayState display_getTimeState (const DATETIME* i_newDateTime)
{

#if 1
  // only unchanged fields will be reset ( with exeption of random all mode )
  static e_TixModes s_lastMode  = 0;
  static uint8_t    s_lastMinL  = 0;
  static uint8_t    s_lastMinH  = 0;
  static uint8_t    s_lastHourL = 0;
  static uint8_t    s_lastHourH = 0;
  static uint32_t   s_lastLed   = 0; /**< @TODO  last state allready saved in display.c */
  uint32_t leds = 0;
  uint8_t rem;
  uint8_t fac;

  uint8_t redraw =   (s_lastMode    != g_displayParams->mode)
                   | (tm_random_all == g_displayParams->mode);

  fac = div10(i_newDateTime->mm, &rem);
  if(    redraw
      || (fac != s_lastMinH)){
    leds |= (((uint32_t)display_getNumberHighMinute(fac))<<DP_min11);
    s_lastMinH = fac;
  }else{
    leds |= s_lastLed & display_getHighMinuteMask();
  }

  if(    redraw
      || (rem != s_lastMinL) ){
    leds |= (display_getNumber(rem)<<DP_min01);
    s_lastMinL = rem;
  }else{
    leds |= s_lastLed & display_getMinuteMask();
  }


  fac = div10(i_newDateTime->hh, &rem);
  if(    redraw
      || (fac != s_lastHourH) ){
    leds |= (((uint32_t)display_getNumberHighHour(fac))<<DP_hour11);
    s_lastHourH = fac;
  }else{
    leds |= s_lastLed & display_getHighHoursMask();
  }

  if(   redraw
     || (rem != s_lastHourL) ){
    leds |= (((uint32_t)display_getNumber(rem))<<DP_hour01);
    s_lastHourL = rem;
  }else{
    leds |= s_lastLed & display_getLowHoursMask();
  }

  s_lastMode = g_displayParams->mode;
  s_lastLed  = leds;

#else

  uint32_t leds = 0;
  uint8_t rem;
  uint8_t fac;

  fac = div10(i_newDateTime->mm, &rem);
  leds |=   (((uint32_t)display_getNumberHighMinute(fac))<<DP_min11)
          | (display_getNumber(rem)<<DP_min01);


  fac = div10(i_newDateTime->hh, &rem);
  leds |=   (((uint32_t)display_getNumberHighHour(fac))<<DP_hour11)
          | (((uint32_t)display_getNumber(rem))<<DP_hour01);

#endif



  return leds;
}

void display_autoOffAnimStep1Hz(uint8_t g_animPreview)
{
  static uint8_t s_state = 0;
  ++s_state;
  s_state %=16;
  if(s_state &1)
  {  
    // only the new led current state
    uint8_t num = s_state>>1;
    uint16_t s = c_numbers9[tm_border][num];
    if( num ) // if number > 1 remove led from last states (quick hack)
    {
      s ^= c_numbers9[tm_border][num-1];
    }

    DisplayState leds = ((DisplayState)(s)) <<DP_min01;
    display_fadeDisplayState( leds );
  }else{
    display_fadeDisplayState(0);
  }
}


#endif /* TIX_DISP */
