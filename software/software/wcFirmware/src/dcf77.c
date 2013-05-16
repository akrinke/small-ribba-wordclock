/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file dcf77.c
 * 
 *  dcf (radio controlled time signal) interpretation
 *
 * \version $Id: dcf77.c 332 2010-07-22 21:07:02Z tg $
 * 
 * \author Copyright (c) 2010 Torsten Giese
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

#include "main.h"
#include "dcf77.h"
#include "base.h"
#include "uart.h"

#if (DCF_PRESENT == 1)



#if  (DCF77_LOG == 1)
#define log_dcf77(x) uart_puts_P(x)
#else
#define log_dcf77(x)
#endif
/* ********************************************************************************* */
#define boolean uint8_t
uint8_t DCF_FLAG;

typedef enum FLAGS_e{
   CHECK = 0,
   DEFINED,
   AVAILABLE,
   HIGH_ACTIVE,

   _FLAG_COUNT
}FLAGS;

#if (_FLAG_COUNT>8)
#  error To much data for Flag register
#endif

#define FLAG_REGISTER (DCF_FLAG)

static inline boolean getFlag(FLAGS flag){ return FLAG_REGISTER & (1<<flag); }
static inline void setFlag(FLAGS flag)   { FLAG_REGISTER |= (1<<flag);       }
static inline void clearFlag(FLAGS flag) { FLAG_REGISTER &= ~(1<<flag);      }

/* ********************************************************************************* */
// Input definition of DCF Modul
#define DCF_INPUT_PORT          PORTB
#define DCF_INPUT_DDR           DDRB
#define DCF_INPUT_PIN           PINB
#define DCF_INPUT_BIT           7

// Output definition of DCF Signal (Control - LED)
#define DCF_OUTPUT_PORT         PORTD
#define DCF_OUTPUT_DDR          DDRD
#define DCF_OUTPUT_BIT          4

volatile typedef struct{
  uint8_t   PauseCounter;                                                       // Counter for pause length (+1 each 10 ms)
  uint8_t   BitCounter;                                                         // which Bit is currently transfered
  uint8_t   Parity;                                                             // must be even when checked
  uint8_t   BCDShifter;                                                         // Data will be received in BCD Code
  uint8_t   NewTime[6];                                                         // store Date & Time while receiving DCF signal
  uint8_t   NewTimeShifter;                                                     // used to identify which data is currently transfered
  uint8_t   OldTime;                                                            // stores the last successful received time
} DCF_Struct;

static DCF_Struct             DCF;
uint8_t                       enable_dcf77_ISR;                                 // En- / Disable DCF77 examination
static uint8_t                count_low;
static uint8_t                count_high;

const static uint8_t          BCD_Kodierung[] = { 1,  2,  4,  8, 10, 20, 40, 80 };
/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * dcf77: clear all dcf variables
 * Reset all used variables if an error occurs while receiving the DCF77 signal
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
static void
dcf77_reset(void)
{
  uint8_t i;
  log_dcf77("DCF77 Reset\n");
  DCF.Parity          = 0;                                                      // Count of high pulse must be even
  DCF.PauseCounter    = 0;                                                      // Count length of pause to identify High/Low Signal
  DCF.BCDShifter      = 0;                                                      // transform transfered data from BCD to decimal
  DCF.BitCounter      = 0;                                                      // Count all received pulses per stream
  DCF.NewTimeShifter  = 0;                                                      // used to identify which data is currently transfered
  setFlag(CHECK);                                                               // TRUE if received DCF77 data stream was valid
  for (i=0; i < 6; i++)
  {
    DCF.NewTime[i]    = 0;                                                      // Store temporary new received data
  }
}
/* --------------------------------------------------------------------------------------------------------------------------------------------------
 * dcf77: check input signal (low or high active) and if a pull up is needed
 * --------------------------------------------------------------------------------------------------------------------------------------------------
 */
