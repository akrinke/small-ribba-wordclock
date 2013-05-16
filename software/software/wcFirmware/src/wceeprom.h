/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file wceeprom.h
 * 
 *  The eeprom module handles the persistent data of the wordclock modules
 *  \details
 *    It is responsible to provide the stored data on program start 
 *    as well as the writeback of updated params
 *
 * \version $Id: wceeprom.h 285 2010-03-24 21:43:24Z vt $
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

#ifndef _WC_EEPROM_H_
#define _WC_EEPROM_H_

#include "user.h"
#include "display.h"
#include "pwm.h"

#ifdef __cplusplus
extern  "C"
{
#endif

/**
 *  this structure holds the persistent data of all modules
 *  \details  Internally ther will be two instanciations of this structure: \n
 *            - one in the eeprom
 *            - one in the sdram
 *
 *            Each module had its own substructure and should provide the defaults
 *            with a macro upcase(structname)_DEFAULTS 
 *            (for example: UserEepromParams -> USEREEPROMPARAMS_DEFAULTS).
 */
typedef struct WcEepromData{
  UserEepromParams    userParams;    /**< parameters from user interface module               */
  DisplayEepromParams displayParams; /**< parameters from display module                      */
  PwmEepromParams     pwmParams;     /**< parameters from pwm module                          */
  uint8_t             swVersion;     /**< version identifier - for basic data integrity test  */
  uint8_t             structSize;    /**< size of structure - for basic data integrity test   */
}WcEepromData;

/**
 *  reads the eeprom data into the internal WcEepromData memory
 *  this should be called before any other module with persistent data is initialized
 */
void wcEeprom_init(void);

/**
 *  gives a pointer to a working copy of the data from eeprom
 *  \details   This data can be modiefied.\n
 *             To save the changed parameters back to eeprom call wcEeprom_writeback
 *  \warning   If WcEepromData becomes larger then 254 byte the code has to be reviewd and changed
 *             because many loopcounter and indexes are only 8bit
 */
static inline WcEepromData* wcEeprom_getData(void)
{
  extern WcEepromData g_epromWorking;
  return &g_epromWorking;
};

/**
 *  Starts the writeback of all changed eeprom data within the specified area.
 *  \warning  Because writing takes some time it is possible 
 *            that some interupts will be missed.
 *  @param    start   pointer to the start of tha area that has to be written back\n
 *                    Has to be a pointer that points into the structure received from wcEeprom_getData
 *  @param    len     the length of the datablock
 */
void wcEeprom_writeback(const void* start, uint8_t len);



#ifdef __cplusplus
}
#endif


#endif /* _WC_EEPROM_H_ */


