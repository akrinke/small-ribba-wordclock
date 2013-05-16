/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file user.h
 * 
 *  The user-module handles the user interface of the wordclock
 *  \details
 *    This includes the reaction on the input via IR-RC as well as
 *    the output of the time.
 *
 * \version $Id: user.h 403 2011-11-13 00:08:32Z sm $
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


#ifndef _WC_USER_H_
#define _WC_USER_H_

#include "main.h"
#include "display.h"

#ifdef __cplusplus
extern  "C"
{
#endif


typedef enum e_MenuStates{

  MS_irTrain=0,
  MS_normalMode,
  MS_demoMode,

#if (MONO_COLOR_CLOCK != 1)
  MS_hueMode,
#endif //(MONO_COLOR_CLOCK != 1)
  MS_pulse,


  MS_setSystemTime,
  MS_setOnOffTime,
  MS_enterTime,

  MS_showNumber,

  MS_COUNT,

}e_MenuStates;



#ifndef INDIVIDUAL_CONFIG
/** if active only the commands of activated functions will be saved and trained */
#  define INDIVIDUAL_CONFIG 1
#endif

#ifndef USER_AUTOSAVE
/** enables or disables auto save of parameters to eeprom 
 *  after a while without received IR-Data
 *  also see USER_DELAY_BEFORE_SAVE_EEPROM_s
 */
#  define USER_AUTOSAVE 1
#endif

/** the time that has to elapse without any received IR-cmd before update eeprom data */
#define USER_DELAY_BEFORE_SAVE_EEPROM_s 120

/** the minimum time that has to elapse without any received IR-cmd before checking, if autoOff is active (@EDI)
(the check is done every full minute only) */
#define USER_DELAY_CHECK_IF_AUTO_OFF_REACHED_s 10

/**
 *  This enum defines all commands that can be given via an remote control.
 *  It is used as an index to the array that holds the ir codes.
 *  So this rder also defines the order that the training will use.
 *  \remark if changed order here make sure that the order of USER_COMMANDCODES_DEFAULTS
 *          is changed as well
 */
typedef enum e_userCommands
{
  UI_ONOFF           = 0,   /**< switch on/off LEDs  */
  UI_BRIGHTNESS_UP   ,      /**< increase brightness */
  UI_BRIGHTNESS_DOWN ,      /**< decrease brighntess */

  UI_UP              ,      /**< up/increase  (depends on current state)   */
  UI_DOWN            ,      /**< down/decrease  (depends on current state) */

  UI_SET_TIME        ,      /**< set time manually                         */
  UI_SET_ONOFF_TIMES ,      /**< configure times to deactivate the clock   */

#if INDIVIDUAL_CONFIG==0 || DCF_PRESENT==1
  UI_DCF_GET_TIME    ,      /**< forces update via dcf radio clock signal   */
#endif

  UI_NORMAL_MODE     ,      /**< activates normal display mode and switches color profiles if MONO_COLOR_CLOCK not configured  */
  UI_PULSE_MODE      ,      /**< (de)activates pulsing leds if in normal or hue-fading mode   */

  UI_DEMO_MODE       ,      /**< activates demo mode (activates each LED in sewquence)        */

#if INDIVIDUAL_CONFIG==0 || MONO_COLOR_CLOCK!=1
  UI_HUE_MODE        ,      /**< activades hue auto fadiing mode   slowly changes colors  */
  UI_CHANGE_R        ,      /**< to change the red channel with up/down                   */
  UI_CHANGE_G        ,      /**< to change the green channel with up/down                 */
  UI_CHANGE_B        ,      /**< to change the blue channel with up/down                  */
  UI_CHANGE_HUE      ,      /**< to change the color (hue) with up/down                   */
#endif

  UI_CALIB_BRIGHTNESS,      /**< sets the current user value into the look up table       */

#if INDIVIDUAL_CONFIG==0 || AMBILIGHT_PRESENT==1
  UI_AMBIENT_LIGHT   ,      /**< (de)activates the ambilight GPO1                         */
#endif
#if INDIVIDUAL_CONFIG==0 || BLUETOOTH_PRESENT==1
  UI_BLUETOOTH       ,      /**< (de)activates the bluetooth GPO2                         */
#endif
#if INDIVIDUAL_CONFIG==0 || AUXPOWER_PRESENT==1
  UI_AUXPOWER        ,      /**< (de)activates the auxpower  GPO3                         */
#endif

  DISPLAY_SPECIAL_USER_COMMANDS   /**< see documentation of activated display interface for detailed description */

  UI_COMMAND_COUNT
}e_userCommands;



/* command defaults */
#define USER_ADDRESS_DEFAULT          0x7B80  /* Orion RC-BM 07660BM070 */

#define USER_CMD_DEF_ONOFF            0x0013  /* power     */
#define USER_CMD_DEF_BRIGHTNESS_UP    0x0018  /* fwd       */
#define USER_CMD_DEF_BRIGHTNESS_DOWN  0x0019  /* bwd       */
#define USER_CMD_DEF_UP               0x000A  /* ch+       */
#define USER_CMD_DEF_DOWN             0x000B  /* ch-       */
#define USER_CMD_DEF_SET_TIME         0x0009  /* 9         */
#define USER_CMD_DEF_SET_ONOFF_TIMES  0x0015  /* rec/otr   */
#define USER_CMD_DEF_DCF_GET_TIME     0x0000  /* 0         */
#define USER_CMD_DEF_NORMAL_MODE      0x0001  /* 1         */
#define USER_CMD_DEF_PULSE_MODE       0x001e  /* SP/LP     */
#define USER_CMD_DEF_DEMO_MODE        0x0003  /* 3         */
#define USER_CMD_DEF_HUE_MODE         0x0002  /* 2         */
#define USER_CMD_DEF_CHANGE_R         0x0004  /* 4         */
#define USER_CMD_DEF_CHANGE_G         0x0005  /* 5         */
#define USER_CMD_DEF_CHANGE_B         0x0006  /* 6         */
#define USER_CMD_DEF_CHANGE_HUE       0x0007  /* 7         */
#define USER_CMD_DEF_CALIB_BRIGHTNESS 0x0045  /* enter     */
#define USER_CMD_DEF_AMBILIGHT        0x0052  /* atr       */
#define USER_CMD_DEF_BLUETOOTH        0x0013  /* inactive  */
#define USER_CMD_DEF_AUXPOWER         0x0013  /* inactive  */
// for display commands see appropriate display header





/** a simplified time structure that contains only the necessary things*/
typedef struct UiTime{
  uint8_t h;  /**< hours [0..23]*/
  uint8_t m;  /**< hours [0..59]*/
}UiTime;


/** maximum number of supported auto-switch off cycles 
 *  \remark  if changing number also the defaults have to be changed!
 */
#define UI_MAX_ONOFF_TIMES 1

#define UI_AUTOOFFTIMES_COUNT (UI_MAX_ONOFF_TIMES*2)

#define USER_DEFAULT_TIME  { /* .h = */ 0,  /* .m = */ 0 }
#define USER_ON_OFF_TIME_DEFAULTS         \
{ /* [0] =  off = */ USER_DEFAULT_TIME,   \
  /* [1] =  on  = */ USER_DEFAULT_TIME    \
}


/** the maximum number of color-presets that can be stored */
#define UI_COLOR_PRESET_COUNT 4

#define USER_COLORPRESETS_DEFAULTS {\
   { MAX_PWM_STEPS-1,               0,               0 } /* red   */, \
   {               0, MAX_PWM_STEPS-1,               0 } /* green */, \
   {               0,               0, MAX_PWM_STEPS-1 } /* blue  */, \
   { MAX_PWM_STEPS-1, MAX_PWM_STEPS-1, MAX_PWM_STEPS-1 } /* white */, \
}




