#include "biguint128.h"
#include "test_common128.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define HEX_UINTLEN (2*sizeof(UInt))

typedef struct {
 BigUInt128 a;
 BigUInt128 b;
 buint_bool c;
 bool sbc;
 BigUInt128 exp_res;
 buint_bool exp_c;
} AdcSbcTestV;

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
int hex_sample_len = ARRAYSIZE(hex_samples);

static void fprintf_biguint128_xxc_testresult(FILE *out, BigUInt128 *op0, BigUInt128 *op1, buint_bool op2, BigUInt128 *expected, buint_bool exp2, BigUInt128 *actual, buint_bool actual2, const char *funname) {
 ArgType argt[] = {BUINT_XROREF, BUINT_XROREF, BUINT_TBOOL};
 GenArgU values[] = {
  {.x = *op0},
  {.x = *op1},
  {.b = op2},
  {.x = *expected},
  {.b = exp2},
  {.x = *actual},
  {.b = actual2}
 };
 fprintf_biguint128_genfun0_testresult(out, funname, values, values + 5, values + 3, 3, argt, 2, argt + 1, FMT_HEX);
}

bool test_adcsbc_all() {
 bool fail=false;

 void (*fun[])(BigUInt128 *dest, const BigUInt128 *a, const BigUInt128 *b, buint_bool *c)={
  biguint128_adc_replace,
  biguint128_sbc_replace
 };
 const char *op[]={"adc","sbc"};

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
   fprintf_biguint128_xxc_testresult(
           stderr, &tests[i].a, &tests[i].b, tests[i].c, &tests[i].exp_res, tests[i].exp_c, &act_res, act_c, op[tests[i].sbc]);
   fail |= 1;
  }
 }

 return fail;
}

bool test_incdec0() {
 bool fail=false;
 // init values
 char auxhex[2][HEX_BIGUINTLEN+1]; // stores 'FFFF...FFFF' and 'FFFF...FFFE'
 char uintmax[HEX_UINTLEN+1]; // stores 'FF..FF' (full uint)
 char uintoflow[HEX_UINTLEN+2]; // stores '100..00' (full uint + 1)

 memset(auxhex[0],'F',HEX_BIGUINTLEN);
 auxhex[0][HEX_BIGUINTLEN]=0;
 memset(auxhex[1],'F',HEX_BIGUINTLEN);
 auxhex[1][HEX_BIGUINTLEN-1]='E';
 auxhex[1][HEX_BIGUINTLEN]=0;
 memset(uintmax,'F', HEX_UINTLEN);
 uintmax[HEX_UINTLEN]=0;
 memset(uintoflow+1,'0',HEX_UINTLEN);
 uintoflow[0]='1';
 uintoflow[HEX_UINTLEN+1]=0;

 CStr incsamples[][2]={
  {STR(auxhex[1]), STR(auxhex[0])},
  {STR(auxhex[0]), STR("0")},
  {STR("0"),STR("1")},
  {STR("1"),STR("2")},
  {STR(uintmax),STR(uintoflow)}
 };

 unsigned int inc_params[]={0,1};
 unsigned int dec_params[]={1,0};
 // check
 fail|=test_genfun(&incsamples[0][0], 2, ARRAYSIZE(incsamples), FMT_HEX, inc_params, XUFUN0A(biguint128_inc), "inc", NULL);
 fail|=test_genfun(&incsamples[0][0], 2, ARRAYSIZE(incsamples), FMT_HEX, dec_params, XUFUN0A(biguint128_dec), "dec", NULL);

 return fail;
}

int main() {
 unsigned int sum_params[]={0,1,2};
 unsigned int diff_params1[]={2,1,0};
 unsigned int diff_params2[]={2,0,1};

 assert(test_genfun(&hex_samples[0][0], 3, hex_sample_len, FMT_HEX, sum_params, XBFUN0(biguint128_add), "add", NULL) == 0);
 assert(test_genfun(&hex_samples[0][0], 3, hex_sample_len, FMT_HEX, diff_params1, XBFUN0(biguint128_sub), "sub", NULL) == 0);
 assert(test_genfun(&hex_samples[0][0], 3, hex_sample_len, FMT_HEX, diff_params2, XBFUN0(biguint128_sub), "sub", NULL) == 0);

#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
 assert(test_genfun(&hex_samples[0][0], 3, hex_sample_len, FMT_HEX, sum_params, XBFUN0V(biguint128_addv), "addv", NULL) == 0);
 assert(test_genfun(&hex_samples[0][0], 3, hex_sample_len, FMT_HEX, diff_params1, XBFUN0V(biguint128_subv), "subv", NULL) == 0);
 assert(test_genfun(&hex_samples[0][0], 3, hex_sample_len, FMT_HEX, diff_params2, XBFUN0V(biguint128_subv), "subv", NULL) == 0);
#endif

 assert(test_genfun(&hex_samples[0][0], 3, hex_sample_len, FMT_HEX, sum_params, XBFUN0A(biguint128_add_assign), "add_assign", NULL) == 0);
 assert(test_genfun(&hex_samples[0][0], 3, hex_sample_len, FMT_HEX, diff_params1, XBFUN0A(biguint128_sub_assign), "sub_assign", NULL) == 0);
 assert(test_genfun(&hex_samples[0][0], 3, hex_sample_len, FMT_HEX, diff_params2, XBFUN0A(biguint128_sub_assign), "sub_assign", NULL) == 0);

 init_testvalues();
 assert(test_adcsbc_all() == 0);

 assert(test_incdec0() == 0);
 return 0;
}

