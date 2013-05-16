/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file usermodes.c
 * 
 *  This file contains implementation of the internal states and modes
 *  \details
 *       This file is designed to be included in user.c.
 *       SO DO NOT ADD IT TO PROJECT OR MAKEFILE!
 *
 * \version $Id: usermodes.c 403 2011-11-13 00:08:32Z sm $
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


/** contains Data for ir training state */
typedef struct TrainIrState{
  uint8_t   seconds;  /**< number of seconds since start $to$ used to abort training   */
  uint8_t   curKey;   /**< current key to train                                        */
}TrainIrState;
static TrainIrState mode_trainIrState; 
static uint8_t TrainIrState_handleIR( const IRMP_DATA* i_irCode ); // forward decl because special handling



/** contains Data for show number state */
typedef struct ShowNumberState{
  uint8_t   delay100ms;  /** count doen how long to display number */
}ShowNumberState;
static ShowNumberState mode_showNumberState;


#if (MONO_COLOR_CLOCK != 1)

/** contains Data for normal state */
typedef struct NormalState{
  uint8_t   colorToSet;        /**< color to change with color change mode                                   */
  Hue_t    curHue;             /**< current hue value for manual hue state  @todo  calc current hue from RGB */
}NormalState;
static NormalState mode_normalState;

#endif //(MONO_COLOR_CLOCK != 1)

/** contains Data for pulse state */
typedef struct PulseState{
  uint8_t   curBrightness;     /**< current animation step                                  */
  uint8_t   delay100ms;        /**< current prescaler for animation progress                */
}PulseState;
static PulseState mode_pulseState;

#if (MONO_COLOR_CLOCK != 1)
/** contains Data for autmaic hue date state */
typedef struct AutoHueState{
  Hue_t     curHue;              /**< current hue value                                       */
  uint8_t   delay100ms;          /**< current prescaler for animation progress                */
}AutoHueState;
static AutoHueState mode_autoHueState;
#endif  // (MONO_COLOR_CLOCK != 1)


/** contains Data for demo state */
typedef struct DemoState{
  uint8_t   demoStep;       /**< current step in demo animation             */
  uint8_t   delay100ms;     /**< current prescaler for animation progress   */
}DemoState;
static DemoState mode_demoState;


/** contains Data for set system time state */
typedef struct SetSystemTimeState{
  uint8_t   prohibitLeave;     /**<  is it allowed to leave state at the moment? */
}SetSystemTimeState;
static SetSystemTimeState mode_setSystemTimeState;


/** contains Data for set on off time state */
typedef struct SetOnOffmTimeState{
  uint8_t   prohibitLeave;     /**<  is it allowed to leave state at the moment? */
  uint8_t   currentTimeToSet;  /**<  index of time to set at the moment          */
}SetOnOffmTimeState;
static SetOnOffmTimeState mode_setOnOffTimeState;


/** contains Data for time input state */
typedef struct EnterTimeState{
  uint8_t   prohibitLeave;     /**<  is it allowed to leave state at the moment?         */
  DATETIME  time;              /**<  buffer that will be manipulated during time input   */
  enum e_EnterTimesubStates{
    ETS_hour,
    ETS_minutes, 
  }curSubState;                /**<  current substate of time input                      */
}EnterTimeState;
static EnterTimeState mode_enterTimeState;




/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/

/** initializes all states  */
static void UserState_init();

/** Dispatcher routine to determine if the given state can be left now  */
static uint8_t UserState_prohibitLeave( e_MenuStates state );

/** Dispatcher routine to determine if the new time can be displayed now */
static uint8_t UserState_prohibitTimeDisplay( e_MenuStates state );

/** Dispatcher routine for the 100Hz timer event */
static void UserState_Isr100Hz( e_MenuStates state );

/** Dispatcher routine for the 10Hz timer event */
static void UserState_Isr10Hz( e_MenuStates state );

/** Dispatcher routine for the 1Hz timer event */
static void UserState_Isr1Hz( e_MenuStates state );

/** Dispatcher routine for leaving a state */
static void UserState_LeaveState( e_MenuStates state );

/** Dispatcher routine for processing the IR signals */
static uint8_t UserState_HanbdleIr (e_MenuStates state, uint8_t cmdCode);

