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

#define UINT_BITS (buint_size_t)(8*sizeof(UInt))

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
static inline UInt uint_add(UInt a, UInt b, buint_bool *carry) {
 UInt c = a + b + !!(*carry);
 *carry = *carry ? c <= a : c < a;
 return c;
}

/**
 @brief Incrementing an UInt value with other UInt and carry bit.
 @param a points to the value to increment.
 @param b value to add to a.
 @param carry Input carry bit read from, output carry bit written to.
 @return same as a.
*/
static inline UInt *uint_add_assign(UInt *a, UInt b, buint_bool *carry) {
 *a += b + !!(*carry);
 *carry = (*carry ? *a <= b : *a < b);
 return a;
}

/**
 @brief Subtraction of UInt value from UInt value with carry bit.
 @param a minuend
 @param b subtrahend
 @param carry Input carry bit read from, output carry bit written to.
*/
static inline UInt uint_sub(UInt a, UInt b, buint_bool *carry) {
 UInt c = a - b - !!(*carry);
 *carry = *carry ? a <= c : a < c;
 return c;
}

static inline UInt *uint_sub_assign(UInt *a, UInt b, buint_bool *carry) {
 UInt orig = *a;
 *a -= b;
 if (*carry) --(*a);
 *carry = (*carry ? orig <= *a : orig < *a);
 return a;
}

/**
 @brief Splits a value by means of bit masking into two.
 @return first: high bits greater or equal to lsb; second: low bits.
*/
static inline UIntPair uint_split(UInt a, buint_size_t lsb) {
 UIntPair retv;
 UInt mask = (lsb < UINT_BITS ? ((UInt)1 << lsb):0) - 1;
 retv.first = a & ~mask;
 retv.second = a & mask;
 return retv;
}

/**
 @brief Splits a value by means if bit shifting into two.
 @return first: high bits of value a (greater or equal to lsb) at the bottom bits;
 second: low bits of a at the top bits.
 */
static inline UIntPair uint_split_shift(UInt a, buint_size_t lsb) {
 UIntPair retv;
 retv.first= (lsb < UINT_BITS)?a >> lsb : 0;
 retv.second= (0<lsb) ? (a << (UINT_BITS - lsb)) : 0;
 return retv;
}

/**
 @brief Multiplication of two values.
 @return first: high bits; second: low bits.
*/
UIntPair uint_mul(UInt a, UInt b);
/**
 @return most significant bit (set to 1) of the value.
*/
buint_size_t uint_msb(UInt a);

#undef UINT_BITS
#endif

