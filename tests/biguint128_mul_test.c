#include "biguint128.h"
#include "test_common.h"
#include "test_common128.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

const unsigned int std_idx[] = {0, 1, 2, 3, 4};

#define HEXSAMPLES_W 4U
// [0]: a, [1]: b, [2]: a*b, [3]: (a*b)%a
const CStr hex_samples[][HEXSAMPLES_W] = {
 { STR("0"), STR("1"), STR("0"), STR("0")},
 { STR("1"), STR("1"), STR("1"), STR("0")},
 { STR("1"), STR("FFFFFFFF"), STR("FFFFFFFF"), STR("0")},
 { STR("2"), STR("FFFFFFFF"), STR("1FFFFFFFE"), STR("0")},
 { STR("FFFFFFFF"), STR("FFFFFFFF"), STR("FFFFFFFE00000001"), STR("0")},
 { STR("100000001"), STR("200000001"), STR("20000000300000001"), STR("0")},
 { STR("400000005"), STR("600000003"), STR("180000002A0000000F"), STR("0")}
};
const unsigned int hex_sample_len = ARRAYSIZE(hex_samples);

#define SDECSAMPLES_W 5U
const CStr bigint_dec_samples[][SDECSAMPLES_W] = {
 { STR("-10"), STR("10"), STR("-100"), STR("-105"), STR("-5")},
 { STR("10"), STR("-10"), STR("-100"), STR("-103"), STR("-3")},
 { STR("-10"), STR("-10"), STR("100"), STR("102"), STR("2")},
};
const unsigned int bigint_dec_sample_len = ARRAYSIZE(bigint_dec_samples);

const CStr div10_sample[] = {
 STR("2"),
 STR("10"),
 STR("22"),
 STR("100"),
 STR("303"),
 STR("4205"),
 STR("46901"),
 STR("653436"),
 STR("7884235"),
 STR("83425465"),
 STR("1234567890"),
 STR("12345678901234567890"),
 STR("123456789012345678901234567890")
};
const unsigned int div10_sample_len = ARRAYSIZE(div10_sample);

bool test_mul1() {
 bool fail = false;
 ArgType argt[] = {BUINT_XROREF, BUINT_XROREF, BUINT_XVAL};
 GenArgU params[2];
 params[0].x = max;
 GenArgU prod;
 GenArgU exp;
 char exp_str[2 * HEX_BIGUINTLEN + 1]; // 'FFFF...FFF000...0000' - we always read the exp value from this string in a 128 bit window.
 memset(exp_str, 'F', HEX_BIGUINTLEN);
 memset(exp_str + HEX_BIGUINTLEN, '0', HEX_BIGUINTLEN);
 exp_str[2 * HEX_BIGUINTLEN] = 0; // not necessary, but let it be a normal cstr.

 for (buint_size_t i = 0; i < HEX_BIGUINTLEN; ++i) {

  params[1].x = biguint128_shl(&one, 4 * i);
  prod.x = biguint128_mul(&params[0].x, &params[1].x);
  exp.x = biguint128_ctor_hexcstream(exp_str+i, HEX_BIGUINTLEN);

  if (!biguint128_eq(&exp.x, &prod.x)) {
   fprintf_biguint128_genfun0_testresult(stderr, "mul", params, &prod, &exp, 2, argt, 1, argt + 2, FMT_HEX);
   fail = true;
  }
 }
 return !fail;
}

bool test_mul2() {
 bool fail = false;
 BigUInt128 max2 = biguint128_mul(&max, &max);

 if (!biguint128_eq(&max2, &one)) {
  char max_str[HEX_BIGUINTLEN + 1];
  max_str[biguint128_print_hex(&max, max_str, HEX_BIGUINTLEN)] = 0;
  char max2_str[HEX_BIGUINTLEN + 1];
  max2_str[biguint128_print_hex(&max2, max2_str, HEX_BIGUINTLEN)] = 0;
  fprintf(stderr, "[%s * %s] -- expected: [%s], actual [%s]\n", max_str, max_str, "1", max2_str);
  fail = true;
 }
 return !fail;
}

bool test_dmul0() {
 bool fail = false;

 GenArgU params[][2] = {
  {{.x=two}, {.x=max}},
  {{.x=max}, {.x=max}}
 };
 GenArgU exps[][2] = {
  {{.x=one}, {.x=maxbutone}},
  {{.x=maxbutone}, {.x=one}}
 };
 ArgType atypes[] = {BUINT_XVAL, BUINT_XVAL};

 for (unsigned int i=0; i<ARRAYSIZE(params); ++i) {
  BigUIntPair128 result = biguint128_dmul(&params[i][0].x, &params[i][1].x);
  if (!biguint128_eq(&result.first, &exps[i][1].x)
    || !biguint128_eq(&result.second, &exps[i][0].x)) {
   GenArgU acts[] = {{.x=result.second}, {.x=result.first}};
   fprintf_biguint128_genfun0_testresult(stderr, "dmul", &params[i][0], acts, &exps[i][0], 2, atypes, 2, atypes, FMT_HEX);
   fail |= true;
  }
 }

 return !fail;
}

