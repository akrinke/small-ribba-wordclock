/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file color_effects.h
 * 
 *  This file contains the routines for some color animations.
 *
 * \version $Id: color_effects.h 403 2011-11-13 00:08:32Z sm $
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


#ifndef _COLOR_EFFECTS_H_
#define _COLOR_EFFECTS_H_

#ifdef __cplusplus
extern "C"
{
#endif 

#include "pwm.h"

#if 1
// direkt pwm for Hue fading
#define HUE_STEPS 256
#define HUE_MAX (HUE_STEPS * 6)      // 192 @ 32 PWM Steps /// 384 @ 64 PWM Steps
#define HUE_MANUAL_STEPS 10
#define SetPWMs pwm_set_colors
typedef uint16_t Hue_t;

#else
#  if (MAX_PWM_STEPS==32)

     // 32Step Pwm
     typedef uint8_t Hue_t;
#    define HUE_STEPS MAX_PWM_STEPS
#    define HUE_MAX (HUE_STEPS * 6)
#    define SetPWMs pwm_set_color_step
#    define HUE_MANUAL_STEPS 1

#  elif (MAX_PWM_STEPS==64)
     // 64 Step PWM
     typedef uint16_t Hue_t;
#    define HUE_STEPS MAX_PWM_STEPS
#    define HUE_MAX (HUE_STEPS * 6)
#    define SetPWMs pwm_set_color_step
#    define HUE_MANUAL_STEPS 5

#  else
#    error unknown pwm step size
#  endif
#endif


/**
 * generates rgb from hue with saturation 1 and brightness 1
 * @param  h   the hue value to transform [0..HUE_MAX)
 * @param  r   retreives the calculated red value
 * @param  g   retreives the calculated green value
 * @param  b   retreives the calculated blue value
 */
extern void hue2rgb(
              Hue_t h, /*uint8  s, uint8  v,*/
              uint8_t* r, uint8_t* g, uint8_t* b
              /* ,sint32 relsat= -1 */ );

/** 
 *  generates a spiky symmetric cubic waveform 
 *  @param step   current step of animation [0..255]
 */
extern uint8_t pulseWaveForm(uint8_t step);


#ifdef __cplusplus
}
#endif 

#endif  //_COLOR_EFFECTS_H_

