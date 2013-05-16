/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file user.c
 * 
 *  This file contains implementation of the user interface.
 *
 * \version $Id: user.c 409 2011-12-11 20:16:21Z vt $
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


/*
    An/Ausschaltlogik:
    ==================

    Z {normal_on, auto_off, user_off, override_on, }  
    E {ontime, offtime, IROnOff}
      
        Z            E          Z'
     normal_on    ontime     normal_on
     normal_on    offTime    auto_off
     normal_on    IROnOff    user_off
     auto_off     ontime     normal_on
     auto_off     offtime    auto_off
     auto_off     IROnOff    override_on
     user_off     ontime     user_off
     user_off     offtime    user_off		// @EDI: would propose "auto_off"
     user_off     IROnOff    normal_on
     override_on  ontime     normal_on
     override_on  offTime    override_on
     override_on  IROnOff    user_off		// @EDI: would propose "auto_off"


*/


#include <inttypes.h>
#include <avr/interrupt.h>

#include "main.h"
#include "user.h"
#include "irmp.h"
#include "pwm.h"
#include "i2c-rtc.h"
#include "display.h"
#include "dcf77.h"

#include "base.h"
#include "wceeprom.h"
#include "uart.h"
#include "color_effects.h"


#define USER_AMBILIGHT_PORT PORTB
#define USER_AMBILIGHT_DDR  DDRB
#define USER_AMBILIGHT_PIN  PINB
#define USER_AMBILIGHT_BIT  PIN1

#define USER_BLUETOOTH_PORT PORTC
#define USER_BLUETOOTH_DDR  DDRC
#define USER_BLUETOOTH_PIN  PINC
#define USER_BLUETOOTH_BIT  PIN1

#define USER_AUXPOWER_PORT PORTD
#define USER_AUXPOWER_DDR  DDRD
#define USER_AUXPOWER_PIN  PIND
#define USER_AUXPOWER_BIT  PIN2
/** 
 * leaves the given substate and all following states on stack 
 * @param   indexOfStateToLeave   stack position of State to leave
 */
static uint8_t leaveSubState( int8_t indexOfStateToLeave);

/** 
 *  adds a state top of the stack 
 *  @param mode   the state to add
 *  @param param  parameter for the sub state
 */
static void    addState(e_MenuStates mode, const void* param);

/**
 *  adds a sub state to a specific state from stack and tries to leave all following states
 *  @param  curState    the type of current State
 *  @param  mode        the state to add
 *  @param param  parameter for the sub state
 */
static void    addSubState( int8_t curState, e_MenuStates mode, const void* param);

/**
 *  a state quits it self
 *  This should be called by states that fullfilled their task.
 *  The result will be transmitted to the parant mode
 *  @param state   the state to leave
 *  @param result  the result that will be transmitted to underlieing state
 */
static void    quitMyself ( e_MenuStates state, const void* result);

#define  USER_MAX_STATE_DEPTH 10


/**
 *  Contains the current active states.
 *  the state at index 0 is the most parent state.
 *  Each mode can only be one Time at the stack
 *
 *  Mapping  index -> e_MenuStates
 */
static uint8_t     g_stateStack[USER_MAX_STATE_DEPTH];

/**
 *  contains the current position on stack for each mode.
 *  Will not be cleared on leave state!
 *
 *  Mapping  e_MenuStates -> index
 */
static uint8_t     g_currentIdxs[MS_COUNT];

/** the first free field on the stack*/
static int8_t      g_topOfStack;


static uint8_t      g_keyDelay;

enum SWITCHED_OFF{
  USO_NORMAL_ON = 0,
  USO_OVERRIDE_ON,
  USO_AUTO_OFF,
  USO_MANUAL_OFF,
};
static uint8_t      g_userSwitchedOff;

#if (AMBILIGHT_PRESENT == 1)
static uint8_t      g_settingOfAmbilightBeforeAutoOff;  // only of interest when clock is turning on again, AutoAnim
#endif                                                  // is on and ambilight is installed to know the old state (on/off) of AL

static uint8_t      g_animPreview = 0;                  // @EDI: activates a preview of the autoOffAnimation while setting on/off

