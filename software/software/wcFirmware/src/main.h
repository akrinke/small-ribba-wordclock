/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file main.h
 * 
 *  The main configuration file.
 *  \details
 *      Written for ATMEGA88 @ 8 MHz. \n
 *      This File contains the most important configuration options.
 *      The hardware dependet defines are in the individual files
 *      because the hardware is relativly fix.
 *
 * \version $Id: main.h 407 2011-11-24 21:55:09Z vt $
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


#ifndef _WC_X_H_
#define _WC_X_H_

#ifdef __cplusplus
extern "C"
{
#endif


#ifndef F_CPU
#error F_CPU unkown
#endif


  
/** Macro to build software version byte */
#define BUILD_VERSION( x,y )    ( (uint8_t)((x<<4)|(y)))

#define MAJOR_VERSION 0
#define MINOR_VERSION 12

/** Defines the software version  */
#define SW_VERSION   BUILD_VERSION(  MAJOR_VERSION, MINOR_VERSION  )

#define WC_DISP_GER   0   /**< Word Clock with german layout with 2 regional idoms */
#define WC_DISP_GER3  1   /**< Word Clock with german layout with 3 regional idoms */
#define WC_DISP_ENG   0   /**< Word Clock with english layout                      */
#define TIX_DISP      0   /**< TIX Clock -> google                                 */

/**
 *  if only one color LEDs are used.
 *  changing this to 1 will save a lot of memory
 *  by disabling all the RGB-Code
 *  @remark  Tix clock is allways mono color
 */
#define MONO_COLOR_CLOCK  ((TIX_DISP == 1) ||  0)

/**
 *  activates code for DCF control 
 *  \details
 *      If active there is also a detection if really a dcf module is connected.\n
 *      So there is no need to deactivate it. 
 *      Never the less deactivating saves ~1k so it may help using a smaller controller.
 */
#define DCF_PRESENT           1
#define AMBILIGHT_PRESENT     1  /**< activates Code for ambilight control GPO1 */
#define BLUETOOTH_PRESENT     1  /**< activates Code for bluetooth control GPO2 */
#define AUXPOWER_PRESENT      1  /**< activates Code for auxiliary control GPO3 */
	
/**
 *  activates code for UART Reset to start bootloader
 *  \ details
 *       If active the UART is enable to jump to the bootloader.\n
 *       Therefore just send an 'R' via UART and the bootloader will be started.\n
 *       It is possible to reset via Watchdog (BOOTLOADER_RESET_WDT = 1)
 *       or to jump direclty to the bootloader (BOOTLOADER_RESET_WDT = 0).
 */
#define BOOTLOADER_RESET_UART 1
#define BOOTLOADER_RESET_WDT  1


/* most important user parameters */

/** if '1' only the commands of activated functions (e.g. ambilight) will be 
 *  saved and trained. Also individual latex manual will be created. 
 *  Else if '0' all functions (present or not) have to be assigned in training sequence
 */
#define INDIVIDUAL_CONFIG 1

/** enables or disables auto save of parameters to eeprom 
 *  after a while without received IR-Data
 *  also see USER_DELAY_BEFORE_SAVE_EEPROM_s
 */
#define USER_AUTOSAVE 1




// Debugging:
#define MAIN_LOG               0  /**< reports clock init on UART                               */
#define MAIN_LOG_BRIGHTNESS    0  /**< reports brightnes updates to UART                        */
#define LDR_LOG                0  /**< reports all ldr measurements to UART                     */
#define LDR2PWM_LOG            0  /**< reports changes on ldr2pwm mapping to UART               */
#define IR_LOGGING             0  /**< reports the scanned IR signal to UART                    */
#define DISPLAY_LOG_STATE      0  /**< reports the display output data to UART                  */
#define USER_LOG_IR_CMD        0  /**< reports the detected IR Command to UART                  */
#define USER_LOG_STATE         0  /**< reports the internal state changes to UART               */
#define USER_LOG_TIME          0  /**< reports internal time updates to UART                    */
#define USER_LOG_IR_TRAIN      0  /**< reports some usefull information on IR-Training to UART  */
#define EEPROM_LOG_INIT        0  /**< reports EEPROM data on init to UART                      */
#define EEPROM_LOG_WRITEBACK   0  /**< reports changed EEPROM data to UART                      */
#define DCF77_LOG              0

#define TRUE                   1
#define FALSE                  0

#ifndef NULL
#  define NULL 0
#endif

typedef struct
{
  unsigned char  YY;   ///< year     0-99
  unsigned char  MM;   ///< month    1-12
  unsigned char  DD;   ///< day      0-31
  unsigned char  hh;   ///< hour     0-23
  unsigned char  mm;   ///< minutes  0-59
  unsigned char  ss;   ///< seconds  0-59
  unsigned char  wd;   ///< weekday  0-6, 0 = sunday
} DATETIME;

extern void       main_ISR(void);

#ifdef __cplusplus
}
#endif
#endif /* _WC_X_H_ */
