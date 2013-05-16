/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file display_wc_ger.c
 * 
 *  This files implements the german language specific.
 *
 * \version $Id: display_wc_ger.c 328 2010-07-15 20:28:16Z vt $
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



#if (WC_DISP_GER == 1)


/* TODO: make PROGMEM if more ram is needed (+10B Progmem)*/
/* TODO: make more flexible */
#define DISP_SETBIT(x) ( 1L <<( (x) -DWP_MIN_FIRST))
/* consider to put translationmatrix to eeprom  */
static const uint8_t minDataOssi[11] =  {
  (DISP_SETBIT(DWP_fuenfMin) | DISP_SETBIT(DWP_nach) ),
  (DISP_SETBIT(DWP_zehnMin)  | DISP_SETBIT(DWP_nach) ),
  (DISP_SETBIT(DWP_viertel) ),
  (DISP_SETBIT(DWP_zehnMin)  | DISP_SETBIT(DWP_halb) | DISP_SETBIT(DWP_vorMin)),
  (DISP_SETBIT(DWP_fuenfMin) | DISP_SETBIT(DWP_halb) | DISP_SETBIT(DWP_vorMin)),
  (DISP_SETBIT(DWP_halb)),
  (DISP_SETBIT(DWP_fuenfMin) | DISP_SETBIT(DWP_halb) | DISP_SETBIT(DWP_nach)),
  (DISP_SETBIT(DWP_zehnMin)  | DISP_SETBIT(DWP_halb) | DISP_SETBIT(DWP_nach)),
  (DISP_SETBIT(DWP_viertel)  | DISP_SETBIT(DWP_dreiHour) ),
  (DISP_SETBIT(DWP_zehnMin)  | DISP_SETBIT(DWP_vorMin) ),
  (DISP_SETBIT(DWP_fuenfMin) | DISP_SETBIT(DWP_vorHour) ),

};
static const uint8_t minWessiViertel    = ( DISP_SETBIT(DWP_viertel)  | DISP_SETBIT(DWP_nach) );
static const uint8_t minWessidreiVirtel = ( DISP_SETBIT(DWP_viertel)  | DISP_SETBIT(DWP_vorHour) );

#undef DISP_SETBIT



DisplayState display_getTimeState (const DATETIME* i_newDateTime)
{
  uint8_t hour    = i_newDateTime->hh;
  uint8_t minutes = i_newDateTime->mm;

  uint8_t minuteLeds = minutes%5;
  minutes = minutes/5;

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



  if(hour>12)
    hour-=12;

  if(hour==0)
    hour = 12;

/*

          ossi                            wessi

   5:  5- 9, 25-29, 35-39, 55-59
  10: 10-14, 20-24, 40-44, 50-54
virt: 15-19, 45-49
halb: 20-44
drvt: 45-49                               -
vorM: 20-29, 50-59
nach:  5-14, 35-44                        5-19, 35-44
vorH:  -                                  45-49

 /5
        ossi               wessi

   5:  1, 5, 7, 11
  10:  2, 4, 8, 10
virt:  3, 9
halb:  4, 5, 6, 7, 8
drvt:  9                     -
vorM:  4, 5, 10,11
nach:  1, 2, 7, 8         1, 2, 3, 7, 8
vorH:  -                     9
*/


  if(minutes>0){
    uint8_t minState ;
    minState = ( minDataOssi[minutes-1]);
    if( tm_wessi == langMode){
      if( minutes==3 ){
        minState = minWessiViertel;
      }else if(minutes == 9){
        minState = minWessidreiVirtel;
      }
      if(minutes>=4)
        ++hour;
    }else{
      if(minutes>=3)
        ++hour;
    }
    leds |= ((uint32_t)minState)<<DWP_MIN_FIRST;
  }else{
    leds |= (1L << DWP_clock);
  }

  for(;minuteLeds;--minuteLeds){
    leds |= (1L << (minuteLeds-1 + DWP_MIN_LEDS_BEGIN));
  }

  //uint8_t minuteLeds = minutes%5;
  //minutes = (minutes/5 * 5);

  //for(;minuteLeds;--minuteLeds){
  //  leds |= (1<< (minuteLeds-1 + DWP_MIN_LEDS_BEGIN));
  //}
  //if(minutes<5){
  //  leds |= (1<<DWP_clock);
  //}else if(minutes < 10){
  //  leds |= (1<<DWP_fuenfMin);
  //  leds |= (1<<DWP_nach);
  //}else if(minutes < 15){
  //  leds |= (1<<DWP_zehnMin);
  //  leds |= (1<<DWP_nach);
  //}else if(minutes < 20){
  //  leds |= (1<<DWP_viertel);
  //  if(g_displayParams.ossiMode){
  //    leds |= (1<<DWP_nach);
  //  }else{
  //    ++hour;
  //  }
  //}else if(minutes < 25){
  //  leds |= (1<<DWP_zehnMin);
  //  leds |= (1<<DWP_vorMin);
  //  leds |= (1<<DWP_halb);
  //  ++hour;
  //}else if(minutes < 30){
  //  leds |= (1<<DWP_fuenfMin);
  //  leds |= (1<<DWP_vorMin);
  //  leds |= (1<<DWP_halb);
  //  ++hour;
  //}else if(minutes < 35){
  //  leds |= (1<<DWP_halb);
  //  ++hour;
  //}else if(minutes < 40){
  //  leds |= (1<<DWP_fuenfMin);
  //  leds |= (1<<DWP_nach);
  //  leds |= (1<<DWP_halb);
  //  ++hour;
  //}else if(minutes < 45){
  //  leds |= (1<<DWP_zehnMin);
  //  leds |= (1<<DWP_nach);
  //  leds |= (1<<DWP_halb);
  //  ++hour;
  //}else if(minutes < 50){
  //  leds |= (1<<DWP_viertel);
  //  ++hour;
  //  if(g_displayParams.ossiMode){
  //    leds |= (1<<DWP_vorHour);
  //  }else{
  //    leds |= (1<<DWP_dreiHour);
  //  }
  //}else if(minutes < 55){
  //  leds |= (1<<DWP_zehnMin);
  //  leds |= (1<<DWP_vorHour);
  //  ++hour;
  //}else if(minutes < 60){
  //  leds |= (1<<DWP_fuenfMin);
  //  leds |= (1<<DWP_vorMin);
  //  ++hour;
  //}


  if(hour>12)
    hour-=12;
  if(hour==1 && minutes>=1)
    leds |= (1L << DWP_s);


  leds |= (1L << (DWP_HOUR_BEGIN-1 + hour));

  return leds;

}

#endif /* WC_DISP_GER */