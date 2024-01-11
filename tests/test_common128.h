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

extern BigUInt128 zero;
extern BigUInt128 one;
extern BigUInt128 two;
extern BigUInt128 uintmax;
extern BigUInt128 uintoflow;
extern BigUInt128 max;
extern BigUInt128 maxbutone;

typedef BigUInt128 (*BigUInt128BinaryFun)(const BigUInt128 *a, const BigUInt128 *b);
typedef BigUInt128* (*BigUInt128BinaryAsgnFun)(BigUInt128 *a, const BigUInt128 *b);
typedef BigUInt128 (*BigUInt128BinaryVFun)(const BigUInt128 a, const BigUInt128 b);
typedef union {
 BigUInt128BinaryFun fun2;
 BigUInt128BinaryAsgnFun fun2asg;
 BigUInt128BinaryVFun fun2v;
} BigUInt128BinFun;

void init_testvalues();

bool read_biguint128(BigUInt128 *result, const char* const str, size_t len, Format fmt);
bool read_cstr_biguint128(BigUInt128 *result, const CStr* const a, Format fmt);
bool read_more_cstr_biguint128(BigUInt128 *result_arr, const CStr* const hex_arr, size_t num, Format fmt);

void fprint_funres_buint128_x_bsz_buint128(FILE *out, const char *funname, const BigUInt128 *a, buint_size_t b, const BigUInt128 *expected, const BigUInt128 *actual);
void fprint_funres_buint128_x_bsz_bb(FILE *out, const char *funname, const BigUInt128 *a, buint_size_t b, buint_bool expected, buint_bool actual);
void fprintf_biguint128_genfun_testresult(
 FILE *out, const char *funname,
 int parnum, BigUInt128 *param,
 int resnum, BigUInt128 *expected, BigUInt128 *actual,
 Format fmt);

void fprintf_biguint128_binop_testresult(FILE *out, BigUInt128 *op0, BigUInt128 *op1, BigUInt128 *expected, BigUInt128 *actual, const char *op_str);
void fprintf_biguint128_unop_testresult(FILE *out, const BigUInt128 *op0, const BigUInt128 *expected, const BigUInt128 *actual, const char *op_str);

int test_binop0(const CStr *samples, unsigned int sample_len, unsigned int sample_n, Format fmt, unsigned int *param_idx, BigUInt128BinaryFun fun, const char *op_str);
int test_binop0_assign(const CStr *samples, unsigned int sample_len, unsigned int sample_n, Format fmt, unsigned int *param_idx, BigUInt128BinaryAsgnFun fun, const char *op_str);
int test_binop0v(const CStr *samples, unsigned int sample_len, unsigned int sample_n, Format fmt, unsigned int *param_idx, BigUInt128BinaryVFun fun, const char *op_str);

#endif /* TEST_COMMON128_H */

