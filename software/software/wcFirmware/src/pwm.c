/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file pwm.c
 * 
 *  Pwm control.
 *  \details
 *      Written for ATMEGA88 @ 8 MHz. \n
 *      This module implements the initialization and control of the pwm channels
 *      to control the rgb colors and brightness.
 *
 * \version $Id: pwm.c 408 2011-12-11 14:10:30Z vt $
 * 
 * \author Copyright (c) 2010 Frank Meyer - frank(at)fli4l.de
 * \author Copyright (c) 2010 Vlad Tepesch  
 *
 * \remarks
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *//*-----------------------------------------------------------------------------------------------------------------------------------------------*/


#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "wceeprom.h"

#include "uart.h"
#include "base.h"

#include "main.h"
#include "pwm.h"

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 * PWM: constants/variables
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
#define PWM_RED_PORT          PORTD                                             ///< port D
#define PWM_RED_DDR           DDRD                                              ///< ddr D
#define PWM_RED_BIT           6                                                 ///< OC0A

#define PWM_GREEN_PORT        PORTD                                             ///< port D
#define PWM_GREEN_DDR         DDRD                                              ///< ddr D
#define PWM_GREEN_BIT         5                                                 ///< OC0B

#define PWM_BLUE_PORT         PORTD                                             ///< port D
#define PWM_BLUE_DDR          DDRD                                              ///< ddr D
#define PWM_BLUE_BIT          3                                                 ///< OC2B



#if (MAX_PWM_STEPS==32)
uint8_t pwm_table[MAX_PWM_STEPS]  PROGMEM =
{
     1,   2,   3,   4,   5,   6,   7,   8,
     9,  10,  12,  14,  16,  18,  21,  24,
    28,  32,  37,  42,  48,  55,  63,  72,
    83,  96, 111, 129, 153, 182, 216, 255
};
#elif (MAX_PWM_STEPS==64)
uint8_t pwm_table[MAX_PWM_STEPS]  PROGMEM =
{
	1,	 2,	  3,   4,   5,   6,   7,   8,
    9,  10,  11,  12,  13,  14,  15,  16,
   17,  18,  19,  21,  22,  23,  25,  26,
   27,  29,  30,  32,  33,  35,  36,  38,
   40,  42,  44,  46,  48,  51,  53,  56,
   58,  61,  63,  66,  69,  72,  75,  78,
   81,  85,  89,  93,  98, 103, 109, 116,
  125, 135, 146, 158, 171, 189, 214, 255
};
#else
#  error unknown pwm step size
#endif

static uint8_t                pwm_is_on;                                        ///< flag: pwm is on
static uint8_t                base_pwm_idx;                                     ///< current base pwm index
static uint8_t                brightness_pwm_val;                               ///< current brightness pwm value
static uint8_t                brightness_lock;                                  ///< flag that forbids that brightness changes take effect
#define offset_pwm_idx   (wcEeprom_getData()->pwmParams.brightnessOffset)
//static int8_t                 offset_pwm_idx;                                   ///< current offset pwm index
#define g_occupancy               (wcEeprom_getData()->pwmParams.occupancy)
#define g_ldrBrightness2pwmStep   (wcEeprom_getData()->pwmParams.brightness2pwmStep)

// for testing without messing eeprom:
//uint8_t g_ldrBrightness2pwmStep[LDR2PWM_COUNT]={
//  1,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
//};
//LDR2PWM_OCC_TYPE g_occupancy = ((LDR2PWM_OCC_TYPE)1) | (((LDR2PWM_OCC_TYPE)1)<<(LDR2PWM_COUNT-1));


static uint8_t                base_ldr_idx;                                     ///< current index to g_ldrBrightness2pwmStep array

#if (MONO_COLOR_CLOCK != 1)
  static uint8_t                red_pwm_idx;                                      ///< current red pwm index
  static uint8_t                green_pwm_idx;                                    ///< current green pwm index
  static uint8_t                blue_pwm_idx;                                     ///< current blue pwm index
  static uint8_t                red_pwm;                                      ///< current red pwm value
  static uint8_t                green_pwm;                                    ///< current green pwm value
  static uint8_t                blue_pwm;                                     ///< current blue pwm value
