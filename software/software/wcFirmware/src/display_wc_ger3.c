/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file display_wc_ger3.c
 * 
 *  This files implements the german language specific.
 *
 * \version $Id: display_wc_ger3.c 412 2012-01-10 20:03:08Z vt $
 * 
 * \author Copyright (c) 2012 Vlad Tepesch    
 * \author Copyright (c) 2012 Uwe Höß
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
#include "base.h"
#include "display.h"
#include "shift.h"

#include "wceeprom.h"



#if (WC_DISP_GER3 == 1)


/* TODO: make PROGMEM if more ram is needed (+10B Progmem)*/
/* TODO: make more flexible */
#define DISP_SETBIT(x) ( 1 <<( (x) - DWP_MIN_FIRST))
/* consider to put translationmatrix to eeprom  */
static const uint8_t s_minData[TM_COUNT][11] =  {
  {
    (DISP_SETBIT(DWP_fuenfMin)   | DISP_SETBIT(DWP_nach) ),
    (DISP_SETBIT(DWP_zehnMin)    | DISP_SETBIT(DWP_nach) ),
    (DISP_SETBIT(DWP_viertel)    | DISP_SETBIT(DWP_nach) ),
    (DISP_SETBIT(DWP_zehnMin)    | DISP_SETBIT(DWP_vor )   | DISP_SETBIT(DWP_halb)),
    (DISP_SETBIT(DWP_fuenfMin)   | DISP_SETBIT(DWP_vor )   | DISP_SETBIT(DWP_halb)),
    (DISP_SETBIT(DWP_halb)),
    (DISP_SETBIT(DWP_fuenfMin)   | DISP_SETBIT(DWP_nach)   | DISP_SETBIT(DWP_halb)),
    (DISP_SETBIT(DWP_zehnMin)    | DISP_SETBIT(DWP_nach)   | DISP_SETBIT(DWP_halb)),
    (DISP_SETBIT(DWP_viertel)    | DISP_SETBIT(DWP_vor ) ),
    (DISP_SETBIT(DWP_zehnMin)    | DISP_SETBIT(DWP_vor ) ),
    (DISP_SETBIT(DWP_fuenfMin)   | DISP_SETBIT(DWP_vor ) ),
  },
  {
    (DISP_SETBIT(DWP_fuenfMin)   | DISP_SETBIT(DWP_nach) ),
    (DISP_SETBIT(DWP_zehnMin)    | DISP_SETBIT(DWP_nach) ),
    (DISP_SETBIT(DWP_viertel)    | DISP_SETBIT(DWP_nach) ),
    (DISP_SETBIT(DWP_zwanzigMin) | DISP_SETBIT(DWP_nach) ),
    (DISP_SETBIT(DWP_fuenfMin)   | DISP_SETBIT(DWP_vor ) | DISP_SETBIT(DWP_halb) ),
    (DISP_SETBIT(DWP_halb)),
    (DISP_SETBIT(DWP_fuenfMin)   | DISP_SETBIT(DWP_nach)   | DISP_SETBIT(DWP_halb)),
    (DISP_SETBIT(DWP_zwanzigMin) | DISP_SETBIT(DWP_vor ) ),
    (DISP_SETBIT(DWP_viertel)    | DISP_SETBIT(DWP_vor ) ),
    (DISP_SETBIT(DWP_zehnMin)    | DISP_SETBIT(DWP_vor ) ),
    (DISP_SETBIT(DWP_fuenfMin)   | DISP_SETBIT(DWP_vor ) ),
  },
  {
    (DISP_SETBIT(DWP_fuenfMin)   | DISP_SETBIT(DWP_nach)   ),
    (DISP_SETBIT(DWP_zehnMin)    | DISP_SETBIT(DWP_nach)   ),
    (DISP_SETBIT(DWP_viertel)                              ),
    (DISP_SETBIT(DWP_zehnMin)    | DISP_SETBIT(DWP_vor )   | DISP_SETBIT(DWP_halb)),
    (DISP_SETBIT(DWP_fuenfMin)   | DISP_SETBIT(DWP_vor )   | DISP_SETBIT(DWP_halb) ),
    (DISP_SETBIT(DWP_halb)),
    (DISP_SETBIT(DWP_fuenfMin)   | DISP_SETBIT(DWP_nach)   | DISP_SETBIT(DWP_halb)),
    (DISP_SETBIT(DWP_zehnMin)    | DISP_SETBIT(DWP_nach)   | DISP_SETBIT(DWP_halb)),
    (DISP_SETBIT(DWP_dreiMin)    | DISP_SETBIT(DWP_viertel)),
    (DISP_SETBIT(DWP_zehnMin)    | DISP_SETBIT(DWP_vor )   ),
    (DISP_SETBIT(DWP_fuenfMin)   | DISP_SETBIT(DWP_vor )   ),
  },
  {
    (DISP_SETBIT(DWP_fuenfMin)   | DISP_SETBIT(DWP_nach)                           ),
    (DISP_SETBIT(DWP_zehnMin)    | DISP_SETBIT(DWP_nach)                           ),
    (DISP_SETBIT(DWP_viertel)                                                      ),
    (DISP_SETBIT(DWP_zwanzigMin) | DISP_SETBIT(DWP_nach)                           ),
    (DISP_SETBIT(DWP_fuenfMin)   | DISP_SETBIT(DWP_vor )   | DISP_SETBIT(DWP_halb) ),
    (DISP_SETBIT(DWP_halb)                                                         ),
    (DISP_SETBIT(DWP_fuenfMin)   | DISP_SETBIT(DWP_nach)   | DISP_SETBIT(DWP_halb) ),
    (DISP_SETBIT(DWP_zwanzigMin) | DISP_SETBIT(DWP_vor )                           ),
    (DISP_SETBIT(DWP_dreiMin)    | DISP_SETBIT(DWP_viertel)                        ),
    (DISP_SETBIT(DWP_zehnMin)    | DISP_SETBIT(DWP_vor )                           ),
    (DISP_SETBIT(DWP_fuenfMin)   | DISP_SETBIT(DWP_vor )                           ),
  },
};

