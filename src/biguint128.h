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

/**
 Auxiliary type for the result of tiny division (Storing quotient and remainder).
*/
typedef struct {
 BigUInt128 first;
 UInt second;
} BigUIntTinyPair128;


static inline BigUIntTinyPair128 biguint128_div1024(const BigUInt128 *a);
static inline BigUIntPair128 biguint128_div1000(const BigUInt128 *a);
static inline BigUInt128 biguint128_mul10(const BigUInt128 *a);
static inline BigUInt128 biguint128_mul100(const BigUInt128 *a);
static inline BigUInt128 biguint128_mul1000(const BigUInt128 *a);


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
 @brief Signed value initialization from char array with decimal digits.
 @param dec_digits Input character array.
 @param len Length of the input array.
*/
BigUInt128 bigint128_ctor_deccstream(const char *dec_digits, buint_size_t len);

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
 @brief Add-replacement with in/out carry bit.
 */
void biguint128_adc_replace(BigUInt128 *dest, const BigUInt128 *a, const BigUInt128 *b, buint_bool *carry);

/**
 @brief Add-assignment of UInt to biguint.
*/
BigUInt128 *biguint128_add_tiny(BigUInt128 *a, const UInt b);

/**
 @brief Subtraction with underflow.
*/
BigUInt128 biguint128_sub(const BigUInt128 *a, const BigUInt128 *b);

/**
 @brief Sub-assignment with underflow.
*/
BigUInt128 *biguint128_sub_assign(BigUInt128 *a, const BigUInt128 *b);

/**
 @brief Sub-replacement with underflow.
 */
void biguint128_sub_replace(BigUInt128 *dest, const BigUInt128 *a, const BigUInt128 *b);

/**
 @brief Sub-replacement with in/out carry.
 */
void biguint128_sbc_replace(BigUInt128 *dest, const BigUInt128 *a, const BigUInt128 *b, buint_bool *carry);

/**
 @brief Sub-assignment of UInt to biguint.
*/
BigUInt128 *biguint128_sub_tiny(BigUInt128 *a, const UInt b);

// inc/dec
BigUInt128 *biguint128_inc(BigUInt128 *a);

BigUInt128 *biguint128_dec(BigUInt128 *a);


// shift operations
/**
 @brief Shift left operation.
*/
BigUInt128 biguint128_shl(const BigUInt128 *a, const buint_size_t shift);
BigUInt128 *biguint128_shl_or(BigUInt128 *dest, const BigUInt128 *a, const buint_size_t shift);
/**
 @brief Shift right operation.
*/
BigUInt128 biguint128_shr(const BigUInt128 *a, const buint_size_t shift);
BigUInt128 *biguint128_shr_assign(BigUInt128 *a, const buint_size_t shift);

/**
 @brief Rotate left operation.
*/
BigUInt128 biguint128_rol(const BigUInt128 *a, const buint_size_t shift);
/**
 @brief Rotate right operation.
*/
BigUInt128 biguint128_ror(const BigUInt128 *a, const buint_size_t shift);


// bitwise functions
/**
 @brief Bitwise AND operation.
*/
BigUInt128 biguint128_and(const BigUInt128 *a, const BigUInt128 *b);
BigUInt128 *biguint128_and_assign(BigUInt128 *a, const BigUInt128 *b);
/**
 @brief Bitwise OR operation.
*/
BigUInt128 biguint128_or(const BigUInt128 *a, const BigUInt128 *b);
BigUInt128 *biguint128_or_assign(BigUInt128 *a, const BigUInt128 *b);
/**
 @brief Bitwise NOT operation.
*/
BigUInt128 biguint128_not(const BigUInt128 *a);
BigUInt128 *biguint128_not_assign(BigUInt128 *a);
/**
 @brief Bitwise XOR operation.
*/
BigUInt128 biguint128_xor(const BigUInt128 *a, const BigUInt128 *b);
BigUInt128 *biguint128_xor_assign(BigUInt128 *a, const BigUInt128 *b);

// multiplication / division
/**
 @brief Multiplication of two numbers.
 @return Product of the factors (Least significant 128 bits).
*/
BigUInt128 biguint128_mul(const BigUInt128 *a, const BigUInt128 *b);

/**
 @brief Multiplication of two numbers resulting in double long (BigUIntPair128) value.
 @return Product of the factors (first: low 128 bits, second: high 128 bits).
*/
BigUIntPair128 biguint128_dmul(const BigUInt128 *a, const BigUInt128 *b);

/**
 @brief Division of a by b.
 @return First: quotient, second: remainder.
*/
BigUIntPair128 biguint128_div(const BigUInt128 *a, const BigUInt128 *b);

/**
 @brief Division of signed values.
 @return First: quotient, second: remainder so that (a == b * quotient + remainder) is true.
*/
BigUIntPair128 bigint128_div(const BigUInt128 *a, const BigUInt128 *b);

// comparison
/**
 @brief 'Less than' relation.
 @return Not zero: a is less than b. Zero: a is not less than b.
*/
buint_bool biguint128_lt(const BigUInt128 *a, const BigUInt128 *b);

