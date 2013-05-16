/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file shift.c
 * 
 *  This file implements the SPI control of the shift register cascade
 *
 * \version $Id: shift.c 285 2010-03-24 21:43:24Z vt $
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


#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "main.h"
#include "shift.h"


void shift24_init (void)
{
  SHIFT_SR_SPI_DDR |= (1<< SHIFT_SR_SPI_MOSI) 
                   |  (1 << SHIFT_SR_SPI_RCLK)
                   |  (1 << SHIFT_SR_SPI_SCK);
  SHIFT_SR_SPI_DDR  &= ~(1<< SHIFT_SR_SPI_MISO); /* MISO muss eingang sein */
  SHIFT_SR_SPI_PORT |= (1<< SHIFT_SR_SPI_RCLK)
                    |  (1<< SHIFT_SR_SPI_MISO);

   // SPI als Master 
   // High-Bits zuerst 
   // SCK ist HIGH wenn inaktiv 
   SPCR = (1 << SPE) | (1 << MSTR) | (1 << CPOL);
	
   // maximale Geschwindigkeit: F_CPU / 2 
   SPSR |= (1 << SPI2X);

   shift24_output(0); /* send dummybytes to intialize */
}

void
shift24_output (uint32_t value)
{
  uint8_t u0 = (uint8_t)(value);     /* to if this somehow can be put between SPDR=x and while() */
  uint16_t u16 = (uint16_t)(value  >>  8);
  uint8_t u2 = (uint8_t)(u16 >> 8);

 
  SPDR = u2;                      // SPDR schreiben startet Uebertragung 
  while (!(SPSR & (1 << SPIF)));  // warten auf Ende der Uebertragung für dieses Byte 

  uint8_t u1 = (uint8_t)(u16);
  SPDR = u1;
  while (!(SPSR & (1 << SPIF)));

  SPDR = u0;
  while (!(SPSR & (1 << SPIF)));

  /* latch data */
  SHIFT_SR_SPI_PORT &= ~(1<< SHIFT_SR_SPI_RCLK);
  SHIFT_SR_SPI_PORT |=  (1<< SHIFT_SR_SPI_RCLK);
}


