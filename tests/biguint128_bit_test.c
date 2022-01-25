#include "biguint128.h"
#include "test_common128.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define BIGUINT_BITS 128
#define PRINTBUFFER_LEN 80

static void print_fun2_error(const char *expr, const char *a, const char *b, const char *expected, const BigUInt128 *actual) {
 char pbuffer[PRINTBUFFER_LEN];
 char buffer[HEX_BIGUINTLEN + 1];
 snprintf(pbuffer, PRINTBUFFER_LEN, "[%s] -- expected: [%%s], actual [%%s]\n", expr);
 buffer[biguint128_print_hex(actual, buffer, HEX_BIGUINTLEN)] = 0;
 fprintf(stderr, pbuffer, a, b, expected, buffer);
}

bool test_and_or_not0() {
 const CStr samples_a[]={
  STR("0"),
  STR("1"),
  STR("7FFFFFFF"),
  STR("100000000"),
  STR("A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5")
 };
 const unsigned int samples_a_len = sizeof(samples_a) / sizeof(samples_a[0]);
 const CStr samples_b[]={
  STR("3"),
  STR("300000000"),
  STR("30000000000000000"),
  STR("3000000000000000000000000")
 };
 const unsigned int samples_b_len = sizeof(samples_b) / sizeof(samples_b[0]);

 const CStr intersections[][4]={
  {STR("0"),STR("0"),STR("0"),STR("0")},
  {STR("1"),STR("0"),STR("0"),STR("0")},
  {STR("3"),STR("0"),STR("0"),STR("0")},
  {STR("0"),STR("100000000"),STR("0"),STR("0")},
  {STR("1"),STR("100000000"),STR("10000000000000000"),STR("1000000000000000000000000")}
 };
 const CStr unions[][4]={
  {STR("3"),STR("300000000"),STR("30000000000000000"),STR("3000000000000000000000000")},
  {STR("3"),STR("300000001"),STR("30000000000000001"),STR("3000000000000000000000001")},
  {STR("7FFFFFFF"),STR("37FFFFFFF"),STR("3000000007FFFFFFF"),STR("300000000000000007FFFFFFF")},
  {STR("100000003"),STR("300000000"),STR("30000000100000000"),STR("3000000000000000100000000")},
  {STR("A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A7"),STR("A5A5A5A5A5A5A5A5A5A5A5A7A5A5A5A5"),STR("A5A5A5A5A5A5A5A7A5A5A5A5A5A5A5A5"),STR("A5A5A5A7A5A5A5A5A5A5A5A5A5A5A5A5")}
 };

 const CStr sym_diffs[][4]={
  {STR("3"),STR("300000000"),STR("30000000000000000"),STR("3000000000000000000000000")},
  {STR("2"),STR("300000001"),STR("30000000000000001"),STR("3000000000000000000000001")},
  {STR("7FFFFFFC"),STR("37FFFFFFF"),STR("3000000007FFFFFFF"),STR("300000000000000007FFFFFFF")},
  {STR("100000003"),STR("200000000"),STR("30000000100000000"),STR("3000000000000000100000000")},
  {STR("A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A6"),STR("A5A5A5A5A5A5A5A5A5A5A5A6A5A5A5A5"),STR("A5A5A5A5A5A5A5A6A5A5A5A5A5A5A5A5"),STR("A5A5A5A6A5A5A5A5A5A5A5A5A5A5A5A5")}
 };

 bool fail = false;

 for (unsigned int a_i = 0; a_i < samples_a_len; ++a_i) {
  BigUInt128 a = biguint128_ctor_hexcstream(samples_a[a_i].str, samples_a[a_i].len);
  BigUInt128 not_a = biguint128_not(&a);
  for (unsigned int b_i = 0; b_i < samples_b_len; ++b_i) {
   BigUInt128 b = biguint128_ctor_hexcstream(samples_b[b_i].str, samples_b[b_i].len);
   BigUInt128 not_b = biguint128_not(&b);

   BigUInt128 expected_and = biguint128_ctor_hexcstream(intersections[a_i][b_i].str, intersections[a_i][b_i].len);
   BigUInt128 expected_or = biguint128_ctor_hexcstream(unions[a_i][b_i].str, unions[a_i][b_i].len);
   BigUInt128 expected_xor = biguint128_ctor_hexcstream(sym_diffs[a_i][b_i].str, sym_diffs[a_i][b_i].len);


   BigUInt128 result_and = biguint128_and(&a, &b);
   BigUInt128 result_or = biguint128_or(&a, &b);
   BigUInt128 tmp_or = biguint128_or(&not_a, &not_b);
   BigUInt128 result_and2 = biguint128_not(&tmp_or);
   BigUInt128 result_xor = biguint128_xor(&a, &b);

   buint_bool eq_and = biguint128_eq(&expected_and, &result_and);
   buint_bool eq_and2 = biguint128_eq(&expected_and, &result_and2);
   buint_bool eq_or = biguint128_eq(&expected_or, &result_or);
   buint_bool eq_xor = biguint128_eq(&expected_xor, &result_xor);

   if (!eq_and) {
    print_fun2_error("%s & %s", samples_a[a_i].str, samples_b[b_i].str, intersections[a_i][b_i].str, &result_and);
    fail = true;
   }

   if (!eq_and2) {
    print_fun2_error("~(~%s | ~%s)", samples_a[a_i].str, samples_b[b_i].str, intersections[a_i][b_i].str, &result_and2);
    fail = true;
   }

   if (!eq_or) {
    print_fun2_error("%s | %s", samples_a[a_i].str, samples_b[b_i].str, unions[a_i][b_i].str, &result_or);
    fail = true;
   }

   if (!eq_xor) {
    print_fun2_error("%s ^ %s", samples_a[a_i].str, samples_b[b_i].str, sym_diffs[a_i][b_i].str, &result_xor);
    fail = true;
   }
  }
 }

 return !fail;
}