static uint8_t      g_eepromSaveDelay;
static uint8_t      g_checkIfAutoOffDelay;

static e_userCommands g_lastPressedKey;

static uint8_t        g_keyPressTime;
static DATETIME g_dateTime;

#define g_params (&(wcEeprom_getData()->userParams))


static void dispInternalTime(const DATETIME* i_time, uint32_t blinkmask);
static uint8_t checkActivation(void);
static uint8_t curTimeIsBetween ( uint8_t h1, uint8_t m1, uint8_t h2, uint8_t m2);


#if (USER_LOG_STATE == 1) 
static void printStateStack(){
  uint8_t i=0;
  uart_puts_P("stack: [");
  for(;i<g_topOfStack;++i)
  {
    uart_putc(g_stateStack[i] + '0');
    uart_putc(' ');
  }

  for(;i<USER_MAX_STATE_DEPTH;++i)
  {
    uart_putc('_');
    uart_putc(' ');
  }
  uart_putc(']');
  uart_puts_P("top: ");
  uart_putc( g_topOfStack + '0');
  uart_putc('\n');

}

#define PRINT_STATE_STACK() do{printStateStack();}while(0)
#else
#define PRINT_STATE_STACK() 
#endif




#if (USER_LOG_STATE == 1)
#  define   log_state(x)  uart_puts_P(x)
#else
#  define   log_state(x)  
#endif


#if (USER_LOG_TIME== 1)
#  define   log_time(x)  uart_puts_P(x)
void putTime( const DATETIME* time)
{
  char txt[8];
  byteToStrLessHundred(time->hh, txt);
  uart_puts(txt);
  uart_putc(':');
  byteToStrLessHundred(time->mm, txt);
  uart_puts(txt);
  uart_putc('\n');
}
#else
#  define   log_time(x)  
#  define   putTime(x)
#endif

// just include file here to give compiler more possibilities for optimizing 
/// @todo consider making normal interface and using whole program optimization
#include "usermodes.c"


static void addState(e_MenuStates mode, const void* param)
{
    log_state( "addstate\n");
    // now add new State
    if(    (g_topOfStack == 0)
        || (g_stateStack[g_topOfStack-1] != mode) )  // avoid stacking of same state
    {
      g_stateStack[g_topOfStack] = mode;
      g_currentIdxs[mode] = g_topOfStack;
      ++g_topOfStack;
    }
    UserState_enter(mode, param);

    PRINT_STATE_STACK();
}

static void addSubState(int8_t curState, e_MenuStates mode, const void* param)
{
  uint8_t success = TRUE;
  int8_t nextIdx;
  if( curState == -1 ){
    nextIdx = 0;
    log_state("replace Base state\n");
  }else{
    nextIdx = g_currentIdxs[curState] +1;
  }
  log_state( "addSubstate\n");
  if( nextIdx != g_topOfStack)  // first remove all pending sub states
  {
    success = leaveSubState( nextIdx );
  }

  if( success ){
    addState(mode, param);
  }else{
    log_state( "ERROR: leaving substates failed\n");
  }

}

static uint8_t leaveSubState( int8_t indexOfStateToLeave)
{
  int8_t i;
  uint8_t success = TRUE;
  log_state("leaving substates: ");
  for( i = g_topOfStack-1; i>=indexOfStateToLeave; --i)   /* check if all substates are ready to leave */
  {
      uint8_t canLeave = ! UserState_prohibitLeave( g_stateStack[i] );
      success = success && canLeave;
#     if (USER_LOG_STATE == 1) 
      {
        char buff[5];
        byteToStrLessHundred(g_stateStack[i], buff);
        uart_puts(buff);
        uart_putc(':');
        uart_putc(canLeave?'y':'n');
        uart_puts_P(", ");
      }
#     endif

  }

  if( success ){
    for( i = g_topOfStack-1; i>=indexOfStateToLeave; --i)   /* remove all pending substates*/
    {
      UserState_LeaveState(g_stateStack[i]);
      --g_topOfStack;
      PRINT_STATE_STACK();
    }
  }

  return success;
}