/** Dispatcher routine to handle the result of a finished sub state */
static void    UserState_SubstateFinished(e_MenuStates state, e_MenuStates finishedState, const void* result);

/** Dispatcher routine for enter a state */
static void UserState_enter( e_MenuStates state,  const void* param );



/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/



static void incDecRange(uint8_t* val, int8_t dir,  uint8_t min, uint8_t max)
{ 
  if( dir<0){
    if( *val>min ){
      *val +=dir;
    }
  }else{
    if(*val<max)
      *val +=dir;
  }
}

static void incDecRangeOverflow(uint8_t* val, int8_t dir,  uint8_t max)
{
  if( dir<0 ){
    if(*val < -dir ){
      *val = max+1 + dir;
      return;
    }
  }
  *val +=dir;
  if(*val>max){
    *val = 0;
  }
}

/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/

#if (USER_LOG_IR_TRAIN == 1)
#  define   log_irTrain(x)  uart_puts_P(x)
#else
#  define   log_irTrain(x)  
#endif



static void TrainIrState_enter( const void* param )
{
  log_state( "enter train\n");
  display_setDisplayState( display_getIndicatorMask(), display_getIndicatorMask() );
}


static void TrainIrState_1Hz( void )
{
  if( mode_trainIrState.seconds != UINT8_MAX){
    ++mode_trainIrState.seconds;
    if( mode_trainIrState.seconds == USER_STARTUP_WAIT_4_IR_TRAIN_s ){
      log_irTrain("leave IR-wait4train\n");
      quitMyself(MS_irTrain,NULL);
    }
  }
}

static uint8_t TrainIrState_handleIR( const IRMP_DATA* i_irCode )
{
  DisplayState disp;

  if( mode_trainIrState.curKey > 0){  // save key
    if( g_params->irAddress != i_irCode->address ){
      log_irTrain("invalid ir-address\n");
    }else{
      g_params->irCommandCodes[mode_trainIrState.curKey-1] = i_irCode->command;
      if( mode_trainIrState.curKey == UI_COMMAND_COUNT ){ /* finished */
        log_irTrain("Ir train finished\n");
        wcEeprom_writeback( g_params, sizeof(*g_params) ); /* save all */
        quitMyself(MS_irTrain,NULL);
        return TRUE;
      }
      ++mode_trainIrState.curKey;
    }
  }else{
    mode_trainIrState.seconds = UINT8_MAX;
    g_params->irAddress = i_irCode->address;
    ++mode_trainIrState.curKey;
  }

# if USER_LOG_IR_TRAIN
  {
    char buff[5];
    uart_puts_P("Ir train. Enter cmd #");
    byteToStrLessHundred(mode_trainIrState.curKey, buff);
    uart_puts(buff);
    uart_putc('\n');
  }
# endif

  disp = display_getNumberDisplayState(mode_trainIrState.curKey) | display_getIndicatorMask();
  display_setDisplayState( disp, disp );

  return TRUE;
}

static void TrainIrState_init( void )
{

}


/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/


static void ShowNumberState_10Hz( void )
{
  --mode_showNumberState.delay100ms;
  if( mode_showNumberState.delay100ms == 0 ){
    quitMyself(MS_showNumber, NULL);
  }
}

static void ShowNumberState_enter( const void* param )
{
  DisplayState dispData;
  uint8_t paramSN = (uint8_t)((uint16_t)param);  /* double cast to avoid warning */
  log_state( "enter showNumber\n");
  mode_showNumberState.delay100ms = USER_NORMAL_SHOW_NUMBER_DELAY_100ms;

  dispData = display_getNumberDisplayState(paramSN);
  display_setDisplayState(dispData, 0);
}

static void ShowNumberState_init( void )
{
}

/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/


static void NormalState_enter( const void* param )
{
  log_state("nm\n");

#if (MONO_COLOR_CLOCK != 1)
  pwm_set_color_step( g_params->colorPresets[g_params->curColorProfile].r,
                      g_params->colorPresets[g_params->curColorProfile].g,
                      g_params->colorPresets[g_params->curColorProfile].b);
  if( ((uint16_t)param) != 0 ){
    addSubState(MS_normalMode, MS_showNumber, (void*)(((uint16_t)g_params->curColorProfile+1)) ); /* doulbe cast to avoid warning */
  }
#else
  dispInternalTime( &g_dateTime,  0);  // force update of display (in multicolor this is done on substate exit)
#endif
}