#undef DISP_SETBIT

#define DISP_SETBIT(x) ( 1L <<( (x) ))
const uint16_t s_numbers[12] = {
  ( DISP_SETBIT(DWP_zwoelf)                                              ),
  ( DISP_SETBIT(DWP_ei)       | DISP_SETBIT(DWP_n)  | DISP_SETBIT(DWP_s) ),
  ( DISP_SETBIT(DWP_zw)       | DISP_SETBIT(DWP_ei)                      ),
  ( DISP_SETBIT(DWP_drei)                                                ),
  ( DISP_SETBIT(DWP_vier)                                                ),
  ( DISP_SETBIT(DWP_fuenf)                                               ),
  ( DISP_SETBIT(DWP_sechs)                                               ),
  ( DISP_SETBIT(DWP_s)        | DISP_SETBIT(DWP_ieben)                   ),
  ( DISP_SETBIT(DWP_acht)                                                ),
  ( DISP_SETBIT(DWP_neun)                                                ),
  ( DISP_SETBIT(DWP_zehn)                                                ),
  ( DISP_SETBIT(DWP_elf)                                                 ),
};
#undef DISP_SETBIT



/** 
 * bitset for each mode 
 * one indicates that at the specified 5min-block the next 
 * hour is displayed
 */
static const uint16_t s_hourIncrement[TM_COUNT] = {
  BIN16(0,0,0,0,   1,1,1,1,1,1,1,1,0,0,0,0 ),  /* zehn vor halb */
  BIN16(0,0,0,0,   1,1,1,1,1,1,1,0,0,0,0,0 ),  /* fünf vor halb */
  BIN16(0,0,0,0,   1,1,1,1,1,1,1,1,1,0,0,0 ),  /* viertel */
  BIN16(0,0,0,0,   1,1,1,1,1,1,1,0,1,0,0,0 ),  /* viertel, aber 20 nach */
};



DisplayState display_getTimeState (const DATETIME* i_newDateTime)
{
  uint8_t hour       = i_newDateTime->hh;
  uint8_t minutes    = i_newDateTime->mm/5;
  uint8_t minuteLeds = i_newDateTime->mm%5;
  uint8_t minuteLedSubState = 0;



#if DISPLAY_DEACTIVATABLE_ITIS == 1
  uint32_t leds     = 0;
  uint8_t  langMode = g_displayParams->mode/2;
  if(   ((g_displayParams->mode & 1) == 0 ) // Es ist zur halb/vollen Stunde oder bei gerader Modusnummer
      || (0 == minutes)
      || (6 == minutes) )
  {    
    leds |= (1L << DWP_itis);
  }
#else
  uint32_t leds     = (1L << DWP_itis);
  uint8_t  langMode = g_displayParams->mode;
#endif



  if(minutes == 0){
    leds |= (1L << DWP_clock);
  }else{
    leds |= ((DisplayState)(s_minData[langMode][minutes-1])) << DWP_MIN_FIRST;
    if( (1<<minutes) & s_hourIncrement[langMode] ){
      ++hour;
    }
  }

  if(minuteLeds >= 4){
      minuteLedSubState |= (1 << (DWP_min4-DWP_MIN_LEDS_BEGIN));
  }
  if(minuteLeds >= 3){
      minuteLedSubState |= (1 << (DWP_min3-DWP_MIN_LEDS_BEGIN));
  }
  if(minuteLeds >= 2){
      minuteLedSubState |= (1 << (DWP_min2-DWP_MIN_LEDS_BEGIN));
  }
  if(minuteLeds >= 1){
      minuteLedSubState |= (1 << (DWP_min1-DWP_MIN_LEDS_BEGIN));
  }

  leds |= ((DisplayState) minuteLedSubState) << DWP_MIN_LEDS_BEGIN;
 
  //for(;minuteLeds;--minuteLeds){
  //  leds |= (1L << (minuteLeds-1 + DWP_MIN_LEDS_BEGIN));
  //}

  leds |= display_getNumberDisplayState(hour);

  if(   (hour==1 || hour==13 )   // if "Es ist ein Uhr" <- remove 's' from "eins"
     && (minutes==0))
  { 
    leds &= ~(1L << DWP_s); 
  }

  return leds;

}

#endif /* WC_DISP_GER3 */
