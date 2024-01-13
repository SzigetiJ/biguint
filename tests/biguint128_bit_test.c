#include "biguint128.h"
#include "test_common128.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define BIGUINT_BITS 128
#define UINT_BITS (8*sizeof(UInt))
#define PRINTBUFFER_LEN 80

// Logical bitwise test data
// A, B, A|B, A&B, A^B
#define LSAMPLE_W 5U
const CStr lsamples[][LSAMPLE_W] = {
 {STR("FF0"), STR("F00F"), STR("FFFF"), STR("0"), STR("FFFF")},
 {STR("FFF"), STR("F00F"), STR("FFFF"), STR("F"), STR("FFF0")},
 {STR("3FFFFFFFFFFFFFFFF"), STR("5FFFFFFFFFFFFFFFF"), STR("7FFFFFFFFFFFFFFFF"), STR("1FFFFFFFFFFFFFFFF"), STR("60000000000000000")}
};
const unsigned int lsamples_n = ARRAYSIZE(lsamples);

// Shift test data

#define SHSAMPLE_W 4U
typedef struct {
 buint_size_t bits_limit;
 CStr dat[SHSAMPLE_W];
} ShiftSample;
const ShiftSample shsamples[] = {
 {-1,
  {STR("FFFFFFFF000000000000"), STR("0"), STR("FFFFFFFF000000000000"), STR("FFFFFFFF000000000000")}},
 {-1,
  {STR("1"), STR("1"), STR("2"), STR("0")}},
 {-1,
  {STR("1"), STR("2"), STR("4"), STR("0")}},
 {-1,
  {STR("12345678"), STR("4"), STR("123456780"), STR("1234567")}},
 {-1,
  {STR("FFFFFFFF000000000000"), STR("8"), STR("FFFFFFFF00000000000000"), STR("FFFFFFFF0000000000")}},
 {-1,
  {STR("FFFFFFFF000000000000"), STR("32"), STR("FFFFFFFF00000000000000000000"), STR("FFFFFFFF0000")}},
 {127+1,
  {STR("A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5"), STR("127"), STR("80000000000000000000000000000000"), STR("1")}},
 {-1,
  {STR("5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5"), STR("1"), STR("B4B4B4B4B4B4B4B4B4B4B4B4B4B4B4B4A"), STR("2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2")}}
};
const unsigned int shsamples_n = ARRAYSIZE(shsamples);

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

