#ifndef TEST_COMMON128_H
#define TEST_COMMON128_H

#include <stdbool.h>
#include <stdio.h>
#include "test_common.h"
#include "biguint128.h"

#define BIGUINT_BITS 128
#define HEX_BIGUINTLEN (BIGUINT_BITS / 4)
// if a decimal number is longer than this limit, it is not sure, that it can be represented by BigUInt
#define DEC_BIGUINTLEN_LO (BIGUINT_BITS * 3 / 10)            // a good lower bound estimation, since 2^10 >~ 10^3
// if we want to print the number in decimal format, it requires at most that many characters.
#define DEC_BIGUINTLEN_HI ((BIGUINT_BITS * 302 ) / 1000 + 1) // log(2)/log(10) ~= 0.30103 

#define DEC_BIGINTLEN_HI (DEC_BIGUINTLEN_HI + 1)
#define BIGDECLEN_HI (DEC_BIGINTLEN_HI + 2)

typedef enum {FMT_HEX=0, FMT_DEC, FMT_SDEC} Format;

extern BigUInt128 zero;
extern BigUInt128 one;
extern BigUInt128 two;
extern BigUInt128 uintmax;
extern BigUInt128 uintoflow;
extern BigUInt128 max;
extern BigUInt128 maxbutone;

void init_testvalues();
bool readhex_biguint128(BigUInt128 *result, const char * const hexstr, size_t hexlen);
bool readhex_cstr_biguint128(BigUInt128 *result, const CStr * const hexstr);
bool readhex_more_cstr_biguint128(BigUInt128 *result_arr, const CStr * const hex_arr, size_t num);
bool readdec_more_cstr_bigint128(BigUInt128 *result_arr, const CStr * const dec_arr, size_t num);

void fprint_funres_buint128_x_bsz_buint128(FILE *out, const char *funname, const BigUInt128 *a, buint_size_t b, const BigUInt128 *expected, const BigUInt128 *actual);
void fprint_funres_buint128_x_bsz_bb(FILE *out, const char *funname, const BigUInt128 *a, buint_size_t b, buint_bool expected, buint_bool actual);
void fprintf_biguint128_genfun_testresult(
 FILE *out, const char *funname,
 int parnum, BigUInt128 *param,
 int resnum, BigUInt128 *expected, BigUInt128 *actual,
 Format fmt);

#endif /* TEST_COMMON128_H */

