/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file i2c-rtc.h
 * 
 *  Handling of an DS1307 I2C real time clock
 *
 * \version $Id: i2c-rtc.h 285 2010-03-24 21:43:24Z vt $
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



#ifndef _WC_I2C_RTC_H_
#define _WC_I2C_RTC_H_

#ifdef __cplusplus
extern "C"
{
#endif


/**
 *  Get I2C status
 *  @details  Returns I2C status
 *  @return    i2c rtc status
 */
extern uint8_t                i2c_rtc_get_status (void);

/**
 *  Write date & time
 *  @details  Writes date & time into RTC
 *  @param    datetime   date & time
 *  @return    TRUE = successful, FALSE = failed
 */
extern uint8_t                i2c_rtc_write (const DATETIME * datetime);

/**
 *  Read date & time
 *  @details  Reads date & time from rtc
 *  @param    datetime  date & time
 *  @return    TRUE = successful, FALSE = failed
 */
extern uint8_t                i2c_rtc_read (DATETIME * datetime);

/**
 *  Write data into SRAM
 *  @details  Writes data into SRAM
 *  @param    addr         address
 *  @param    void_valuep  pointer to buffer
 *  @param    length       length of buffer
 *  @return    TRUE = successful, FALSE = failed
 */
extern uint8_t                i2c_rtc_sram_write (uint8_t addr, void * void_valuep, uint8_t length);

/**
 *  Read data into SRAM
 *  @details  Reads data into SRAM
 *  @param    addr         address
 *  @param    void_valuep  pointer to buffer
 *  @param    length      length of buffer
 *  @return    TRUE = successful, FALSE = failed
 */
extern uint8_t                i2c_rtc_sram_read (uint8_t addr, void * void_valuep, uint8_t length);

/**
 *  Initialize RTC
 *  @details  Initializes & configures RTC
 *  @param    errorcode_p   pointer to byte in order to store errorcode
 *  @param    status_p      pointer to byte in order to store I2C status
 *  @return    TRUE = successful, FALSE = failed
 */
extern uint8_t                i2c_rtc_init (uint8_t * errorcode_p, uint8_t * status_p);

#ifdef __cplusplus
}
#endif


#endif /* _WC_I2C_RTC_H_ */
