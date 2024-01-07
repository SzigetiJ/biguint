#include "biguint128.h"
#include "test_common128.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define BIGUINT_BITS 128
#define UINT_BITS (8*sizeof(UInt))
#define PRINTBUFFER_LEN 80


static void print_fun1_error(const char *expr, const BigUInt128 *a, buint_size_t expected, buint_size_t actual) {
 char abuf[BIGUINT_BITS/4 + 1];
 abuf[biguint128_print_hex(a, abuf, BIGUINT_BITS/4)]=0;
 fprintf(stderr, "%s(%s) -- expected: %"PRIbuint_size_t", actual: %"PRIbuint_size_t"\n", expr, abuf, expected, actual);
}

static void print_fun2_error(const char *expr, const char *a, const char *b, const char *expected, const BigUInt128 *actual) {
 char pbuffer[PRINTBUFFER_LEN];
 char buffer[HEX_BIGUINTLEN + 1];
 snprintf(pbuffer, PRINTBUFFER_LEN, "[%s] -- expected: [%%s], actual [%%s]\n", expr);
 buffer[biguint128_print_hex(actual, buffer, HEX_BIGUINTLEN)] = 0;
 fprintf(stderr, pbuffer, a, b, expected, buffer);
}

static void print_assign_ptr_error(const char *expr, const BigUInt128 *expected, const BigUInt128 *actual) {
 fprintf(stderr, "%s does not preserve pointer -- input parameter: %p, return value: %p\n", expr, (void*)expected, (void*)actual);
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

   BigUInt128 obj_and_assign = biguint128_ctor_copy(&a);
   BigUInt128 *res_and_assign = biguint128_and_assign(&obj_and_assign, &b);
   BigUInt128 obj_or_assign = biguint128_ctor_copy(&a);
   BigUInt128 *res_or_assign = biguint128_or_assign(&obj_or_assign, &b);
   BigUInt128 obj_xor_assign = biguint128_ctor_copy(&a);
   BigUInt128 *res_xor_assign = biguint128_xor_assign(&obj_xor_assign, &b);
   BigUInt128 obj_not_assign = biguint128_ctor_copy(&tmp_or);
   BigUInt128 *res_not_assign = biguint128_not_assign(&obj_not_assign);

   buint_bool eq_and = biguint128_eq(&expected_and, &result_and);
   buint_bool eq_and2 = biguint128_eq(&expected_and, &result_and2);
   buint_bool eq_or = biguint128_eq(&expected_or, &result_or);
   buint_bool eq_xor = biguint128_eq(&expected_xor, &result_xor);

   buint_bool eq_and_assign = biguint128_eq(&expected_and, &obj_and_assign);
   buint_bool eq2_and_assign = (res_and_assign == &obj_and_assign);
   buint_bool eq_or_assign = biguint128_eq(&expected_or, &obj_or_assign);
   buint_bool eq2_or_assign = (res_or_assign == &obj_or_assign);
   buint_bool eq_xor_assign = biguint128_eq(&expected_xor, &obj_xor_assign);
   buint_bool eq2_xor_assign = (res_xor_assign == &obj_xor_assign);
   buint_bool eq_not_assign = biguint128_eq(&expected_and, &obj_not_assign);
   buint_bool eq2_not_assign = (res_not_assign == &obj_not_assign);

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

   if (!eq_and_assign) {
    print_fun2_error("%s &= %s", samples_a[a_i].str, samples_b[b_i].str, intersections[a_i][b_i].str, &obj_and_assign);
    fail = true;
   }
   if (!eq2_and_assign) {
    print_assign_ptr_error("a &= b", &obj_and_assign, res_and_assign);
    fail = true;
   }

   if (!eq_or_assign) {
    print_fun2_error("%s |= %s", samples_a[a_i].str, samples_b[b_i].str, unions[a_i][b_i].str, &obj_or_assign);
    fail = true;
   }
   if (!eq2_or_assign) {
    print_assign_ptr_error("a |= b", &obj_or_assign, res_or_assign);
    fail = true;
   }

   if (!eq_xor_assign) {
    print_fun2_error("%s ^= %s", samples_a[a_i].str, samples_b[b_i].str, sym_diffs[a_i][b_i].str, &obj_xor_assign);
    fail = true;
   }
   if (!eq2_xor_assign) {
    print_assign_ptr_error("a ^= b", &obj_xor_assign, res_xor_assign);
    fail = true;
   }

   if (!eq_not_assign) {
    print_fun2_error("flip(~%s | ~%s)", samples_a[a_i].str, samples_b[b_i].str, intersections[a_i][b_i].str, &obj_not_assign);
    fail = true;
   }
   if (!eq2_not_assign) {
    print_assign_ptr_error("a = ~a", &obj_not_assign, res_not_assign);
    fail = true;
   }

  }
 }

 return !fail;
}

