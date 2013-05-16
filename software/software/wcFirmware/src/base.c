/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file base.c
 * 
 *  This file implements some usefull or basic tool routines
 *
 * \version $Id: base.c 285 2010-03-24 21:43:24Z vt $
 * 
 * \author Copyright (c) 2010 Frank Meyer - frank(at)fli4l.de
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
#include "base.h"

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * converts a bcd pair to integer
 * @param     bcd bcd pair
 * @return    converted integer
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
uint8_t
bcdtoi (uint8_t bcd)
{
  uint8_t i = 10 * (bcd >> 4) + (bcd & 0x0F);
  return (i);
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * converts an integer to bcd pair
 * @param     i integer number (0-255)
 * @return    converted bcd pair
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
uint8_t
itobcd (uint8_t i)
{
  uint8_t   bcd;

#if 0
  bcd = (i / 10) << 4;
  bcd |= (i % 10);
#else
  uint8_t   r;

  bcd = div10 (i, &r) << 4;
  bcd |= r;
#endif

  return (bcd);
}

void byteToStr( uint8_t val, char o_buf[4] )
{
  uint8_t v=val;
  uint8_t r;
  o_buf[3]=0;
  o_buf[1]=' ';
  o_buf[0]=' ';
  v = div10(v, &r);
  o_buf[2] = r + '0';
  if(v>0){
    v = div10(v, &r);
    o_buf[1] = r + '0';
    if(v>0){
      o_buf[0] = v + '0';
    }
  }
}

void byteToStrLessHundred( uint8_t val, char o_buf[3] )
{
  uint8_t v=val;
  uint8_t r;
  o_buf[2] ='\0';
  v = div10(v, &r);
  o_buf[0] = v?v+'0':' ';
  o_buf[1] = r + '0';
}


#if 0 // yet not used
/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * Calculate day of week, perhaps later used for weekday display (by 3-to-8 decoder)
 * @detail  Calculates day of week, 0=sunday, 1=monday, ... 6=saturday
 * @return  day of week
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
uint8_t
dayofweek (int d, int m, int y)
{
   return (d+=m<3?y--:y-2,23*m/9+d+4+y/4-y/100+y/400)%7;
}

#endif // 0