static void
dcf77_check_module_type(void)
{
  static uint8_t count_pass;
  static uint8_t count_switch;

  if (count_low + count_high >= 100)                                            // one second over?
  {                                                                             // YES ->
    #if (DCF77_LOG == 1)
    {
      char log_text[8];
      byteToStr (count_low, log_text);
      uart_puts(log_text);
      uart_puts(" ");
      byteToStr (count_high, log_text);
      uart_puts(log_text);
      uart_puts(" ");
      byteToStr (count_high+count_low, log_text);
      uart_puts(log_text);
      uart_puts(" ");
      byteToStr (count_pass, log_text);
      uart_puts(log_text);
      uart_puts(" ");
      byteToStr (count_switch, log_text);
      uart_puts(log_text);
      uart_puts("\n");
    }
    #endif
    if ((count_low == 0) || (count_high == 0))                                  //  one of the counters are low?
    {                                                                           //  YES ->
      count_pass++;                                                             //    increase count of passes
      if (count_pass == 20)                                                     //    tried 20 times to identify the signal?
      {                                                                         //    YES ->
        count_pass = 0;
        count_switch++;                                                         //        increase count of activates and deactivaes of the Pull-Up Resistor
        if ((DCF_INPUT_PIN & (1 << DCF_INPUT_BIT)))                             //      PULL-UP activated?
        {                                                                       //      YES ->
          DCF_INPUT_PORT            &= ~(1 << DCF_INPUT_BIT);                   //        deactivate Pull-Up Resistor
          log_dcf77(" Pull-UP deactivated\n");
         }
        else
        {                                                                       //      NO ->
          DCF_INPUT_PORT            |= (1 << DCF_INPUT_BIT);                    //        activate Pull-Up Resistor
          log_dcf77(" Pull-UP activated\n");
        }
        if (count_switch == 30)                                                 //      switched 30 times between active and deactice Pull-Up?
        {                                                                       //      YES ->
          DCF_INPUT_PORT            |= (1 << DCF_INPUT_BIT);                    //        activate Pull-Up Resistor
          setFlag(DEFINED);                                                     //        DCF Module defined
          clearFlag(AVAILABLE);                                                 //        no DCF Module detected
          log_dcf77("\nNo DCF77 Module detected!\n");
        }
      }
    }
    else
    {                                                                           //  NO ->
      count_pass++;                                                             //        increase count of passes
      if (count_pass == 30)                                                     //    30 passes without a change of the Pull-Up?
      {                                                                         //    YES ->
        setFlag(DEFINED);                                                       //      DCF Module defined
        setFlag(AVAILABLE);                                                     //      DCF Module detected
        #if (DCF77_LOG == 1)
        {
          if (getFlag(HIGH_ACTIVE))
          {
            log_dcf77("\nhigh active DCF77 Module detected!\n");
          }
          else
          {
            log_dcf77("\nlow active DCF77 Module detected!\n");
          }
        }
        #endif
        return;
      }
      if (count_low > count_high)                                               //    low counter less then high counter?
      {                                                                         //    YES ->
        if (getFlag(HIGH_ACTIVE))                                               //      DCF Module low active?
        {                                                                       //      YES ->
          count_pass = 0;                                                       //        reset count of passes
          clearFlag(HIGH_ACTIVE);
        }
      }
      else
      {                                                                         //    NO ->
        if (!(getFlag(HIGH_ACTIVE)))                                            //      DCF Module high active?
        {                                                                       //      YES ->
          count_pass = 0;                                                       //        reset count of passes
          setFlag(HIGH_ACTIVE);
        }
      }
    }
    count_low   = 0;
    count_high  = 0;
  }
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * dcf77: analyze received signal
 * Analyze the received Signal and calculate the reflect data
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
static uint8_t
dcf77_check(void)
{
  #if (DCF77_LOG == 1)
  {
    if (DCF.PauseCounter > 0 )
    {
      char log_text[8];
      byteToStr (DCF.PauseCounter, log_text);
      uart_puts(log_text);
      uart_puts(" ");
    }
  }
  #endif
  if ((DCF.PauseCounter <= 6)                                                   // spike or
      || ((DCF.PauseCounter <= 77)                                              // pause too short and
          && (DCF.PauseCounter >= 96)))                                         // pause too long for data
  {
    DCF.PauseCounter = 0;                                                       // clear pause length counter
    return(FALSE);
  }
  if (((DCF.PauseCounter >= 170)                                                // Sync puls
      && (DCF.BitCounter != 58))                                                // but not 58 bits transfered
      || (DCF.BitCounter >= 59))                                                // or more then 58 Bits
  {
    dcf77_reset();                                                              //   then reset
    return(FALSE);
  }
  if (DCF.BitCounter <= 20)                                                     // Bit 0 - 20 = Weather data
  {
    DCF.BitCounter++;                                                           // increase bit counter
    DCF.PauseCounter = 0;                                                       // clear pause length counter
    return(FALSE);
  }
  if ((DCF.PauseCounter >= 78)
      && (DCF.PauseCounter <= 95))                                              // 0 or 1 detect?
  {
    if (DCF.PauseCounter <= 86)                                                 // 1 detect?
    {
      DCF.Parity++;                                                             // increase parity counter
      if (!((DCF.BitCounter == 28)                                              // not Minutes Parity or
          || (DCF.BitCounter == 35)))                                           // not Hours Parity?
      {
        DCF.NewTime[DCF.NewTimeShifter] +=
            BCD_Kodierung[DCF.BCDShifter];                                      // NewTime Value + BCD-Value
      }
    }
    DCF.BCDShifter++;                                                           // increase BCD Shifter for next data value
    if (((DCF.BitCounter == 28)                                                 // Minutes Parity or
      || (DCF.BitCounter == 35))                                                // Hours Parity and
          && (DCF.Parity%2 != 0))                                               // ParityCount not even?
    {
      dcf77_reset();                                                            //   then reset
      return(FALSE);
    }
    if ((DCF.BitCounter == 28)                                                  // next will be Hours
      || (DCF.BitCounter == 35)                                                 // next will be Day of Month
      || (DCF.BitCounter == 41)                                                 // next will be Day of Week
      || (DCF.BitCounter == 44)                                                 // next will be Month
      || (DCF.BitCounter == 49))                                                // next will be Year
    {
      DCF.NewTimeShifter++;                                                     // increase new time shifter
      DCF.BCDShifter = 0;                                                       // reset BCD Shifter for next data stream
    }
    DCF.BitCounter++;                                                           // increase Bit Counter
    DCF.PauseCounter = 0;                                                       // clear pause length counter
    return(FALSE);
  }
  if (DCF.PauseCounter >= 170)                                                  // sync pause (longer then 1700 ms)?
  {
    if (!(DCF.PauseCounter >= 187))                                             // last Bit = 1 ?
    {
      DCF.Parity++;                                                             //   then Parity + 1
    }
    if ((DCF.BitCounter != 58)                                                  // do not receive 58 bits or
      || (DCF.Parity%2 != 0))                                                   // ParityCount not even?
    {
      dcf77_reset();                                                            //   then reset
      return(FALSE);
    }
    uint8_t NewTime;
    NewTime = DCF.NewTime[0] + DCF.NewTime[1]*60;
    if ((DCF.OldTime + 1) == NewTime)
    {
      log_dcf77(" 2nd DCF77 correct\n");
      return(TRUE);                                                               // Everything fine, received Data could be take over
    }
    else
    {
      log_dcf77(" 1st DCF77 correct\n");
      DCF.OldTime = NewTime;
      dcf77_reset();
      return(FALSE);
    }
  }
  return(FALSE);
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * dcf77: Initialize Port
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
dcf77_init(void)
{
  DCF_INPUT_DDR             &= ~(1 << DCF_INPUT_BIT);                           // set DCF Pin as input
  DCF_INPUT_PORT            &= ~(1 << DCF_INPUT_BIT);                           // deactivate Pull-Up Resistor

  DCF_OUTPUT_DDR            |=  (1<<DCF_OUTPUT_BIT);                            // set Control-LED as output
  DCF_OUTPUT_PORT           &= ~(1<<DCF_OUTPUT_BIT);                            // set Control-LED to low

  clearFlag(DEFINED);                                                           // DCF Module not defined yet
  setFlag(AVAILABLE);                                                           // TRUE to check if DCF77 module is installed

  dcf77_reset();

  DCF.OldTime = 0;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * dcf77: interrupt routine to count the pause length
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
dcf77_ISR(void)
{
  if (getFlag(AVAILABLE))                                                       // DCF77 Module available?
  {                                                                             // YES ->
  if (!(getFlag(DEFINED)))                                                      //  Module defined yet?
    {                                                                           //  NO ->
      if (!(DCF_INPUT_PIN & (1 << DCF_INPUT_BIT)))                              //    DCF77 receiver port HIGH? (low active)
      {                                                                         //    YES ->
        count_low++;                                                            //      increase low counter
      }
      else
      {                                                                         //    NO ->
        count_high++;                                                           //      increase high counter
      }
      dcf77_check_module_type();                                                //    check module type
    }
    else
    {                                                                           //  YES ->
      if (enable_dcf77_ISR)                                                     //    DCF77 analysis enabled?
      {                                                                         //    YES ->
        uint8_t dcf_signal;
        if (getFlag(HIGH_ACTIVE))                                               //      High Active Module?
        {                                                                       //      YES ->
          dcf_signal = (DCF_INPUT_PIN & (1 << DCF_INPUT_BIT));                  //        read DCF Signal
        }
        else
        {                                                                       //      NO ->
          dcf_signal = !(DCF_INPUT_PIN & (1 << DCF_INPUT_BIT));                 //        read and invert DCF Signal
        }

        if (dcf_signal)                                                         //        DCF77 signal HIGH?
        {                                                                       //        YES ->
          DCF.PauseCounter++;                                                   //          increment PauseCounter
          DCF_OUTPUT_PORT &= ~(1<<DCF_OUTPUT_BIT);                              //          deactivate Control-LED
        }
        else
        {                                                                       //        NO ->
          setFlag(CHECK);                                                       //          Set DCF.Check (Pulse has ended)
          DCF_OUTPUT_PORT |= (1<<DCF_OUTPUT_BIT);                               //          activate Control-LED
        }
      }
    }
  }
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * dcf77: set datetime with dcf77 time
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
uint8_t
dcf77_getDateTime(DATETIME * DateTime_p)
{
  if (getFlag(AVAILABLE) && getFlag(CHECK))                                     // DCF Module available and full pulse received?
  {                                                                             // YES ->
      if (dcf77_check())                                                        //    received data are correct?
      {                                                                         //    YES ->
        DateTime_p->ss  = 0;                                                    //      then take over the DCF-Time
        DateTime_p->mm  = DCF.NewTime[0];
        DateTime_p->hh  = DCF.NewTime[1];
        DateTime_p->DD  = DCF.NewTime[2];
        DateTime_p->wd  = DCF.NewTime[3];
        DateTime_p->MM  = DCF.NewTime[4];
        DateTime_p->YY  = DCF.NewTime[5];
        dcf77_reset();                                                          //      Reset Variables
        enable_dcf77_ISR = FALSE;                                               //      Clear enable_dcf77_ISR
        DCF_OUTPUT_PORT &= ~(1<<DCF_OUTPUT_BIT);                                //      deactivate Control-LED
        return (TRUE);
      }                                                                         //    NO ->
      clearFlag(CHECK);                                                         //      do nothing and return false
  }
  return (FALSE);
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * dcf77: END
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */

#endif  /** (DCF_PRESENT == 1) */


