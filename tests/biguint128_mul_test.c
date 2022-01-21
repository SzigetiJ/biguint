#include "biguint128.h"
#include "test_common.h"
#include "test_common128.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

const CStr hex_samples[][3] = {
 { STR("0"), STR("1"), STR("0")},
 { STR("1"), STR("1"), STR("1")},
 { STR("1"), STR("FFFFFFFF"), STR("FFFFFFFF")},
 { STR("2"), STR("FFFFFFFF"), STR("1FFFFFFFE")},
 { STR("FFFFFFFF"), STR("FFFFFFFF"), STR("FFFFFFFE00000001")},
 { STR("100000001"), STR("200000001"), STR("20000000300000001")},
 { STR("400000005"), STR("600000003"), STR("180000002A0000000F")}
};
int hex_sample_len = sizeof(hex_samples) / (sizeof(hex_samples[0]));

bool test_mul0() {
 bool fail = false;
 for (int i=0; i<hex_sample_len; ++i) {
  BigUInt128 val[3];
  if (!readhex_more_cstr_biguint128(val, hex_samples[i],3)) {
   continue;
  }

  BigUInt128 prod = biguint128_mul(&val[0], &val[1]);
  BigUInt128 prod_rev = biguint128_mul(&val[1], &val[0]);

  buint_bool result_eq = biguint128_eq(&val[2], &prod);
  buint_bool result_eq_rev = biguint128_eq(&val[2], &prod_rev);

  if (!result_eq) {
   char buffer[4][HEX_BIGUINTLEN + 1];
   buffer[0][biguint128_print_hex(&val[0], buffer[0], HEX_BIGUINTLEN)]=0;
   buffer[1][biguint128_print_hex(&val[1], buffer[1], HEX_BIGUINTLEN)]=0;
   buffer[2][biguint128_print_hex(&val[2], buffer[2], HEX_BIGUINTLEN)]=0;
   buffer[3][biguint128_print_hex(&prod, buffer[3], HEX_BIGUINTLEN)]=0;
   fprintf(stderr, "[%s * %s] -- expected: [%s], actual [%s]\n", buffer[0], buffer[1], buffer[2], buffer[3]);
   fail = true;
  }

  if (!result_eq_rev) {
   char buffer[4][HEX_BIGUINTLEN + 1];
   buffer[0][biguint128_print_hex(&val[0], buffer[0], HEX_BIGUINTLEN)]=0;
   buffer[1][biguint128_print_hex(&val[1], buffer[1], HEX_BIGUINTLEN)]=0;
   buffer[2][biguint128_print_hex(&val[2], buffer[2], HEX_BIGUINTLEN)]=0;
   buffer[3][biguint128_print_hex(&prod_rev, buffer[3], HEX_BIGUINTLEN)]=0;
   fprintf(stderr, "[%s * %s] -- expected: [%s], actual [%s]\n", buffer[0], buffer[1], buffer[2], buffer[3]);
   fail = true;
  }
 }

 return !fail;
}

bool test_mul1() {
 bool fail = false;
 BigUInt128 a0 = biguint128_ctor_default();
 BigUInt128 a1 = biguint128_ctor_unit();
 BigUInt128 max = biguint128_sub(&a0, &a1);
 char max_str[HEX_BIGUINTLEN + 1];
 max_str[biguint128_print_hex(&max, max_str, sizeof(max_str)/sizeof(max_str[0])-1)]=0;

 for (buint_size_t i = 0; i < HEX_BIGUINTLEN; ++i) {
  char buffer[HEX_BIGUINTLEN + 1];
  char expected[HEX_BIGUINTLEN + 1];

  BigUInt128 mult = biguint128_shl(&a1, 4*i);
  BigUInt128 prod = biguint128_mul(&max, &mult);

  for (buint_size_t digit = 0; digit < HEX_BIGUINTLEN; ++digit) {
   expected[digit] = digit + i < HEX_BIGUINTLEN ? 'F' : '0';
  }
  expected[HEX_BIGUINTLEN]=0;

  buffer[biguint128_print_hex(&prod, buffer, HEX_BIGUINTLEN)]=0;

  if (strcmp(expected,buffer)!=0) {
   char mult_str[HEX_BIGUINTLEN + 1];
   mult_str[biguint128_print_hex(&mult, mult_str, HEX_BIGUINTLEN)]=0;
   fprintf(stderr, "[%s * %s] -- expected: [%s], actual [%s]\n", max_str, mult_str, expected, buffer);
   fail = true;
  }
 }
 return !fail;
}