/**
 @brief 'Less than' relation between signed values.
 @return Not zero: a is less than b. Zero: a is not less than b.
*/
buint_bool bigint128_lt(const BigUInt128 *a, const BigUInt128 *b);

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
buint_bool biguint128_gbit(const BigUInt128 *a, buint_size_t bit);

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
 @brief Export the value treated as signed in character array format, base 10.
 The method does not write terminating 0 character.
 However, the method returns where the exported data terminates
 (where to put terminating 0 if the caller wants to treat to character array as a C-string).
 @param a Pointer to the value to export.
 @param buf Target of the export.
 @param buf_len Length of the target buffer.
 @return Length of the written characters. Zero: buf_len is to small to store the value.
*/
buint_size_t bigint128_print_dec(const BigUInt128 *a, char *buf, buint_size_t buf_len);

/**
 @brief Exports data into byte array.
 @return Number of written bytes.
*/
buint_size_t biguint128_export(const BigUInt128 *a, char *dest);


/**
 * Division by 1000. Definitely faster than biguint128_div().
 * The decimal printer function HEX->DEC conversion,
 * and without this function it would take a long time.
 * Maybe this function will be promoted to the interface,
 * since div/mod by 1000 is called frequently
 * (e.g., ms->s, mm->m, etc. conversions).
 * @param a Divident.
 * @return Pair of quotient and remainder.
 */
static inline BigUIntPair128 biguint128_div1000(const BigUInt128 *a) {
 // The procedure goes like this:
 // We have to containers (retv.first, retv.second), and at the end
 // these will store the quotient and the remainder, respectively.
 // Initially, retv.first is empty and retv.second stores the whole amount of a.
 // Step-by-step retv.first is increased while retv.second is decreased.
 // Note that 1000 * retv.first + retv.second = *a remains invariant
 // during the whole process.
 // In phase #1 we exploit that
 // a = 1024*b + c = (1000*b) + (24*b+c)
 // and reduce retv.second iteratively until it gets small enough.
 // In phase #2 we just subtract 1000 from the remainder if it is still too high.
 // Well, the while loop is an overkill for this limit (2000).
 static const BigUInt128 x1000={{0x3E8}};
 static const BigUInt128 x2000={{0x7D0}};

 BigUIntPair128 retv= {biguint128_ctor_default(), biguint128_ctor_copy(a)};
 // Phase 1:
 while (biguint128_lt(&x2000, &retv.second)) {
  BigUIntTinyPair128 x= biguint128_div1024(&retv.second);
  biguint128_add_assign(&retv.first, &x.first);
  BigUInt128 d_mul8= biguint128_shl(&x.first, 3);
  BigUInt128 d_mul16= biguint128_shl(&x.first, 4);
  retv.second = biguint128_add(&d_mul8, &d_mul16);
  biguint128_add_tiny(&retv.second, x.second);
 }
 // Phase 2:
 while (!biguint128_lt(&retv.second, &x1000)) {
  biguint128_add_tiny(&retv.first, 1);
  biguint128_sub_tiny(&retv.second, x1000.dat[0]);
 }
 return retv;
}


/**
 * Adapts biguint128_div1000 to signed values.
 */
BigUIntPair128 bigint128_div1000(const BigUInt128 *a);

/**
 * Multiplication by 10 (= 8 + 2).
 */
static inline BigUInt128 biguint128_mul10(const BigUInt128 *a) {
 BigUInt128 a3 = biguint128_shl(a,3);
 BigUInt128 a1 = biguint128_shl(a,1);
 biguint128_add_assign(&a3, &a1);
 return a3;
}

/**
 * Division by 1024. Division by 1000 relies on this function.
 * @param a Divident.
 * @return Pair of quotient and remainder.
 */
static inline BigUIntTinyPair128 biguint128_div1024(const BigUInt128 *a) {
 BigUIntTinyPair128 retv;
 retv.first= biguint128_shr(a, 10);
 retv.second= (a->dat[0]) & (UInt)0x3FF;
 return retv;
}

/**
 * Multiplication by 100 (= 64 + 32 + 4).
 */
static inline BigUInt128 biguint128_mul100(const BigUInt128 *a) {
 BigUInt128 a6 = biguint128_shl(a,6);
 BigUInt128 a5 = biguint128_shl(a,5);
 BigUInt128 a2 = biguint128_shl(a,2);
 biguint128_add_assign(&a6, &a5);
 biguint128_add_assign(&a6, &a2);
 return a6;
}

/**
 * Multiplication by 1000 (= 1024 - 16 - 8).
 */
static inline BigUInt128 biguint128_mul1000(const BigUInt128 *a) {
 BigUInt128 a10 = biguint128_shl(a,10);
 BigUInt128 a4 = biguint128_shl(a,4);
 BigUInt128 a3 = biguint128_shl(a,3);
 biguint128_sub_assign(&a10, &a4);
 biguint128_sub_assign(&a10, &a3);
 return a10;
}


#endif