bool test_gen_write_get0(bool clr) {
 bool fail = false;

 BigUInt128 value = biguint128_ctor_default();
 if (clr) {
  BigUInt128 unit = biguint128_ctor_unit();
  biguint128_sub_assign(&value,&unit);
 }

 buint_bool pre_expected[]={!clr,clr,clr};
 buint_bool post_expected[]={!clr,!clr,clr};

 for (int i=0; i<BIGUINT_BITS; ++i) {
  BigUInt128 pre_value = biguint128_ctor_copy(&value);
  BigUInt128 owrite_value = biguint128_ctor_copy(&value);
  buint_bool pre_change[3];
  buint_bool post_change[3];
  for (int j = 0; j < 3; ++j) {
   int pos = (i - 1 + j + BIGUINT_BITS) % BIGUINT_BITS;
   pre_change[j] = biguint128_gbit(&value, pos);
  }

  clr?
   biguint128_cbit(&value,i):
   biguint128_sbit(&value,i);
  biguint128_obit(&owrite_value,i,!clr);

  for (int j = 0; j < 3; ++j) {
   int pos = (i - 1 + j + BIGUINT_BITS) % BIGUINT_BITS;
   post_change[j] = biguint128_gbit(&value, pos);
  }

  // In general, our expectations are valid for each bit,
  // except for the first and the last bits.
  // In case i==0, the preceeding bit is taken from the end of the interval,
  // and in case of the last bit, the succeeding bit is taken from the begin of the interval.
  // Thus in these cases we have to invert our preceeding / succeeding bit expectations.
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
            stderr, clr?"cbit":"sbit",
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
   fprintf(stderr, "%cbit(a,b) and obit(a,b,%u) results differ: %cbit: [%s], obit: [%s]\n",
    clr?'c':'s',clr?0U:1U,clr?'c':'s',buffer_value,buffer_ovalue);
   fail=true;
  }
 }

 return !fail;
}


// checks: rol(x,a) == or(shl(x,a), shr(x,128-a))
bool test_shiftrot0() {
 bool fail=false;

 const CStr val_a[]={
  STR("12345678"),
  STR("123456789ABCDEF0"),
  STR("1223334444555556666667777777"),
  STR("123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0"),
  STR("122333444455555666666777777788888888999999999AAAAAAAAAABBBBBBBBB")
 };
 size_t val_len=sizeof(val_a)/sizeof(val_a[0]);
 const buint_size_t sh_a[]={0,1,2,4,7,16,31,63,64,127,128,129,255,256};
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
   BigUInt128 res_shra= biguint128_ctor_copy(&a);
   BigUInt128 *res2_shra= biguint128_shr_assign(&res_shra, sh_a[si] % BIGUINT_BITS);

   if (sh_a[si]<UINT_BITS) {
    BigUInt128 res_shlt = biguint128_ctor_copy(&a);
    biguint128_shl_tiny(&res_shlt, sh_a[si]);

    BigUInt128 res_shrt = biguint128_ctor_copy(&a);
    biguint128_shr_tiny(&res_shrt, sh_a[si]);

    if (!biguint128_eq(&res_shl, &res_shlt)) {
     fprint_funres_buint128_x_bsz_buint128(stderr, "shlt", &a, sh_a[si], &res_shl, &res_shlt);
     fail=true;
    }

    if (!biguint128_eq(&res_shr, &res_shrt)) {
     fprint_funres_buint128_x_bsz_buint128(stderr, "shrt", &a, sh_a[si], &res_shr, &res_shrt);
     fail=true;
    }
   }

   if (!biguint128_eq(&exp_rol, &act_rol)) {
    fprint_funres_buint128_x_bsz_buint128(stderr, "rol", &a, sh_a[si], & exp_rol, &act_rol);
    fail=true;
   }
   if (!biguint128_eq(&exp_ror, &act_ror)) {
    fprint_funres_buint128_x_bsz_buint128(stderr, "ror", &a, sh_a[si], & exp_ror, &act_ror);
    fail=true;
   }

   if (!biguint128_eq(&res_shra, &res_shr)) {
    fprint_funres_buint128_x_bsz_buint128(stderr, "shra (param)", &a, sh_a[si], &res_shr, &res_shra);
    fail=true;
   }

   if (&res_shra != res2_shra) {
    print_assign_ptr_error("shr_a", &res_shra, res2_shra);
    fail=true;
   }

  }
 }

 return !fail;
}

static bool check_lzx(const BigUInt128 *a, buint_size_t exp_cell, buint_size_t res_cell, buint_size_t exp_bit, buint_size_t res_bit) {
 bool pass = true;
 if (exp_cell != res_cell) {
  print_fun1_error("lzc", a, exp_cell, res_cell);
  pass = false;
 }

 if (exp_bit != res_bit) {
  print_fun1_error("lzb", a, exp_bit, res_bit);
  pass = false;
 }
 return pass;
}

bool test_lzx_single_bit() {
 bool pass = true;

 for (unsigned int i=0; i< BIGUINT128_CELLS; ++i) {
  for (unsigned int j = 0; j<UINT_BITS; ++j) {
   BigUInt128 a = biguint128_ctor_default();
   a.dat[i] = (UInt)1<<(j);
   buint_size_t exp_cell = i+1;
   buint_size_t exp_bit = i*UINT_BITS + j + 1;

   buint_size_t res_cell = biguint128_lzc(&a);
   buint_size_t res_bit = biguint128_lzb(&a);

   pass &= check_lzx(&a, exp_cell, res_cell, exp_bit, res_bit);
  }
 }
 return pass;
}

bool test_lzx_single_bit0() {
 BigUInt128 a = biguint128_ctor_default();

 buint_size_t res_cell = biguint128_lzc(&a);
 buint_size_t res_bit = biguint128_lzb(&a);

 return check_lzx(&a, 0, res_cell, 0, res_bit);
}

int main(int argc, char **argv) {

 assert(test_and_or_not0());
 assert(test_gen_write_get0(false));
 assert(test_gen_write_get0(true));

 assert(test_shiftrot0());

 assert(test_lzx_single_bit());
 assert(test_lzx_single_bit0());
 return 0;
}

