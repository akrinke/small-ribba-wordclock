/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file display_wc_ger3.h
 * 
 *  This files contain the german language specific definitions that are required for display.
 *  \details
 *           This version of the german language supports four regional differences in speking.
 *
            \verbatim 
                E S K I S T L F Ü N F    ==> ES IST FÜNF
                Z E H N Z W A N Z I G    ==> ZEHN ZWANZIG
                D R E I V I E R T E L    ==> DREI|VIERTEL
                T G N A C H V O R J M    ==> NACH VOR
                H A L B Q Z W Ö L F P    ==> HALB ZWÖLF
                Z W E I N S I E B E N    ==> ZW|EI|N|S|IEBEN
                K D R E I R H F Ü N F    ==> DREI FÜNF
                E L F N E U N V I E R    ==> ELF NEUN VIER
                W A C H T Z E H N R S    ==> ACHT ZEHN
                B S E C H S F M U H R    ==> SECHS UHR
            \endverbatim 

 *             -# Wessi-Modus:
 *                - es ist ein uhr
 *                - es ist fünf nach eins
 *                - es ist zehn nach eins
 *                - es ist viertel nach eins
 *                - es ist zehn vor halb zwei
 *                - es ist fünf vor halb zwei
 *                - es ist halb zwei
 *                - es ist fünf nach halb zwei
 *                - es ist zehn nach halb zwei
 *                - es ist viertel vor zwei
 *                - es ist zehn vor zwei
 *                - es ist fünf vor zwei
 *             -# Rhein-Ruhr-Modus:
 *               - es ist ein uhr
 *               - es ist fünf nach eins
 *               - es ist zehn nach eins
 *               - es ist viertel nach eins
 *               - es ist zwanzig nach eins
 *               - es ist fünf vor halb zwei
 *               - es ist halb zwei
 *               - es ist fünf nach halb zwei
 *               - es ist zwanzig vor zwei
 *               - es ist viertel vor zwei
 *               - es ist zehn vor zwei
 *               - es ist fünf vor zwei
 *             -# Ossi-Modus:
 *               - es ist ein uhr
 *               - es ist fünf nach eins
 *               - es ist zehn nach eins
 *               - es ist viertel zwei
 *               - es ist zehn vor halb zwei
 *               - es ist fünf vor halb zwei
 *               - es ist halb zwei
 *               - es ist fünf nach halb zwei
 *               - es ist zehn nach halb zwei
 *               - es ist dreiviertel zwei
 *               - es ist zehn vor zwei
 *               - es ist fünf vor zwei
 *             -# Swabian-Modus:
 *               - es ist ein uhr
 *               - es ist fünf nach eins
 *               - es ist zehn nach eins
 *               - es ist viertel zwei
 *               - es ist zwanzig nach eins
 *               - es ist fünf vor halb zwei
 *               - es ist halb zwei
 *               - es ist fünf nach halb zwei
 *               - es ist zwanzig vor zwei
 *               - es ist dreiviertel zwei
 *               - es ist zehn vor zwei
 *               - es ist fünf vor zwei
 *
 * \version $Id: display_wc_ger3.h 412 2012-01-10 20:03:08Z vt $
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