bool test_set_get0() {
 bool fail = false;

 BigUInt128 value = biguint128_ctor_default();
 buint_bool pre_expected[]={1,0,0};
 buint_bool post_expected[]={1,1,0};

 for (int i=0; i<BIGUINT_BITS; ++i) {
  BigUInt128 pre_value = biguint128_ctor_copy(&value);
  BigUInt128 owrite_value = biguint128_ctor_copy(&value);
  buint_bool pre_change[3];
  buint_bool post_change[3];
  for (int j = 0; j < 3; ++j) {
   int pos = (i - 1 + j + BIGUINT_BITS) % BIGUINT_BITS;
   pre_change[j] = biguint128_gbit(&value, pos);
  }
  biguint128_sbit(&value,i);
  biguint128_obit(&owrite_value,i,true);
  for (int j = 0; j < 3; ++j) {
   int pos = (i - 1 + j + BIGUINT_BITS) % BIGUINT_BITS;
   post_change[j] = biguint128_gbit(&value, pos);
  }

  bool invert_expected[]={(i==0),false,(i==BIGUINT_BITS-1)};

  for (int j = 0; j < 3; ++j) {
   unsigned int pos = (i - 1 + j + BIGUINT_BITS) % BIGUINT_BITS;
   if ((pre_expected[j]!=invert_expected[j]) != pre_change[j]) {
    fprint_funres_buint128_x_bsz_bb(
            stderr, "gbit",
            &pre_value, pos,
            (pre_expected[j]!=invert_expected[j]), pre_change[j]);
    fail=true;
   }

   if ((post_expected[j]!=invert_expected[j]) != post_change[j]) {
    fprint_funres_buint128_x_bsz_bb(
            stderr, "gbit",
            &value, pos,
            (post_expected[j]!=invert_expected[j]), post_change[j]);
    fail=true;
   }
  }
  if (!biguint128_eq(&owrite_value, &value)) {
   char buffer_value[HEX_BIGUINTLEN + 1];
   char buffer_ovalue[HEX_BIGUINTLEN + 1];
   buffer_value[biguint128_print_hex(&value, buffer_value, HEX_BIGUINTLEN)]=0;
   buffer_ovalue[biguint128_print_hex(&owrite_value, buffer_ovalue, HEX_BIGUINTLEN)]=0;
   fprintf(stderr, "sbit(a,b) and obit(a,b,1) results differ: sbit: [%s], obit: [%s]\n", buffer_value,buffer_ovalue);
   fail=true;
  }
 }

 return !fail;
}

