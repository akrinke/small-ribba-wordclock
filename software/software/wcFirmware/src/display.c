/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file display.c
 * 
 *  Implementation of the language-independent display stuff
 *
 * \version $Id: display.c 403 2011-11-13 00:08:32Z sm $
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
#include <avr/interrupt.h>

#include "main.h"
#include "display.h"
#include "user.h"
#include "uart.h"

#include "pwm.h"


/** the amount of pwm cycles after that the dutycycle has to be adapted */
#define DISPLAY_FADE_PRESCALE ((uint8_t)((( ((uint32_t)F_TIMER2)*DISPLAY_FADE_TIME_ms )/1000)/MAX_PWM_STEPS))
#define DISPLAY_FADE_PRESCALE_ANIM ((uint8_t)((( ((uint32_t)F_TIMER2)*DISPLAY_FADE_TIME_ANIM_ms )/1000)/MAX_PWM_STEPS))



static uint32_t g_oldDispState;
static uint32_t g_curDispState;
static uint32_t g_blinkState;
volatile uint8_t g_curFadeStep;


void display_setDisplayState( DisplayState i_showStates, uint32_t i_blinkstates)
{
  g_blinkState   = i_blinkstates & i_showStates;
  g_oldDispState = g_curDispState;
  g_curDispState = i_showStates;
  g_curFadeStep  = 0;
  display_outputData(g_curDispState); 
}


void display_fadeDisplayState( DisplayState i_showStates)
{
  extern uint8_t pwm_table[MAX_PWM_STEPS];
  g_blinkState   = 0;
  g_oldDispState = g_curDispState;
  g_curDispState = i_showStates;
  g_curFadeStep  = MAX_PWM_STEPS-1;										// @EDI: removes flickering when fading with improved fast-PWM-mode
  DISPLAY_TIMER_OCR = (pgm_read_byte (pwm_table + g_curFadeStep)<3) ? 255 : (258 - pgm_read_byte (pwm_table + g_curFadeStep));
}



//void fadeTimerOCR(void)
ISR( DISPLAY_TIMER_OCR_vect )
{
  extern uint8_t pwm_table[MAX_PWM_STEPS];

  static uint8_t s_fadePrescale;
  if( g_curFadeStep >1)
  {
    display_outputData( g_oldDispState );									// @EDI: changed from "current state" to "old state" because of new PWM-mode
    ++s_fadePrescale;

    if(useAutoOffAnimation)
    {
      if( s_fadePrescale ==  DISPLAY_FADE_PRESCALE_ANIM){					// @EDI: we need another prescale, if the animation is on at the moment
        s_fadePrescale = 0;
        --g_curFadeStep;													// @EDI: removes flickering when fading with improved fast-PWM-mode
        DISPLAY_TIMER_OCR = (pgm_read_byte (pwm_table + g_curFadeStep)<3) ? 255 : (258 - pgm_read_byte (pwm_table + g_curFadeStep)); 	
      }
    } else {
      if( s_fadePrescale ==  DISPLAY_FADE_PRESCALE){
        s_fadePrescale = 0;
        --g_curFadeStep;													// @EDI: removes flickering when fading with improved fast-PWM-mode
        DISPLAY_TIMER_OCR = (pgm_read_byte (pwm_table + g_curFadeStep)<3) ? 255 : (258 - pgm_read_byte (pwm_table + g_curFadeStep)); 	
      }
    }

  } else {																		
    display_outputData( g_curDispState );								// @EDI: last action of fading is displaying the ACTUAL time
  }
}


//void displayFadeTimerOvf (void)
ISR( DISPLAY_TIMER_OVF_vect )
{

  if( g_curFadeStep ){
    display_outputData( g_curDispState );								// @EDI: changed from "old state" to "current state" because of new PWM-mode
  }
}





void display_blinkStep (void)
{
  if(    g_blinkState 
     && (g_curFadeStep == 0))
  {
    static uint8_t s_blinkPrescale = DISPLAY_BLINK_INT_100ms;
    if( ! (--s_blinkPrescale) )
    {
      g_curDispState ^= g_blinkState;
      display_outputData(g_curDispState);
      s_blinkPrescale = DISPLAY_BLINK_INT_100ms;
    }
  }
}