#ifndef _WC_DISPLAY_GER3_H_
#define _WC_DISPLAY_GER3_H_

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
 * in the state data ( that's why the minutes (gpio, not shift register) are also in this enum )
 */
enum e_displayWordPos
{
  DWP_zw  = 0,
  DWP_ei     ,
  DWP_n      ,
  DWP_s      ,
  DWP_ieben  ,
  DWP_drei   ,
  DWP_vier   ,
  DWP_fuenf  ,
  DWP_sechs  ,
  DWP_acht   ,
  DWP_neun   ,
  DWP_zehn   ,
  DWP_elf    ,
  DWP_zwoelf  ,

  DWP_itis  ,
  DWP_clock ,

  DWP_fuenfMin   ,
  DWP_zehnMin    ,
  DWP_zwanzigMin ,
  DWP_dreiMin    ,
  DWP_viertel    ,
  DWP_nach       ,
  DWP_vor        ,
  DWP_halb       ,

  DWP_min1,
  DWP_min2,
  DWP_min3,
  DWP_min4,

  DWP_WORDSCOUNT  
};

/** the first of the minute words */
#define DWP_MIN_FIRST       DWP_fuenfMin
/** the first hour word */
#define DWP_HOUR_BEGIN      DWP_zw
/** the first of the dots that reside on gpio */
#define DWP_MIN_LEDS_BEGIN  DWP_min1


/**  
 *  enumerates the modes how the time senteces will be build
 *  For desciptions of the modes see display_wc_ger3.h
 */
typedef enum e_WcGerModes{
  tm_wessi = 0,    /**<  Wess-Mode                        */
  tm_rheinRuhr,    /**<  Rhein-Ruhr-Mode                  */
  tm_ossi,         /**<  Ossi-Mode                        */
  tm_swabian,      /**<  Swabian-Mode                     */
  TM_COUNT         /**<  the number of different modes    */
}e_WcGerModes;

/**
 * contains display parameters
 */
struct DisplayEepromParams{

  /** the idom to use for time display */
  e_WcGerModes mode;
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
              g_displayParams->mode %= (TM_COUNT*(DISPLAY_DEACTIVATABLE_ITIS+1)); \
              addState( MS_showNumber, (void*)(g_displayParams->mode+1));         \
              log_state("WRO\n");

// declare the ir-handler for the Key
#define DISPLAY_SPECIAL_USER_COMMANDS_HANDLER \
   USER_CREATE_IR_HANDLER(UI_SELECT_DISP_MODE, _DISP_TOGGLE_DISPMODE_CODE)



/* for documentation see prototype in display.h */
static inline DisplayState display_getMinuteMask(void)
{ 
  return 
       ( 1L<< DWP_fuenfMin  ) 
     | ( 1L<< DWP_zehnMin   ) 
     | ( 1L<< DWP_zwanzigMin) 
     | ( 1L<< DWP_dreiMin   ) 
     | ( 1L<< DWP_viertel   ) 
     | ( 1L<< DWP_nach      ) 
     | ( 1L<< DWP_vor       ) 
     | ( 1L<< DWP_halb      ) 
     | ( 1L<< DWP_min1    ) 
     | ( 1L<< DWP_min2    ) 
     | ( 1L<< DWP_min3    ) 
     | ( 1L<< DWP_min4    );
}



/* for documentation see prototype in display.h */
static inline DisplayState display_getHoursMask(void)
{
  return
       ( 1L<< DWP_zw     ) 
     | ( 1L<< DWP_ei     ) 
     | ( 1L<< DWP_n      ) 
     | ( 1L<< DWP_s      ) 
     | ( 1L<< DWP_ieben  ) 
     | ( 1L<< DWP_drei   ) 
     | ( 1L<< DWP_vier   ) 
     | ( 1L<< DWP_fuenf  ) 
     | ( 1L<< DWP_sechs  ) 
     | ( 1L<< DWP_acht   ) 
     | ( 1L<< DWP_neun   ) 
     | ( 1L<< DWP_zehn   ) 
     | ( 1L<< DWP_elf    ) 
     | ( 1L<< DWP_zwoelf );
}

/* for documentation see prototype in display.h */
static inline DisplayState display_getTimeSetIndicatorMask(void)
{
  return (1L<<DWP_clock);
}

/* for documentation see prototype in display.h */
static inline DisplayState display_getNumberDisplayState( uint8_t number )
{
    extern const uint16_t s_numbers[12];
    number = number%12;
    return ((DisplayState)(s_numbers[number])) << DWP_HOUR_BEGIN;
}

#ifdef __cplusplus
}
#endif 


#endif /* _WC_DISPLAY_GER3_H_ */
