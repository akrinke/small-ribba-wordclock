/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file display_tix.h
 * 
 *  This files contain the tix led clock specific definitions that are required for display.
 *  \details
      This clock uses the following led matrix to display the time.\n
      \pre
      [ ]   [ ][ ][ ]   [ ][ ]   [ ][ ][ ]
      [ ]   [ ][ ][ ]   [ ][ ]   [ ][ ][ ]
      [ ]   [ ][ ][ ]   [ ][ ]   [ ][ ][ ]

 *  
 *
 * \version $Id: display_tix.h 389 2011-10-18 10:59:38Z vt $
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


#ifndef _WC_DISPLAY_TIX_H_
#define _WC_DISPLAY_TIX_H_

#include "base.h"
#ifdef __cplusplus
extern "C"
{
#endif 

/* ports, pins and ddrs for minute LEDs*/
#define DISPLAY_HOUR11_PORT PORTB
#define DISPLAY_HOUR11_DDR  DDRB
#define DISPLAY_HOUR11_PIN  PIN0

#define DISPLAY_HOUR12_PORT PORTD
#define DISPLAY_HOUR12_DDR  DDRD
#define DISPLAY_HOUR12_PIN  PIN7

#define DISPLAY_HOUR13_PORT PORTC
#define DISPLAY_HOUR13_DDR  DDRC
#define DISPLAY_HOUR13_PIN  PIN2


/**  
 *  enumerates the modes how the numbers will be displaysd
 *  For desciptions of the modes see display_tix_patterns.h
 */
typedef enum e_TixModes{
  tm_fill = 0,     /**<  fills the fields from buttom to top                       */
  tm_dice,         /**<  reassembles the pips on a dice                            */
  tm_border,       /**<  draws a border                                            */
  tm_random,       /**<  creates a random pattern for changed fields               */
  tm_random_all,   /**<  creates a random pattern for whole clock on every change  */
  TM_COUNT         /**<  the number of different modes                             */
}e_TixModes;

/**
 * contains display paraeters
 */
struct DisplayEepromParams{
  uint8_t mode; 
};

#define DISPLAYEEPROMPARAMS_DEFAULT { \
  /* .mode = */ 0 \
}



// declare toggle ossi functionality
#define DISPLAY_SPECIAL_USER_COMMANDS  \
  UI_SWITCH_DISPLAY_MODE     ,

// give default IR-Code
#define DISPLAY_SPECIAL_USER_COMMANDS_CODES  \
 /* [UI_SWITCH_DISPLAY_MODE    ] =  */  0x0008  /* 8     */,


// code to execute on keypress
#define _DISP_TOGGLE_TOGGLE_RANDOM_CODE                  \
              ++g_displayParams->mode;                   \
              g_displayParams->mode %= TM_COUNT;         \
              addState( MS_showNumber, (void*)(g_displayParams->mode+1));\
              log_state("tix\n");

// declare the ir-handler for the Key
#define DISPLAY_SPECIAL_USER_COMMANDS_HANDLER \
   USER_CREATE_IR_HANDLER(UI_SWITCH_DISPLAY_MODE, _DISP_TOGGLE_TOGGLE_RANDOM_CODE)





/**
 * This Enum defines how the leds  are connected to the Board and the position 
 * in the state data (	that's why the upper hours (gpio, not shift register) are also in this enum )
 * @details 
 *    numbering:
      \pre
          [1]   [1][2][3]   [1][2]   [1][2][3]
          [2]   [4][5][6]   [3][4]   [4][5][6]
          [3]   [7][8][9]   [5][6]   [7][8][9]
*/
enum e_displayWordPos
{
  DP_min01 = 0,
  DP_min02,
  DP_min03,
  DP_min04,
  DP_min05,
  DP_min06,
  DP_min07,
  DP_min08,
  DP_min09,
  DP_min11,
  DP_min12,
  DP_min13,
  DP_min14,
  DP_min15,
  DP_min16,
  DP_hour01,
  DP_hour02,
  DP_hour03,
  DP_hour04,
  DP_hour05,
  DP_hour06,
  DP_hour07,
  DP_hour08,
  DP_hour09,
  DP_hour11,
  DP_hour12,
  DP_hour13,

  DP_COUNT  
};


static inline DisplayState display_getHighMinuteMask(void)
{

  return  ( 1L<< DP_min11 )
        | ( 1L<< DP_min12 )
        | ( 1L<< DP_min13 )
        | ( 1L<< DP_min14 )
        | ( 1L<< DP_min15 )
        | ( 1L<< DP_min16 );
}

static inline DisplayState display_geLowMinuteMask(void)
{
  return  ( 1L<< DP_min01 )
        | ( 1L<< DP_min02 )
        | ( 1L<< DP_min03 )
        | ( 1L<< DP_min04 )
        | ( 1L<< DP_min05 )
        | ( 1L<< DP_min06 )
        | ( 1L<< DP_min07 )
        | ( 1L<< DP_min08 )
        | ( 1L<< DP_min09 );
}


/* for documentation see prototype in display.h */
static inline DisplayState display_getMinuteMask(void)
{ 
  return  display_geLowMinuteMask()
        | display_getHighMinuteMask();
}


static inline DisplayState display_getHighHoursMask(void)
{
  return  ( 1L<< DP_hour11 )
        | ( 1L<< DP_hour12 )
        | ( 1L<< DP_hour13 );
}

static inline DisplayState display_getLowHoursMask(void)
{
  return  ( 1L<< DP_hour01 )
        | ( 1L<< DP_hour02 )
        | ( 1L<< DP_hour03 )
        | ( 1L<< DP_hour04 )
        | ( 1L<< DP_hour05 )
        | ( 1L<< DP_hour06 )
        | ( 1L<< DP_hour07 )
        | ( 1L<< DP_hour08 )
        | ( 1L<< DP_hour09 );

}

/* for documentation see prototype in display.h */
static inline DisplayState display_getHoursMask(void)
{
  return  display_getLowHoursMask()
        | display_getHighHoursMask();
}

/* for documentation see prototype in display.h */
static inline DisplayState display_getIndicatorMask(void)
{
  return   ( 1L<< DP_hour11 )
         | ( 1L<< DP_hour13 )
         | ( 1L<< DP_min11  )
         | ( 1L<< DP_min15  );
}

/* for documentation see prototype in display.h */
static inline DisplayState display_getTimeSetIndicatorMask(void)
{
  return 0;  // tix clock doeas not support such a indicator
}

/* for documentation see prototype in display.h */
static inline DisplayState display_getNumberDisplayState( uint8_t number )
{
  extern uint16_t display_getNumber(uint8_t number);
  uint8_t rem;
  uint8_t fac = div10(number, &rem);
  DisplayState res = display_getNumber(fac);
  res <<= DP_hour01;
  res |= (display_getNumber(rem)<<DP_min01);
  return  res;
}

#ifdef __cplusplus
}
#endif 


#endif /* _WC_DISPLAY_TIX_H_ */
