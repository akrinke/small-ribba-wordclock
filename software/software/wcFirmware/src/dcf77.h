/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file dcf77.h
 * 
 *  Interface for the dcf (radio controlled time signal) interpretation
 *
 * \version $Id: dcf77.h 285 2010-03-24 21:43:24Z vt $
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


#ifndef _WC_DCF77_H_
#define _WC_DCF77_H_

#ifdef __cplusplus
extern "C"
{
#endif


#if (DCF_PRESENT == 1)

extern uint8_t   enable_dcf77_ISR;     // En- / Disable DCF77 examination
/**
 *  Initializes DCF77
 *  @details  Configures DCF77 IO hardware and clear variables
 *
 */
extern void      dcf77_init          (void);
/**
 *  receive and decode DCF77 signal
 *  @details  Decode DCF77 signal - measure pulse time to
 *            decode the received signal
 *            must be called each 10ms (1000 HZ)
 *
 */
extern void      dcf77_ISR           (void);
/**
 *  update RTC with DCF77
 *  @details  update RTC time with dcf77 time
 *            called 1 time per day
 *  @param    DateTime_p as the actual date and time
 *  @return   True if valid dcf77 signal was received and datetime_p updated
 *
 */
extern uint8_t   dcf77_getDateTime   (DATETIME * DateTime_p);
#else

#define  dcf77_ISR()

#endif  /** (DCF_PRESENT == 1) */

#ifdef __cplusplus
}
#endif


#endif // _WC_DCF77_H_
