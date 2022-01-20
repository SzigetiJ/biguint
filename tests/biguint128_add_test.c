#include "biguint128.h"
#include "test_common128.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define BIGUINT_BITS 128

const CStr hex_samples[][3] = {
 { STR("0"), STR("1"), STR("1")},
 { STR("1"), STR("0"), STR("1")},
 { STR("0"), STR("FFFFFFFF"), STR("FFFFFFFF")},
 { STR("1"), STR("FFFFFFFF"), STR("100000000")},
 { STR("FFFFFFFF"), STR("FFFFFFFF"), STR("1FFFFFFFE")},
 { STR("100000000"), STR("200000001"), STR("300000001")},
 { STR("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE"), STR("1"), STR("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
 { STR("EFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"), STR("1"), STR("F0000000000000000000000000000000")},
 { STR("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE"), STR("1"), STR("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")},
 { STR("EFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"), STR("1"), STR("F000000000000000000000000000000000000000000000000000000000000000")}
};
int hex_sample_len = sizeof(hex_samples) / (sizeof(hex_samples[0]));

void fprintf_biguint128_binop_testresult(FILE *out, BigUInt128 *op0, BigUInt128 *op1, BigUInt128 *expected, BigUInt128 *actual, const char *op_str) {
   char buffer[4][HEX_BIGUINTLEN + 1];
   BigUInt128 *v_refs[] = {op0, op1, expected, actual};
   for (int j = 0; j < 4; ++j) {
     buffer[j][biguint128_print_hex(v_refs[j], buffer[j], HEX_BIGUINTLEN)]=0;
   }
   fprintf(out, "[%s %s %s] -- expected: [%s], actual [%s]\n", buffer[0], op_str, buffer[1], buffer[2], buffer[3]);
}

static void fprintf_biguint128_unnop_testresult(FILE *out, const BigUInt128 *op0, const BigUInt128 *expected, const BigUInt128 *actual, const char *op_str) {
   char buffer[3][HEX_BIGUINTLEN + 1];
   const BigUInt128 *v_refs[] = {op0, expected, actual};
   for (int j = 0; j < 3; ++j) {
     buffer[j][biguint128_print_hex(v_refs[j], buffer[j], HEX_BIGUINTLEN)]=0;
   }
   fprintf(out, "[%s%s] -- expected: [%s], actual [%s]\n", op_str, buffer[0], buffer[1], buffer[2]);
}


int test_addsub0() {
 int fail = 0;
 for (int i=0; i<hex_sample_len; ++i) {
   BigUInt128 sum, diff;
   BigUInt128 values[3];
   BigUInt128 *a=&values[0];
   BigUInt128 *b=&values[1];
   BigUInt128 *c=&values[2];

   if (!readhex_more_cstr_biguint128(values, hex_samples[i], 3)) {
    continue;
   }

   // operation
  sum = biguint128_add(a, b);
  diff = biguint128_sub(c, b);

  // eval sum
  buint_bool result_sum = biguint128_eq(c, &sum);
  if (!result_sum) {
    fprintf_biguint128_binop_testresult(stderr, a, b, c, &sum, "+");
   fail = 1;
  }

  buint_bool result_diff = biguint128_eq(a, &diff);
  if (!result_diff) {
    fprintf_biguint128_binop_testresult(stderr, a, b, c, &sum, "-");
   fail = 1;
  }
 }

 return fail;
}

int test_addsub1() {
 int fail = 0;
 for (int i=0; i<hex_sample_len; ++i) {
   BigUInt128 values[3];
   BigUInt128 *a=&values[0];
   BigUInt128 *b=&values[1];
   BigUInt128 *c=&values[2];
   BigUInt128 a_orig;

   if (!readhex_more_cstr_biguint128(values, hex_samples[i], 3)) {
    continue;
   }
   a_orig=*a;

   // operation
  biguint128_add_assign(a, b);

  // eval sum
  buint_bool result_sum = biguint128_eq(c, a);
  if (!result_sum) {
    fprintf_biguint128_binop_testresult(stderr, &a_orig, b, c, a, "+=");
   fail = 1;
  }

 }

 return fail;
}

bool test_incdec0() {
 bool fail=false;
 // interval borders
 BigUInt128 zero= biguint128_ctor_default();
 BigUInt128 one= biguint128_ctor_unit();
 BigUInt128 two= biguint128_add(&one, &one);
 BigUInt128 max= biguint128_sub(&zero, &one);
 BigUInt128 maxbutone= biguint128_sub(&max, &one);

 BigUInt128 arr_orig[]= {maxbutone, max, zero, one, two};    // copy
 BigUInt128 *arr[]= {&maxbutone, &max, &zero, &one, &two};   // ref
 size_t arrlen= sizeof(arr) / sizeof(arr[0]);

 for (size_t i= 0u; i < arrlen; ++i) {
  biguint128_inc(arr[i]);
  if (i != arrlen-1) {
   if (!biguint128_eq(arr[i], &arr_orig[i + 1])) {
    fprintf_biguint128_unnop_testresult(stderr, &arr_orig[i], &arr_orig[i + 1], arr[i], "++");
    fail=true;
   }
  }
 }

 for (size_t i= 0u; i < arrlen; ++i) {
  size_t j= arrlen - i - 1;
  BigUInt128 tmp= *arr[j];
  biguint128_dec(arr[j]);
  if (!biguint128_eq(arr[j], &arr_orig[j])) {
   fprintf_biguint128_unnop_testresult(stderr, &tmp, &arr_orig[j], arr[j], "--");
    fail= true;
  }
 }
 return fail;
}

int main(int argc, char **argv) {

 assert(test_addsub0() == 0);
 assert(test_addsub1() == 0);

 assert(test_incdec0() == 0);
 return 0;
}

