#include "biguint128.h"
#include "test_common128.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

typedef struct {
 BigUInt128 a;
 BigUInt128 b;
 buint_bool c;
 bool sbc;
 BigUInt128 exp_res;
 buint_bool exp_c;
} AdcSbcTestV;

typedef struct {
 BigUInt128 a;
 bool is_inc;
 BigUInt128 exp_res;
} IncDecTestV;

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

static void fprintf_biguint128_unop_testresult(FILE *out, const BigUInt128 *op0, const BigUInt128 *expected, const BigUInt128 *actual, const char *op_str) {
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

bool test_adcsbc_all() {
 bool fail=false;

 void (*fun[])(BigUInt128 *dest, const BigUInt128 *a, const BigUInt128 *b, buint_bool *c)={
  biguint128_adc_replace,
  biguint128_sbc_replace
 };
 const char *op[]={"+'","-'"};

 AdcSbcTestV tests[]= {
  {max, zero, 0, 0, max, 0},
  {max, zero, 1, 0, zero, 1},
  {max, one, 0, 0, zero, 1},
  {max, one, 1, 0, one, 1},
  {zero, zero, 1, 0, one, 0},
  {max, max, 1, 0, max, 1},

  {zero, zero, 0, 1, zero, 0},
  {zero, zero, 1, 1, max, 1},
  {one, zero, 0, 1, one, 0},
  {one, zero, 1, 1, zero, 0},
  {one, one, 0, 1, zero, 0},
  {one, one, 1, 1, max, 1},
  {zero, max, 0, 1, one, 1},
  {zero, max, 1, 1, zero, 1},
 };
 int tests_num= sizeof(tests)/sizeof(tests[0]);

 for (int i= 0; i < tests_num; ++i) {
  BigUInt128 act_res;
  buint_bool act_c=tests[i].c;
  fun[tests[i].sbc](&act_res, &tests[i].a, &tests[i].b, &act_c);
  if (!biguint128_eq(&tests[i].exp_res, &act_res) || tests[i].exp_c!=act_c) {
   fprintf_biguint128_binop_testresult(
           stderr, &tests[i].a, &tests[i].b, &tests[i].exp_res, &act_res, op[tests[i].sbc]);
   fprintf(stderr,"carry expected: %u, actual: %u\n", tests[i].exp_c, act_c);
   fail = 1;
  }
 }

 return fail;
}

bool test_incdec0() {
 bool fail=false;

  BigUInt128 * (*fun[])(BigUInt128 *dest)={
  biguint128_dec,
  biguint128_inc
 };
 const char *op[]={"--","++"};

 IncDecTestV tests[]={
  {maxbutone,1,max},
  {max,1,zero},
  {zero,1,one},
  {one,1,two},
  {uintmax,1,uintoflow},

  {max,0,maxbutone},
  {zero,0,max},
  {one,0,zero},
  {two,0,one},
  {uintoflow,0,uintmax}
 };
 int tests_num= sizeof(tests)/sizeof(tests[0]);

 for (int i= 0; i < tests_num; ++i) {
  BigUInt128 *act_res;
  BigUInt128 act_param = tests[i].a;
  act_res=fun[tests[i].is_inc](&act_param);
  if (!biguint128_eq(&tests[i].exp_res, &act_param) || act_res!=&act_param) {
   fprintf_biguint128_unop_testresult(
           stderr, &tests[i].a, &tests[i].exp_res, &act_param, op[tests[i].is_inc]);
   fprintf(stderr,"returned expected: %p, actual: %p\n", &act_param, act_res);
   fail = 1;
  }
 }

 return fail;
}

int main(int argc, char **argv) {

 assert(test_addsub0() == 0);
 assert(test_addsub1() == 0);

 init_testvalues();
 assert(test_adcsbc_all() == 0);

 assert(test_incdec0() == 0);
 return 0;
}

