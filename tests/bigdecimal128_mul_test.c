#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "bigdecimal128.h"
#include "test_common.h"

#define BIGUINT_BITS 128
#define BIGDECCAP ((BIGUINT_BITS / 10 + 1) * 3 + 1)
#define BUFLEN (BIGDECCAP + 3)

// INTERNAL TYPES

typedef struct {
 CStr num1;
 CStr num2;
 UInt resprec;
} TestInputType;

typedef struct {
 CStr num;
} TestOutputType;

typedef BigDecimal128 (*BigDecimalFun)(const BigDecimal128 *a, const BigDecimal128 *b, UInt prec);

// TEST DATA
const TestInputType any_in[] = {
 {STR("20"),STR("0.5"),0},
 {STR("0.2"),STR("0.6"),3},
 {STR("10"),STR("30"),5},
 {STR("-1.5"),STR("1.5"),3},
 {STR("2.0"),STR("-0.5"),1},
 {STR("-10"),STR("0.001"),2},
 {STR("0.000"),STR("10.00"),4}
};

const TestOutputType mul_out[] = {
 {STR("10.0")},
 {STR("0.12")},
 {STR("300")},
 {STR("-2.25")},
 {STR("-1.00")},
 {STR("-0.010")},
 {STR("0.00000")}
};

const TestOutputType div_out[] = {
 {STR("40")},
 {STR("0.333")},
 {STR("0.33333")},
 {STR("-1.000")},
 {STR("-4.0")},
 {STR("-10000.00")},
 {STR("0.0000")}
};

int input_len = sizeof(any_in) / sizeof(any_in[0]);

// INTERNAL FUNCTIONS
/**
 * @return true: test failed, false: test passed.
 */
static inline bool eval_divtestcase_(const TestInputType *tin, const TestOutputType *expected, BigDecimalFun fun, char *funstr) {
 char buffer[BUFLEN + 1];
 if (BIGDECCAP < tin->num1.len || BIGDECCAP < tin->num2.len)
  return false;

 BigDecimal128 a = bigdecimal128_ctor_cstream(tin->num1.str, tin->num1.len);
 BigDecimal128 b = bigdecimal128_ctor_cstream(tin->num2.str, tin->num2.len);

 BigDecimal128 quotient = fun(&a, &b, tin->resprec);
 buint_size_t len = bigdecimal128_print(&quotient, buffer, sizeof(buffer) / sizeof(char) - 1);
 buffer[len] = 0;

 int result = strcmp(expected->num.str, buffer);

 if (result != 0) {
  fprintf(stderr, "input: %s(%s, %s , %"PRIuint"); expected output: [%s], actual [%s]\n",
   funstr, tin->num1.str, tin->num2.str, tin->resprec, expected->num.str, buffer);
  return true;
 }
 return false;
}

// TEST FUNCTIONS
bool test_mul0() {
 bool fail = false;
 char buffer[BUFLEN + 1];
 for (int i = 0; i < input_len; ++i) {
  const TestInputType *ti = &any_in[i];
  const TestOutputType *expected = &mul_out[i];
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
 for (int i = 0; i < input_len; ++i) {
  bool testfailed = eval_divtestcase_(&any_in[i], &div_out[i], bigdecimal128_div, "div");
  fail|=testfailed;
 }
 return !fail;
}

bool test_div_fast0() {
 bool fail = false;
 for (int i = 0; i < input_len; ++i) {
  bool testfailed = eval_divtestcase_(&any_in[i], &div_out[i], bigdecimal128_div_fast, "div");
  fail|=testfailed;
 }
 return !fail;
}

int main(int argc, char **argv) {

 assert(test_mul0());
 assert(test_div0());
 assert(test_div_fast0());

 return 0;
}