static uint8_t NormalState_handleIR(  uint8_t cmdCode )
{

#if (MONO_COLOR_CLOCK != 1)
  if( UI_NORMAL_MODE == cmdCode){
    ++(g_params->curColorProfile);
    g_params->curColorProfile %=  UI_COLOR_PRESET_COUNT;
    NormalState_enter((void*)1);
  }else if( UI_CHANGE_R == cmdCode ){
    log_state("CR\n");
    mode_normalState.colorToSet = 0;
  }else if( UI_CHANGE_G == cmdCode ){
    log_state("CG\n");
    mode_normalState.colorToSet = 1;
  }else if( UI_CHANGE_B == cmdCode ){
    log_state("CB\n");
    mode_normalState.colorToSet = 2;
  }else if( UI_CHANGE_HUE == cmdCode ){
    log_state("CH\n");
    mode_normalState.colorToSet = 4;

  }else if(    UI_UP == cmdCode
            || UI_DOWN == cmdCode)
  {
    log_state("CC\n");
    int8_t dir = UI_UP == cmdCode?1:-1;
    if( mode_normalState.colorToSet < 4 ){  // handle RGB-Changes
      uint8_t* rgb = (uint8_t*)(&g_params->colorPresets[ g_params->curColorProfile ]);
      incDecRange(&rgb[ mode_normalState.colorToSet ], dir,0,  MAX_PWM_STEPS-1);
      pwm_set_color_step( rgb[0], rgb[1], rgb[2] );
    }else{     // handle HUE-Changes
      uint8_t r,g,b;
      if( dir<0 && mode_normalState.curHue < HUE_MANUAL_STEPS ){
        mode_normalState.curHue = HUE_MAX;
      }else if( dir>0 && mode_normalState.curHue >= HUE_MAX-HUE_MANUAL_STEPS){
        mode_normalState.curHue = 0;
      }else{
        mode_normalState.curHue += dir*HUE_MANUAL_STEPS;
      }
      hue2rgb(mode_normalState.curHue, &r, &g, &b);
      SetPWMs(r,g,b);
    }
  }else

#endif
  {
    return FALSE;
  }

  return TRUE;
}

static void NormalState_init( void )
{
}


/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/

#if (MONO_COLOR_CLOCK != 1)



static void AutoHueState_10Hz( void )
{
  --mode_autoHueState.delay100ms;
  if( mode_autoHueState.delay100ms >= (volatile uint8_t)(g_params->hueChangeIntervall) )
  {
    ++mode_autoHueState.curHue;
    //AutoHueStat_enter(0);
    uint8_t r;
    uint8_t g;
    uint8_t b;
    mode_autoHueState.curHue %= (HUE_MAX+1);
    hue2rgb(mode_autoHueState.curHue, &r, &g, &b);
    SetPWMs(r,g,b);
    mode_autoHueState.delay100ms = g_params->hueChangeIntervall;
  }
}

static void AutoHueStat_enter( const void* param )
{
  AutoHueState_10Hz();
}

static uint8_t AutoHueState_handleIR(  uint8_t cmdCode )
{
  if(    UI_UP == cmdCode
      || UI_DOWN == cmdCode)
  {
    log_state("CHS \n");
    int8_t dir = UI_UP == cmdCode?-1:1;
    incDecRange(    &g_params->hueChangeIntervall
                  , dir
                  , USER_HUE_CHANGE_INT_100ms_MIN
                  , USER_HUE_CHANGE_INT_100ms_MAX);
#if (USER_LOG_STATE == 1)
    uart_putc('0' + g_params->hueChangeIntervall);
#endif
  }else{
    return FALSE;
  }
   
  return TRUE;

}

static void AutoHueState_init( void )
{
}

#endif  // (MONO_COLOR_CLOCK != 1)


/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/

static void DemoState_10Hz( void )
{
  ++mode_demoState.delay100ms;
  if( mode_demoState.delay100ms >= USER_DEMO_CHANGE_INT_100ms )
  {
    display_setDisplayState((1L << mode_demoState.demoStep), 0);
    ++mode_demoState.demoStep;
    mode_demoState.demoStep %= 32;
    mode_demoState.delay100ms = 0;
  }

}

