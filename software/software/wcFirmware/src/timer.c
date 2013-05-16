/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file timer.c
 * 
 *  Handles the timer ISRs and the providing of diffrent clock sources
 *
 * \version $Id: timer.c 285 2010-03-24 21:43:24Z vt $
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

#include "main.h"
#include "timer.h"
#include "dcf77.h"
#include "irmp.h"
#include "ldr.h"
#include "user.h"
#include "display.h"

#include "uart.h"
#include "wceeprom.h"

#define F_INTERRUPT           10000                                             // frequency of interrupts

#define  INTERRUPT_10000HZ    {  irmp_ISR(); }
#define  INTERRUPT_1000HZ     { }
#define  INTERRUPT_100HZ      { dcf77_ISR(); user_isr100Hz(); }
#define  INTERRUPT_10HZ       { user_isr10Hz(); display_blinkStep(); }
#define  INTERRUPT_1HZ        { main_ISR(); ldr_ISR(); user_isr1Hz(); }
#define  INTERRUPT_1M         { }

void
timer_init (void)
{
  ICR1    = (F_CPU / F_INTERRUPT) - 1;                           // compare value: 1/10000 of CPU frequency
  TCCR1A  = (1 << WGM11);
  TCCR1B  = (1 << WGM13)|(1 << WGM12) | (1 << CS10);             // switch CTC PWM Mode on, set prescaler to 1
  TIMSK1  = 1 << ICIE1;                                          // ICIE1: Interrupt if timer reaches the Top (ICR1 register)
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * timer 1 compare handler, called every 1/10000 sec
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
ISR(TIMER1_CAPT_vect)
{
  static uint8_t  thousands_counter;
  static uint8_t  hundreds_counter;
  static uint8_t  tenths_counter;
  static uint8_t  seconds_counter;
  static uint8_t  minutes_counter;

  INTERRUPT_10000HZ;

  thousands_counter++;

  if (thousands_counter != 10)
  {
    return;
  }

  thousands_counter = 0;

  INTERRUPT_1000HZ;

  hundreds_counter++;

  if (hundreds_counter != 10)
  {
    return;
  }

  hundreds_counter = 0;

  INTERRUPT_100HZ;

  tenths_counter++;

  if (tenths_counter != 10)                                                     // generate 10Hz ....
  {
    return;
  }

  tenths_counter = 0;

  INTERRUPT_10HZ;

  seconds_counter++;

  if (seconds_counter != 10)                                                    // generate 1Hz ....
  {
    return;
  }

  seconds_counter = 0;

  INTERRUPT_1HZ;

  minutes_counter++;

  if (minutes_counter != 60)                                                    // generate 1/60 Hz ....
  {
    return;
  }

  minutes_counter = 0;

  INTERRUPT_1M;
}
