#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "bigdecimal128.h"
#include "test_common.h"

#define BUFLEN_MAX 10

#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
// INTERNAL TYPES
typedef BigDecimal128 (*BDFun)(const BigDecimal128 *a, const BigDecimal128 *b);
typedef BigDecimal128 (*BDFunV)(const BigDecimal128 a, const BigDecimal128 b);
typedef buint_bool (*BDRel)(const BigDecimal128 *a, const BigDecimal128 *b);
typedef buint_bool (*BDRelV)(const BigDecimal128 a, const BigDecimal128 b);

// TEST DATA
const CStr samples[][2] = {
 {STR("21"), STR("0.21")},
 {STR("21"), STR("2.1")},
 {STR("21"), STR("21.0")},
 {STR("21"), STR("21.00")},
 {STR("21"), STR("210.0")},
 {STR("0.2"), STR("0.7")},
 {STR("10"), STR("30")},
 {STR("-1.7"), STR("1.7")},
 {STR("4.0"), STR("-0.25")},
 {STR("-10"), STR("0.001")},
 {STR("0.000"), STR("10.00")}
};

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

static bool compare_result_(FILE *out, const char *fndesc, const BigDecimal128 *expected, const BigDecimal128 *actual, unsigned int sidx, unsigned int fidx) {
 bool pass = true;
    if (!biguint128_eq(&expected->val, &actual->val)) {
     fprintf(out, "pass-by-value value error at sample #%u, %s, prec %u\n", sidx, fndesc, fidx);
     pass = false;
    }
    if (expected->prec != actual->prec) {
     fprintf(out, "pass-by-value precision error at sample #%u, %s, prec %u\n", sidx, fndesc, fidx);
     pass = false;
    }
 return pass;
}

static bool compare_all() {
 bool pass = true;

 for (unsigned int i = 0; i < ARRAYSIZE(samples); ++i) {
  BigDecimal128 a = bigdecimal128_ctor_cstream(samples[i][0].str, samples[i][0].len);
  BigDecimal128 b = bigdecimal128_ctor_cstream(samples[i][1].str, samples[i][1].len);

  for (unsigned int pi = 0; pi < 5; ++pi) {
   BigDecimal128 expected = bigdecimal128_ctor_prec(&a, pi);
   BigDecimal128 actual = bigdecimal128_ctor_precv(a, pi);
   pass &= compare_result_(stderr, "set precision", &expected, &actual, i, pi);
  }
  for (unsigned int j = 0; j < ARRAYSIZE(xfun); ++j) {
   BigDecimal128 expected = xfun[j](&a, &b);
   BigDecimal128 actual = xfunv[j](a, b);
   pass &= compare_result_(stderr, "std function", &expected, &actual, i, j);
  }
  if (!biguint128_eqz(&b.val)) {
   for (unsigned int pi = 0; pi < 5; ++pi) {
    BigDecimal128 expected = bigdecimal128_div(&a, &b, pi);
    BigDecimal128 actual = bigdecimal128_divv(a, b, pi);
    pass &= compare_result_(stderr, "std div function", &expected, &actual, i, pi);
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
  for (unsigned int blen = 0; blen < BUFLEN_MAX; ++blen) {
   char exp_buf[BUFLEN_MAX];
   char act_buf[BUFLEN_MAX];
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

