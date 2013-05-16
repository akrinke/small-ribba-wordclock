/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file base.h
 * 
 *  This module provides some usefull or basic tool routines
 *
 * @version $Id: base.h 285 2010-03-24 21:43:24Z vt $
 * 
 * \author Copyright (c) 2010 Frank Meyer - frank(at)fli4l.de
 * \author Copyright (c) 2010 Vlad Tepesch    
 * 
 * \remarks
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 */
 /*-----------------------------------------------------------------------------------------------------------------------------------------------*/



#ifndef _WC_BASE_H_
#define _WC_BASE_H_

#ifdef __cplusplus
extern "C"
{
#endif


/**
 *  Can be used to create 8bit constants with binary numbers.\n
 *  Only 0 and 1 must be used for b0..b7
 */
#define BIN8(b7, b6, b5, b4, b3, b2, b1, b0) \
        (  ((b7)<<7) | ((b6)<<6) | ((b5)<<5) | ((b4)<<4) \
         | ((b3)<<3) | ((b2)<<2) | ((b1)<<1) | ((b0)<<0))


/**
 *  Can be used to create 16bit constants with binary numbers.\n
 *  Only 0 and 1 must be used for b0..b15
 */
#define BIN16( b15, b14, b13, b12, b11, b10, b9, b8, b7, b6, b5, b4, b3, b2, b1, b0) \
        (   (((uint16_t)BIN8(b15, b14, b13, b12, b11, b10, b9, b8))<<8)             \
          | BIN8(b7, b6, b5, b4, b3, b2, b1, b0))


/**
 * converts a bcd pair to integer
 * @param bcd  bcd pair
 * @return    converted integer
 */
extern uint8_t          bcdtoi (uint8_t bcd);

/**
 * converts an integer to bcd pair
 * @param i    integer number (0-255)
 * @return    converted bcd pair
 */
extern uint8_t          itobcd (uint8_t i);


/**
 * Divides a byte by ten. Also returns remainder
 * @param x         the value to divide
 * @param o_remaind pointer to memory there the remainder will be stored
 * @return the quotient
 */
static inline uint8_t div10 (uint8_t x, uint8_t* o_remaind)
{
  // seems to produce bug in byteToStrLessHundred, but why?
    //uint8_t y;
    //register uint8_t hilf;
    //asm(
    //   "ldi %[temp], 205     \n"
    //   "mul %[temp], %[input]   \n"
    //   "lsr R1             \n"
    //   "lsr R1             \n"
    //   "lsr R1             \n"
    //   "mov %[result], R1  \n"
    //    : [result] "=d" (y), [temp]"=d" (hilf)
    //    : [input]"d" (x)
    //    : "r1","r0"
    // );
    //*o_remaind = x-(10*y);
    //return y;
  uint8_t y = (((uint16_t)x)*205)>>11;
  *o_remaind = x-(y*10);
  return y;
}

/**
 *  translates a byte to a string
 *  Stripped verion of byteToStr that only can handle values between 0-99
 * @param val    the value to transform into string [0,99]
 * @param o_buf  Buffer that will receive the transformed string
 *               Has to be at least 3 bytes long because result is
 *               right justified. a \\0 is written to o_buf[2]
 *
 */
void byteToStrLessHundred( uint8_t val, char o_buf[3] );


/**
 *  translates a byte to a string
 * @param val    the value to transform into string
 * @param o_buf  Buffer that will receive the transformed string
 *               Has to be at least 4 bytes long because result is
 *               right justified. a \\0 is written to o_buf[3]
 *
 */
void byteToStr( uint8_t val, char o_buf[4] );

/**
 * creates a hexadecimal character from a nibble
 */
static inline char getHexDigit(uint8_t i_4bit)
{
  return i_4bit + ( (i_4bit<10)?'0':'A'-10 );
}


/**
 *  converts data into hex-format and prints data into o_text
 *  @param   data    the 16bit word to convert
 *  @param   o_text  The buffer that will receive the converted number.
 *                   The buffer should have at least a length of 5.
 *                   
 */
static inline void uint16ToHexStr(uint16_t data, char* o_text)
{
  o_text[4]  = 0;
  o_text[3]  = getHexDigit(data & 0xF);
  o_text[2]  = getHexDigit((data>> 4) & 0xF);
  o_text[1]  = getHexDigit((data>> 8) & 0xF);
  o_text[0]  = getHexDigit((data>>12) & 0xF);
}


#if 0 // yet not used

/**
 * Calculate day of week, perhaps later used for weekday display (by 3-to-8 decoder)
 * @detail  Calculates day of week, 0=sunday, 1=monday, ... 6=saturday
 * @return  day of week
 */
extern uint8_t          dayofweek (int, int, int);

#endif // 0


#ifdef __cplusplus
}
#endif


#endif /* _WC_BASE_H_ */