bool test_div10_a() {
 BigUInt128 divisor = biguint128_value_of_uint(10U);
 bool fail = false;
 for (unsigned int i = 0; i < div10_sample_len; ++i) {
  BigUInt128 val;
  if (!read_more_cstr_biguint128(&val, &div10_sample[i], std_idx, 1, FMT_SDEC)) {
   continue;
  }
  BigUIntPair128 expected = biguint128_div(&val, &divisor);

  BigUInt128 tmp1 = biguint128_mul100(&val);
  BigUInt128 actual1 = biguint128_div1000(&tmp1).first;

  BigUInt128 tmp2 = biguint128_mul3(&val);
  BigUInt128 actual2 = biguint128_div30(&tmp2).first;

  BigUIntTinyPair128 actual3 = biguint128_div10(&val);

  if (!biguint128_eq(&actual1, &expected.first)) {
   fprintf(stderr, "failed div1000(mul100(x)) test at test input #%u\n", i);
   fail = true;
  }

  if (!biguint128_eq(&actual2, &expected.first)) {
   fprintf(stderr, "failed div30(mul3(x)) test at test input #%u\n", i);
   fail = true;
  }

  if (!biguint128_eq(&actual3.first, &expected.first)) {
   fprintf(stderr, "failed div10(x) test at test input #%u\n", i);
   fail = true;
  }
  if (actual3.second != expected.second.dat[0]) {
   fprintf(stderr, "failed div10(x) remainder test at test input #%u\n", i);
   fail = true;
  }

 }

 return !fail;
}

bool test_div3_a() {
 BigUInt128 divisor = biguint128_value_of_uint(3U);
 bool fail = false;
 for (unsigned int i = 0; i < div10_sample_len; ++i) {
  BigUInt128 val;
  if (!read_more_cstr_biguint128(&val, &div10_sample[i], std_idx, 1, FMT_SDEC)) {
   continue;
  }
  BigUIntPair128 expected = biguint128_div(&val, &divisor);

  BigUIntTinyPair128 actual = biguint128_div3(&val);

  if (!biguint128_eq(&actual.first, &expected.first)) {
   fprintf(stderr, "failed div3(x) test at test input #%u\n", i);
   fail = true;
  }
  if (actual.second != expected.second.dat[0]) {
   fprintf(stderr, "failed div3(x) remainder test at test input #%u\n", i);
   fail = true;
  }

 }

 return !fail;
}

// filters
static buint_bool check_divisor_(const GenArgU *aa, unsigned int n) {
 return !biguint128_eqz(&aa[1].x);
}

static buint_bool check_altb_(const GenArgU *aa, unsigned int n) {
 return biguint128_lt(&aa[0].x, &aa[1].x);
}

int main() {

 init_testvalues();

 {
  unsigned int mul_params[] = {0, 1, 2};
  unsigned int dmul_params[] = {0, 1, 2, 3};
  unsigned int div_params1[] = {2, 1, 0, 3};
  unsigned int div_params2[] = {2, 0, 1, 3};
  unsigned int div_params3[] = {0, 2, 3, 0};

  assert(test_genfun(&hex_samples[0][0], HEXSAMPLES_W, hex_sample_len, FMT_HEX, mul_params, XBFUN0(biguint128_mul), "mul", NULL) == 0);
  assert(test_genfun(&hex_samples[0][0], HEXSAMPLES_W, hex_sample_len, FMT_HEX, dmul_params, XBFUN3(biguint128_dmul), "dmul", NULL) == 0);
  assert(test_genfun(&hex_samples[0][0], HEXSAMPLES_W, hex_sample_len, FMT_HEX, div_params1, XBFUN3(biguint128_div), "div", check_divisor_) == 0);
  assert(test_genfun(&hex_samples[0][0], HEXSAMPLES_W, hex_sample_len, FMT_HEX, div_params2, XBFUN3(biguint128_div), "div", check_divisor_) == 0);
  assert(test_genfun(&hex_samples[0][0], HEXSAMPLES_W, hex_sample_len, FMT_HEX, div_params3, XBFUN3(biguint128_div), "div", check_altb_) == 0);

#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
  assert(test_genfun(&hex_samples[0][0], HEXSAMPLES_W, hex_sample_len, FMT_HEX, mul_params, XBFUN0V(biguint128_mulv), "mulv", NULL) == 0);
  assert(test_genfun(&hex_samples[0][0], HEXSAMPLES_W, hex_sample_len, FMT_HEX, div_params1, XBFUN3V(biguint128_divv), "divv", check_divisor_) == 0);
#endif
 }

 assert(test_mul1());
 assert(test_mul2());

 assert(test_dmul0());

 {
  unsigned int mul_params[] = {0, 1, 2};
  unsigned int div_params[] = {3, 1, 0, 4};
  assert(test_genfun(&bigint_dec_samples[0][0], SDECSAMPLES_W, bigint_dec_sample_len, FMT_SDEC, mul_params, XBFUN0(biguint128_mul), "imul", NULL) == 0);
  assert(test_genfun(&bigint_dec_samples[0][0], SDECSAMPLES_W, bigint_dec_sample_len, FMT_SDEC, div_params, XBFUN3(bigint128_div), "idiv", check_divisor_) == 0);

#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
  assert(test_genfun(&bigint_dec_samples[0][0], SDECSAMPLES_W, bigint_dec_sample_len, FMT_SDEC, div_params, XBFUN3V(bigint128_divv), "idivv", check_divisor_) == 0);
#endif
 }

 assert(test_div10_a());
 assert(test_div3_a());
 return 0;
}