/** contains the persistent data of the user module that should be stored in eeprom */
typedef struct UserEepromParams{

  /** Address of the trained remote control */
  uint16_t irAddress;

  /**
   *  IR - controll commands for user interaction 
   *  see e_userCommands for indexes 
   */
  uint16_t irCommandCodes[UI_COMMAND_COUNT];

  /** color presets */
  struct rgb{
    uint8_t r;
    uint8_t g;
    uint8_t b;
  }colorPresets[UI_COLOR_PRESET_COUNT];

  /** the current selected profile */
  uint8_t curColorProfile;

  /**
   *  holds times for automatic switch off 
   *  \details  2 consecutive times belong to one switch off/on time set.
   *            Set both times to same time to deactivate the switch off.\n
   *            If times overlap undefined behaviour may occur.
   *  
   */
  UiTime  autoOffTimes [UI_AUTOOFFTIMES_COUNT];

  /** 1=Animation in autoOff state, 0=animation off */
  uint8_t useAutoOffAnimation;

  /** compare value for prescaler between to animation steps for pulse state */
  uint8_t pulseUpdateInterval;

  /** compare value for prescaler between to animation steps for hue fading mode */
  uint8_t  hueChangeIntervall;  

  /**
   * The base mode (the bottom of the mode stack) 
   * \details  The Pulse state is coded in bit 7
   */
  uint8_t  mode;
}UserEepromParams;