bool test_mul2() {
 bool fail = false;
 BigUInt128 a0 = biguint128_ctor_default();
 BigUInt128 a1 = biguint128_ctor_unit();
 BigUInt128 max = biguint128_sub(&a0, &a1);
 BigUInt128 max2 = biguint128_mul(&max, &max);

 if (!biguint128_eq(&max2, &a1)) {
  char max_str[HEX_BIGUINTLEN + 1];
  max_str[biguint128_print_hex(&max, max_str, HEX_BIGUINTLEN)]=0;
  char max2_str[HEX_BIGUINTLEN + 1];
  max2_str[biguint128_print_hex(&max2, max2_str, HEX_BIGUINTLEN)]=0;
  fprintf(stderr, "[%s * %s] -- expected: [%s], actual [%s]\n", max_str, max_str, "1", max2_str);
  fail = true;
 }
 return !fail;
}

bool test_dmul0() {
 bool fail = false;
 BigUInt128 zero = biguint128_ctor_default();
 BigUInt128 one = biguint128_ctor_unit();
 BigUInt128 max= biguint128_sub(&zero, &one);
 BigUInt128 maxbutone= biguint128_sub(&max, &one);
 BigUInt128 two= biguint128_add(&one, &one);

 BigUIntPair128 result = biguint128_dmul(&two, &max);
 if (!biguint128_eq(&result.first, &maxbutone)
         || !biguint128_eq(&result.second, &one)) {
  char buffer[6][HEX_BIGUINTLEN + 1];
  buffer[0][biguint128_print_hex(&two, buffer[0], HEX_BIGUINTLEN)] = 0;
  buffer[1][biguint128_print_hex(&max, buffer[1], HEX_BIGUINTLEN)] = 0;
  buffer[2][biguint128_print_hex(&one, buffer[2], HEX_BIGUINTLEN)] = 0;
  buffer[3][biguint128_print_hex(&maxbutone, buffer[3], HEX_BIGUINTLEN)] = 0;
  buffer[2][biguint128_print_hex(&result.second, buffer[4], HEX_BIGUINTLEN)] = 0;
  buffer[3][biguint128_print_hex(&result.first, buffer[5], HEX_BIGUINTLEN)] = 0;
  fprintf(stderr, "[%s * %s] -- expected: [%s%s], actual [%s%s]\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
  fail = true;
 }

 return !fail;
}

bool test_dmul1() {
 bool fail = false;
 BigUInt128 zero = biguint128_ctor_default();
 BigUInt128 one = biguint128_ctor_unit();
 BigUInt128 max= biguint128_sub(&zero, &one);
 BigUInt128 maxbutone= biguint128_sub(&max, &one);
 BigUInt128 two= biguint128_add(&one, &one);

 BigUIntPair128 result = biguint128_dmul(&max, &max); // see: 9 * 9 = 81; 99 * 99 = 9801
 if (!biguint128_eq(&result.second, &maxbutone)
         || !biguint128_eq(&result.first, &one)) {
  char buffer[6][HEX_BIGUINTLEN + 1];
  buffer[0][biguint128_print_hex(&two, buffer[0], HEX_BIGUINTLEN)] = 0;
  buffer[1][biguint128_print_hex(&max, buffer[1], HEX_BIGUINTLEN)] = 0;
  buffer[2][biguint128_print_hex(&one, buffer[2], HEX_BIGUINTLEN)] = 0;
  buffer[3][biguint128_print_hex(&maxbutone, buffer[3], HEX_BIGUINTLEN)] = 0;
  buffer[2][biguint128_print_hex(&result.second, buffer[4], HEX_BIGUINTLEN)] = 0;
  buffer[3][biguint128_print_hex(&result.first, buffer[5], HEX_BIGUINTLEN)] = 0;
  fprintf(stderr, "[%s * %s] -- expected: [%s%s], actual [%s%s]\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
  fail = true;
 }

 return !fail;
}

bool test_div0() {
 bool fail = false;
 BigUInt128 zero = biguint128_ctor_default();
 for (int i=0; i<hex_sample_len; ++i) {
  BigUInt128 val[3];
  if (!readhex_more_cstr_biguint128(val, hex_samples[i],3)) {
   continue;
  }

  buint_bool div_eq_a=true;
  buint_bool div_eq_b=true;
  buint_bool mod_eq_a=true;
  buint_bool mod_eq_b=true;
  BigUIntPair128 div_c_a;
  BigUIntPair128 div_c_b;
  if (!biguint128_eq(&zero,&val[0])) {
   div_c_a = biguint128_div(&val[2], &val[0]);
   div_eq_a = biguint128_eq(&val[1], &div_c_a.first);
   mod_eq_a = biguint128_eq(&zero, &div_c_a.second);
  }
  if (!biguint128_eq(&zero,&val[1])) {
   div_c_b = biguint128_div(&val[2], &val[1]);
   div_eq_b = biguint128_eq(&val[0], &div_c_b.first);
   mod_eq_b = biguint128_eq(&zero, &div_c_b.second);
  }

  if (!div_eq_a || !mod_eq_a) {
   char buffer[6][HEX_BIGUINTLEN + 1];
   buffer[0][biguint128_print_hex(&val[2], buffer[0], HEX_BIGUINTLEN)]=0;
   buffer[1][biguint128_print_hex(&val[0], buffer[1], HEX_BIGUINTLEN)]=0;
   buffer[2][biguint128_print_hex(&val[1], buffer[2], HEX_BIGUINTLEN)]=0;
   buffer[3][biguint128_print_hex(&zero, buffer[3], HEX_BIGUINTLEN)]=0;
   buffer[4][biguint128_print_hex(&div_c_a.first, buffer[4], HEX_BIGUINTLEN)]=0;
   buffer[5][biguint128_print_hex(&div_c_a.second, buffer[5], HEX_BIGUINTLEN)]=0;
   fprintf(stderr, "[%s / %s] -- expected: [%s,%s], actual [%s,%s]\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
   fail = true;
  }

  if (!div_eq_b || !mod_eq_b) {
   char buffer[6][HEX_BIGUINTLEN + 1];
   buffer[0][biguint128_print_hex(&val[2], buffer[0], HEX_BIGUINTLEN)]=0;
   buffer[1][biguint128_print_hex(&val[1], buffer[1], HEX_BIGUINTLEN)]=0;
   buffer[2][biguint128_print_hex(&val[0], buffer[2], HEX_BIGUINTLEN)]=0;
   buffer[3][biguint128_print_hex(&zero, buffer[3], HEX_BIGUINTLEN)]=0;
   buffer[4][biguint128_print_hex(&div_c_b.first, buffer[4], HEX_BIGUINTLEN)]=0;
   buffer[5][biguint128_print_hex(&div_c_b.second, buffer[5], HEX_BIGUINTLEN)]=0;
   fprintf(stderr, "[%s / %s] -- expected: [%s,%s], actual [%s,%s]\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
   fail = true;
  }

 }

 return !fail;
}

// if (a lt b) then div(a,b)=(0,a)
bool test_div1() {
 bool fail = false;
 BigUInt128 zero = biguint128_ctor_default();
 for (int i=0; i<hex_sample_len; ++i) {
  BigUInt128 val[3];
  BigUInt128 *a=&val[0];
  BigUInt128 *b=&val[2];
  if (!readhex_more_cstr_biguint128(val, hex_samples[i],3)) {
   continue;
  }
  if (biguint128_eq(a, &zero)) {
   continue;
  }
  if (!biguint128_lt(a, b)) {
   continue;
  }

  BigUIntPair128 div_a_b = biguint128_div(a,b);
  buint_bool res_div = biguint128_eq(&div_a_b.first,&zero);
  buint_bool res_mod = biguint128_eq(&div_a_b.second,a);

  if (!res_div || !res_mod) {
   char buffer[6][HEX_BIGUINTLEN + 1];
   buffer[0][biguint128_print_hex(a, buffer[0], HEX_BIGUINTLEN)]=0;
   buffer[1][biguint128_print_hex(b, buffer[1], HEX_BIGUINTLEN)]=0;
   buffer[2][biguint128_print_hex(&zero, buffer[2], HEX_BIGUINTLEN)]=0;
   buffer[3][biguint128_print_hex(a, buffer[3], HEX_BIGUINTLEN)]=0;
   buffer[4][biguint128_print_hex(&div_a_b.first, buffer[4], HEX_BIGUINTLEN)]=0;
   buffer[5][biguint128_print_hex(&div_a_b.second, buffer[5], HEX_BIGUINTLEN)]=0;
   fprintf(stderr, "[%s / %s] -- expected: [%s,%s], actual [%s,%s]\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
   fail = true;
  }
 }

 return !fail;
}

int main(int argc, char **argv) {

 assert(test_mul0());
 assert(test_mul1());
 assert(test_mul2());

 assert(test_dmul0());
 assert(test_dmul1());

 assert(test_div0());
 assert(test_div1());

 return 0;
}
