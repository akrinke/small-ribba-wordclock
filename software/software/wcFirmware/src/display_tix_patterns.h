/*------------------------------------------------------------------------------------------------------------------------------------------------*//**
 * @file display_tix_patterns.h
 *
 *  This File defines the number patterns for the tix display
 *
 * \version $Id: display_tix_patterns.h 285 2010-03-24 21:43:24Z vt $
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

#ifndef _TIX_PATTERNS_
#define _TIX_PATTERNS_

/* numbering:

[1]   [1][2][3]   [1][2]   [1][2][3]
[2]   [4][5][6]   [3][4]   [4][5][6]
[3]   [7][8][9]   [5][6]   [7][8][9]

*/




/*
tm_fill
[ ][ ][ ]   [ ][ ][ ]   [ ][ ][ ]   [ ][ ][ ]
[ ][ ][ ]   [ ][ ][ ]   [ ][ ][ ]   [ ][ ][x]
[x][ ][ ]   [x][x][ ]   [x][x][x]   [x][x][x]

[ ][ ][ ]   [ ][ ][ ]   [x][ ][ ]   [x][x][ ]   [x][x][x]
[ ][x][x]   [x][x][x]   [x][x][x]   [x][x][x]   [x][x][x]
[x][x][x]   [x][x][x]   [x][x][x]   [x][x][x]   [x][x][x]

[ ][ ]   [ ][ ]   [ ][ ]   [ ][ ]   [x][ ]   [x][x]
[ ][ ]   [ ][ ]   [ ][x]   [x][x]   [x][x]   [x][x]
[x][ ]   [x][x]   [x][x]   [x][x]   [x][x]   [x][x]

[ ]  [ ]  [x]
[ ]  [x]  [x]
[x]  [x]  [x]

*/

/*
tm_dice
[ ][ ][ ]   [x][ ][ ]   [ ][ ][x]   [x][ ][x]
[ ][x][ ]   [ ][ ][ ]   [ ][x][ ]   [ ][ ][ ]
[ ][ ][ ]   [ ][ ][x]   [x][ ][ ]   [x][ ][x]

[x][ ][x]   [x][ ][x]   [x][ ][x]   [x][x][x]   [x][x][x]
[ ][x][ ]   [x][ ][x]   [x][x][x]   [x][ ][x]   [x][x][x]
[x][ ][x]   [x][ ][x]   [x][ ][x]   [x][x][x]   [x][x][x]

[ ][ ]   [x][ ]   [x][ ]   [x][x]   [x][x]   [x][x]
[x][ ]   [ ][ ]   [ ][x]   [ ][ ]   [x][ ]   [x][x]
[ ][ ]   [ ][x]   [x][ ]   [x][x]   [x][x]   [x][x]

[ ]  [x]  [x]
[x]  [ ]  [x]
[ ]  [x]  [x]

*/

/*
tm_border
[ ][ ][ ]   [ ][ ][ ]   [ ][ ][ ]   [ ][ ][ ]
[ ][ ][ ]   [ ][ ][ ]   [ ][ ][ ]   [x][ ][ ]
[ ][ ][x]   [ ][x][x]   [x][x][x]   [x][x][x]

[x][ ][ ]   [x][x][ ]   [x][x][x]   [x][x][x]   [x][x][x]
[x][ ][ ]   [x][ ][ ]   [x][ ][ ]   [x][ ][x]   [x][x][x]
[x][x][x]   [x][x][x]   [x][x][x]   [x][x][x]   [x][x][x]

[ ][ ]   [ ][ ]   [ ][x]   [x][x]   [x][x]   [x][x]
[ ][ ]   [ ][x]   [ ][x]   [ ][x]   [x][x]   [x][x]
[ ][x]   [ ][x]   [ ][x]   [ ][x]   [ ][x]   [x][x]

[x]  [x]  [x]
[ ]  [x]  [x]
[ ]  [ ]  [x]

*/


// some Macros for nicer definitions
#define X 'x'
#define _ ' '
#define _ELEM(x) ( ((x)=='x')?1:0 )
#define _SHAPE9( s1,s2,s3,s4,s5,s6,s7,s8,s9) \
     BIN16( 0,0,0,0,0,0,0,                   \
           _ELEM(s9), _ELEM(s8), _ELEM(s7),  \
           _ELEM(s6), _ELEM(s5), _ELEM(s4),  \
           _ELEM(s3), _ELEM(s2), _ELEM(s1) )

#define _SHAPE6( s1,s2,s3,s4,s5,s6)          \
     BIN8( 0,0,                             \
           _ELEM(s6), _ELEM(s5), _ELEM(s4),  \
           _ELEM(s3), _ELEM(s2), _ELEM(s1) )

#define _SHAPE3( s1,s2,s3)                   \
     BIN8( 0,0, 0, 0, 0,                    \
           _ELEM(s3), _ELEM(s2), _ELEM(s1) )


#define C_NUMBERS9_FULL BIN16( 0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,1)