static void quitMyself( e_MenuStates state, const void* result)
{
  int8_t currentIdx = g_currentIdxs[state];
  log_state( "quit self\n");
  uint8_t success = leaveSubState( currentIdx );
  if( ! success ){
    log_state( "ERROR: leaving substates failed\n");
  }
  dispInternalTime( &g_dateTime,  0);
  UserState_SubstateFinished(g_stateStack[currentIdx-1], state, result);
  log_state( "quit self finished\n");
}





/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * USER: handle RC5
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
handle_ir_code (void)
{
  IRMP_DATA ir_data;

  if (irmp_get_data (&ir_data))
  {
    if(g_keyDelay){
      return;
    }
#   if (USER_LOG_IR_CMD == 1)
    {
      char text[20];
      uart_puts_P("IR-cmd: ");
      uint16ToHexStr(ir_data.protocol, text);
      uart_puts(text);
      uint16ToHexStr(ir_data.address, text);
      uart_puts(text);
      uint16ToHexStr(ir_data.command, text);
      uart_puts(text);
      uart_putc('\n');
    }
#   endif
    g_keyDelay = USER_KEY_PRESS_DELAY_100ms;

    if( g_stateStack[ g_topOfStack-1] == MS_irTrain )  // special handling in training mode
    {

      TrainIrState_handleIR( &ir_data );
    }else{
        uint8_t ir_code = 0;
        if( g_params->irAddress != ir_data.address )         // check if received datagramm is from correct IRC
          return;
        while(    (ir_code<UI_COMMAND_COUNT)                                 // translate Code into
               && (g_params->irCommandCodes[ir_code] != ir_data.command)){   // intern number
                 ++ir_code;
        }
        if( UI_ONOFF == ir_code ){  /* has to be first because keys that the user does not need will have ONOFF code */
            log_state("OF\n");

           //normal_on    IROnOff    user_off
           //override_on  IROnOff    user_off                // @EDI: would propose "auto_off"
           //auto_off     IROnOff    override_on
           //user_off     IROnOff    normal_on
            if(g_userSwitchedOff < USO_AUTO_OFF){
              g_userSwitchedOff = USO_MANUAL_OFF;
              pwm_off();
              } else {
              if(g_userSwitchedOff == USO_MANUAL_OFF){
              g_userSwitchedOff = USO_NORMAL_ON;
              }else{
                g_userSwitchedOff = USO_OVERRIDE_ON;
              }
              pwm_on();
              user_setNewTime(NULL);
          }

          wcEeprom_writeback( wcEeprom_getData(), sizeof(WcEepromData)); /* save whole data */
        }else{
          int8_t i;
          uint8_t handled = FALSE;
          for( i = g_topOfStack-1; i>=0 && !handled; --i)                           // goto state stack and check
          {                                                                         // if someone handles the code
            handled  =    handled || UserState_HanbdleIr(g_stateStack[i], ir_code);
          }

          if( !handled )                                     // command was not handled by subState
          {   
            if( UI_BRIGHTNESS_UP == ir_code ){
                log_state("B+\n");
                pwm_step_up_brightness();
            }else if( UI_BRIGHTNESS_DOWN == ir_code ){
                log_state("B-\n");
                pwm_step_down_brightness();

            }else if( UI_NORMAL_MODE == ir_code ){
                addSubState(-1, MS_normalMode, (void*)1);

            }else if( UI_SET_TIME == ir_code ){
              addState(MS_setSystemTime, NULL);

            }else if( UI_SET_ONOFF_TIMES == ir_code ){
              addState(MS_setOnOffTime, NULL);

            }else if( UI_DEMO_MODE == ir_code ){
              log_state("BS\n");
              e_MenuStates curTop = g_stateStack[g_topOfStack-1];
              if( MS_demoMode == curTop )
              {
                quitMyself(MS_demoMode, NULL);
              }else{
                addState(MS_demoMode, NULL);
              }

            }else if( UI_CALIB_BRIGHTNESS == ir_code ){
              pwm_modifyLdrBrightness2pwmStep();

            }else if( UI_PULSE_MODE == ir_code ){
              e_MenuStates curTop = g_stateStack[g_topOfStack-1];
              log_state("PLS\n");
              if( MS_pulse == curTop )
              {
                leaveSubState( g_topOfStack-1 );
              }else{
                if(    (MS_normalMode == curTop)
#                  if (MONO_COLOR_CLOCK != 1)
                   || (MS_hueMode == curTop)
#                  endif // (MONO_COLOR_CLOCK != 1)
                  )
                {
                  addState(MS_pulse, NULL);
                }
              }

            DISPLAY_SPECIAL_USER_COMMANDS_HANDLER

#           if (MONO_COLOR_CLOCK != 1)
              }else if( UI_HUE_MODE == ir_code ){
                    log_state("HM");
                    addSubState(-1, MS_hueMode, NULL);
#           endif // (MONO_COLOR_CLOCK != 1)
#           if (DCF_PRESENT == 1)
              }else if( UI_DCF_GET_TIME == ir_code ){
                  log_state("DCF\n");
                  enable_dcf77_ISR = TRUE;
#           endif  /** (DCF_PRESENT == 1) */
#           if (AMBILIGHT_PRESENT == 1)
              }else if( UI_AMBIENT_LIGHT == ir_code ){
                log_state("AL\n");
                USER_AMBILIGHT_PIN |= (1<< USER_AMBILIGHT_BIT);
#           endif
#           if (BLUETOOTH_PRESENT == 1)
              }else if( UI_BLUETOOTH == ir_code ){
                  log_state("BT\n");
                  USER_BLUETOOTH_PIN |= (1<< USER_BLUETOOTH_BIT);
#           endif
#           if (AUXPOWER_PRESENT == 1)
              }else if( UI_AUXPOWER == ir_code ){
                  log_state("AUX\n");
                USER_AUXPOWER_PIN |= (1<< USER_AUXPOWER_BIT);
#           endif
        //    }else if( g_params->irCommandCodes[] == ir_code ){
            }else{
              return;
            }// if( xxx == ir_code)
          }//if( i <0 )  // command was not handled by subState
        }// if( UI_ONOFF == ir_code ) else 
        g_params->mode = g_stateStack[0];
        if(  MS_pulse == g_stateStack[1]){
          g_params->mode |= 0x80;
        }
        g_eepromSaveDelay = 0; /* on recognized keypress reset the counters */
		g_checkIfAutoOffDelay = 0;
    }
  }else{
    g_lastPressedKey = UI_COMMAND_COUNT;
  }
}