uint8_t useAutoOffAnimation;



/** the delay after a keypress before another will be recognized */
#define USER_KEY_PRESS_DELAY_100ms   3

#define USER_KEY_PRESS_REALLY_LONG_DURATION_100ms 30

/** the initial delay between hue changes in auto-hue mode */
#define USER_HUE_CHANGE_INT_100ms   1
/** the minimal delay between hue changes in auto-hue mode that the user can select */
#define USER_HUE_CHANGE_INT_100ms_MIN   1
/** the maximal delay between hue changes in auto-hue mode that the user can select */
#define USER_HUE_CHANGE_INT_100ms_MAX  8

/** the initial delay between brightness changes in pulse mode */
#define USER_PULSE_CHANGE_INT_10ms   1
/** the minimal delay between brightness changes in pulse mode that the user can select */
#define USER_PULSE_CHANGE_INT_10ms_MIN   1
/** the maximal delay between brightness changes in pulse mode that the user can select */
#define USER_PULSE_CHANGE_INT_10ms_MAX  5



/** the delay between demo mode steps */
#define USER_DEMO_CHANGE_INT_100ms   5
/** how long the color profile number will be displayed */
#define USER_NORMAL_SHOW_NUMBER_DELAY_100ms 10

/** the waiting time (in s) for an IR-command after startup */
#define USER_STARTUP_WAIT_4_IR_TRAIN_s    7


/** defines the hour when the brightness changes in time input modes     */
#define USER_ENTERTIME_DAY_NIGHT_CHANGE_HOUR  8  
/** defines the brightness then entering day hours in time input modes   */
#define USER_ENTERTIME_DAY_BRIGHTNESS       255
/** defines the brightness then entering night hours in time input modes */
#define USER_ENTERTIME_NIGHT_BRIGHTNESS      50

/** enter step size while entering on/off time */
#define USER_ENTER_ONOFF_TIME_STEP 15


/**
 *  This routine checks if a new ir commands was received and process it if there was one.
 *  \details  This function should be called regularly but without any
 *            realtime constraints. (Just not to seldom to lose events.)
 */
extern void    handle_ir_code ();


/**
 * Sets a new Time
 * @details  Encapsulates the display-routine setNewTime
 *           to realize different modes of operation, so
 *           the display haven't to be changed externally.
 *           A call to this may not change the display.
 * @param i_time  the new time to set
 */
extern void  user_setNewTime( const DATETIME* i_time);


/**
 * does an animation step in different modes
 * Has to be called with 100Hz.
 */
extern void  user_isr100Hz(void);


/**
 * does an animation step in different modes
 * Has to be called with 10Hz.
 */
extern void  user_isr10Hz(void);


/**
 * handles some long-term jobs
 * Has to be called with 1Hz.
 */
extern void  user_isr1Hz(void);


/** initializes the user module
 *  have to be called after initialization of eeprom
 */
extern void user_init(void);


