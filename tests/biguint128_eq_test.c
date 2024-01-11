#include "biguint128.h"
#include "test_common.h"
#include "test_common128.h"
#include <assert.h>

// a, b, lt, ilt, eq
CStr samplex[][5]= {
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

// a, ltz, eqz
CStr samplez[][3]={
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

int main() {
 unsigned int lt_params[] = {0, 1, 2};
 unsigned int ilt_params[] = {0, 1, 3};
 unsigned int eq_params[] = {0, 1, 4};

 assert(test_genfun(&samplex[0][0], 5, ARRAYSIZE(samplex), FMT_SDEC, lt_params, XBREL0(biguint128_lt), "lt", NULL) == 0);
 assert(test_genfun(&samplex[0][0], 5, ARRAYSIZE(samplex), FMT_SDEC, ilt_params, XBREL0(bigint128_lt), "ilt", NULL) == 0);
 assert(test_genfun(&samplex[0][0], 5, ARRAYSIZE(samplex), FMT_SDEC, eq_params, XBREL0(biguint128_eq), "eq", NULL) == 0);


 unsigned int ltz_params[] = {0, 1};
 unsigned int eqz_params[] = {0, 2};

 assert(test_genfun(&samplez[0][0], 3, samplez_len, FMT_SDEC, ltz_params, XUREL0(bigint128_ltz), "ltz", NULL) == 0);
 assert(test_genfun(&samplez[0][0], 3, samplez_len, FMT_SDEC, eqz_params, XUREL0(biguint128_eqz), "eqz", NULL) == 0);

#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
 assert(test_genfun(&samplex[0][0], 5, ARRAYSIZE(samplex), FMT_SDEC, lt_params, XBREL0V(biguint128_ltv), "ltv", NULL) == 0);
 assert(test_genfun(&samplex[0][0], 5, ARRAYSIZE(samplex), FMT_SDEC, ilt_params, XBREL0V(bigint128_ltv), "iltv", NULL) == 0);
 assert(test_genfun(&samplex[0][0], 5, ARRAYSIZE(samplex), FMT_SDEC, eq_params, XBREL0V(biguint128_eqv), "eqv", NULL) == 0);

 assert(test_genfun(&samplez[0][0], 3, samplez_len, FMT_SDEC, ltz_params, XUREL0V(bigint128_ltzv), "ltzv", NULL) == 0);
 assert(test_genfun(&samplez[0][0], 3, samplez_len, FMT_SDEC, eqz_params, XUREL0V(biguint128_eqzv), "eqzv", NULL) == 0);
#endif

 return 0;
}

