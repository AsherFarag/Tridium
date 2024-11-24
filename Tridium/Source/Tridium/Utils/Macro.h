#pragma once

#define BIT(x) (1 << x)

#define Stringize( x ) #x 
#define WRAP( m, x ) m(x)
#define PRAGMA(p) _Pragma(#p)
#define EXPAND(x) x

#pragma region Selectors

#define SELECT_MACRO_2(_1, _2, x, ...) x
#define SELECT_MACRO_3(_1, _2, _3, x, ...) x
#define SELECT_MACRO_4(_1, _2, _3, _4, x, ...) x
#define SELECT_MACRO_5(_1, _2, _3, _4, _5, x, ...) x
#define SELECT_MACRO_6(_1, _2, _3, _4, _5, _6, x, ...) x
#define SELECT_MACRO_7(_1, _2, _3, _4, _5, _6, _7, x, ...) x
#define SELECT_MACRO_8(_1, _2, _3, _4, _5, _6, _7, _8, x, ...) x
#define SELECT_MACRO_9(_1, _2, _3, _4, _5, _6, _7, _8, _9, x, ...) x
#define SELECT_MACRO_10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, x, ...) x

#pragma endregion

#define MIN(x, y) (x > y ? y : x)
#define MAX(x, y) (x > y ? x : y)
#define CLAMP(min, max, val) (val < min ? min : (val > max ? max : val))