static void DemoState_init( void )
{
}

/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/

static void EnterTimeState_enter( const void* param )
{
  log_time("TH\n");
  mode_enterTimeState.time = *((DATETIME*)param);
  mode_enterTimeState.curSubState = ETS_hour;
  mode_enterTimeState.prohibitLeave = TRUE;

  if(   mode_enterTimeState.time.hh >=USER_ENTERTIME_DAY_NIGHT_CHANGE_HOUR 
     && mode_enterTimeState.time.hh < USER_ENTERTIME_DAY_NIGHT_CHANGE_HOUR+12 )
  {
    pwm_lock_brightness_val(USER_ENTERTIME_DAY_BRIGHTNESS);
  }else{
    pwm_lock_brightness_val(USER_ENTERTIME_NIGHT_BRIGHTNESS);
  }
  dispInternalTime( &mode_enterTimeState.time, display_getHoursMask()|display_getTimeSetIndicatorMask() );
}


static uint8_t EnterTimeState_handleIr(  uint8_t cmdCode )
{
  uint8_t caller = g_stateStack[ g_currentIdxs[MS_enterTime] - 1];
  if(    (   (MS_setSystemTime == caller) 
          && (UI_SET_TIME      == cmdCode))
      || (   (MS_setOnOffTime    == caller) 
          && (UI_SET_ONOFF_TIMES == cmdCode)))
  {
    if( ETS_hour == mode_enterTimeState.curSubState ){
      log_time("TM\n");
      mode_enterTimeState.curSubState = ETS_minutes;
    }else if( ETS_minutes == mode_enterTimeState.curSubState){
      log_time("TS\n");
      mode_enterTimeState.time.ss = 0;
      mode_enterTimeState.prohibitLeave = FALSE;
      pwm_release_brightness();
      quitMyself(MS_enterTime, &(mode_enterTimeState.time));
      return TRUE;
    }
  }else if(    UI_UP   == cmdCode
            || UI_DOWN == cmdCode)
  {
    log_state("CHS\n");
    int8_t dir = UI_UP == cmdCode?1:-1;
    if( ETS_hour == mode_enterTimeState.curSubState )
    {
      incDecRangeOverflow(&(mode_enterTimeState.time.hh), dir, 23);
      if(   mode_enterTimeState.time.hh >=USER_ENTERTIME_DAY_NIGHT_CHANGE_HOUR 
         && mode_enterTimeState.time.hh < USER_ENTERTIME_DAY_NIGHT_CHANGE_HOUR+12 )
      {
        pwm_lock_brightness_val(USER_ENTERTIME_DAY_BRIGHTNESS);
      }else{
        pwm_lock_brightness_val(USER_ENTERTIME_NIGHT_BRIGHTNESS);
      }
    }else if( ETS_minutes == mode_enterTimeState.curSubState){
      if( MS_setOnOffTime == caller ){
        dir *= USER_ENTER_ONOFF_TIME_STEP;
      }
      incDecRangeOverflow(&(mode_enterTimeState.time.mm), dir, 59);
    }

  }else{
    //return FALSE;
  }
  dispInternalTime(&mode_enterTimeState.time, 
                   ((ETS_hour == mode_enterTimeState.curSubState)
                    ? display_getHoursMask()
                    : display_getMinuteMask()
                    ) | display_getTimeSetIndicatorMask()) ;

  return TRUE;
}

static void EnterTimeState_init( void )
{
}


/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/



static void SetSystemTimeState_enter( const void* param )
{
  log_state("SST\n");
  addSubState(MS_setSystemTime, MS_enterTime,  &g_dateTime );
  mode_setSystemTimeState.prohibitLeave = TRUE;

}

static void SetSystemTimeState_substateFinished (e_MenuStates finishedState, const void* result)
{
  if( finishedState == MS_enterTime)
  {
    const DATETIME* time = result;
    i2c_rtc_write( time );
    g_dateTime = *time;
    mode_setSystemTimeState.prohibitLeave = FALSE;
    quitMyself( MS_setSystemTime, NULL);
  }
}