#define USER_CREATE_IR_HANDLER( command, code) \
          }else if( command == ir_code ){      \
              code                             \
              user_setNewTime( 0 );            








#if INDIVIDUAL_CONFIG==0 || DCF_PRESENT==1
#  define USER_CMD_DEFAULT_DCF                                  \
     /* [UI_DCF_GET_TIME     ] =  */  USER_CMD_DEF_DCF_GET_TIME, 
#else
#  define USER_CMD_DEFAULT_DCF
#endif


#if INDIVIDUAL_CONFIG==0 || MONO_COLOR_CLOCK!=1
#  define USER_CMD_DEFAULT_MULTICOLOR                           \
     /* [UI_HUE_MODE         ] =  */  USER_CMD_DEF_HUE_MODE  ,  \
     /* [UI_CHANGE_R         ] =  */  USER_CMD_DEF_CHANGE_R  ,  \
     /* [UI_CHANGE_G         ] =  */  USER_CMD_DEF_CHANGE_G  ,  \
     /* [UI_CHANGE_B         ] =  */  USER_CMD_DEF_CHANGE_B  ,  \
     /* [UI_CHANGE_HUE       ] =  */  USER_CMD_DEF_CHANGE_HUE,  
#else
#  define USER_CMD_DEFAULT_MULTICOLOR 
#endif

#if INDIVIDUAL_CONFIG==0 || AMBILIGHT_PRESENT==1
#  define USER_CMD_DEFAULT_AMBILIGHT                           \
    /* [UI_AMBIENT_LIGHT    ] =  */  USER_CMD_DEF_AMBILIGHT,  
#else
#  define USER_CMD_DEFAULT_AMBILIGHT
#endif

#if INDIVIDUAL_CONFIG==0 || BLUETOOTH_PRESENT==1
#  define USER_CMD_DEFAULT_BLUETOOTH                           \
     /* [UI_BLUETOOTH        ] =  */  USER_CMD_DEF_BLUETOOTH,
#else
#  define USER_CMD_DEFAULT_BLUETOOTH
#endif

#if INDIVIDUAL_CONFIG==0 || AUXPOWER_PRESENT==1
#  define USER_CMD_DEFAULT_AUXPOWER                            \
     /* [UI_AUXPOWER         ] =  */  USER_CMD_DEF_AUXPOWER,
#else
#  define USER_CMD_DEFAULT_AUXPOWER
#endif

#define USER_COMMANDCODES_DEFAULTS { \
 /* [UI_ONOFF            ] =  */  USER_CMD_DEF_ONOFF            ,  \
 /* [UI_BRIGHTNESS_UP    ] =  */  USER_CMD_DEF_BRIGHTNESS_UP    ,  \
 /* [UI_BRIGHTNESS_DOWN  ] =  */  USER_CMD_DEF_BRIGHTNESS_DOWN  ,  \
                                                                   \
 /* [UI_UP               ] =  */  USER_CMD_DEF_UP               ,  \
 /* [UI_DOWN             ] =  */  USER_CMD_DEF_DOWN             ,  \
                                                                   \
 /* [UI_SET_TIME         ] =  */  USER_CMD_DEF_SET_TIME         ,  \
 /* [UI_SET_ONOFF_TIMES  ] =  */  USER_CMD_DEF_SET_ONOFF_TIMES  ,  \
                                                                   \
 USER_CMD_DEFAULT_DCF                                              \
                                                                   \
 /* [UI_NORMAL_MODE      ] =  */  USER_CMD_DEF_NORMAL_MODE      ,  \
 /* [UI_PULSE_MODE       ] =  */  USER_CMD_DEF_PULSE_MODE       ,  \
 /* [UI_DEMO_MODE        ] =  */  USER_CMD_DEF_DEMO_MODE        ,  \
                                                                   \
 USER_CMD_DEFAULT_MULTICOLOR                                       \
                                                                   \
 /* [UI_CALIB_BRIGHTNESS ] =  */  USER_CMD_DEF_CALIB_BRIGHTNESS ,  \
                                                                   \
 USER_CMD_DEFAULT_AMBILIGHT                                        \
 USER_CMD_DEFAULT_BLUETOOTH                                        \
 USER_CMD_DEFAULT_AUXPOWER                                         \
                                                                   \
 DISPLAY_SPECIAL_USER_COMMANDS_CODES                               \
}



#define USEREEPROMPARAMS_DEFAULT  {                          \
  /* .irAddress           = */ USER_ADDRESS_DEFAULT,         \
  /* .irCommandCodes      = */ USER_COMMANDCODES_DEFAULTS,   \
  /* .colorPresets        = */ USER_COLORPRESETS_DEFAULTS,   \
  /*  curColorProfile     = */ 0,                            \
  /* .autoOffTimes        = */ USER_ON_OFF_TIME_DEFAULTS,    \
  /* .useAutoOffAnimation = */ 1,                            \
  /* .pulseUpdateInterval = */ USER_PULSE_CHANGE_INT_10ms,   \
  /* .hueChangeIntervall  = */ USER_HUE_CHANGE_INT_100ms,    \
  /* .mode                = */ MS_normalMode                 \
}
  





#ifdef __cplusplus
}
#endif

#endif /* _WC_USER_H_ */
