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
 CStr num1;
 CStr num2;
 UInt resprec;
} PrecTestInputType;

typedef struct {
 CStr num;
} PrecTestOutputType;

const PrecTestInputType any_in[] = {
 {STR("20"),STR("0.5"),0},
 {STR("0.2"),STR("0.6"),3},
 {STR("10"),STR("30"),5},
 {STR("-1.5"),STR("1.5"),3},
 {STR("2.0"),STR("-0.5"),1},
 {STR("-10"),STR("0.001"),2},
 {STR("0.000"),STR("10.00"),4}
};

const PrecTestOutputType mul_out[] = {
 {STR("10.0")},
 {STR("0.12")},
 {STR("300")},
 {STR("-2.25")},
 {STR("-1.00")},
 {STR("-0.010")},
 {STR("0.00000")}
};

const PrecTestOutputType div_out[] = {
 {STR("40")},
 {STR("0.333")},
 {STR("0.33333")},
 {STR("-1.000")},
 {STR("-4.0")},
 {STR("-10000.00")},
 {STR("0.0000")}
};

int input_len = sizeof(any_in) / sizeof(any_in[0]);

bool test_mul0() {
 bool fail = false;
 char buffer[BUFLEN + 1];
 for (int i = 0; i < input_len; ++i) {
  const PrecTestInputType *ti = &any_in[i];
  const PrecTestOutputType *expected = &mul_out[i];
  if (BIGDECCAP < ti->num1.len || BIGDECCAP < ti->num2.len)
   continue;

  BigDecimal128 a = bigdecimal128_ctor_cstream(ti->num1.str, ti->num1.len);
  BigDecimal128 b = bigdecimal128_ctor_cstream(ti->num2.str, ti->num2.len);

  BigDecimal128 prod = bigdecimal128_mul(&a, &b);

  buint_size_t len = bigdecimal128_print(&prod, buffer, sizeof(buffer) / sizeof(char) - 1);
  buffer[len] = 0;

  int result = strcmp(expected->num.str, buffer);

  if (result != 0) {
   fprintf(stderr, "input: (%s * %s); expected output: [%s], actual [%s]\n",
    ti->num1.str, ti->num2.str, expected->num.str, buffer);
   fail = true;
  }
 }
 return !fail;
}

bool test_div0() {
 bool fail = false;
 char buffer[BUFLEN + 1];
 char buffer_fast[BUFLEN + 1];
 for (int i = 0; i < input_len; ++i) {
  const PrecTestInputType *ti = &any_in[i];
  const PrecTestOutputType *expected = &div_out[i];
  if (BIGDECCAP < ti->num1.len || BIGDECCAP < ti->num2.len)
   continue;

  BigDecimal128 a = bigdecimal128_ctor_cstream(ti->num1.str, ti->num1.len);
  BigDecimal128 b = bigdecimal128_ctor_cstream(ti->num2.str, ti->num2.len);

  BigDecimal128 quotient = bigdecimal128_div(&a, &b, ti->resprec);
  buint_size_t len = bigdecimal128_print(&quotient, buffer, sizeof(buffer) / sizeof(char) - 1);
  buffer[len] = 0;

  BigDecimal128 quotient_fast = bigdecimal128_div_fast(&a, &b, ti->resprec);
  buint_size_t len_fast = bigdecimal128_print(&quotient_fast, buffer_fast, sizeof(buffer_fast) / sizeof(char) - 1);
  buffer_fast[len_fast] = 0;

  int result = strcmp(expected->num.str, buffer);
  int result_fast = strcmp(expected->num.str, buffer_fast);

  if (result != 0) {
   fprintf(stderr, "input: (%s / %s , %"PRIuint"); expected output: [%s], actual [%s]\n",
    ti->num1.str, ti->num2.str, ti->resprec, expected->num.str, buffer);
   fail = true;
  }

  if (result_fast != 0) {
   fprintf(stderr, "input: (%s / %s , %"PRIuint" (fast)); expected output: [%s], actual [%s]\n",
    ti->num1.str, ti->num2.str, ti->resprec, expected->num.str, buffer_fast);
   fail = true;
  }

 }
 return !fail;
}

int main(int argc, char **argv) {

 assert(test_mul0());
 assert(test_div0());

 return 0;
}

