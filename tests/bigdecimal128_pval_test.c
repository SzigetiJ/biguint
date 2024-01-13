#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "bigdecimal128.h"
#include "test_common.h"

#define BIGUINT_BITS 128
#define BIGDECCAP ((BIGUINT_BITS / 10 + 1) * 3 + 1)
#define BUFLEN (BIGDECCAP + 3)


#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
// INTERNAL TYPES
// TEST DATA
const CStr samples[][2] = {
 {STR("20"),STR("0.5")},
 {STR("0.2"),STR("0.6")},
 {STR("10"),STR("30")},
 {STR("-1.5"),STR("1.5")},
 {STR("2.0"),STR("-0.5")},
 {STR("-10"),STR("0.001")},
 {STR("0.000"),STR("10.00")}
};

typedef BigDecimal128 (*BDFun)(const BigDecimal128 *a, const BigDecimal128 *b);
typedef BigDecimal128 (*BDFunV)(const BigDecimal128 a, const BigDecimal128 b);
typedef buint_bool (*BDRel)(const BigDecimal128 *a, const BigDecimal128 *b);
typedef buint_bool (*BDRelV)(const BigDecimal128 a, const BigDecimal128 b);

const BDFun xfun[]={
 bigdecimal128_add,
 bigdecimal128_sub,
 bigdecimal128_mul
};
const BDFunV xfunv[]={
 bigdecimal128_addv,
 bigdecimal128_subv,
 bigdecimal128_mulv
};

const BDRel xrel[]={
 bigdecimal128_lt,
 bigdecimal128_eq
};
const BDRelV xrelv[]={
 bigdecimal128_ltv,
 bigdecimal128_eqv
};

static bool compare_all() {
 bool pass = true;

 for (unsigned int i = 0; i < ARRAYSIZE(samples); ++i) {
  BigDecimal128 a = bigdecimal128_ctor_cstream(samples[i][0].str, samples[i][0].len);
  BigDecimal128 b = bigdecimal128_ctor_cstream(samples[i][1].str, samples[i][1].len);

  for (unsigned int j = 0; j < ARRAYSIZE(xfun); ++j) {
   for (unsigned int pi = 0; pi < 5; ++pi) {
    BigDecimal128 expected = bigdecimal128_ctor_prec(&a, pi);
    BigDecimal128 actual = bigdecimal128_ctor_precv(a, pi);
    if (!biguint128_eq(&expected.val, &actual.val)) {
     fprintf(stderr, "pass-by-value value error at sample #%u, set precision, prec %u\n", i, pi);
     pass = false;
    }
    if (expected.prec != actual.prec) {
     fprintf(stderr, "pass-by-value precision error at sample #%u, set precision, prec %u\n", i, pi);
     pass = false;
    }
   }
   BigDecimal128 expected = xfun[j](&a, &b);
   BigDecimal128 actual = xfunv[j](a, b);
   if (!biguint128_eq(&expected.val, &actual.val)) {
    fprintf(stderr, "pass-by-value value error at sample #%u, std function #%u\n", i, j);
    pass = false;
   }
   if (expected.prec != actual.prec) {
    fprintf(stderr, "pass-by-value precision error at sample #%u, std function #%u\n", i, j);
    pass = false;
   }
  }
  if (!biguint128_eqz(&b.val)) {
   for (unsigned int pi = 0; pi < 5; ++pi) {
    BigDecimal128 expected = bigdecimal128_div(&a, &b, pi);
    BigDecimal128 actual = bigdecimal128_divv(a, b, pi);
    if (!biguint128_eq(&expected.val, &actual.val)) {
     fprintf(stderr, "pass-by-value value error at sample #%u, std function div, prec %u\n", i, pi);
     pass = false;
    }
    if (expected.prec != actual.prec) {
     fprintf(stderr, "pass-by-value precision error at sample #%u, std function div, prec %u\n", i, pi);
     pass = false;
    }
   }
  }
  for (unsigned int j = 0; j < ARRAYSIZE(xrel); ++j) {
   buint_bool expected = xrel[j](&a, &b);
   buint_bool actual = xrelv[j](a, b);
   if (!!expected != !!actual) {
    fprintf(stderr, "pass-by-value result error at sample #%u, std relation #%u\n", i, j);
    pass = false;
   }
  }
  for (unsigned int blen = 0; blen < 8; ++blen) {
   char exp_buf[9];
   char act_buf[9];
   buint_size_t expected = bigdecimal128_print(&a, exp_buf, blen);
   buint_size_t actual = bigdecimal128_printv(a, act_buf, blen);
   if (expected != actual) {
    fprintf(stderr, "pass-by-value result error at sample #%u, print, buflen: %u\n", i, blen);
    pass = false;
   }
   if (strncmp(exp_buf, act_buf, expected)) {
    fprintf(stderr, "pass-by-value output error at sample #%u, print, buflen: %u\n", i, blen);
    pass = false;
   }
  }
 }
 return pass;
}
#endif

int main() {

#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
 assert(compare_all());
#endif
 return 0;
}

