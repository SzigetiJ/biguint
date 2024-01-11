#include "biguint128.h"
#include "test_common.h"
#include "test_common128.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

const unsigned int std_idx[] = {0, 1, 2, 3, 4};

// [0]: a, [1]: b, [2]: a*b, [3]: (a*b)%a
const CStr hex_samples[][4] = {
 { STR("0"), STR("1"), STR("0"), STR("0")},
 { STR("1"), STR("1"), STR("1"), STR("0")},
 { STR("1"), STR("FFFFFFFF"), STR("FFFFFFFF"), STR("0")},
 { STR("2"), STR("FFFFFFFF"), STR("1FFFFFFFE"), STR("0")},
 { STR("FFFFFFFF"), STR("FFFFFFFF"), STR("FFFFFFFE00000001"), STR("0")},
 { STR("100000001"), STR("200000001"), STR("20000000300000001"), STR("0")},
 { STR("400000005"), STR("600000003"), STR("180000002A0000000F"), STR("0")}
};
int hex_sample_len = sizeof (hex_samples) / (sizeof (hex_samples[0]));

const CStr bigint_dec_samples[][5] = {
 { STR("-10"), STR("10"), STR("-100"), STR("-105"), STR("-5")},
 { STR("10"), STR("-10"), STR("-100"), STR("-103"), STR("-3")},
 { STR("-10"), STR("-10"), STR("100"), STR("102"), STR("2")},
};
int bigint_dec_sample_len = sizeof (bigint_dec_samples) / (sizeof (bigint_dec_samples[0]));

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
int div10_sample_len = sizeof (div10_sample) / (sizeof (div10_sample[0]));

