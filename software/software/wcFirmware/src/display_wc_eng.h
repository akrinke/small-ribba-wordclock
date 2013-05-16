/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file display_wc_eng.h
 * 
 *  This files contain the english language specific definitions that are required for display.
 *
 * \version $Id: display_wc_eng.h 411 2012-01-09 20:46:26Z vt $
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


#ifndef _WC_DISPLAY_ENG_H_
#define _WC_DISPLAY_ENG_H_

#ifdef __cplusplus
extern "C"
{
#endif 

/** 
 * wether "it is" can be deactivated via remote or not 
 * - 0 = off tm_wessi -> tm_rheinRuhr -> ossi,
 * - 1 = on  tm_wessi -> tm_wessi "it is" off -> tm_rheinRuhr -> tm_rheinRuhr "it is" off -> ossi -> ossi "it is" off,
 */
#define DISPLAY_DEACTIVATABLE_ITIS 1

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
enum e_displayWordPos
{
  DWP_itis = 0,
  DWP_fiveMin ,
  DWP_tenMin ,
  DWP_quarter ,
  DWP_twenty ,
  DWP_half ,
  DWP_to ,
  DWP_past ,
  DWP_one ,
  DWP_two ,
  DWP_three ,
  DWP_four ,
  DWP_five ,
  DWP_six ,
  DWP_seven ,
  DWP_eight ,
  DWP_nine ,
  DWP_ten ,
  DWP_eleven ,
  DWP_twelve ,
  DWP_clock ,
  DWP_sr_nc1,
  DWP_sr_nc2,
  DWP_sr_nc3,
  DWP_min1,
  DWP_min2,
  DWP_min3,
  DWP_min4,

  DWP_WORDSCOUNT  
};

/** the first of the minute words */
#define DWP_MIN_FIRST       DWP_fiveMin
/** the first hour word */
#define DWP_HOUR_BEGIN      DWP_one
/** the last pos in the emu that has acorresponding bit in the shift register */
#define DWP_LAST_SR_POS     DWP_clock
/** the first of the dots that reside on gpio */
#define DWP_MIN_LEDS_BEGIN  DWP_min1



#if (DISPLAY_DEACTIVATABLE_ITIS==1)

/**  
 *  enumerates the modes how the time senteces will be build
 *  For desciptions of the modes see display_wc_ger.h
 */
typedef enum e_WcEngModes{
  tm_itIsOn = 0,    /**<  Wess-Mode                        */
  tm_itIsOff,         /**<  Ossi-Mode                        */
  TM_COUNT         /**<  the number of different modes    */
}e_WcEngModes;

/**
 * contains display parameters
 */
struct DisplayEepromParams{

  /** the idom to use for time display */
  e_WcEngModes mode;
};

#define DISPLAYEEPROMPARAMS_DEFAULT { \
  /* .mode = */ 0 \
}

// declare toggle ossi functionality
#define DISPLAY_SPECIAL_USER_COMMANDS  \
  UI_SELECT_DISP_MODE     ,

// give default IR-Code
#define DISPLAY_SPECIAL_USER_COMMANDS_CODES  \
 /* [UI_Select_Disp_Mode    ] =  */  0x0008  /* 8     */,


// code to execute on keypress
#define _DISP_TOGGLE_DISPMODE_CODE                                                \
              ++g_displayParams->mode;                                            \
              g_displayParams->mode %= TM_COUNT; \
              addState( MS_showNumber, (void*)(g_displayParams->mode+1));         \
              log_state("DM\n");

// declare the ir-handler for the Key
#define DISPLAY_SPECIAL_USER_COMMANDS_HANDLER \
   USER_CREATE_IR_HANDLER(UI_SELECT_DISP_MODE, _DISP_TOGGLE_DISPMODE_CODE)

#else


/**
 * contains display paraeters
 */
struct DisplayEepromParams{
  uint8_t dummy; 
};

#define DISPLAYEEPROMPARAMS_DEFAULT { \
  /* .dummy = */ 0 \
}

// empty because no special params for english version 
#define DISPLAY_SPECIAL_USER_COMMANDS  
#define DISPLAY_SPECIAL_USER_COMMANDS_CODES
#define DISPLAY_SPECIAL_USER_COMMANDS_HANDLER

#endif

/* for documentation see prototype in display.h */
static inline DisplayState display_getMinuteMask(void)
{ 
  return 
       ( 1L<< DWP_fiveMin ) 
     | ( 1L<< DWP_tenMin  ) 
     | ( 1L<< DWP_quarter ) 
     | ( 1L<< DWP_twenty  ) 
     | ( 1L<< DWP_half    ) 
     | ( 1L<< DWP_to      ) 
     | ( 1L<< DWP_past    ) 
     | ( 1L<< DWP_min1    ) 
     | ( 1L<< DWP_min2    ) 
     | ( 1L<< DWP_min3    ) 
     | ( 1L<< DWP_min4    );
}


/* for documentation see prototype in display.h */
static inline DisplayState display_getHoursMask(void)
{
  return
       ( 1L<< DWP_one    ) 
     | ( 1L<< DWP_two    ) 
     | ( 1L<< DWP_three  ) 
     | ( 1L<< DWP_four   ) 
     | ( 1L<< DWP_five   ) 
     | ( 1L<< DWP_six    ) 
     | ( 1L<< DWP_seven  ) 
     | ( 1L<< DWP_eight  ) 
     | ( 1L<< DWP_nine   ) 
     | ( 1L<< DWP_ten    ) 
     | ( 1L<< DWP_eleven ) 
     | ( 1l<< DWP_twelve );
}

/* for documentation see prototype in display.h */
static inline DisplayState display_getTimeSetIndicatorMask(void)
{
  return (1L<<DWP_clock);
}

/* for documentation see prototype in display.h */
static inline DisplayState display_getNumberDisplayState( uint8_t number )
{
    number = number%12;
    if( number == 0 ){
      number = 12;
    }
    return (1L << (number + DWP_HOUR_BEGIN-1));
}

#ifdef __cplusplus
}
#endif 


#endif /* _WC_DISPLAY_ENG_H_ */