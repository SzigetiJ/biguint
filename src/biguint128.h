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
#ifndef _BIGUINT128_H_
#define _BIGUINT128_H_

#include "uint_types.h"
#define BIGUINT128_CELLS (128u / (8u * sizeof(UInt)))

/**
 Stores the value in array.
 Least significant item is at index 0,
 most significant item is at index (BIGUINT128_CELLS - 1)
*/
typedef struct {
 UInt dat[ BIGUINT128_CELLS ];
} BigUInt128;

/**
 Auxiliary type for the result of division (Storing quotient and remainder).
*/
typedef struct {
 BigUInt128 first;
 BigUInt128 second;
} BigUIntPair128;

// constructors
/**
 @brief Generates BigUInt128 instance initialized to 0.
*/
BigUInt128 biguint128_ctor_default();

/**
 @brief Generates BigUInt128 instance initialized to 1.
*/
BigUInt128 biguint128_ctor_unit();

/**
 @brief Generates BigUInt128 instance with the internal array taking its value from the dat array.
*/
BigUInt128 biguint128_ctor_standard(const UInt *dat);

/**
 @brief Generates a copy of orig.
 Actually, the bitwise copy also has the desired effect.
*/
BigUInt128 biguint128_ctor_copy(const BigUInt128 *orig);

/**
 @brief Generates BigUInt128 instance initialized to value.
*/
BigUInt128 biguint128_value_of_uint(UInt value);

/**
 @brief Value initialization from char array with hexadecimal digits.
 @param hex_digits Input character array.
 @param len Length of the input array.
*/
BigUInt128 biguint128_ctor_hexcstream(const char *hex_digits, buint_size_t len);

/**
 @brief Value initialization from char array with decimal digits.
 @param dec_digits Input character array.
 @param len Length of the input array.
*/
BigUInt128 biguint128_ctor_deccstream(const char *dec_digits, buint_size_t len);

/**
 * @brief Import from byte array.
 * Overwrites the existing BigUInt128 instance.
 * @return Number of read bytes.
*/
buint_size_t biguint128_import(BigUInt128 *dest, const char *src);


// addition / subtraction
/**
 @brief Addition with overflow.
*/
BigUInt128 biguint128_add(const BigUInt128 *a, const BigUInt128 *b);

/**
 @brief Add-assignment with overflow.
*/
BigUInt128 *biguint128_add_assign(BigUInt128 *a, const BigUInt128 *b);

/**
 @brief Add-replacement with overflow.
 */
void biguint128_add_replace(BigUInt128 *dest, const BigUInt128 *a, const BigUInt128 *b);

/**
 @brief Subtraction with underflow.
*/
BigUInt128 biguint128_sub(const BigUInt128 *a, const BigUInt128 *b);

/**
 @brief Sub-assignment with underflow.
*/
BigUInt128 *biguint128_sub_assign(BigUInt128 *a, const BigUInt128 *b);

/**
 @brief Sub-replacement with overflow.
 */
void biguint128_sub_replace(BigUInt128 *dest, const BigUInt128 *a, const BigUInt128 *b);


// shift operations
/**
 @brief Shift left operation.
*/
BigUInt128 biguint128_shl(const BigUInt128 *a, const buint_size_t shift);
/**
 @brief Shift right operation.
*/
BigUInt128 biguint128_shr(const BigUInt128 *a, const buint_size_t shift);

// bitwise functions
/**
 @brief Bitwise AND operation.
*/
BigUInt128 biguint128_and(const BigUInt128 *a, const BigUInt128 *b);
/**
 @brief Bitwise OR operation.
*/
BigUInt128 biguint128_or(const BigUInt128 *a, const BigUInt128 *b);
/**
 @brief Bitwise NOT operation.
*/
BigUInt128 biguint128_not(const BigUInt128 *a);
/**
 @brief Bitwise XOR operation.
*/
BigUInt128 biguint128_xor(const BigUInt128 *a, const BigUInt128 *b);

// multiplication / division
/**
 @brief Multiplication of two numbers.
 @return Product of the factors (Least significant 128 bits).
*/
BigUInt128 biguint128_mul(const BigUInt128 *a, const BigUInt128 *b);
/**
 @brief Division of a by b.
 @return First: quotient, second: remainder.
*/
BigUIntPair128 biguint128_div(const BigUInt128 *a, const BigUInt128 *b);

// comparison
/**
 @brief 'Less than' relation.
 @return Not zero: a is less than b. Zero: a is not less than b.
*/
buint_bool biguint128_lt(const BigUInt128 *a, const BigUInt128 *b);
/**
 @brief Checks equality.
 @return Not zero: a is equal to b. Zero: a is not equal to b.
*/
buint_bool biguint128_eq(const BigUInt128 *a, const BigUInt128 *b);

// misc.
/**
 @brief Most significant bit.
 @param a (Pointing to the) input value to examine.
 @return Index of the most significant bit set to 1.
*/
buint_size_t biguint128_msb(const BigUInt128 *a);
/**
 @brief Set a bit (to 1) of the value.
 @param a (Pointer to) the value to operate on.
 @param bit Index of the bit.
*/
void biguint128_sbit(BigUInt128 *a, buint_size_t bit);
/**
 @brief Clears a bit (set to 0) of the value.
 @param a (Pointer to) the value to operate on.
 @param bit Index of the bit.
*/
void biguint128_cbit(BigUInt128 *a, buint_size_t bit);
/**
 @brief Overwrite a bit of the value.
 @param a (Pointer to) the value to operate on.
 @param bit Index of the bit.
 @param value Set or clear the bit.
*/
void biguint128_obit(BigUInt128 *a, buint_size_t bit, buint_bool value);
/**
 @brief Get a bit of the value.
 @param a (Pointer to) the value to operate on.
 @param bit Index of the bit.
*/
buint_bool biguint128_gbit(BigUInt128 *a, buint_size_t bit);

// out
/**
 @brief Export the value in character array format, base 16.
 Digits greater than 9 are written in upper case.
 The method does not write terminating 0 character.
 However, the method returns where the exported data terminates
 (where to put terminating 0 if the caller wants to treat to character array as a C-string).
 @param a Pointer to the value to export.
 @param buf Target of the export. 
 @param buf_len Length of the target buffer.
 @return Length of the written characters. Zero: buf_len is to small to store the value.
*/
buint_size_t biguint128_print_hex(const BigUInt128 *a, char *buf, buint_size_t buf_len);
/**
 @brief Export the value in character array format, base 10.
 The method does not write terminating 0 character.
 However, the method returns where the exported data terminates
 (where to put terminating 0 if the caller wants to treat to character array as a C-string).
 @param a Pointer to the value to export.
 @param buf Target of the export. 
 @param buf_len Length of the target buffer.
 @return Length of the written characters. Zero: buf_len is to small to store the value.
*/
buint_size_t biguint128_print_dec(const BigUInt128 *a, char *buf, buint_size_t buf_len);

/**
 @brief Exports data into byte array.
 @return Number of written bytes.
*/
buint_size_t biguint128_export(const BigUInt128 *a, char *dest);

#endif

