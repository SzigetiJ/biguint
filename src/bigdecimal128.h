/*****************************************************************************

    Copyright 2023 SZIGETI János

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
#ifndef _BIGDECIMAL128_H_
#define _BIGDECIMAL128_H_

#include "biguint128.h"

/**
 Basic Storage type of big decimal values.
*/

typedef struct {
 BigUInt128 val;	///< Row value
 UInt prec;		///< Precision (number of digits after the decimal dot)
} BigDecimal128;

/**
 @brief Value initialization to 0 (with 0 precision).
*/
BigDecimal128 bigdecimal128_ctor_default();

/**
 @brief Value initialization from char array with decimal numbers (digits and optional sign, decimal dot).
 @param dec_digits Input character array.
 @param len Length of the input array.
*/
BigDecimal128 bigdecimal128_ctor_cstream(const char *dec_digits, buint_size_t len);


/**
 @brief Value initialization with existing number and new precision.
 If the new precision is lower than the original one,
 then the created number is trucated.
 @param a Existing, original instance.
 @param prec Precision of the created number.
 @return Value of the original instance trunctated/extended to the given precision.
*/
BigDecimal128 bigdecimal128_ctor_prec(const BigDecimal128 *a, UInt prec);

/**
 * @brief Sets the precision of a decimal number.
 * @param dest Writes the output data here.
 * @param a Input value.
 * @param prec New precision of the number.
 * @return Success, based on capacity estimation expoiting 10^3 &lt; 2^10.
 * Thus, for big precision values, at the boundaries, it may return false negative.
 */
buint_bool bigdecimal128_prec_safe(BigDecimal128 *dest, const BigDecimal128 *a, UInt prec);

/**
 * @brief Breaks a decimal number into two: integer and fractional parts.
 * @param a Input value.
 * @return first: integer part, second: fractional part (together with a.prec forms a BigDecimal128 value).
 */
BigUIntPair128 bigdecimal128_trunc(const BigDecimal128 *a);

/**
 @brief Addition.
*/
BigDecimal128 bigdecimal128_add(const BigDecimal128 *a, const BigDecimal128 *b);
buint_bool bigdecimal128_add_safe(BigDecimal128 *dest, const BigDecimal128 *a, const BigDecimal128 *b);

/**
 @brief Subtraction.
*/
BigDecimal128 bigdecimal128_sub(const BigDecimal128 *a, const BigDecimal128 *b);
buint_bool bigdecimal128_sub_safe(BigDecimal128 *dest, const BigDecimal128 *a, const BigDecimal128 *b);

/**
 @brief Multiplication.
*/
BigDecimal128 bigdecimal128_mul(const BigDecimal128 *a, const BigDecimal128 *b);
buint_bool bigdecimal128_mul_safe(BigDecimal128 *dest, const BigDecimal128 *a, const BigDecimal128 *b);

/**
 @brief Fast division algorithm with narrow operating range.
 Internally, it performs a
 q.val = (a.val * 10^(prec + b.prec)) / b.val bigint division.
 This internal division can easily fail if (prec + b.prec) is high.
*/
BigDecimal128 bigdecimal128_div_fast(const BigDecimal128 *a, const BigDecimal128 *b, UInt prec);

/**
 @brief Slower division with wide operating range.
 To get decimal digits, this algorithm iteratively applies
 a.val/b.val and a.val*=10 bigint operations.
 The higher the prec parameter is, the slower the function runs.
 TODO: insert if (remainder==0) break.
*/
BigDecimal128 bigdecimal128_div(const BigDecimal128 *a, const BigDecimal128 *b, UInt prec);
buint_bool bigdecimal128_div_safe(BigDecimal128 *dest, const BigDecimal128 *a, const BigDecimal128 *b, UInt prec);

/**
 @brief 'Less than' relation between signed decimal numbers.
 @return Not zero: a is less than b. Zero: a is not less than b.
*/
buint_bool bigdecimal128_lt(const BigDecimal128 *a, const BigDecimal128 *b);

/**
 @brief Checks equality.
 @return Not zero: a is equal to b. Zero: a is not equal to b.
*/
buint_bool bigdecimal128_eq(const BigDecimal128 *a, const BigDecimal128 *b);

/**
 @brief Export the signed decimal number into character array format.
 The method does not write terminating 0 character.
 However, the method returns where the exported data terminates
 (where to put terminating 0 if the caller wants to treat to character array as a C-string).
 @param a Pointer to the value to export.
 @param buf Target of the export.
 @param buf_len Length of the target buffer.
 @return Length of the written characters. Zero: buf_len is to small to store the value.
*/
buint_size_t bigdecimal128_print(const BigDecimal128 *a, char *buf, buint_size_t buf_len);

// Pass-by-value functions
#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
BigDecimal128 bigdecimal128_ctor_precv(const BigDecimal128 a, UInt prec);
BigDecimal128 bigdecimal128_addv(const BigDecimal128 a, const BigDecimal128 b);
BigDecimal128 bigdecimal128_subv(const BigDecimal128 a, const BigDecimal128 b);
BigDecimal128 bigdecimal128_mulv(const BigDecimal128 a, const BigDecimal128 b);
BigDecimal128 bigdecimal128_divv(const BigDecimal128 a, const BigDecimal128 b, UInt prec);
BigDecimal128 bigdecimal128_absv(const BigDecimal128 a, buint_bool *result_inv);
buint_bool bigdecimal128_ltv(const BigDecimal128 a, const BigDecimal128 b);
buint_bool bigdecimal128_eqv(const BigDecimal128 a, const BigDecimal128 b);
buint_size_t bigdecimal128_printv(const BigDecimal128 a, char *buf, buint_size_t buf_len);
#endif

#endif

