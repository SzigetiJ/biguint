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

// types of interface function args and returns
typedef union {
 BigUInt128 x;
 BigUInt128 *xptr;
 buint_size_t sz;
 buint_bool b;
 const char *str;
} GenArgU;

typedef enum {
 BUINT_XROREF = 0, // read-only argument
 BUINT_XRWREF, // in case of assignment
 BUINT_XVAL,
 BUINT_TSIZE,
 BUINT_TBOOL,
 BUINT_CWOREF, // write-only char ptr
 BUINT_VOID
} ArgType;

typedef struct {
 ArgType t;
 GenArgU u;
} GenArg;

typedef buint_bool (*ParamRelation)(const GenArgU *a, unsigned int n);

// types of interface functions
typedef BigUInt128 (*BigUInt128UnaryFun)(const BigUInt128 *a);
typedef BigUInt128* (*BigUInt128UnaryAsgFun)(BigUInt128 *a);

typedef BigUInt128 (*BigUInt128BinaryFun)(const BigUInt128 *a, const BigUInt128 *b);
typedef BigUInt128* (*BigUInt128BinaryAsgFun)(BigUInt128 *a, const BigUInt128 *b);
typedef BigUInt128 (*BigUInt128BinaryFun1)(const BigUInt128 *a, const buint_size_t b);
typedef BigUInt128* (*BigUInt128BinaryAsgFun1)(BigUInt128 *a, const buint_size_t b);
typedef buint_size_t (*BigUInt128BinaryFun2)(const BigUInt128 *a, char *b);
typedef BigUIntPair128 (*BigUInt128BinaryFun3)(const BigUInt128 *a, const BigUInt128 *b);

typedef buint_size_t (*BigUInt128TernaryFun)(const BigUInt128 *a, char *b, buint_size_t c);

typedef buint_bool (*BigUInt128UnaryRel)(const BigUInt128 *a);
typedef buint_bool (*BigUInt128BinaryRel)(const BigUInt128 *a, const BigUInt128 *b);

typedef BigUInt128 (*BigUInt128BinaryVFun)(const BigUInt128 a, const BigUInt128 b);
typedef BigUInt128 (*BigUInt128BinaryVFun1)(const BigUInt128 a, const buint_size_t b);
typedef buint_size_t (*BigUInt128BinaryVFun2)(const BigUInt128 a, char *b);
typedef BigUIntPair128 (*BigUInt128BinaryVFun3)(const BigUInt128 a, const BigUInt128 b);

typedef buint_size_t (*BigUInt128TernaryVFun)(const BigUInt128 a, char *b, buint_size_t c);

typedef buint_bool (*BigUInt128UnaryVRel)(const BigUInt128 a);
typedef buint_bool (*BigUInt128BinaryVRel)(const BigUInt128 a, const BigUInt128 b);

typedef buint_bool (*BigUInt128NRelation)(const BigUInt128 *a, unsigned int n);

typedef union {
 BigUInt128UnaryFun ufun;
 BigUInt128UnaryAsgFun ufun_asg;
 BigUInt128BinaryFun bfun;
 BigUInt128BinaryAsgFun bfun_asg;
 BigUInt128BinaryFun1 bfun1;
 BigUInt128BinaryAsgFun1 bfun1_asg;
 BigUInt128BinaryFun3 bfun3;
 BigUInt128TernaryFun tfun;
 BigUInt128UnaryRel urel;
 BigUInt128BinaryRel brel;

 BigUInt128BinaryVFun bfunv;
 BigUInt128BinaryVFun1 bfun1v;
 BigUInt128BinaryVFun3 bfun3v;
 BigUInt128TernaryVFun tfunv;
 BigUInt128UnaryVRel urelv;
 BigUInt128BinaryVRel brelv;
} BigUInt128GenFunU;

typedef enum {
 FUN_UN0 = 0,
 FUN_UN0_ASG,
 FUN_BIN0,
 FUN_BIN0_ASG,
 FUN_BIN1,
 FUN_BIN1_ASG,
 FUN_BIN3,
 FUN_TER0,
 REL_UN0,
 REL_BIN0,

 FUN_BIN0V,
 FUN_BIN1V,
 FUN_BIN3V,
 FUN_TER0V,
 REL_UN0V,
 REL_BIN0V
} BigUInt128FunType;

typedef struct {
 BigUInt128FunType t;
 BigUInt128GenFunU u;
} BigUInt128GenFun;

#define XUFUN0(X) (BigUInt128GenFun){FUN_UN0, {.ufun=X}}
#define XUFUN0A(X) (BigUInt128GenFun){FUN_UN0_ASG, {.ufun_asg=X}}
#define XBFUN0(X) (BigUInt128GenFun){FUN_BIN0, {.bfun=X}}
#define XBFUN0A(X) (BigUInt128GenFun){FUN_BIN0_ASG, {.bfun_asg=X}}
#define XBFUN1(X) (BigUInt128GenFun){FUN_BIN1, {.bfun1=X}}
#define XBFUN1A(X) (BigUInt128GenFun){FUN_BIN1_ASG, {.bfun1_asg=X}}
#define XBFUN3(X) (BigUInt128GenFun){FUN_BIN3, {.bfun3=X}}
#define XTFUN0(X) (BigUInt128GenFun){FUN_TER0, {.tfun=X}}
#define XUREL0(X) (BigUInt128GenFun){REL_UN0, {.urel=X}}
#define XBREL0(X) (BigUInt128GenFun){REL_BIN0, {.brel=X}}
#define XBFUN0V(X) (BigUInt128GenFun){FUN_BIN0V, {.bfunv=X}}
#define XBFUN1V(X) (BigUInt128GenFun){FUN_BIN1V, {.bfun1v=X}}
#define XBFUN3V(X) (BigUInt128GenFun){FUN_BIN3V, {.bfun3v=X}}
#define XTFUN0V(X) (BigUInt128GenFun){FUN_TER0V, {.tfunv=X}}
#define XUREL0V(X) (BigUInt128GenFun){REL_UN0V, {.urelv=X}}
#define XBREL0V(X) (BigUInt128GenFun){REL_BIN0V, {.brelv=X}}

void init_testvalues();

bool read_cstr_biguint128(BigUInt128 *result, const CStr* const a, Format fmt);
bool read_more_cstr_biguint128(BigUInt128 *result_arr, const CStr* const a_arr, const unsigned int *idx_arr, size_t num, Format fmt);

void fprint_funres_buint128_x_bsz_buint128(FILE *out, const char *funname, const BigUInt128 *a, buint_size_t b, const BigUInt128 *expected, const BigUInt128 *actual);
void fprint_funres_buint128_x_bsz_bb(FILE *out, const char *funname, const BigUInt128 *a, buint_size_t b, buint_bool expected, buint_bool actual);
void fprintf_biguint128_genfun0_testresult(FILE *out, const char *funname, GenArgU *values, GenArgU *result, GenArgU *expected, unsigned int arg_n, ArgType *argt, unsigned int res_n, ArgType *rest, Format fmt);

void fprintf_biguint128_binop_testresult(FILE *out, BigUInt128 *op0, BigUInt128 *op1, BigUInt128 *expected, BigUInt128 *actual, const char *op_str);
void fprintf_biguint128_unop_testresult(FILE *out, const BigUInt128 *op0, const BigUInt128 *expected, const BigUInt128 *actual, const char *op_str);

int test_genfun(const CStr *samples, unsigned int sample_width, unsigned int sample_n, Format fmt, const unsigned int *param_idx, BigUInt128GenFun fun, const char *funname, ParamRelation param_valid);

#endif /* TEST_COMMON128_H */

