/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file i2c-master.c
 * 
 *  Interface to I2C Hardware interface
 *  
 *  adapted from Fleury lib - thanks to Peter Fleury
 *
 * \version $Id: i2c-master.c 285 2010-03-24 21:43:24Z vt $
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
#include <compat/twi.h>
#include <util/delay.h>

#include "main.h"
#include "i2c-master.h"

#define SCL_CLOCK  100000L                                                      // I2C clock in Hz, usually 100 kHz

#define WAIT_UNTIL_TRANSMISSION_COMPLETED     while (!(TWCR & (1<<TWINT)))  { ; }
#define WAIT_UNTIL_STOP_CONDITION_EXECUTED    while (TWCR & (1<<TWSTO))     { ; }

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * fm: If SDA is low, you can reset the I2C bus by doing following steps:
 * 1. Clock up to 9 cycles.
 * 2. Look for SDA high in each cycle while SCL is high.
 * 3. Create a start condition.
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
#if defined (__AVR_ATmega88P__) || (__AVR_ATmega168P__) || defined (__AVR_ATmega328P__)
#define HAS_RESET             1

#define SCL_PORT              PORTC
#define SCL_DDR               DDRC
#define SCL_PIN               PINC
#define SCL_BIT               PINC5
#define SDA_PORT              PORTC
#define SDA_DDR               DDRC
#define SDA_PIN               PINC
#define SDA_BIT               PINC4