static bool check_lzx(const BigUInt128 *a, buint_size_t exp_cell, buint_size_t res_cell, buint_size_t exp_bit, buint_size_t res_bit) {
 bool pass = true;
 ArgType paramt = BUINT_XROREF;
 ArgType rest = BUINT_TSIZE;
 GenArgU arg = {.x=*a};
 GenArgU exp_c = {.sz = exp_cell};
 GenArgU act_c = {.sz = res_cell};
 GenArgU exp_b = {.sz = exp_bit};
 GenArgU act_b = {.sz = res_bit};

 if (exp_cell != res_cell) {
  fprintf_biguint128_genfun0_testresult(stderr, "lzc", &arg, &act_c, &exp_c, 1, &paramt, 1, &rest, FMT_HEX);
  pass = false;
 }

 if (exp_bit != res_bit) {
  fprintf_biguint128_genfun0_testresult(stderr, "lzb", &arg, &act_b, &exp_b, 1, &paramt, 1, &rest, FMT_HEX);
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

bool test_lzx_no_bits() {
 BigUInt128 a = biguint128_ctor_default();

 buint_size_t res_cell = biguint128_lzc(&a);
 buint_size_t res_bit = biguint128_lzb(&a);

 return check_lzx(&a, 0, res_cell, 0, res_bit);
}

// composition
static void create_rotated_(CStr *dst, char *buf, unsigned int buflen, const CStr src, buint_size_t shl) {
 BigUInt128 a = biguint128_ctor_hexcstream(src.str, src.len);
 BigUInt128 ahi = biguint128_shl(&a, shl);
 BigUInt128 alo = biguint128_shr(&a, BIGUINT_BITS-shl);
 BigUInt128 res = biguint128_or(&ahi, &alo);
 buint_size_t len = biguint128_print_hex(&res, buf,buflen);
 buf[len]=0;
 dst->str = buf;
 dst->len = len;
}
// checker
static buint_bool check_tiny_(const GenArgU *aa, unsigned int n) {
 return aa[1].sz < UINT_BITS;
}

// wrapper functions
static BigUInt128 nandn_(const BigUInt128 *a, const BigUInt128 *b) {
 BigUInt128 na = biguint128_not(a);
 BigUInt128 nb = biguint128_not(b);
 BigUInt128 naandnb = biguint128_and(&na,&nb);
 return biguint128_not(&naandnb);
};

static BigUInt128 *notnot_asg_(BigUInt128 *a) {
 return biguint128_not_assign(biguint128_not_assign(a));
};

#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
static BigUInt128 nandnv_(const BigUInt128 a, const BigUInt128 b) {
 BigUInt128 na = biguint128_notv(a);
 BigUInt128 nb = biguint128_notv(b);
 BigUInt128 naandnb = biguint128_andv(na,nb);
 return biguint128_notv(naandnb);
};
#endif

int main() {
 unsigned int or_params[]={0,1,2};
 unsigned int and_params[]={0,1,3};
 unsigned int xor_params[]={0,1,4};
 unsigned int id_params[]={0,0};

 assert(test_genfun(&lsamples[0][0], LSAMPLE_W, lsamples_n, FMT_HEX, and_params,XBFUN0(biguint128_and), "and", NULL) == 0);
 assert(test_genfun(&lsamples[0][0], LSAMPLE_W, lsamples_n, FMT_HEX, or_params, XBFUN0(biguint128_or), "or", NULL) == 0);
 assert(test_genfun(&lsamples[0][0], LSAMPLE_W, lsamples_n, FMT_HEX, xor_params, XBFUN0(biguint128_xor), "xor", NULL) == 0);
 assert(test_genfun(&lsamples[0][0], LSAMPLE_W, lsamples_n, FMT_HEX, or_params, XBFUN0(nandn_), "nandofnots", NULL) == 0);

#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
 assert(test_genfun(&lsamples[0][0], LSAMPLE_W, lsamples_n, FMT_HEX, and_params, XBFUN0V(biguint128_andv), "andv", NULL) == 0);
 assert(test_genfun(&lsamples[0][0], LSAMPLE_W, lsamples_n, FMT_HEX, or_params, XBFUN0V(biguint128_orv), "orv", NULL) == 0);
 assert(test_genfun(&lsamples[0][0], LSAMPLE_W, lsamples_n, FMT_HEX, xor_params, XBFUN0V(biguint128_xorv), "xorv", NULL) == 0);
 assert(test_genfun(&lsamples[0][0], LSAMPLE_W, lsamples_n, FMT_HEX, or_params, XBFUN0V(nandnv_), "nandofnotsv", NULL) == 0);
#endif

 assert(test_genfun(&lsamples[0][0], LSAMPLE_W, lsamples_n, FMT_HEX, and_params, XBFUN0A(biguint128_and_assign), "and_assign", NULL) == 0);
 assert(test_genfun(&lsamples[0][0], LSAMPLE_W, lsamples_n, FMT_HEX, or_params, XBFUN0A(biguint128_or_assign), "or_assign", NULL) == 0);
 assert(test_genfun(&lsamples[0][0], LSAMPLE_W, lsamples_n, FMT_HEX, xor_params, XBFUN0A(biguint128_xor_assign), "xor_assign", NULL) == 0);
 assert(test_genfun(&lsamples[0][0], LSAMPLE_W, lsamples_n, FMT_HEX, id_params, XUFUN0A(notnot_asg_), "notnot_assign", NULL) == 0);

// assert(test_and_or_not0());
 assert(test_gen_write_get0(false));
 assert(test_gen_write_get0(true));

// *** SHIFT / ROTATE ***
 unsigned int shl_params[] = {0, 1, 2};
 unsigned int shr_params[] = {0, 1, 3};
 unsigned int rol_params[] = {0, 1, 4};
 unsigned int ror_params[] = {0, 1, 5};

 const unsigned int SHXSAMPLES_W = 6U;
 CStr shxsamples[shsamples_n][SHXSAMPLES_W];
 char rolbuffer[shsamples_n][HEX_BIGUINTLEN+1];
 char rorbuffer[shsamples_n][HEX_BIGUINTLEN+1];
 for (unsigned int i = 0; i < shsamples_n; ++i) {
  for (unsigned int j = 0; j < SHSAMPLE_W; ++j) {
   shxsamples[i][j] = shsamples[i].dat[j];
  }
  if (shsamples[i].bits_limit < BIGUINT_BITS) { // disable inputs if test suite exceeds size limit
   shxsamples[i][0].len = 0;
  }
  unsigned int shift = atoi(shsamples[i].dat[1].str);
  create_rotated_(&shxsamples[i][4], rolbuffer[i], HEX_BIGUINTLEN, shxsamples[i][0], shift);
  create_rotated_(&shxsamples[i][5], rorbuffer[i], HEX_BIGUINTLEN, shxsamples[i][0], BIGUINT_BITS - shift);
 }
 assert(test_genfun(&shxsamples[0][0], SHXSAMPLES_W, shsamples_n, FMT_HEX, shl_params, XBFUN1(biguint128_shl), "shl", NULL) == 0);
 assert(test_genfun(&shxsamples[0][0], SHXSAMPLES_W, shsamples_n, FMT_HEX, shr_params, XBFUN1(biguint128_shr), "shr", NULL) == 0);
 assert(test_genfun(&shxsamples[0][0], SHXSAMPLES_W, shsamples_n, FMT_HEX, rol_params, XBFUN1(biguint128_rol), "rol", NULL) == 0);
 assert(test_genfun(&shxsamples[0][0], SHXSAMPLES_W, shsamples_n, FMT_HEX, ror_params, XBFUN1(biguint128_ror), "ror", NULL) == 0);
 assert(test_genfun(&shxsamples[0][0], SHXSAMPLES_W, shsamples_n, FMT_HEX, shr_params, XBFUN1A(biguint128_shr_assign), "shr_assign", NULL) == 0);
 assert(test_genfun(&shxsamples[0][0], SHXSAMPLES_W, shsamples_n, FMT_HEX, shl_params, XBFUN1A(biguint128_shl_tiny), "shl_tiny", check_tiny_) == 0);
 assert(test_genfun(&shxsamples[0][0], SHXSAMPLES_W, shsamples_n, FMT_HEX, shr_params, XBFUN1A(biguint128_shr_tiny), "shr_tiny", check_tiny_) == 0);

#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
 assert(test_genfun(&shxsamples[0][0], SHXSAMPLES_W, shsamples_n, FMT_HEX, shl_params, XBFUN1V(biguint128_shlv), "shlv", NULL) == 0);
 assert(test_genfun(&shxsamples[0][0], SHXSAMPLES_W, shsamples_n, FMT_HEX, shr_params, XBFUN1V(biguint128_shrv), "shrv", NULL) == 0);
 assert(test_genfun(&shxsamples[0][0], SHXSAMPLES_W, shsamples_n, FMT_HEX, rol_params, XBFUN1V(biguint128_rolv), "rolv", NULL) == 0);
 assert(test_genfun(&shxsamples[0][0], SHXSAMPLES_W, shsamples_n, FMT_HEX, ror_params, XBFUN1V(biguint128_rorv), "rorv", NULL) == 0);
#endif

 assert(test_lzx_single_bit());
 assert(test_lzx_no_bits());
 return 0;
}
