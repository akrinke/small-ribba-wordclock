/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file i2c-rtc.c
 * 
 *  Handling of an DS1307 I2C real time clock
 *
 * \version $Id: i2c-rtc.c 285 2010-03-24 21:43:24Z vt $
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



#include <avr/io.h>
#include <util/delay.h>

#include "main.h"
#include "base.h"
#include "i2c-master.h"
#include "i2c-rtc.h"

#define DEVRTC                0xD0                                              // device address of RTC DS1307

static volatile uint8_t       i2c_rtc_pulsetype;
static volatile uint8_t       i2c_rtc_pulselen;

static uint8_t                rtc_initialized = 0;
static uint8_t                i2c_rtc_status;

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * bits of control register:
 *
 * NAME     PURPOSE             REMARK
 * OUT      output control      if SQWE=0, the logic level on the SQW/OUT pin is 1 if OUT=1 and is 0 if OUT=0
 * SQWE     square wave enable  enable the oscillator output, rate see below
 * RS1/RS0  rate select         rate select
 *
 * RS1  RS0    SQW output frequency
 *  0    0      1 Hz
 *  0    1      4.096 kHz
 *  1    0      8.192 kHz
 *  1    0      32.768 kHz
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */

#define CTRL_REG_OUT          0x80                                              // output control
#define CTRL_REG_SQWE         0x10                                              // square wave enable
#define CTRL_REG_RS1          0x02                                              // rate select RS1
#define CTRL_REG_RS0          0x01                                              // rate select RS0

static volatile uint8_t       ctrlreg;

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Get I2C status
 *  @details  Returns I2C status
 *  @return    i2c rtc status
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */

