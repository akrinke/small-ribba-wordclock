/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file main.c
 * 
 *  The main program file.
 *  \details
 *      Written for ATMEGA88 @ 8 MHz. \n
 *      This File contains main routine with top level and initialization stuff.
 *
 * \version $Id: main.c 403 2011-11-13 00:08:32Z sm $
 * 
 * \author Copyright (c) 2010 Frank Meyer - frank(at)fli4l.de
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
#include <avr/wdt.h>


#include "main.h"
#include "dcf77.h"
#include "display.h"
#include "i2c-master.h"
#include "i2c-rtc.h"
#include "ldr.h"
#include "pwm.h"
#include "irmp.h"
#include "timer.h"
#include "user.h"
#include "uart.h"
#include "base.h"
#include "wceeprom.h"

#define READ_DATETIME_INTERVAL    15                                              // interval in seconds: read rtc
static volatile uint8_t           soft_seconds;                                   // seconds of a softclock, see main_ISR()

#if  (MAIN_LOG == 1)
#  define log_main(x) uart_puts_P(x)
#else
#  define log_main(x)
#endif

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * MAIN: check date & time
 *
 * This function uses a softclock to avoid extensive calls of i2c_rtc_read() by
 * reading the RTC only READ_DATETIME_INTERVAL seconds.
 *
 * fm: This softclock algorithm adjusts itself:
 * - if the RC oscillator is too slow, handle_datetime() will call i2c_rtc_read() every second in the last part
 *   of a minute in order to reach the next full minute as close as possible.
 * - if the RC oscillator is too fast, the softclock will be slowdowned by updating the softclock every
 *   READ_DATETIME_INTERVAL - softclock_too_fast_seconds
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
static void
handle_datetime (DATETIME * datetimep)
{
  static uint8_t    last_hour = 0xff;                                             // value of last hour
  static uint8_t    last_minute = 0xff;                                           // value of minutes evaluated last call
  static uint8_t    last_seconds = 0xff;                                          // value of seconds evaluated last call
  static uint8_t    next_read_seconds = 0;                                        // time in seconds when to read RTC again
  uint8_t           softclock_too_fast_seconds = 0;                               // if softclock is too fast, store difference of seconds
  uint8_t           rtc;

  if (last_seconds != soft_seconds)                                               // time changed?
  {                                                                               // yes...
    if (soft_seconds >= next_read_seconds)                                        // next time (to read RTC) reached?
    {                                                                             // yes...
      rtc = i2c_rtc_read (datetimep);                                             // read RTC now!
    }
    else
    {                                                                             // next time not reached...
      datetimep->ss = soft_seconds;                                               // update only seconds
      rtc = TRUE;
    }

    if (rtc)
    {
      if (last_minute != datetimep->mm)                                           // minute change?
      {                                                                           // yes...
        user_setNewTime (datetimep);                                              // display current time
        last_minute = datetimep->mm;                                              // store current minute as last minute

        if (last_hour != datetimep->hh)
        {
#         if (DCF_PRESENT == 1)
            enable_dcf77_ISR = TRUE;                                              // enable DCF77 every hour
#         endif  /** (DCF_PRESENT == 1) */
          last_hour = datetimep->hh;                                              // store current hour as last hour
        }
      }

      if (last_seconds != 0xff && soft_seconds > datetimep->ss)
      {
        softclock_too_fast_seconds = soft_seconds - datetimep->ss;
      }

      last_seconds = soft_seconds = datetimep->ss;                                // store actual value of seconds for softclock

      if (softclock_too_fast_seconds > 0)
      {                                                                           // set next time we have to read RTC again (with correction)
        next_read_seconds = soft_seconds + READ_DATETIME_INTERVAL - softclock_too_fast_seconds;
      }
      else
      {
        next_read_seconds = soft_seconds + READ_DATETIME_INTERVAL;                // set next time we have to read RTC again
      }

      if (next_read_seconds >= 60)                                                // we have to read it in the next minute...
      {
        next_read_seconds = 0;                                                    // reset next time: read at next full minute
      }
    }
    else
    {
      log_main("RTC error\n");
    }
  }
  else
  {
    rtc = TRUE;                                                                   // time not changed, do nothing
  }
}

static void
handle_brightness (void)
{
  static uint8_t  last_ldr_brightness = 0xff;
  uint8_t      ldr_brightness;

  ldr_brightness = ldr_get_brightness () >> 3;                                  // use only upper 5 bits, values 0-31

  if (last_ldr_brightness != ldr_brightness)
  {
#if (MAIN_LOG_BRIGHTNESS==1)
    char buff[5];
    byteToStrLessHundred(ldr_brightness, buff);
    uart_puts_P("brightness: ");
    uart_puts(buff);
    uart_putc('\n');

#endif

    pwm_set_base_brightness_step (ldr_brightness);
    last_ldr_brightness = ldr_brightness;
  }
}



#if (BOOTLOADER_RESET_WDT == 1)
/** deactivates the watch dog timer as soon as possible in Program flow */
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init1")));
void wdt_init(void)
{
    MCUSR = 0;
    wdt_disable();
    return;
}
#endif


/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * MAIN: main routine
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */

//int main(void) __attribute__((noreturn)); /* saves some Bytes but produces warning */
int
main (void)
{
  static DATETIME  datetime;  

  uart_init();                                                                  // initialize uart
  log_main("Init...\n");

  wcEeprom_init();

# if (DCF_PRESENT == 1)
    dcf77_init ();                                                              // initialize dcf77
# endif

  display_init ();                                                              // initialize display

  { // local to save stack
    uint8_t     i2c_errorcode;
    uint8_t     i2c_status;
    if (! i2c_rtc_init (&i2c_errorcode, &i2c_status))                           // initialize rtc
    {
      log_main("RTC init failed\n");
      // TODO: error handling
    }
  }

  ldr_init ();                                                                  // initialize ldr
  pwm_init ();                                                                  // initialize pwm
  irmp_init ();                                                                 // initialize irmp
  timer_init ();                                                                // initialize timer
  user_init();

  sei ();                                                                       // enable interrupts

  pwm_on ();                                                                    // switch on pwm

  //pwm_set_base_brightness_step(MAX_PWM_STEPS-1); /// @todo remove if ldr stuff is working


  log_main("Init finished\n");

  for (;;)
  {
    handle_brightness ();
    handle_datetime (&datetime);                                                // check & display new time, if necessary
    handle_ir_code ();                                                          // handle ir user interaction

#   if (DCF_PRESENT == 1)
      if (dcf77_getDateTime (&datetime))                                        // handle dcf77 examination (enable_dcf77_ISR must be TRUE to enable analysis)
      {
        i2c_rtc_write (&datetime);
        soft_seconds = datetime.ss;
        user_setNewTime (&datetime);
      }
#   endif  /** (DCF_PRESENT == 1) */
  }
}

void
main_ISR (void)
{
  soft_seconds++;
}