static void SetSystemTimeState_init( void )
{
}


/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/



static void SetOnOffTimeState_enter( const void* param )
{
  DATETIME dt = {0,0,0,0,0,0,0};
  dt.hh = g_params->autoOffTimes[0].h;
  dt.mm = g_params->autoOffTimes[0].m;

  log_state("SOOT\n");
  mode_setOnOffTimeState.currentTimeToSet = 0;
  
  addSubState(MS_setOnOffTime
             , MS_enterTime
             , &dt);
  mode_setOnOffTimeState.prohibitLeave = TRUE;

}

static void SetOnOffTimeState_substateFinished (e_MenuStates finishedState, const void* result)
{
  if( finishedState == MS_enterTime)
  {
    const DATETIME* time = result;
    DATETIME dt = *time;
    g_params->autoOffTimes[mode_setOnOffTimeState.currentTimeToSet].h = dt.hh;
    g_params->autoOffTimes[mode_setOnOffTimeState.currentTimeToSet].m = dt.mm;

    ++mode_setOnOffTimeState.currentTimeToSet;

    if( UI_AUTOOFFTIMES_COUNT == mode_setOnOffTimeState.currentTimeToSet )
    {
      DisplayState dispData;
      dispData = display_getNumberDisplayState(g_params->useAutoOffAnimation+1);
      display_setDisplayState(dispData, dispData);
      if(g_params->useAutoOffAnimation)
      {
        g_animPreview = 1;
      } else {
        g_animPreview = 0;
      }
    }else{
      dt.hh = g_params->autoOffTimes[mode_setOnOffTimeState.currentTimeToSet].h;
      dt.mm = g_params->autoOffTimes[mode_setOnOffTimeState.currentTimeToSet].m;
      addSubState(MS_setOnOffTime, MS_enterTime, &dt);
    }
  }
}

static void SetOnOffTimeState_init( void )
{
}

static uint8_t SetOnOffTimeState_handleIr(  uint8_t cmdCode )
{

  if( UI_AUTOOFFTIMES_COUNT == mode_setOnOffTimeState.currentTimeToSet) // only after all times where set
  {
    if( (cmdCode == UI_DOWN) || (cmdCode == UI_UP) )
    {
      DisplayState dispData;
      g_params->useAutoOffAnimation ^= 1;
      dispData = display_getNumberDisplayState(g_params->useAutoOffAnimation+1);
      display_setDisplayState(dispData, dispData);
      if(g_params->useAutoOffAnimation)
      {
        g_animPreview = 1;
      } else {
        g_animPreview = 0;
      }
    }
    if( cmdCode == UI_SET_ONOFF_TIMES )
    {
      mode_setOnOffTimeState.prohibitLeave = FALSE;
      g_animPreview = 0;
      quitMyself( MS_setOnOffTime, NULL);
    }
  }


  return TRUE;
}


/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/



static uint8_t PulseState_handleIR(  uint8_t cmdCode )
{
  if(    UI_UP == cmdCode
      || UI_DOWN == cmdCode)
  {
    log_state("CPS \n");
    int8_t dir = UI_UP == cmdCode?-1:1;
    incDecRange(    &g_params->pulseUpdateInterval
                  , dir
                  , USER_PULSE_CHANGE_INT_10ms_MIN
                  , USER_PULSE_CHANGE_INT_10ms_MAX);
#if (USER_LOG_STATE == 1)
    uart_putc('0' + g_params->pulseUpdateInterval);
#endif
  }else{
    return FALSE;
  }
   
  return TRUE;

}


static void PulseState_100Hz( void )
{

  ++mode_pulseState.delay100ms;
  if( mode_pulseState.delay100ms >= (volatile uint8_t)(g_params->pulseUpdateInterval) )
  {
    pwm_lock_brightness_val(pulseWaveForm(mode_pulseState.curBrightness));
    ++mode_pulseState.curBrightness;
    mode_pulseState.delay100ms = 0;
  }
}

static void PulseState_10Hz( void )
{
  UserState_Isr10Hz( g_stateStack[ g_currentIdxs[MS_pulse]-1] );  // ensure the update of lower state
}


static void PulseState_leave( void )
{
  pwm_release_brightness();
}


