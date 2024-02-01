#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "bigdecimal128.h"
#include "test_common.h"

#define BIGUINT_BITS 128
#define BIGDECCAP ((BIGUINT_BITS / 10 + 1) * 3 + 1)
#define BUFLEN (BIGDECCAP + 3)

typedef struct {
 CStr num;
 UInt prec;
} PrecTestInputType;

typedef struct {
 CStr num;
} PrecTestOutputType;

const PrecTestInputType input[] = {
 {STR("0"),0},
 {STR("0"),1},
 {STR("0.0"),0},
 {STR("0.0"),1},
 {STR("-0"),0},
 {STR("-0"),3},
 {STR("1.1"),4},
 {STR("0.00000000000000000000000001"),0},
 {STR("-0.00000000000000000000000001"),0},
 {STR("10000000000000.0000000000001"),0},
 {STR("-10000000000000.0000000000001"),0}
};

const PrecTestOutputType output[] = {
 {STR("0")},
 {STR("0.0")},
 {STR("0")},
 {STR("0.0")},
 {STR("0")},
 {STR("0.000")},
 {STR("1.1000")},
 {STR("0")},
 {STR("0")},
 {STR("10000000000000")},
 {STR("-10000000000000")}
};

const CStr trunc_samples[][3] = {
 {STR("0.00"),STR("0"),STR("0")},
 {STR("10"),STR("10"),STR("0")},
 {STR("-21"),STR("-21"),STR("0")},
 {STR("-12.004"),STR("-12"),STR("-4")},
 {STR("51.300"),STR("51"),STR("300")},
 {STR("44.3035"),STR("44"),STR("3035")},
 {STR("-0.00000000000000000000012"),STR("0"),STR("-12")}
};

int input_len = ARRAYSIZE(input);

// Assert print(prec(parse(input.str),input.prec))==output.
bool test_prec0() {
 bool fail = false;
 char buffer[BUFLEN + 1];
 for (int i = 0; i < input_len; ++i) {
  const PrecTestInputType *ti = &input[i];
  const CStr *expected = &output[i].num;
  if (BIGDECCAP < ti->num.len)
   continue;
  BigDecimal128 a = bigdecimal128_ctor_cstream(ti->num.str, ti->num.len);
  BigDecimal128 b = bigdecimal128_ctor_prec(&a, ti->prec);
  buint_size_t len = bigdecimal128_print(&b, buffer, sizeof(buffer) / sizeof(char) - 1);
  buffer[len] = 0;
  int result = strcmp(expected->str, buffer);
  if (result != 0) {
   fprintf(stderr, "input: (%s,%"PRIuint"); expected output: [%s], actual [%s]\n",
    ti->num.str, ti->prec, expected->str, buffer);
   fail = true;
  }
 }
 return !fail;
}

bool test_prec_safe(unsigned int init_prec, unsigned int str_crop) {
 bool pass = true;

 BigUInt128 a = bigint128_value_of_uint(-1);   // FFFFFF..FFF
 BigUInt128 max_bint = biguint128_shr(&a, 1);  // 7FFFFF..FFF
 BigUInt128 min_bint = max_bint;
 biguint128_inc(&min_bint);                    // 800000..000

 char buf[BUFLEN + 1];  // for error printing
 // max_bint as deccstr
 char bufp[BUFLEN + 1];
 unsigned int bufplen = bigint128_print_dec(&max_bint, bufp, BUFLEN);
 bufp[bufplen] = 0;
 // min_bint as deccstr
 char bufn[BUFLEN + 1];
 unsigned int bufnlen = bigint128_print_dec(&min_bint, bufn, BUFLEN);
 bufp[bufnlen] = 0;

 BigDecimal128 maxd_orig = bigdecimal128_ctor_cstream(bufp, bufplen - str_crop);
 maxd_orig.prec = init_prec;
 BigDecimal128 mind_orig = bigdecimal128_ctor_cstream(bufn, bufnlen - str_crop);
 mind_orig.prec = init_prec;

 for (UInt i = 0; i < init_prec + str_crop + 5; ++i) {
  BigDecimal128 max_di;
  BigDecimal128 min_di;
  bool max_ok = bigdecimal128_prec_safe(&max_di, &maxd_orig, i);
  bool min_ok = bigdecimal128_prec_safe(&min_di, &mind_orig, i);
  if (!!max_ok != !!(0 <= (int) (init_prec + str_crop - i))) {
   fprintf(stderr, "original: %.*s.%.*s\n", bufplen - init_prec - str_crop, bufp, init_prec, bufp + bufplen - init_prec - str_crop);
   buf[bigdecimal128_print(&max_di, buf, BUFLEN)] = 0;
   fprintf(stderr, "prec_safe(%u): %s\n", (unsigned int) i, max_ok ? buf : "OVERFLOW");
   if ((init_prec + str_crop - i) != 0) { // this is edge case, it is allowed to fail.
    pass = false;
   }
  }
  if (!!min_ok != !!(0 <= (int) (init_prec + str_crop - i))) {
   fprintf(stderr, "original: %.*s.%.*s\n", bufnlen - init_prec - str_crop, bufn, init_prec, bufn + bufnlen - init_prec - str_crop);
   buf[bigdecimal128_print(&min_di, buf, BUFLEN)] = 0;
   fprintf(stderr, "prec_safe(%u): %s\n", (unsigned int) i, min_ok ? buf : "OVERFLOW");
   if ((init_prec + str_crop - i) != 0) { // this is edge case, it is allowed to fail.
    pass = false;
   }
  }
 }
 return pass;
}

bool test_trunc() {
 bool pass = true;
 for (unsigned int i = 0; i < ARRAYSIZE(trunc_samples); ++i) {
  BigDecimal128 a = bigdecimal128_ctor_cstream(trunc_samples[i][0].str, trunc_samples[i][0].len);
  BigUInt128 exp_int = bigint128_ctor_deccstream(trunc_samples[i][1].str, trunc_samples[i][1].len);
  BigUInt128 exp_frac = bigint128_ctor_deccstream(trunc_samples[i][2].str, trunc_samples[i][2].len);

  BigUIntPair128 result = bigdecimal128_trunc(&a);

  if (!biguint128_eq(&exp_int, &result.first) || !biguint128_eq(&exp_frac, &result.second)) {
   char buf_int[BUFLEN + 1];
   char buf_frac[BUFLEN + 1];
   buf_int[bigint128_print_dec(&result.first, buf_int, BUFLEN)] = 0;
   buf_frac[bigint128_print_dec(&result.second, buf_frac, BUFLEN)] = 0;
   fprintf(stderr, "trunc(%s) failed. expected: (%s, %s), actual: (%s, %s)\n",
     trunc_samples[i][0].str,
     trunc_samples[i][1].str,
     trunc_samples[i][2].str,
     buf_int, buf_frac
     );
   pass = false;
  }
 }
 return pass;
}

int main() {

 assert(test_prec0());
 assert(test_prec_safe(7,2));
 assert(test_prec_safe(8,4));
 assert(test_prec_safe(8,20));
 assert(test_trunc());

 return 0;
}

