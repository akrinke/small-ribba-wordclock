/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file display_wc_eng.c
 * 
 *  This files implements the english language specific.
 *
 * \version $Id: display_wc_eng.c 411 2012-01-09 20:46:26Z vt $
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

#include "wceeprom.h"


#if (WC_DISP_ENG == 1)

/* TODO: make PROGMEM if more ram is needed (+10B Progmem)*/
/* TODO: make more flexible */
#define DISP_SETBIT(x) ( 1L << ( (x) -DWP_MIN_FIRST))
/* consider to put translationmatrix to flash  */
static const uint8_t minData[11] =  {
  (DISP_SETBIT(DWP_fiveMin)  | DISP_SETBIT(DWP_past) ),
  (DISP_SETBIT(DWP_tenMin)   | DISP_SETBIT(DWP_past) ),
  (DISP_SETBIT(DWP_quarter)  | DISP_SETBIT(DWP_past) ),
  (DISP_SETBIT(DWP_twenty )  | DISP_SETBIT(DWP_past) ),
  (DISP_SETBIT(DWP_twenty )  | DISP_SETBIT(DWP_fiveMin) | DISP_SETBIT(DWP_past) ),
  (DISP_SETBIT(DWP_half)     | DISP_SETBIT(DWP_past) ),
  (DISP_SETBIT(DWP_twenty )  | DISP_SETBIT(DWP_fiveMin) | DISP_SETBIT(DWP_to) ),
  (DISP_SETBIT(DWP_twenty )  | DISP_SETBIT(DWP_to) ),
  (DISP_SETBIT(DWP_quarter)  | DISP_SETBIT(DWP_to) ),
  (DISP_SETBIT(DWP_tenMin)   | DISP_SETBIT(DWP_to) ),
  (DISP_SETBIT(DWP_fiveMin)  | DISP_SETBIT(DWP_to) ),


};

#undef DISP_SETBIT



DisplayState display_getTimeState (const DATETIME* i_newDateTime)
{
  uint8_t hour    = i_newDateTime->hh;
  uint8_t minutes = i_newDateTime->mm;

  uint8_t minuteLeds = minutes%5;

  minutes = minutes/5;

#if DISPLAY_DEACTIVATABLE_ITIS == 1
  uint32_t leds     = 0;
  if(   ((g_displayParams->mode & 1) == 0 ) // Es ist zur halb/vollen Stunde oder bei gerader Modusnummer
      || (0 == minutes))
  {    
    leds |= (1L << DWP_itis);
  }
#else
  uint32_t leds     = (1L << DWP_itis);
#endif


  if(hour>12)
    hour-=12;

  if(hour==0)
    hour = 12;

  if(minutes>0){
    uint8_t minState ;
    minState = ( minData[minutes-1]);
    if(minutes>6)
      ++hour;
    leds |= ((uint32_t)minState)<<DWP_MIN_FIRST;
  }else{
    leds |= (1L << DWP_clock);
  }

  for(;minuteLeds;--minuteLeds){
    leds |= (1L << (minuteLeds-1 + DWP_MIN_LEDS_BEGIN));
  }


  if(hour>12)
    hour-=12;

  leds |= (1L<< (DWP_HOUR_BEGIN-1 + hour));

  return leds;

}

#endif /* WC_DISP_ENG */