static void PulseState_init( void )
{
}


/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/

static void UserState_init()
{
  TrainIrState_init();
  ShowNumberState_init();
  NormalState_init();
  DemoState_init();
  SetSystemTimeState_init();
  SetOnOffTimeState_init();
  EnterTimeState_init();
  PulseState_init();

# if (MONO_COLOR_CLOCK != 1)
  AutoHueState_init();
# endif // (MONO_COLOR_CLOCK != 1)

}

static void UserState_enter( e_MenuStates state,  const void* param )
{
  if( MS_enterTime == state ){
    EnterTimeState_enter(param);

  }else if( MS_normalMode == state ){
    NormalState_enter(param);

  }else if( MS_setOnOffTime  == state ){
    SetOnOffTimeState_enter(param);

  }else if( MS_setSystemTime == state ){
    SetSystemTimeState_enter(param);

  }else if( MS_showNumber == state ){
    ShowNumberState_enter(param);

  }else if( MS_irTrain == state ){
    TrainIrState_enter(param);
#if (MONO_COLOR_CLOCK != 1)
  }else if( MS_hueMode  == state ){
    AutoHueStat_enter(param);
#endif  // (MONO_COLOR_CLOCK != 1)
  }


}




static void    UserState_SubstateFinished(e_MenuStates state, e_MenuStates finishedState, const void* result)
{
  if( MS_setOnOffTime  == state ){
    SetOnOffTimeState_substateFinished(finishedState, result);
  }else if( MS_setSystemTime == state ){
    SetSystemTimeState_substateFinished(finishedState, result);
  }
}



static uint8_t UserState_HanbdleIr (e_MenuStates state, uint8_t cmdCode)
{ 
  uint8_t handled = FALSE;
 
  // Train Ir not here because of special handling due to raw data needed

  if( MS_enterTime == state ){
    handled = EnterTimeState_handleIr( cmdCode);

  }else if( MS_normalMode == state ){
    handled = NormalState_handleIR( cmdCode);

  }else if( MS_pulse == state ){
    handled = PulseState_handleIR( cmdCode);

  }else if( MS_setOnOffTime == state ){
    handled = SetOnOffTimeState_handleIr( cmdCode);

#if (MONO_COLOR_CLOCK != 1)
  }else if( MS_hueMode  == state ){
    handled = AutoHueState_handleIR( cmdCode);
#endif  // (MONO_COLOR_CLOCK != 1)

  } 

  return handled;
}



static void UserState_LeaveState( e_MenuStates state )
{
  if( MS_pulse == state ){
    PulseState_leave();
  }

}


static void UserState_Isr1Hz( e_MenuStates state )
{
  if( MS_irTrain == state ){
    TrainIrState_1Hz();
  }
}


static void UserState_Isr10Hz( e_MenuStates state )
{
  if( MS_demoMode  == state ){
    DemoState_10Hz();

  }else if( MS_showNumber  == state ){
    ShowNumberState_10Hz();

  }else if( MS_pulse  == state ){
    PulseState_10Hz();

#if (MONO_COLOR_CLOCK != 1)
  }else if( MS_hueMode  == state ){
    AutoHueState_10Hz();
#endif  // (MONO_COLOR_CLOCK != 1)

  }
}

static void UserState_Isr100Hz( e_MenuStates state )
{
  if( MS_pulse == state ){
    PulseState_100Hz();
  }
}



static uint8_t UserState_prohibitTimeDisplay( e_MenuStates state )
{
  return     (MS_irTrain       == state)
          || (MS_showNumber    == state)
          || (MS_demoMode      == state)
          || (MS_setSystemTime == state)
          || (MS_setOnOffTime  == state)
          || (MS_enterTime     == state)
          ;
   
}

static uint8_t UserState_prohibitLeave( e_MenuStates state )
{
  uint8_t prohibit = FALSE;

  if( MS_normalMode == state ){
    prohibit = mode_enterTimeState.prohibitLeave;
  }if( MS_setOnOffTime  == state ){
    prohibit = mode_setOnOffTimeState.prohibitLeave;
  }else if( MS_setSystemTime == state ){
    prohibit = mode_setSystemTimeState.prohibitLeave;
  }


  return prohibit;
}