uint8_t
i2c_rtc_get_status (void)
{
  return i2c_rtc_status;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Write date & time
 *  @details  Writes date & time into RTC
 *  @param    date & time
 *  @return    TRUE = successful, FALSE = failed
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
uint8_t
i2c_rtc_write (const DATETIME * datetime)
{
  uint8_t      rtcbuf[7];
  uint8_t      rtc = FALSE;

  if (rtc_initialized)
  {
    rtcbuf[0] = itobcd (datetime->ss);                                          // seconds    00-59
    rtcbuf[1] = itobcd (datetime->mm);                                          // minutes    00-59
    rtcbuf[2] = itobcd (datetime->hh);                                          // hours    00-23
    rtcbuf[3] = itobcd (datetime->wd) + 1;                                      // weekday     1-7
    rtcbuf[4] = itobcd (datetime->DD);                                          // day of month  00-31
    rtcbuf[5] = itobcd (datetime->MM);                                          // month    00-12
    rtcbuf[6] = itobcd (datetime->YY);                                          // year      00-99

    if (i2c_rtc_sram_write (0x00, rtcbuf, 7))
    {
      rtc = TRUE;
    }
  }
  return rtc;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Read date & time
 *  @details  Reads date & time from rtc
 *  @param    date & time
 *  @return    TRUE = successful, FALSE = failed
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
uint8_t
i2c_rtc_read (DATETIME * datetime)
{
  uint8_t  rtcbuf[7];
  uint8_t  rtc = 0;

  if (rtc_initialized)
  {
    if (i2c_rtc_sram_read (0x00, rtcbuf, 7))
    {
      datetime->YY = bcdtoi (rtcbuf[6]);                                        // year
      datetime->MM = bcdtoi (rtcbuf[5]);                                        // month
      datetime->DD = bcdtoi (rtcbuf[4]);                                        // day of month
      datetime->wd = bcdtoi (rtcbuf[3]) - 1;                                    // weekday 0-6
      datetime->hh = bcdtoi (rtcbuf[2]);                                        // hours
      datetime->mm = bcdtoi (rtcbuf[1]);                                        // minutes
      datetime->ss = bcdtoi (rtcbuf[0]);                                        // seconds
      rtc = 1;
    }
  }
  return rtc;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Write data into SRAM
 *  @details  Writes data into SRAM
 *  @param    address
 *  @param    pointer to buffer
 *  @param    length
 *  @return    TRUE = successful, FALSE = failed
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
uint8_t
i2c_rtc_sram_write (uint8_t addr, void * void_valuep, uint8_t length)
{
  unsigned char *   valuep = void_valuep;
  uint8_t           rtc = 0;

  if (rtc_initialized)
  {
    if (length && addr + length <= 64)
    {
      i2c_master_start_wait(DEVRTC+I2C_WRITE);                                  // set device address and write mode

      if (i2c_master_write (addr, &i2c_rtc_status) == 0)                        // write address
      {
        rtc = TRUE;                                                             // indicate success

        while (length--)
        {
          if (i2c_master_write (*valuep++, &i2c_rtc_status) != 0)               // write value
          {
            rtc = FALSE;                                                        // indicate errror
            break;
          }
        }
      }

      i2c_master_stop();                                                        // set stop conditon = release bus
    }
  }
  return rtc;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Read data into SRAM
 *  @details  Reads data into SRAM
 *  @param    address
 *  @param    pointer to buffer
 *  @param    length
 *  @return   TRUE = successful, FALSE = failed
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
uint8_t
i2c_rtc_sram_read (uint8_t addr, void * void_valuep, uint8_t length)
{
  unsigned char *  valuep = void_valuep;
  uint8_t      rtc = FALSE;

  if (rtc_initialized)
  {
    if (length && addr + length <= 64)
    {
      i2c_master_start_wait(DEVRTC+I2C_WRITE);                                  // set device address and write mode

      if (i2c_master_write (addr, &i2c_rtc_status) == 0)                        // write address
      {
        if (i2c_master_rep_start(DEVRTC+I2C_READ, &i2c_rtc_status) == 0)        // set device address and read mode
        {
          rtc = TRUE;                                                           // indicate success

          while (--length)
          {
            *valuep++ = i2c_master_read_ack();                                  // read n'th byte
          }
          *valuep++ = i2c_master_read_nak();                                    // read last byte
        }
      }

      i2c_master_stop();                                                        // set stop conditon = release bus
    }
  }
  return rtc;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Write control register
 *  @details    Writes control register
 *  @return     TRUE = successful, FALSE = failed
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
static uint8_t
i2c_write_ctrlreg (void)
{
  uint8_t     value[1];
  uint8_t     rtc = 0;

  value[0] = ctrlreg;

  if (i2c_rtc_sram_write (0x07, value, 1))
  {
    rtc = 1;
  }
  return rtc;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Initialize RTC
 *  @details  Initializes & configures RTC
 *  @param    pointer to byte in order to store errorcode
 *  @param    pointer to byte in order to store I2C status
 *  @return   TRUE = successful, FALSE = failed
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
uint8_t
i2c_rtc_init (uint8_t * errorcode_p, uint8_t * status_p)
{
  uint8_t     rtc = FALSE;
  uint8_t     seconds;

  *status_p = 0xFF;
  *errorcode_p = i2c_master_init();                                             // init I2C interface

  if (*errorcode_p == 0)
  {
    rtc_initialized = TRUE;
    ctrlreg = CTRL_REG_OUT;                                                     // set SQWE pin to output and set it to high (deactivated, active low!)

    if (i2c_write_ctrlreg ())
    {
      rtc = TRUE;

      if (i2c_rtc_sram_read (0x00, &seconds, 1))
      {
        if (seconds & 0x80)                                                     // clock stopped?
        {
          seconds &= ~0x80;
          (void) i2c_rtc_sram_write (0x00, &seconds, 1);
        }
      }
    }
    else
    {
      *errorcode_p = I2C_ERROR_SLAVE_NOT_FOUND;
      *status_p = i2c_rtc_status;
    }
  }

  return rtc;
}
