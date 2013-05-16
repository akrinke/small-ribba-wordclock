/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file wceeprom.c
 * 
 *  This file implements the handling of the persistent data of the wordclock modules
 *
 * \version $Id: wceeprom.c 285 2010-03-24 21:43:24Z vt $
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

#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "wceeprom.h"
#include "uart.h"
#include "base.h"


#if (EEPROM_LOG_WRITEBACK == 1)
#  define   log_eeprom(x)  uart_puts_P(x)
#else
#  define   log_eeprom(x)  
#endif

WcEepromData EEMEM eepromParams;

WcEepromData PROGMEM pm_eepromDefaultParams = {
  /* .userParams    = */ USEREEPROMPARAMS_DEFAULT,
  /* .displayParams = */ DISPLAYEEPROMPARAMS_DEFAULT,
  /* .pwmParams     = */ PWMEEPROMPARAMS_DEFAULT,
  /* .swVersion     = */ SW_VERSION,
  /* .structSize    = */ ((uint8_t)sizeof(WcEepromData)),
};

WcEepromData g_epromWorking; 


# if (EEPROM_LOG_INIT == 1) || (EEPROM_LOG_WRITEBACK == 1)
static void uart_putHexByte(uint8_t byte)
{
  uart_putc( getHexDigit(byte >> 4 ) );
  uart_putc( getHexDigit(byte & 0xf ) );
}
#endif

void wcEeprom_init(void)
{
  eeprom_read_block(&g_epromWorking, &eepromParams, sizeof(eepromParams));
  if(    (g_epromWorking.swVersion  != SW_VERSION)
      || (g_epromWorking.structSize != sizeof(g_epromWorking)))
  {
#   if (EEPROM_LOG_INIT == 1)
    uart_puts_P("Using defaults instead eeprom\n");
#   endif
    memcpy_P(&g_epromWorking, &pm_eepromDefaultParams, sizeof(WcEepromData));
  }
# if (EEPROM_LOG_INIT == 1)
  {
    uint8_t i = 0;
    uint8_t* ptr = (uint8_t*)(&g_epromWorking);
    uart_puts_P("eeprom: ");
    for(; i<sizeof(eepromParams); ++i){
      uart_putHexByte(*ptr);
      ++ptr;
    }
    uart_putc('\n');
  }
# endif
}


static uint8_t wcEeprom_writeIfChanged(uint8_t index)
{
  uint8_t eepromByte;
  uint8_t sdramByte;
  uint8_t* eepromAdress = ( (uint8_t*)&eepromParams) + index ;
  
  eepromByte = eeprom_read_byte( eepromAdress );
  sdramByte  =  *(((uint8_t*)&g_epromWorking)+index);
  if( eepromByte != sdramByte )
  {
#   if (EEPROM_LOG_WRITEBACK == 1)
    {
      char buf[5];
      uart_puts_P("EEPROM Byte ");
      uint16ToHexStr((uint16_t)eepromAdress, buf);
      uart_puts(buf);
      uart_puts_P(" differs ");
      uart_putHexByte(eepromByte);
      uart_putc('\t');
      uart_putHexByte(sdramByte);
      uart_putc('\n');
    }
#   endif

    eeprom_write_byte(eepromAdress, sdramByte);
    return 1;
  }
  return 0;
}

void wcEeprom_writeback(const void* start, uint8_t len)
{
  uint8_t eepromIndex = ( ((uint8_t*)start) - ( (uint8_t*)&g_epromWorking)); 
  uint8_t eepromIndexEnd = eepromIndex + len-1;
# if (EEPROM_LOG_WRITEBACK == 1)
  {
    uart_puts_P("EEpromWrite idx: ");
    uart_putHexByte(eepromIndex);
    uart_puts_P(" len: ");
    uart_putHexByte(len);
    uart_putc('\n');
  }
# endif

  /** @TODO  rewrite this to use interupts because of waiting for eeprom-write finish */
  for(; eepromIndex<=eepromIndexEnd; ++eepromIndex){
    wcEeprom_writeIfChanged( eepromIndex );
  }

}