#define SCL_LOW               SCL_DDR |= (1<<SCL_BIT)                           // set SCL to low by setting pin to output
#define SCL_HIGH              SCL_DDR &= ~(1<<SCL_BIT)                          // set SCL to how by setting pin to input
#define SDA_IS_HIGH           (SDA_PIN & (1<<SDA_BIT))
#define SDA_IS_LOW            (!SDA_IS_HIGH)
#define SCL_IS_HIGH           (SCL_PIN & (1<<SCL_BIT))
#define SCL_IS_LOW            (!SCL_IS_HIGH)
#else
#define HAS_RESET  0
#endif

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Reset I2C bus
 *  @details  Resets I2C bus by doing the steps above
 *  @return    0 = successful, else failed
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
static uint8_t
i2c_reset (void)
{
  uint8_t    rtc = 0;
#if HAS_RESET == 1
  uint8_t    idx;

  SCL_PORT &= ~(1<<SCL_BIT);                                                    // deactivate intern pullup resistors, use extern resistor
  SCL_DDR  &= ~(1<<SCL_BIT);                                                    // set SCL pin to input
  SDA_PORT &= ~(1<<SDA_BIT);                                                    // deactivate intern pullup resistors, use extern resistor
  SDA_DDR  &= ~(1<<SDA_BIT);                                                    // set SCL pin to input
  _delay_ms (1);

  if (SCL_IS_LOW)
  {
    rtc = I2C_ERROR_SCL_LOW;
  }
  else
  {
    if (SDA_IS_LOW)                                                             // SDA low?
    {                                                                           // yes...
      SCL_LOW;                                                                  // try to release it here....
      _delay_ms (1);

      for (idx = 0; idx < 9; idx++)                                             // step 1...
      {
        SCL_HIGH;
        _delay_ms (1);

        if (SDA_IS_HIGH)                                                        // look for SDA = HIGH
        {                                                                       // step 2!
          break;
        }

        SCL_LOW;
        _delay_ms (1);
      }

      SCL_DDR  &= ~(1<<SCL_BIT);                                                // set SCL pin again to input, step 3 will be done by app-init
    }

    SCL_HIGH;
    _delay_ms (1);

    if (SDA_IS_LOW)                                                              // SDA still low?
    {                                                                            // yes...
      rtc = I2C_ERROR_SDA_LOW;
    }
  }
#endif
  return rtc;
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Initializes the I2C hardware
 *  @details  Configures I2C bus in order to operate as I2C master
 *  @return    TRUE if successful, FALSE, if not
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
uint8_t
i2c_master_init(void)
{
  static uint8_t  already_called;
  uint8_t         rtc;

  if (! already_called)
  {
    rtc = i2c_reset ();                                                         // try to reset I2C bus

    TWSR = 0;                                                                   // no prescaler
    TWBR = ((F_CPU/SCL_CLOCK)-16)/2;                                            // must be > 10 for stable operation
    already_called = 1;
  }
  else
  {
    rtc = 0;
  }
  return rtc;
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Start I2C transfer
 *  @details  Issues a start condition and sends address and transfer direction
 *  @param    I2C address
 *  @param    pointer to byte in order to store I2C status
 *  @return    1 = failed to access device, 0 = device accessible
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
unsigned char
i2c_master_start(uint8_t address, uint8_t * status_p)
{
  uint8_t   twst;

  TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);                                   // send START condition
  WAIT_UNTIL_TRANSMISSION_COMPLETED;                                            // wait until transmission completed
  twst = TW_STATUS & 0xF8;                                                      // check value of TWI Status Register. Mask prescaler bits.

  if ((twst != TW_START) && (twst != TW_REP_START))
  {
    *status_p = twst;                                                           // store status
    return 1;                                                                   // ERROR
  }

  TWDR = address;                                                               // send device address
  TWCR = (1<<TWINT) | (1<<TWEN);
  WAIT_UNTIL_TRANSMISSION_COMPLETED                                             // wail until transmission completed and ACK/NACK has been received
  twst = TW_STATUS & 0xF8;                                                      // check value of TWI Status Register. Mask prescaler bits.

  if ((twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) )
  {
    *status_p = twst;                                                           // store status
    return 1;                                                                   // ERROR
  }

  return 0;                                                                     // OK
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Start I2C transfer and wait until device is ready
 *  @details  Issues a start condition and sends address and transfer direction
 *  @details  If device is busy, use ack polling to wait until device is ready
 *  @param    I2C address
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
i2c_master_start_wait (uint8_t address)
{
  uint8_t   twst;

  while (1)
  {
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);                                 // send START condition
    WAIT_UNTIL_TRANSMISSION_COMPLETED;                                          // wait until transmission completed
    twst = TW_STATUS & 0xF8;                                                    // check value of TWI Status Register. Mask prescaler bits.

    if ((twst != TW_START) && (twst != TW_REP_START))
    {
      continue;
    }

    TWDR = address;                                                             // send device address
    TWCR = (1<<TWINT) | (1<<TWEN);
    WAIT_UNTIL_TRANSMISSION_COMPLETED;                                          // wail until transmission completed
    twst = TW_STATUS & 0xF8;                                                    // check value of TWI Status Register. Mask prescaler bits.

    if ((twst == TW_MT_SLA_NACK) || (twst == TW_MR_DATA_NACK))
    {
      TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);                               // device busy, send stop condition to terminate write operation
      WAIT_UNTIL_STOP_CONDITION_EXECUTED;                                       // wait until stop condition is executed and bus released
      continue;
    }

    // if (twst != TW_MT_SLA_ACK) return 1;
    break;
   }
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Start I2C transfer (repeated)
 *  @details  Issues a repeated start condition and sends address and transfer direction
 *  @param    I2C address
 *  @param    pointer to byte in order to store I2C status
 *  @return    1 = failed to access device, 0 = device accessible
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
uint8_t
i2c_master_rep_start (uint8_t address, uint8_t * status_p)
{
  return i2c_master_start (address, status_p);
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Stop I2C transfer
 *  @details  Terminates the data transfer and releases the I2C bus
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
i2c_master_stop (void)
{
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);                                   // send stop condition
  WAIT_UNTIL_STOP_CONDITION_EXECUTED;                                           // wait until stop condition is executed and bus released
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Send one byte to I2C device
 *  @details  Sends one byte to I2C device
 *  @param    byte to be transfered
 *  @param    pointer to byte in order to store I2C status
 *  @return    0 write successful, 1 write failed
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
uint8_t
i2c_master_write (uint8_t data, uint8_t * status_p)
{
  uint8_t   twst;

  TWDR = data;                                                                  // send data to the previously addressed device
  TWCR = (1<<TWINT) | (1<<TWEN);
  WAIT_UNTIL_TRANSMISSION_COMPLETED;                                            // wait until transmission completed
  twst = TW_STATUS & 0xF8;                                                      // check value of TWI Status Register. Mask prescaler bits

  if (twst != TW_MT_DATA_ACK)
  {
    *status_p = twst;
    return 1;
  }

  return 0;
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Read one byte, request more data
 *  @details  Reads one byte from the I2C device, then requests more data from device
 *  @return    byte read
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
uint8_t
i2c_master_read_ack (void)
{
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
  WAIT_UNTIL_TRANSMISSION_COMPLETED;
  return TWDR;
}/* i2c_master_readAck */


/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Read one byte, followed by a stop condition
 *  @details  Reads one byte from the I2C device, read is followed by a stop condition
 *  @return    byte read
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
uint8_t
i2c_master_read_nak (void)
{
  TWCR = (1<<TWINT) | (1<<TWEN);
  WAIT_UNTIL_TRANSMISSION_COMPLETED;
  return TWDR;
}