bool test_clr_get0() {
 bool fail = false;
 BigUInt128 unit = biguint128_ctor_unit();

 BigUInt128 value = biguint128_ctor_default();
 biguint128_sub_assign(&value,&unit);
 buint_bool pre_expected[]={0,1,1};
 buint_bool post_expected[]={0,0,1};

 for (int i=0; i<BIGUINT_BITS; ++i) {
  BigUInt128 pre_value = biguint128_ctor_copy(&value);
  BigUInt128 owrite_value = biguint128_ctor_copy(&value);
  buint_bool pre_change[3];
  buint_bool post_change[3];
  for (int j = 0; j < 3; ++j) {
   int pos = (i - 1 + j + BIGUINT_BITS) % BIGUINT_BITS;
   pre_change[j] = biguint128_gbit(&value, pos);
  }
  biguint128_cbit(&value,i);
  biguint128_obit(&owrite_value,i,false);
  for (int j = 0; j < 3; ++j) {
   int pos = (i - 1 + j + BIGUINT_BITS) % BIGUINT_BITS;
   post_change[j] = biguint128_gbit(&value, pos);
  }

  bool invert_expected[]={(i==0),false,(i==BIGUINT_BITS-1)};

  for (int j = 0; j < 3; ++j) {
   unsigned int pos = (i - 1 + j + BIGUINT_BITS) % BIGUINT_BITS;
   if ((pre_expected[j]!=invert_expected[j]) != pre_change[j]) {
    fprint_funres_buint128_x_bsz_bb(
            stderr, "gbit",
            &pre_value, pos,
            (pre_expected[j]!=invert_expected[j]), pre_change[j]);
    fail=true;
   }

   if ((post_expected[j]!=invert_expected[j]) != post_change[j]) {
    fprint_funres_buint128_x_bsz_bb(
            stderr, "gbit",
            &value, pos,
            (post_expected[j]!=invert_expected[j]), post_change[j]);
    fail=true;
   }
  }

  if (!biguint128_eq(&owrite_value, &value)) {
   char buffer_value[HEX_BIGUINTLEN + 1];
   char buffer_ovalue[HEX_BIGUINTLEN + 1];
   buffer_value[biguint128_print_hex(&value, buffer_value, HEX_BIGUINTLEN)]=0;
   buffer_ovalue[biguint128_print_hex(&owrite_value, buffer_ovalue, HEX_BIGUINTLEN)]=0;
   fprintf(stderr, "sbit(a,b) and obit(a,b,1) results differ: sbit: [%s], obit: [%s]\n", buffer_value,buffer_ovalue);
   fail=true;
  }
 }
 return !fail;
}

// checks: rol(x,a) == or(shl(x,a), shr(x,128-a))
bool test_shiftrot0() {
 bool fail=false;

 const CStr val_a[]={
  STR("123456789ABCDEF0"),
  STR("1223334444555556666667777777"),
  STR("123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0"),
  STR("122333444455555666666777777788888888999999999AAAAAAAAAABBBBBBBBB")
 };
 size_t val_len=sizeof(val_a)/sizeof(val_a[0]);
 const buint_size_t sh_a[]={0,1,2,4,7,16,31,127,128,129,255,256};
 size_t sh_len=sizeof(sh_a)/sizeof(sh_a[0]);

 for (size_t vi= 0; vi < val_len; ++vi) {
  for (size_t si= 0; si < sh_len; ++si) {
   BigUInt128 a;
   if (!readhex_cstr_biguint128(&a, &val_a[vi])) continue;
   BigUInt128 act_rol= biguint128_rol(&a, sh_a[si]);
   BigUInt128 act_ror= biguint128_ror(&a, sh_a[si]);

   BigUInt128 res_shl= biguint128_shl(&a, sh_a[si] % BIGUINT_BITS);
   BigUInt128 res_shl_oflow= biguint128_shr(&a, (BIGUINT_BITS - sh_a[si]) % BIGUINT_BITS);
   BigUInt128 exp_rol= biguint128_or(&res_shl, &res_shl_oflow);

   BigUInt128 res_shr= biguint128_shr(&a, sh_a[si] % BIGUINT_BITS);
   BigUInt128 res_shr_uflow= biguint128_shl(&a, (BIGUINT_BITS - sh_a[si]) % BIGUINT_BITS);
   BigUInt128 exp_ror= biguint128_or(&res_shr, &res_shr_uflow);

   if (!biguint128_eq(&exp_rol, &act_rol)) {
    fprint_funres_buint128_x_bsz_buint128(stderr, "rol", &a, sh_a[si], & exp_rol, &act_rol);
    fail=true;
   }
   if (!biguint128_eq(&exp_ror, &act_ror)) {
    fprint_funres_buint128_x_bsz_buint128(stderr, "ror", &a, sh_a[si], & exp_ror, &act_ror);
    fail=true;
   }
  }
 }

 return !fail;
}

int main(int argc, char **argv) {

 assert(test_and_or_not0());
 assert(test_set_get0());
 assert(test_clr_get0());

 assert(test_shiftrot0());
 return 0;
}

