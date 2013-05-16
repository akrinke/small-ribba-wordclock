/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file ldr.h
 * 
 *  Handles the ldr sensor.
 *  \details
 *      Written for ATMEGA88 @ 8 MHz. \n
 *
 * \version $Id: ldr.h 285 2010-03-24 21:43:24Z vt $
 * 
 * \author Copyright (c) 2010 René Harsch ( rene@harsch.net )
 * \author Copyright (c) 2010 Vlad Tepesch  
 * 
 * \remarks
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 */
 /*-----------------------------------------------------------------------------------------------------------------------------------------------*/


#ifndef _WC_LDR_H_
#define _WC_LDR_H_

#ifdef __cplusplus
extern "C"
{
#endif 

/** defines the size of the internal buffer to smooth the measurements */
#define LDR_ARRAY_SIZE     16




/**  Initialize the adc for ldr measurements.*/
extern void     ldr_init (void);

/**
 *  returns the smoothed brightness
 *  @details  The brightness is low pass filtered.
 *            The mean value from the last LDR_ARRAY_SIZE measurements will be returned.
 *  @return   A 8bit value containing the brightness.\n
 *            Brightness means that the value is inverse proportional 
 *            to the adc results from reading the ldr. \n
 *            A value of 255 means very bright and 0 means very dark.
 */
extern uint8_t  ldr_get_brightness (void);

/**
 *  Handles the reading of the ldr with the ADC.
 *  @details  Has to be called regularly. (intendet to be called at 1Hz)
 */
extern void     ldr_ISR  (void);


#ifdef __cplusplus
}
#endif 

#endif /* _WC_LDR_H_ */