/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/




void user_init(void)
{
  UserState_init();

  addState(g_params->mode & 0x7f, 0);
  if(    g_params->mode & 0x80){
    addState(MS_pulse, NULL);
  }

  addState(MS_irTrain,    NULL);

# if (AMBILIGHT_PRESENT == 1)
    USER_AMBILIGHT_DDR |= (1 << USER_AMBILIGHT_BIT);
# endif
# if (BLUETOOTH_PRESENT == 1)
    USER_BLUETOOTH_DDR |= (1 << USER_BLUETOOTH_BIT);
# endif
# if (AUXPOWER_PRESENT == 1)
    USER_AUXPOWER_DDR |= (1 << USER_AUXPOWER_BIT);
# endif
}


static void dispInternalTime(const DATETIME* i_time, uint32_t blinkmask)
{
  putTime(i_time);
  display_setDisplayState( display_getTimeState( i_time ), blinkmask);
}



extern void  user_setNewTime( const DATETIME* i_time)
{

  if( i_time ){
    log_time("saved Time ");
    g_dateTime = *i_time;
    if(    (g_checkIfAutoOffDelay >= USER_DELAY_CHECK_IF_AUTO_OFF_REACHED_s))
    {
      if( checkActivation() ){
        //  normal_on    ontime     normal_on
        //  override_on  ontime     normal_on
        //  auto_off     ontime     normal_on
        //  user_off     ontime     user_off  ??
        if( g_userSwitchedOff != USO_MANUAL_OFF )
            {
#         if (AMBILIGHT_PRESENT == 1)
            g_userSwitchedOff = USO_NORMAL_ON;
            USER_AMBILIGHT_PORT |= g_settingOfAmbilightBeforeAutoOff;  // pin is off before this call in each case so no need to clear it before OR
#         endif
            pwm_on();
        }
      }else{
        //  normal_on    offTime    auto_off
        //  override_on  offTime    override_on
        //  auto_off     offtime    auto_off
        //  user_off     offtime    user_off		// @EDI: would propose "auto_off"

        if( g_userSwitchedOff == USO_NORMAL_ON)
        {
            g_userSwitchedOff = USO_AUTO_OFF;
#         if (AMBILIGHT_PRESENT == 1)
            g_settingOfAmbilightBeforeAutoOff = USER_AMBILIGHT_PORT & (1<<USER_AMBILIGHT_BIT); // save io state
            USER_AMBILIGHT_PORT &= ~(1<<USER_AMBILIGHT_BIT); // switch off 
#         endif
            if( !g_params->useAutoOffAnimation ){
              pwm_off();
            }
        }
      }
    }
  }


  if(   ! UserState_prohibitTimeDisplay(g_stateStack[g_topOfStack - 1]) 
      && (g_userSwitchedOff != USO_AUTO_OFF) )
  {
      log_time("disp Time ");
      putTime(&g_dateTime);
      display_fadeNewTime(&g_dateTime);
      log_time("\n");
  }
}




