#ifndef TEST_COMMON128_H
#define TEST_COMMON128_H

#include <stdbool.h>
#include "test_common.h"
#include "biguint128.h"

#define BIGUINT_BITS 128
#define HEX_BIGUINTLEN (BIGUINT_BITS / 4)
#define DEC_BIGUINTLEN ((BIGUINT_BITS / 10 + 1) * 3 + 1) // a good approximation, since 2^10 >~ 10^3
#define DEC_BIGINTLEN (DEC_BIGUINTLEN + 1)

BigUInt128 negate_bigint128(const BigUInt128 *src);
bool readhex_biguint128(BigUInt128 *result, const char * const hexstr, size_t hexlen);
bool readhex_cstr_biguint128(BigUInt128 *result, const CStr * const hexstr);
bool readhex_more_cstr_biguint128(BigUInt128 *result, const CStr * const hexstr, size_t num);

#endif /* TEST_COMMON128_H */