static const uint16_t c_numbers9[TM_COUNT-1][8] = {  // no random and no 9
  /* [tm_fill] = */  {
            /* 1 = */  _SHAPE9( _,_,_,
                                _,_,_,
                                X,_,_  ),

            /* 2 = */  _SHAPE9( _,_,_,
                                _,_,_,
                                X,X,_  ),

            /* 3 = */  _SHAPE9( _,_,_,
                                _,_,_,
                                X,X,X  ),

            /* 4 = */  _SHAPE9( _,_,_,
                                _,_,X,
                                X,X,X  ),

            /* 5 = */  _SHAPE9( _,_,_,
                                _,X,X,
                                X,X,X  ),

            /* 6 = */  _SHAPE9( _,_,_,
                                X,X,X,
                                X,X,X  ),

            /* 7 = */  _SHAPE9( X,_,_,
                                X,X,X,
                                X,X,X  ),

            /* 8 = */  _SHAPE9( X,X,_,
                                X,X,X,
                                X,X,X  )
  },

  /* [tm_dice] = */  {
            /* 1 = */  _SHAPE9( _,_,_,
                                _,X,_,
                                _,_,_  ),

            /* 2 = */  _SHAPE9( X,_,_,
                                _,_,_,
                                _,_,X  ),

            /* 3 = */  _SHAPE9( _,_,X,
                                _,X,_,
                                X,_,_  ),

            /* 4 = */  _SHAPE9( X,_,X,
                                _,_,_,
                                X,_,X  ),

            /* 5 = */  _SHAPE9( X,_,X,
                                _,X,_,
                                X,_,X  ),

            /* 6 = */  _SHAPE9( X,_,X,
                                X,_,X,
                                X,_,X  ),

            /* 7 = */  _SHAPE9( X,_,X,
                                X,X,X,
                                X,_,X  ),

            /* 8 = */  _SHAPE9( X,X,X,
                                X,_,X,
                                X,X,X  )
   },

   /*  [tm_border] = */ {
            /* 1 = */  _SHAPE9( _,_,_,
                                _,_,_,
                                _,_,X  ),

            /* 2 = */  _SHAPE9( _,_,_,
                                _,_,_,
                                _,X,X  ),

            /* 3 = */  _SHAPE9( _,_,_,
                                _,_,_,
                                X,X,X  ),

            /* 4 = */  _SHAPE9( _,_,_,
                                X,_,_,
                                X,X,X  ),

            /* 5 = */  _SHAPE9( X,_,_,
                                X,_,_,
                                X,X,X  ),

            /* 6 = */  _SHAPE9( X,X,_,
                                X,_,_,
                                X,X,X  ),

            /* 7 = */  _SHAPE9( X,X,X,
                                X,_,_,
                                X,X,X  ),

            /* 8 = */  _SHAPE9( X,X,X,
                                X,_,X,
                                X,X,X  )
  },
};


#define C_NUMBERS6_FULL BIN8( 0,0,1,1,1,1,1,1)
static const uint8_t c_numbers6[TM_COUNT-1][5] = {  // no random and no 6
  /* [tm_fill] = */  {
            /* 1 = */  _SHAPE6( _,_,
                                _,_,
                                X,_  ),

            /* 2 = */  _SHAPE6( _,_,
                                _,_,
                                X,X  ),

            /* 3 = */  _SHAPE6( _,_,
                                _,X,
                                X,X  ),

            /* 4 = */  _SHAPE6( _,_,
                                X,X,
                                X,X  ),

            /* 5 = */  _SHAPE6( X,_,
                                X,X,
                                X,X  )

  },

  /* [tm_dice] = */  {
            /* 1 = */  _SHAPE6( _,_,
                                X,_,
                                _,_  ),

            /* 2 = */  _SHAPE6( X,_,
                                _,_,
                                _,X  ),

            /* 3 = */  _SHAPE6( X,_,
                                _,X,
                                X,_  ),

            /* 4 = */  _SHAPE6( X,X,
                                _,_,
                                X,X  ),

            /* 5 = */  _SHAPE6( X,X,
                                X,_,
                                X,X  )
  },

   /*  [tm_border] = */ {
            /* 1 = */  _SHAPE6( _,_,
                                _,_,
                                _,X  ),

            /* 2 = */  _SHAPE6( _,_,
                                _,X,
                                _,X  ),

            /* 3 = */  _SHAPE6( _,X,
                                _,X,
                                _,X  ),

            /* 4 = */  _SHAPE6( X,X,
                                _,X,
                                _,X  ),

            /* 5 = */  _SHAPE6( X,X,
                                X,X,
                                _,X  )
  }
};

#define C_NUMBERS3_FULL BIN8( 0,0,0,0,0,1,1,1)
static const uint8_t c_numbers3[TM_COUNT-1][2] = {  // no random and no 3
  /* [tm_fill] = */  {
            /* 1 = */  _SHAPE3( _,
                                _,
                                X  ),

            /* 2 = */  _SHAPE3( _,
                                X,
                                X  ),
  },

  /* [tm_dice] = */  {
            /* 1 = */  _SHAPE3( _,
                                X,
                                _  ),

            /* 2 = */  _SHAPE3( X,
                                _,
                                X  )
   },

   /*  [tm_border] = */ {
            /* 1 = */  _SHAPE3( X,
                                _,
                                _  ),

            /* 2 = */  _SHAPE3( X,
                                X,
                                _  )
   }
};



// throw away this dangerous defines
#undef X
#undef _



#endif /* _TIX_PATTERNS_ */