#endif


/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Set brightness by step
 *  @details  sets brightness by global step values base_pwm_idx + offset_pwm_idx
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
static void
pwm_set_brightness_step (void)
{
  if(! brightness_lock)
  {
    int8_t  pwm_idx = ((int8_t)base_pwm_idx) + offset_pwm_idx ;

    if (pwm_idx < 0)
    {
      pwm_idx = 0;
    }
    else if (pwm_idx >= MAX_PWM_STEPS)
    {
      pwm_idx = MAX_PWM_STEPS - 1;
    }

    brightness_pwm_val = pgm_read_byte (pwm_table + pwm_idx);
  }

#if (MONO_COLOR_CLOCK == 1)
    OCR0A  = 255-brightness_pwm_val;
#else
    pwm_set_colors(red_pwm, green_pwm, blue_pwm);
# endif
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Initialize the PWM
 *  @details  Configures 0CR0A, 0CR0B and 0CR2B as PWM channels
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
pwm_init (void)
{
  PWM_RED_PORT &= ~(1<<PWM_RED_BIT);                                            // set PWM_RED_BIT to low
  PWM_RED_DDR |= (1<<PWM_RED_BIT);                                              // set PWM_RED_BIT to output

  TCCR0A = (1<<WGM01) | (1<<WGM00);                                             // non-inverted PWM, 8 Bit Fast PWM

  //                                                                            // values for Fast PWM:
  // TCCR0B = (1<<CS00);                                                        // 001: prescaler    1 -> 8 MHz / 256 /    1 = 31250 Hz PWM frequency
  // TCCR0B = (1<<CS01);                                                        // 010: prescaler    8 -> 8 MHz / 256 /    8 =  3906 Hz PWM frequency
  // TCCR0B = (1<<CS01) | (1<<CS00);                                            // 011: prescaler   64 -> 8 MHz / 256 /   64 =   488 Hz PWM frequency
  // TCCR0B = (1<<CS02);                                                        // 100: prescaler  256 -> 8 MHz / 256 /  256 =   122 Hz PWM frequency
  // TCCR0B = (1<<CS02) | (1<<CS00);                                            // 101: prescaler 1024 -> 8 MHz / 256 / 1024 =    31 Hz PWM frequency

  TCCR0B = (1<<CS01) | (1<<CS00);                                               // 011: prescaler   64 -> 8 MHz / 256 /   64 =   488 Hz PWM frequency

  TCCR2A = (1<<WGM21) | (1<<WGM20);                                             // non-inverted PWM, 8 Bit Fast PWM
  //                                                                            // values for Fast PWM:
  // TCCR2B = (1<<CS20);                                                        // 001: prescaler    1 -> 8 MHz / 256 /    1 = 31250 Hz PWM frequency
  // TCCR2B = (1<<CS21);                                                        // 010: prescaler    8 -> 8 MHz / 256 /    8 =  3906 Hz PWM frequency
  // TCCR2B = (1<<CS21) | (1<<CS20);                                            // 011: prescaler   32 -> 8 MHz / 256 /   32 =   977 Hz PWM frequency
  // TCCR2B = (1<<CS22);                                                        // 100: prescaler   64 -> 8 MHz / 256 /   64 =   488 Hz PWM frequency
  // TCCR2B = (1<<CS22) | (1<<CS20);                                            // 101: prescaler  128 -> 8 MHz / 256 /  128 =   244 Hz PWM frequency
  // TCCR2B = (1<<CS22) | (1<<CS21);                                            // 110: prescaler  256 -> 8 MHz / 256 /  256 =   122 Hz PWM frequency
  // TCCR2B = (1<<CS22) | (1<<CS21) | | (1<<CS20);                              // 111: prescaler 1024 -> 8 MHz / 256 / 1024 =    31 Hz PWM frequency

  TCCR2B = (1<<CS22);                                                           // 100: prescaler   64 -> 8 MHz / 256 /   64 =   488 Hz PWM frequency

# if (MONO_COLOR_CLOCK != 1)
    PWM_GREEN_PORT &= ~(1<<PWM_GREEN_BIT);                                        // set PWM_RED_BIT to low
    PWM_GREEN_DDR |= (1<<PWM_GREEN_BIT);                                          // set PWM_RED_BIT to output
    PWM_BLUE_PORT &= ~(1<<PWM_BLUE_BIT);                                          // set PWM_BLUE_BIT to low
    PWM_BLUE_DDR |= (1<<PWM_BLUE_BIT);                                            // set PWM_BLUE_BIT to output


# endif

}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Switch PWM on
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void pwm_on (void)
{

  TCCR0A |= ((1<<COM0A1) | (1<<COM0A0));                                          // @EDI: INVERTED PWM on OC0A, 8 Bit Fast PWM

# if (MONO_COLOR_CLOCK != 1)
    TCCR0A |= ((1<<COM0B1) | (1<<COM0B0));                                        // @EDI: INVERTED PWM on OC0B, 8 Bit Fast PWM
    TCCR2A |= ((1<<COM2B1) | (1<<COM2B0));                                        // @EDI: INVERTED PWM on OC2B, 8 Bit Fast PWM
# endif

  pwm_is_on = TRUE;                                                               // set flag
  pwm_set_brightness_step ();
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Switch PWM off
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void pwm_off (void)
{

  PWM_RED_PORT  &= ~(1<<PWM_RED_BIT);                                             // set PWM_RED_BIT to low
  TCCR0A &= ~((1<<COM0A1) | (1<<COM0A0));                                         // reset TCCR0A (@EDI: INVERSED PWM)

# if (MONO_COLOR_CLOCK != 1)
    PWM_GREEN_PORT  &= ~(1<<PWM_GREEN_BIT);                                       // set PWM_GREEN_BIT to low
    TCCR0A &= ~((1<<COM0B1) | (1<<COM0B0));                                       // reset TCCR0A (@EDI: INVERSED PWM)

    PWM_BLUE_PORT  &= ~(1<<PWM_BLUE_BIT);                                         // set PWM_BLUE_BIT to low
    TCCR2A &= ~((1<<COM2B1) | (1<<COM2B0));                                       // reset TCCR2A (@EDI: INVERSED PWM)
# endif


  pwm_is_on = FALSE;                                                              // reset flag
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Toggle PWM off/on
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
pwm_on_off (void)
{
  if (pwm_is_on)
  {
    pwm_off ();
  }
  else
  {
    pwm_on ();
    pwm_set_brightness_step ();
  }
}


#if (MONO_COLOR_CLOCK != 1)
/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Set RGB colors
 *  @param    red: range 0-255
 *  @param    green: range 0-255
 *  @param    blue: range 0-255
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
pwm_set_colors (uint8_t red, uint8_t green, uint8_t blue)
{
  uint16_t brightnessFactor =  ((uint16_t) brightness_pwm_val) + 1;
  red_pwm   = red;
  green_pwm = green;
  blue_pwm  = blue;


  OCR0A  = 255 - ((brightnessFactor * red)/256);				// @EDI: INVERSED fast PWM mode, thus on phase is towards TOP
  OCR0B  = 255 - ((brightnessFactor * green)/256);				// @EDI: INVERSED fast PWM mode, thus on phase is towards TOP
  OCR2B  = 255 - ((brightnessFactor * blue)/256);				// @EDI: INVERSED fast PWM mode, thus on phase is towards TOP
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Get RGB colors
 *  @param    pointer to value of red pwm: range 0-255
 *  @param    pointer to value of green pwm: range 0-255
 *  @param    pointer to value of blue pwm: range 0-255
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
pwm_get_colors (uint8_t * redp, uint8_t * greenp, uint8_t * bluep)
{
  *redp   = red_pwm;
  *greenp = green_pwm;
  *bluep  = blue_pwm;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Set RGB colors by step values
 *  @param    red_step: range 0 to MAX_PWM_STEPS
 *  @param    green_step: range 0 to MAX_PWM_STEPS
 *  @param    blue_step: range 0 to MAX_PWM_STEPS
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
pwm_set_color_step (uint8_t red_step, uint8_t green_step, uint8_t blue_step)
{
  if (red_step >= MAX_PWM_STEPS)
  {
    red_step = MAX_PWM_STEPS - 1;
  }

  if (green_step >= MAX_PWM_STEPS)
  {
    green_step = MAX_PWM_STEPS - 1;
  }

  if (blue_step >= MAX_PWM_STEPS)
  {
    blue_step = MAX_PWM_STEPS - 1;
  }

  red_pwm_idx   = red_step;
  green_pwm_idx = green_step;
  blue_pwm_idx  = blue_step;

  pwm_set_colors (pgm_read_byte (pwm_table + red_step), pgm_read_byte (pwm_table + green_step), pgm_read_byte (pwm_table + blue_step));
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Get RGB color step values
 *  @param    pointer to red_step: range 0 to MAX_PWM_STEPS
 *  @param    pointer to green_step: range 0 to MAX_PWM_STEPS
 *  @param    pointer to blue_step: range 0 to MAX_PWM_STEPS
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
pwm_get_color_step (uint8_t * red_stepp, uint8_t * green_stepp, uint8_t * blue_stepp)
{
  *red_stepp    = red_pwm_idx;
  *green_stepp  = green_pwm_idx;
  *blue_stepp   = blue_pwm_idx;
}
#endif

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Set base brightness by step 0-31
 *  @param    pwm step 0-31
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
pwm_set_base_brightness_step (uint8_t pwm_idx)
{
  if (pwm_idx >= LDR2PWM_COUNT)
  {
    pwm_idx = LDR2PWM_COUNT - 1;
  }
  base_ldr_idx = pwm_idx;
  base_pwm_idx = g_ldrBrightness2pwmStep[pwm_idx];
  pwm_set_brightness_step();
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Step up brightness
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
pwm_step_up_brightness (void)
{
  if (pwm_is_on && (base_pwm_idx + offset_pwm_idx + 1 < MAX_PWM_STEPS) )
  {
    offset_pwm_idx++;
    pwm_set_brightness_step ();
  }
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
 *  Step down brightness
 *---------------------------------------------------------------------------------------------------------------------------------------------------
 */
void
pwm_step_down_brightness (void)
{
  if (pwm_is_on && (base_pwm_idx + offset_pwm_idx > 0) )
  {
    offset_pwm_idx--;
    pwm_set_brightness_step ();
  }
}

void pwm_lock_brightness_val(uint8_t val)
{
  brightness_lock = TRUE;
  brightness_pwm_val = val;
  pwm_set_brightness_step();
}

void pwm_release_brightness(void)
{
  brightness_lock = FALSE;
  pwm_set_brightness_step();
}





#if (LDR2PWM_LOG == 1)

void outputVals()
{
  char buf[4];
  for(uint8_t i=0; i<LDR2PWM_COUNT;++i)
  {
    byteToStr(g_ldrBrightness2pwmStep[i], buf);
    uart_puts(buf);
  }
  uart_putc('\n');
  for(uint8_t i=0; i<LDR2PWM_COUNT;++i)
  {
    uart_putc(' ');
    if(g_occupancy & (((LDR2PWM_OCC_TYPE)1)<<i))
    {
      uart_putc('x');
    }else{
      uart_putc(' ');
    }
    uart_putc(' ');
  } 
  uart_putc('\n');
}

void outputPointer(uint8_t ind, uint8_t l, uint8_t r)
{
  for(uint8_t i=0; i<LDR2PWM_COUNT;++i)
  {

    if(l == i)
    {
      uart_putc('<');
    }else{
      uart_putc(' ');
    }

    if(ind == i)
    {
      uart_putc('^');
    }else{
      uart_putc(' ');
    }

      
    if(r == i)
    {
      uart_putc('>');
    }else{
      uart_putc(' ');
    }
  }
  uart_putc('\n');
}
#endif


static void getBounds(uint8_t ind, uint8_t val,  uint8_t* left, uint8_t* right)
{

  while(1){
    if(ind>0)
    {
      LDR2PWM_OCC_TYPE mask = ((LDR2PWM_OCC_TYPE)1)<<(ind-1);
      *left  = ind-1;
      while( !(mask&g_occupancy) )
      {
        mask >>= 1;
        --(*left);
      }
    }else{
      *left  = 0;
    }

    if( g_ldrBrightness2pwmStep[*left] > val ) // if value of found position is smaller than new value
    {                                          //
      g_ldrBrightness2pwmStep[*left] = val;    // reduce value
      if(*left > 0)                            // and clear 'user defined  flag'
      {                                        // if not at the border of array
        g_occupancy &= ~(((LDR2PWM_OCC_TYPE)1) << *left);
      }
    }else
    {
      break;
    }

  };


  while(1)
  {
    if(ind<LDR2PWM_COUNT-1)
    {
      LDR2PWM_OCC_TYPE mask = ((LDR2PWM_OCC_TYPE)1)<<(ind+1);
      *right = ind+1;
      while( !(mask&g_occupancy) )
      {
        mask <<= 1;
        ++(*right);
      }
    }else{
      *right = LDR2PWM_COUNT-1;
    }

    if( g_ldrBrightness2pwmStep[*right] < val )  // if value of found position is smaller than new value
    {                                            //
      g_ldrBrightness2pwmStep[*right] = val;     // reduce value
      if(*right < (LDR2PWM_COUNT-1))             // and clear 'user defined  flag'
      {                                          // if not at the border of array
        g_occupancy &= ~(((LDR2PWM_OCC_TYPE)1) << *right);          
      }
    }else
    {
      break;
    }
  }
}

static void interpolate(uint8_t left, uint8_t right)
{
  enum{
    SHIFT = 8,
  };
  uint8_t d = right-left;
  if(d>1)
  {
    int8_t  leftVal  = (int8_t)(g_ldrBrightness2pwmStep[left]);
    int8_t  rightVal = (int8_t)(g_ldrBrightness2pwmStep[right]);
    int16_t l = (((int16_t)leftVal)<<SHIFT)+(((uint16_t)1) << (SHIFT-1));  // scaling 2^SHIFT;  add half scaling for proper rounding
    uint8_t i;
    int16_t slope  = ((int16_t)( rightVal - leftVal ))<<SHIFT;        // scaling 2^SHIFT
    slope /= d;
    for(i=1; i<d; ++i)
    { 
      l += slope;
      g_ldrBrightness2pwmStep[left+i] = (uint8_t)( l >>SHIFT);
    }
  }
}


static void modifyLdrBrightness2pwmStep(uint8_t ind, uint8_t val)
{
  uint8_t left;
  uint8_t right;

# if (LDR2PWM_LOG == 1)
    uart_puts_P("before\n");
    outputVals();
# endif

  g_ldrBrightness2pwmStep[ind] = val;
  getBounds(ind, val, &left, &right);

# if (LDR2PWM_LOG == 1)
    uart_puts_P("bounds\n");
    outputVals();
    outputPointer(ind, left, right);
# endif

  interpolate(left, ind);
  interpolate(ind, right);

  g_occupancy |= (((LDR2PWM_OCC_TYPE)1)<<ind);
# if (LDR2PWM_LOG == 1)
    uart_puts_P("after\n");
    outputVals();
# endif
}


void pwm_modifyLdrBrightness2pwmStep(void)
{
  if(offset_pwm_idx)
  {
    uint8_t val = base_pwm_idx + offset_pwm_idx;
    if( val >= MAX_PWM_STEPS )
    {
      val = MAX_PWM_STEPS;
    }
    modifyLdrBrightness2pwmStep(base_ldr_idx, val);
    base_pwm_idx = val;
    offset_pwm_idx = 0;
    pwm_set_brightness_step();

    pwm_on_off();                            // @EDI: acknowledge, that new brightness has been saved
    _delay_ms(500);
    pwm_on_off();
  }
  
}