void user_isr100Hz(void)
{                       //@EDI: State machine should also be updated when clock off!
  UserState_Isr100Hz(g_stateStack[g_topOfStack-1]);
}

void user_isr10Hz(void)
{                       // @EDI: State machine should also be updated when clock off!
  UserState_Isr10Hz(g_stateStack[g_topOfStack-1]);
  if( g_keyDelay != 0){
    --g_keyDelay;
  }
  ++g_keyPressTime;
}


void  user_isr1Hz(void)
{
  useAutoOffAnimation = 0;			// @EDI: only when anim is active (auto off or preview) this flag will be set to one

  if( g_eepromSaveDelay <= USER_DELAY_BEFORE_SAVE_EEPROM_s ){
    ++g_eepromSaveDelay;
  }

  if( g_checkIfAutoOffDelay <= USER_DELAY_CHECK_IF_AUTO_OFF_REACHED_s ){
    ++g_checkIfAutoOffDelay;
  }

# if USER_AUTOSAVE==1
      if(g_eepromSaveDelay == USER_DELAY_BEFORE_SAVE_EEPROM_s){
        wcEeprom_writeback( wcEeprom_getData(), sizeof(WcEepromData)); /* save whole data */
      }
# endif


  if( (g_userSwitchedOff != USO_AUTO_OFF) && (!g_animPreview) )
  {
    UserState_Isr1Hz(g_stateStack[g_topOfStack-1]);

  }else{
    if(g_params->useAutoOffAnimation)
    {
      display_autoOffAnimStep1Hz(g_animPreview);
      useAutoOffAnimation = 1;
    } 
  }
}



static uint8_t curTimeIsBetween ( uint8_t h1, uint8_t m1, uint8_t h2, uint8_t m2)
{
  uint8_t h = g_dateTime.hh;
  uint8_t m = g_dateTime.mm;
  uint8_t largert1  =     (h > h1)  
                      || ((h == h1) && (m >= m1));
  uint8_t lesst2    =     (h < h2) 
                      || ((h == h2) && (m < m2));

  if(  h2<h1 ){ // overflow over midnight
    return  largert1 || lesst2;
  }else{
    return  largert1 && lesst2;
  }
}

static uint8_t checkActivation(void)
{
  uint8_t on  = TRUE;
  uint8_t i;

  for( i=0; i<UI_AUTOOFFTIMES_COUNT; i+=2)
  {
    if(     curTimeIsBetween( g_params->autoOffTimes[i  ].h, g_params->autoOffTimes[i  ].m,
                              g_params->autoOffTimes[i+1].h, g_params->autoOffTimes[i+1].m)){
      on = FALSE;
    }
  }
  return on;
}



