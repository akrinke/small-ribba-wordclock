/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file pwm.h
 * 
 *  Pwm control.
 *  \details
 *      Written for ATMEGA88 @ 8 MHz. \n
 *      This module declares the interface to the pwm channels
 *      to control the rgb colors and brightness.
 *
 * \version $Id: pwm.h 341 2010-08-06 18:57:00Z vt $
 * 
 * \author Copyright (c) 2010 Frank Meyer - frank(at)fli4l.de
 * \author Copyright (c) 2010 Vlad Tepesch  
 *
 * \remarks
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *//*-----------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef _WC_PWM_H_
#define _WC_PWM_H_

#ifdef __cplusplus
extern  "C"
{
#endif



#define MAX_PWM_STEPS         32                    // 32 PWM steps
//#define MAX_PWM_STEPS         64                    // 64 PWM steps




#define LDR2PWM_COUNT    32           /**< size of look up table brightess -> base_pwm (if changing this you also need to change ldr brightness value*/
#define LDR2PWM_OCC_TYPE uint32_t     /**< data type for the bitset that marks user defined values of ldr2pwm look up table */

/** contains the persistent data of the pwm module that should be stored in eeprom */
typedef struct PwmEepromParams{
  int8_t            brightnessOffset;                  /** The userdefined offset to the brightness control  */
  uint8_t           brightness2pwmStep[LDR2PWM_COUNT]; /**< mapping of received breighness to pwm steps      */
  LDR2PWM_OCC_TYPE  occupancy;                         /**< bitset with user set array members               */
}PwmEepromParams;

#define PWMEEPROMPARAMS_DEFAULT  {                                                \
  /* .brightnessOffset   = */ 0,                                                  \
  /*  brightness2pwmStep = */ { 5, 6, 7, 8, 8, 9,10,11,12,13,13,14,15,16,17,18,   \
                               18,19,20,21,22,23,23,24,25,26,27,28,28,29,30,31},  \
  /*  occupancy          = */ ((LDR2PWM_OCC_TYPE)1) | (((LDR2PWM_OCC_TYPE)1)<<(LDR2PWM_COUNT-1))  \
}
//   1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31


/**
 *  Initialize the PWM
 *  @details  Configures 0CR0A, 0CR0B and 0CR2B as PWM channels
 */
extern void                   pwm_init (void);

/**
 *  Switch PWM on
 */
extern void                   pwm_on (void);

/**
 *  Switch PWM off
 */
extern void                   pwm_off (void);

/**
 *  Toggle PWM off/on
 */
extern void                   pwm_on_off (void);


#if (MONO_COLOR_CLOCK != 1)

/**
 *  Set RGB colors
 *  @param    red    range 0-255
 *  @param    green  range 0-255
 *  @param    blue   range 0-255
 */
extern void                   pwm_set_colors (uint8_t red, 
                                              uint8_t green, 
                                              uint8_t blue);

/**
 *  Get RGB colors
 *  @param    redp    pointer to value of red pwm: range 0-255
 *  @param    greenp  pointer to value of green pwm: range 0-255
 *  @param    bluep   pointer to value of blue pwm: range 0-255
 */
extern void                   pwm_get_colors (uint8_t * redp, 
                                              uint8_t * greenp, 
                                              uint8_t * bluep);

/**
 *  Set RGB colors by step values
 *  @param    red_step    range 0 to MAX_PWM_STEPS
 *  @param    green_step  range 0 to MAX_PWM_STEPS
 *  @param    blue_step   range 0 to MAX_PWM_STEPS
 */
extern void                   pwm_set_color_step (uint8_t red_step, 
                                                  uint8_t green_step, 
                                                  uint8_t blue_step);

/**
 *  Get RGB color step values
 *  @param    red_stepp    pointer zo red_step: range 0 to MAX_PWM_STEPS
 *  @param    green_stepp  pointer zo green_step: range 0 to MAX_PWM_STEPS
 *  @param    blue_stepp   pointer zo blue_step: range 0 to MAX_PWM_STEPS
 */
extern void                   pwm_get_color_step (uint8_t * red_stepp, 
                                                  uint8_t * green_stepp, 
                                                  uint8_t * blue_stepp);
#endif
/**
 *  Set base brightness by step 0-31
 *  @param    pwm_idx  pwm step 0-31
 */
extern void                   pwm_set_base_brightness_step (uint8_t pwm_idx);


/**
 *  Step up brightness
 */
extern void                   pwm_step_up_brightness (void);

/**
 *  Step down brightness
 */
extern void                   pwm_step_down_brightness (void);


/**
 *  Sets the given brightness and ignores further changes 
 *  of brightness until pwm_release_brightness is called.
 *
 *  @details  Other changes of brighness (other than with this function)
 *            will be recognized but not considerd.\n
 *            The change takes effect after pwm_release_brightness
 *            was called.
 *  @param val   the brightness to set [0..255]
 */
extern void                   pwm_lock_brightness_val(uint8_t val);

/**
 *  Releases the brightness control lock.
 *  Updates that occured meanwhile will be take effect now.
 */
extern void                   pwm_release_brightness(void);

extern void                   pwm_modifyLdrBrightness2pwmStep(void);

#ifdef __cplusplus
}
#endif

#endif /* _WC_PWM_H_ */
