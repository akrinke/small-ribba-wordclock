/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file display_wc.h
 * 
 *  The display-module is the interface to the word clock display hardware.
 *  \details
 *    The display-module its to convert the given time to the binary data 
 *    to controll the status of the leds.\n
 *    This file should be left unchanged if making adaptions to other languages. \n
 *    The language specific things resides in display_wc_[language].h/c
 *
 * \version $Id: display_wc.h 285 2010-03-24 21:43:24Z vt $
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

#ifndef _WC_DISPLAY_WC_H_
#define _WC_DISPLAY_WC_H_


/*
 * For language/front related constants look at the approriate display_wc_[language].h
 */

#if (WC_DISP_ENG == 1)
#  include "display_wc_eng.h"
#elif (WC_DISP_GER == 1)
#  include "display_wc_ger.h" 
#elif (WC_DISP_GER3 == 1)
#  include "display_wc_ger3.h" 
#else   /* default to german */
#  define WC_DISP_GER 1
#  include "display_wc_ger.h" 
#endif

#ifdef __cplusplus
extern "C"
{
#endif 


/* ports, pins and ddrs for minute LEDs*/
#define DISPLAY_MIN1_PORT PORTB
#define DISPLAY_MIN1_DDR  DDRB
#define DISPLAY_MIN1_PIN  PIN0

#define DISPLAY_MIN2_PORT PORTD
#define DISPLAY_MIN2_DDR  DDRD
#define DISPLAY_MIN2_PIN  PIN7

#define DISPLAY_MIN3_PORT PORTC
#define DISPLAY_MIN3_DDR  DDRC
#define DISPLAY_MIN3_PIN  PIN2

#define DISPLAY_MIN4_PORT PORTC
#define DISPLAY_MIN4_DDR  DDRC
#define DISPLAY_MIN4_PIN  PIN3

/**
 * This Enum defines how the led words are connected to the Board and the position 
 * in the state data (	that's why the minutes (gpio, not shift register) are also in this enum )
 * @details In different languages at least the constants for the 
 *          hours (DWP_one to DWP_twelve), the it-is (DWP_itis), the four minutes (DWP_min[1234]) and 
 *          clock (DWP_clock) have to exist, so they can be used for interface
 *          the minute words are too diffrent and should not be used outside of display_[language].h/c
 *          Following two preconditions were made:
 *          - the eight Minute words are placed consecutively
 *            (but not neccessaryly ordered)
 *            and DWP_MIN_FIRST defines the first of them
 *          - the twelve hours are orderd consecutively
 *            and DWP_HOUR_BEGIN defines the first of them
 */
typedef enum e_displayWordPos e_displayWordPos;



/* for documentation see prototype in display.h */
static inline DisplayState display_getIndicatorMask(void)
{
  return   ( 1L<< DWP_min1    )
         | ( 1L<< DWP_min2    ) 
         | ( 1L<< DWP_min3    )
         | ( 1L<< DWP_min4    );
}



#ifdef __cplusplus
}
#endif 


#endif /* _WC_DISPLAY_WC_H_ */