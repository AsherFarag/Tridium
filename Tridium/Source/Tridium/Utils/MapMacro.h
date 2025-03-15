/*
 * Created by William Swanson in 2012.
 *
 * I, William Swanson, dedicate this work to the public domain.
 * I waive all rights to the work worldwide under copyright law,
 * including all related and neighboring rights,
 * to the extent allowed by law.
 *
 * You can copy, modify, distribute and perform the work,
 * even for commercial purposes, all without asking permission.
 */

#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "Macro.h"

#define MAP_1(F, x ) F(x)
#define MAP_2(F, x, ...) F(x), MAP_1(F, __VA_ARGS__)
#define MAP_3(F, x, ...) F(x), MAP_2(F, __VA_ARGS__)
#define MAP_4(F, x, ...) F(x), MAP_3(F, __VA_ARGS__)
#define MAP_5(F, x, ...) F(x), MAP_4(F, __VA_ARGS__)
#define MAP_6(F, x, ...) F(x), MAP_5(F, __VA_ARGS__)
#define MAP_7(F, x, ...) F(x), MAP_6(F, __VA_ARGS__)
#define MAP_8(F, x, ...) F(x), MAP_7(F, __VA_ARGS__)
#define MAP_9(F, x, ...) F(x), MAP_8(F, __VA_ARGS__)
#define MAP_10(F, x, ...) F(x), MAP_9(F, __VA_ARGS__)
#define MAP_11(F, x, ...) F(x), MAP_10(F, __VA_ARGS__)
#define MAP_12(F, x, ...) F(x), MAP_11(F, __VA_ARGS__)
#define MAP_13(F, x, ...) F(x), MAP_12(F, __VA_ARGS__)
#define MAP_14(F, x, ...) F(x), MAP_13(F, __VA_ARGS__)
#define MAP_15(F, x, ...) F(x), MAP_14(F, __VA_ARGS__)
#define MAP_16(F, x, ...) F(x), MAP_15(F, __VA_ARGS__)
#define MAP_17(F, x, ...) F(x), MAP_16(F, __VA_ARGS__)
#define MAP_18(F, x, ...) F(x), MAP_17(F, __VA_ARGS__)
#define MAP_19(F, x, ...) F(x), MAP_18(F, __VA_ARGS__)
#define MAP_20(F, x, ...) F(x), MAP_19(F, __VA_ARGS__)


#define MAP(F, ...) EXPAND(SELECT_MACRO_10(__VA_ARGS__, MAP_10, MAP_9, MAP_8, MAP_7, MAP_6, MAP_5, MAP_4, MAP_3, MAP_2, MAP_1))(F, __VA_ARGS__)
#endif
