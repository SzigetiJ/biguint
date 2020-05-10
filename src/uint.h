/*****************************************************************************

    Copyright 2020 SZIGETI János

    This file is part of biguint library (Big Unsigned Integers).

    Biguint is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Biguint is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*****************************************************************************/
#ifndef _UINT_H_
#define _UINT_H_

#include "uint_types.h"

/**
 Auxiliary type for functions returning
 either double UInt long value (e.g., multiplication)
 or pair of values (e.g., split)
*/
typedef struct {
 UInt first;
 UInt second;
} UIntPair;

/**
 @brief Addition of two UInt values with carry bit.
 @param carry Input carry bit read from, output carry bit written to.
*/
UInt uint_add(UInt a, UInt b, buint_bool *carry);

/**
 @brief Substraction of UInt value from UInt value with carry bit.
 @param a minuend
 @param b subtrahend
 @param carry Input carry bit read from, output carry bit written to.
*/
UInt uint_sub(UInt a, UInt b, buint_bool *carry);
/**
 @brief Splits a value into two at bit lsb.
 @return first: high bits greater or equal to lsb; second: low bits.
*/
UIntPair uint_split(UInt a, buint_size_t bit);
/**
 @brief Multiplication of two values.
 @return first: high bits; second: low bits.
*/
UIntPair uint_mul(UInt a, UInt b);
/**
 @return most significant bit (set to 1) of the value.
*/
buint_size_t uint_msb(UInt a);

#endif