bool test_mul1() {
 bool fail = false;
 char max_str[HEX_BIGUINTLEN + 1];
 max_str[biguint128_print_hex(&max, max_str, sizeof (max_str) / sizeof (max_str[0]) - 1)] = 0;

 for (buint_size_t i = 0; i < HEX_BIGUINTLEN; ++i) {
  char buffer[HEX_BIGUINTLEN + 1];
  char expected[HEX_BIGUINTLEN + 1];

  BigUInt128 mult = biguint128_shl(&one, 4 * i);
  BigUInt128 prod = biguint128_mul(&max, &mult);

  for (buint_size_t digit = 0; digit < HEX_BIGUINTLEN; ++digit) {
   expected[digit] = digit + i < HEX_BIGUINTLEN ? 'F' : '0';
  }
  expected[HEX_BIGUINTLEN] = 0;

  buffer[biguint128_print_hex(&prod, buffer, HEX_BIGUINTLEN)] = 0;

  if (strcmp(expected, buffer) != 0) {
   char mult_str[HEX_BIGUINTLEN + 1];
   mult_str[biguint128_print_hex(&mult, mult_str, HEX_BIGUINTLEN)] = 0;
   fprintf(stderr, "[%s * %s] -- expected: [%s], actual [%s]\n", max_str, mult_str, expected, buffer);
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
 for (int i = 0; i < div10_sample_len; ++i) {
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
   fprintf(stderr, "failed div1000(mul100(x)) test at test input #%d\n", i);
   fail = true;
  }

  if (!biguint128_eq(&actual2, &expected.first)) {
   fprintf(stderr, "failed div30(mul3(x)) test at test input #%d\n", i);
   fail = true;
  }

  if (!biguint128_eq(&actual3.first, &expected.first)) {
   fprintf(stderr, "failed div10(x) test at test input #%d\n", i);
   fail = true;
  }
  if (actual3.second != expected.second.dat[0]) {
   fprintf(stderr, "failed div10(x) remainder test at test input #%d\n", i);
   fail = true;
  }

 }

 return !fail;
}

bool test_div3_a() {
 BigUInt128 divisor = biguint128_value_of_uint(3U);
 bool fail = false;
 for (int i = 0; i < div10_sample_len; ++i) {
  BigUInt128 val;
  if (!read_more_cstr_biguint128(&val, &div10_sample[i], std_idx, 1, FMT_SDEC)) {
   continue;
  }
  BigUIntPair128 expected = biguint128_div(&val, &divisor);

  BigUIntTinyPair128 actual = biguint128_div3(&val);

  if (!biguint128_eq(&actual.first, &expected.first)) {
   fprintf(stderr, "failed div3(x) test at test input #%d\n", i);
   fail = true;
  }
  if (actual.second != expected.second.dat[0]) {
   fprintf(stderr, "failed div3(x) remainder test at test input #%d\n", i);
   fail = true;
  }

 }

 return !fail;
}

static buint_bool check_divisor_(const GenArgU *aa, unsigned int n) {
 return !biguint128_eqz(&aa[1].x);
}

static buint_bool check_altb_(const GenArgU *aa, unsigned int n) {
 return biguint128_lt(&aa[0].x, &aa[1].x);
}

// wrapper functions
static BigUInt128 div_(const BigUInt128 *a, const BigUInt128 *b) {
 return biguint128_div(a, b).first;
}

static BigUInt128 mod_(const BigUInt128 *a, const BigUInt128 *b) {
 return biguint128_div(a, b).second;
}

static BigUInt128 idiv_(const BigUInt128 *a, const BigUInt128 *b) {
 return bigint128_div(a, b).first;
}

static BigUInt128 imod_(const BigUInt128 *a, const BigUInt128 *b) {
 return bigint128_div(a, b).second;
}

static BigUInt128 dmul_hi_(const BigUInt128 *a, const BigUInt128 *b) {
 return biguint128_dmul(a, b).second;
}

static BigUInt128 dmul_lo_(const BigUInt128 *a, const BigUInt128 *b) {
 return biguint128_dmul(a, b).first;
}

#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
static BigUInt128 divv_(const BigUInt128 a, const BigUInt128 b) {
 return biguint128_divv(a, b).first;
}

static BigUInt128 modv_(const BigUInt128 a, const BigUInt128 b) {
 return biguint128_divv(a, b).second;
}

static BigUInt128 idivv_(const BigUInt128 a, const BigUInt128 b) {
 return bigint128_divv(a, b).first;
}

static BigUInt128 imodv_(const BigUInt128 a, const BigUInt128 b) {
 return bigint128_divv(a, b).second;
}
#endif

int main(int argc, char **argv) {

 init_testvalues();

 {
  unsigned int mul_params[] = {0, 1, 2};
  unsigned int dmul_hi_params[] = {0, 1, 3};
  unsigned int div_params1[] = {2, 1, 0};
  unsigned int mod_params1[] = {2, 1, 3};
  unsigned int div_params2[] = {2, 0, 1};
  unsigned int mod_params2[] = {2, 0, 3};
  unsigned int div_params3[] = {0, 2, 3};
  unsigned int mod_params3[] = {0, 2, 0};

  assert(test_genfun(&hex_samples[0][0], 4, hex_sample_len, FMT_HEX, mul_params, XBFUN0(biguint128_mul), "mul", NULL) == 0);
  assert(test_genfun(&hex_samples[0][0], 4, hex_sample_len, FMT_HEX, dmul_hi_params, XBFUN0(dmul_hi_), "dmul.hi", NULL) == 0);
  assert(test_genfun(&hex_samples[0][0], 4, hex_sample_len, FMT_HEX, mul_params, XBFUN0(dmul_lo_), "dmul.lo", NULL) == 0);
  assert(test_genfun(&hex_samples[0][0], 4, hex_sample_len, FMT_HEX, div_params1, XBFUN0(div_), "div", check_divisor_) == 0);
  assert(test_genfun(&hex_samples[0][0], 4, hex_sample_len, FMT_HEX, mod_params1, XBFUN0(mod_), "mod", check_divisor_) == 0);
  assert(test_genfun(&hex_samples[0][0], 4, hex_sample_len, FMT_HEX, div_params2, XBFUN0(div_), "div", check_divisor_) == 0);
  assert(test_genfun(&hex_samples[0][0], 4, hex_sample_len, FMT_HEX, mod_params2, XBFUN0(mod_), "mod", check_divisor_) == 0);
  assert(test_genfun(&hex_samples[0][0], 4, hex_sample_len, FMT_HEX, div_params3, XBFUN0(div_), "div", check_altb_) == 0);
  assert(test_genfun(&hex_samples[0][0], 4, hex_sample_len, FMT_HEX, mod_params3, XBFUN0(mod_), "mod", check_altb_) == 0);

#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
  assert(test_genfun(&hex_samples[0][0], 4, hex_sample_len, FMT_HEX, mul_params, XBFUN0V(biguint128_mulv), "mulv", NULL) == 0);
  assert(test_genfun(&hex_samples[0][0], 4, hex_sample_len, FMT_HEX, div_params1, XBFUN0V(divv_), "divv", check_divisor_) == 0);
  assert(test_genfun(&hex_samples[0][0], 4, hex_sample_len, FMT_HEX, mod_params1, XBFUN0V(modv_), "modv", check_divisor_) == 0);
#endif
 }

 assert(test_mul1());
 assert(test_mul2());

 assert(test_dmul0());

 {
  unsigned int mul_params[] = {0, 1, 2};
  unsigned int div_params[] = {3, 1, 0};
  unsigned int mod_params[] = {3, 1, 4};
  assert(test_genfun(&bigint_dec_samples[0][0], 5, bigint_dec_sample_len, FMT_SDEC, mul_params, XBFUN0(biguint128_mul), "imul", NULL) == 0);
  assert(test_genfun(&bigint_dec_samples[0][0], 5, bigint_dec_sample_len, FMT_SDEC, div_params, XBFUN0(idiv_), "idiv", check_divisor_) == 0);
  assert(test_genfun(&bigint_dec_samples[0][0], 5, bigint_dec_sample_len, FMT_SDEC, mod_params, XBFUN0(imod_), "imod", check_divisor_) == 0);

#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
  assert(test_genfun(&bigint_dec_samples[0][0], 5, bigint_dec_sample_len, FMT_SDEC, div_params, XBFUN0V(idivv_), "idivv", check_divisor_) == 0);
  assert(test_genfun(&bigint_dec_samples[0][0], 5, bigint_dec_sample_len, FMT_SDEC, mod_params, XBFUN0V(imodv_), "imodv", check_divisor_) == 0);
#endif
 }

 assert(test_div10_a());
 assert(test_div3_a());
 return 0;
}
