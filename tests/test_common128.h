#ifndef TEST_COMMON128_H
#define TEST_COMMON128_H

#include <stdbool.h>
#include <stdio.h>
#include "test_common.h"
#include "biguint128.h"

#define BIGUINT_BITS 128
#define HEX_BIGUINTLEN (BIGUINT_BITS / 4)
#define DEC_BIGUINTLEN ((BIGUINT_BITS / 10 + 1) * 3 + 1) // a good approximation, since 2^10 >~ 10^3
#define DEC_BIGINTLEN (DEC_BIGUINTLEN + 1)

extern BigUInt128 zero;
extern BigUInt128 one;
extern BigUInt128 two;
extern BigUInt128 uintmax;
extern BigUInt128 uintoflow;
extern BigUInt128 max;
extern BigUInt128 maxbutone;

void init_testvalues();
BigUInt128 negate_bigint128(const BigUInt128 *src);
bool readhex_biguint128(BigUInt128 *result, const char * const hexstr, size_t hexlen);
bool readhex_cstr_biguint128(BigUInt128 *result, const CStr * const hexstr);
bool readhex_more_cstr_biguint128(BigUInt128 *result_arr, const CStr * const hex_arr, size_t num);
bool readdec_more_cstr_bigint128(BigUInt128 *result_arr, const CStr * const dec_arr, size_t num);

void fprint_funres_buint128_x_bsz_buint128(FILE *out, const char *funname, const BigUInt128 *a, buint_size_t b, const BigUInt128 *expected, const BigUInt128 *actual);
void fprint_funres_buint128_x_bsz_bb(FILE *out, const char *funname, const BigUInt128 *a, buint_size_t b, buint_bool expected, buint_bool actual);

#endif /* TEST_COMMON128_H */

