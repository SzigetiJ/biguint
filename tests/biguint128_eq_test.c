#include "biguint128.h"
#include "test_common.h"
#include "test_common128.h"
#include <assert.h>

#define SAMPLEX_W 5U
// a, b, lt, ilt, eq
CStr samplex[][SAMPLEX_W]= {
 {STR("0"), STR("0"), STR("0"), STR("0"), STR("1")},
 {STR("1"), STR("0"), STR("0"), STR("0"), STR("0")},
 {STR("-1"), STR("0"), STR("0"), STR("1"), STR("0")},
 {STR("0"), STR("-1"), STR("1"), STR("0"), STR("0")},
 {STR("-1"), STR("-2"), STR("0"), STR("0"), STR("0")},
 {STR("-2"), STR("-1"), STR("1"), STR("1"), STR("0")},
 {STR("1"), STR("-1"), STR("1"), STR("0"), STR("0")},
 {STR("-1"), STR("1"), STR("0"), STR("1"), STR("0")},
 {STR("-1"), STR("-1"), STR("0"), STR("0"), STR("1")},
 {STR("1"), STR("1"), STR("0"), STR("0"), STR("1")}
};

#define SAMPLEY_W 3U
// a, b, lt, ilt, eq
CStr sampley[][SAMPLEY_W]= {
 {STR("0"), STR("0"), STR("0")},
 {STR("1"), STR("1"), STR("0")},
 {STR("-1"), STR("1"), STR("1")},
 {STR("-0"), STR("0"), STR("0")}
};

#define SAMPLEZ_W 3U
// a, ltz, eqz
CStr samplez[][SAMPLEZ_W]={
 {STR("4294967296"), STR("0"), STR("0")},
 {STR("1234567890"), STR("0"), STR("0")},
 {STR("12345678901234567890"), STR("0"), STR("0")},
 {STR("123456789012345678901234567890"), STR("0"), STR("0")},
 {STR("0"), STR("0"), STR("1")},
 {STR("-0"), STR("0"), STR("1")},
 {STR("-4294967296"), STR("1"), STR("0")},
 {STR("-1234567890"), STR("1"), STR("0")},
 {STR("-12345678901234567890"), STR("1"), STR("0")},
 {STR("-123456789012345678901234567890"), STR("1"), STR("0")}
};
const unsigned int samplez_len = ARRAYSIZE(samplez);

bool test_abs() {
 bool pass = true;
 for (unsigned int i=0; i<ARRAYSIZE(sampley); ++i) {
  unsigned int idx[]={0,1,2};
  BigUInt128 ti[SAMPLEY_W];
  read_more_cstr_biguint128(ti, &sampley[i][0], idx, SAMPLEY_W, FMT_SDEC);
  buint_bool exp_inv = (ti[2].dat[0]==1);
  buint_bool res_inv0;
  BigUInt128 arg1 = ti[0];
  buint_bool res_inv1;
  BigUInt128 arg2 = ti[0];

  BigUInt128 res0 = bigint128_abs(&ti[0], &res_inv0);
  BigUInt128 *res1 = bigint128_abs_assign(&arg1, &res_inv1);
  BigUInt128 *res2 = bigint128_abs_assign(&arg2, NULL);

  if (!biguint128_eq(&res0, &ti[1])) {
   fprintf(stderr, "abs(x) result mismatch\n");
   pass = false;
  }
  if (!!exp_inv != !!res_inv0) {
   fprintf(stderr, "abs(x) result_inv mismatch\n");
   pass = false;
  }

  if (!biguint128_eq(res1, &ti[1]) || res1 != &arg1) {
   fprintf(stderr, "abs_assign(x) result mismatch\n");
   pass = false;
  }
  if (!!exp_inv != !!res_inv1) {
   fprintf(stderr, "abs_assign(x) result_inv mismatch\n");
   pass = false;
  }

  if (!biguint128_eq(res2, &ti[1]) || res2 != &arg2) {
   fprintf(stderr, "abs_assign(x) result mismatch\n");
   pass = false;
  }
 }
 return pass;
}

int main() {
 unsigned int lt_params[] = {0, 1, 2};
 unsigned int ilt_params[] = {0, 1, 3};
 unsigned int eq_params[] = {0, 1, 4};

 assert(test_genfun(&samplex[0][0], SAMPLEX_W, ARRAYSIZE(samplex), FMT_SDEC, lt_params, XBREL0(biguint128_lt), "lt", NULL) == 0);
 assert(test_genfun(&samplex[0][0], SAMPLEX_W, ARRAYSIZE(samplex), FMT_SDEC, ilt_params, XBREL0(bigint128_lt), "ilt", NULL) == 0);
 assert(test_genfun(&samplex[0][0], SAMPLEX_W, ARRAYSIZE(samplex), FMT_SDEC, eq_params, XBREL0(biguint128_eq), "eq", NULL) == 0);


 unsigned int ltz_params[] = {0, 1};
 unsigned int eqz_params[] = {0, 2};

 assert(test_genfun(&samplez[0][0], SAMPLEZ_W, samplez_len, FMT_SDEC, ltz_params, XUREL0(bigint128_ltz), "ltz", NULL) == 0);
 assert(test_genfun(&samplez[0][0], SAMPLEZ_W, samplez_len, FMT_SDEC, eqz_params, XUREL0(biguint128_eqz), "eqz", NULL) == 0);

#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
 assert(test_genfun(&samplex[0][0], SAMPLEX_W, ARRAYSIZE(samplex), FMT_SDEC, lt_params, XBREL0V(biguint128_ltv), "ltv", NULL) == 0);
 assert(test_genfun(&samplex[0][0], SAMPLEX_W, ARRAYSIZE(samplex), FMT_SDEC, ilt_params, XBREL0V(bigint128_ltv), "iltv", NULL) == 0);
 assert(test_genfun(&samplex[0][0], SAMPLEX_W, ARRAYSIZE(samplex), FMT_SDEC, eq_params, XBREL0V(biguint128_eqv), "eqv", NULL) == 0);

 assert(test_genfun(&samplez[0][0], SAMPLEZ_W, samplez_len, FMT_SDEC, ltz_params, XUREL0V(bigint128_ltzv), "ltzv", NULL) == 0);
 assert(test_genfun(&samplez[0][0], SAMPLEZ_W, samplez_len, FMT_SDEC, eqz_params, XUREL0V(biguint128_eqzv), "eqzv", NULL) == 0);
#endif

 assert(test_abs());

 return 0;
}
