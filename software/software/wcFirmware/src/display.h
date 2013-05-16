/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file display.h
 * 
 *  The display-module is main interface to the display hardware of the clock.
 *  \details
 *    The display-module its to convert the given time to the binary data 
 *    to controll the status of the leds.\n
 *    This file should be left unchanged if making adaptions to other displays or languages. \n
 *    Theese things should be changed in display_[displaytype].h/c
 *
 * \version $Id: display.h 403 2011-11-13 00:08:32Z sm $
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

#ifndef _WC_DISPLAY_H_
#define _WC_DISPLAY_H_




#ifdef __cplusplus
extern "C"
{
#endif 



/** interval blinking words (* 100ms) */
#define DISPLAY_BLINK_INT_100ms 7

/** how fast the fading between two times will be */
#define DISPLAY_FADE_TIME_ms 2000
#define DISPLAY_FADE_TIME_ANIM_ms 1000


#define DISPLAY_TIMER_OCR      OCR2A
#define DISPLAY_TIMER_OVF_vect TIMER2_OVF_vect
#define DISPLAY_TIMER_OCR_vect TIMER2_COMPA_vect
#define DISPLAY_TIMER_ENABLE_INTS() do{ TIMSK2 |= (1<<TOIE2) | (1<<OCIE2A);}while(0)
#define F_TIMER2 488  /* 1/s */



/**
 * contains information that words should be displayed
 * @TODO     move type declaration to specific clock definitions
 * @details  DisplayState is a bitset.
 *           The bitset is defined by the enum e_displayWordPos.
 */
typedef uint32_t DisplayState;


/**
 * contains display parameters
 * Definition of struct has to be in language specific header.
 */
typedef struct DisplayEepromParams DisplayEepromParams;


/**
 *  global instance of display Params.
 *  @TODO: consider creating access functions
 *  to use this macro you have to include the 
 *  wceeprom.h (not included here because circular include)
 */
#define g_displayParams (&(wcEeprom_getData()->displayParams))
//extern DisplayEepromParams* g_displayParams;



/**
 *  initializes the display module.
 */
extern void display_init (void);

/**
 * writes the given data to display
 */
extern void display_outputData(DisplayState state);


/**
 * calculates which display elements should be active to display the giben time
 * @details  f_call: extern irregularly  (internal: when called setNewTime)
 * @param    i_newDateTime  the new time that should be displayed
 * @return   DisplayState as needed by setDisplayState
 */
extern DisplayState display_getTimeState (const DATETIME* i_newDateTime);


/**
 *  sets the display to the given state
 *  @details This function can be used to set a special image on the display independent of current time
 *           This is especially usefull for provide some feedback for user interactions
 *           eg. set time, or set Color
 *           f_call: extern irregularly  (internal: when called setNewTime)
 *  @param    i_showStates   defined which words should be shown at all
 *  @param    i_blinkstates  defines which words should blink with DISPLAY_BLINK_INT_ms
 *                           only words that should be displayed will blink 
 *                           words_that_will_blink = blinkstates & showStates
 */
extern void    display_setDisplayState( DisplayState i_showStates, DisplayState i_blinkstates);



/**
 * sets a new time to the Display
 * @details  f_call = on time change ~1-2/min
 * @param i_newDateTime  the new time that should be displayed
 */
static inline void display_setNewTime (const DATETIME* i_newDateTime)
{
  display_setDisplayState( display_getTimeState(i_newDateTime), 0 );
};

/** 
 * fades from one Display state to another
 * @param i_showStates  the new state that should be displayed
 */
extern void display_fadeDisplayState( DisplayState i_showStates);


/**
 * fades out the old and fades in the new time to the Display
 * @details  f_call = on time change ~1-2/min
 * @param i_newDateTime  the new time that should be displayed
 */
static inline void display_fadeNewTime (const DATETIME* i_newDateTime)
{
  display_fadeDisplayState(display_getTimeState(i_newDateTime));
};



/**
 * Updates the display if currently in a blinking-state state, else does nothing
 * @details  have to be called from 10Hz ISR
 *           f_call: 10/s
 */
extern void    display_blinkStep (void);


/**
 * does one step of the auto-OnOff animation 
 * also handles the current step
 * @param number  If 1, the preview-mode is displayed. If 0, normal animation starts
 * @details  assumed to be called with 1Hz
 */
extern void display_autoOffAnimStep1Hz(uint8_t g_animPreview);


/**
 * Returns a State that contains an indicator that can be used for user interactions.
 * For example the minute points on the WC.
 * @return bitset as used in display_setDisplayState with activated indicators 
 */
static inline DisplayState display_getIndicatorMask(void);

/**
 * Returns a State that contains an indicator that indicates a time set mode 
 * and that can be used for user interactions.
 * For example the 'o'clock' word on the WC.
 * @return bitset as used in display_setDisplayState with activated indicators 
 */
static inline DisplayState display_getTimeSetIndicatorMask(void);

/**
 * Returns a State that contains all minuts
 *  @details Can be used to set minutes to blink in display_setDisplayState
 * @return bitset as used in display_setDisplayState with activated minutes 
 */
static inline DisplayState display_getMinuteMask(void);

/**
 * Returns a State that contains all hours
 * @details Can be used to set hours to blink in display_setDisplayState
 * @return bitset as used in display_setDisplayState with activated hours 
 */
static inline DisplayState display_getHoursMask(void);


/**
 *  returns a State that visualizes the given number and can be used for user interactions.
 *  @details   The display is limited by hardware. 
 *             So the number may be the result of some modulo operation.
 *  @param number  the number to visualize
 *  @return bitset as used in display_setDisplayState with the given number
 */
static inline DisplayState display_getNumberDisplayState( uint8_t number );


#ifdef __cplusplus
}
#endif 


/*
 * For language/front related constants look at the approriate display_[displaytype].h
 */

#if (WC_DISP_ENG == 1) || (WC_DISP_GER == 1)|| (WC_DISP_GER3 == 1)
#  include "display_wc.h" 
#elif (TIX_DISP == 1)
#  include "display_tix.h" 
#else   /* default to german */
#  define WC_DISP_GER 1
#  include "display_wc.h" 
#endif

#endif /* _WC_DISPLAY_H_ */
