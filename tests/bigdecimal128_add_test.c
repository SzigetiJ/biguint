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
} PrecTestInputType;

typedef struct {
 CStr num;
} PrecTestOutputType;

const PrecTestInputType any_in[] = {
 {STR("0"),STR("0")},
 {STR("1"),STR("0.1")},
 {STR("10"),STR("0.01")},
 {STR("10"),STR("-0.1")},
 {STR("-10"),STR("0.1")},
 {STR("-10"),STR("0.000")},
 {STR("0.000"),STR("10.00")},
 {STR("3.55"),STR("3.45")}
};

const PrecTestOutputType add_out[] = {
 {STR("0")},
 {STR("1.1")},
 {STR("10.01")},
 {STR("9.9")},
 {STR("-9.9")},
 {STR("-10.000")},
 {STR("10.000")},
 {STR("7.00")}
};

const PrecTestOutputType sub_out[] = {
 {STR("0")},
 {STR("0.9")},
 {STR("9.99")},
 {STR("10.1")},
 {STR("-10.1")},
 {STR("-10.000")},
 {STR("-10.000")},
 {STR("0.10")}
};

int input_len = sizeof(any_in) / sizeof(any_in[0]);

bool test_add0() {
 bool fail = false;
 char buffer[BUFLEN + 1];
 for (int i = 0; i < input_len; ++i) {
  const PrecTestInputType *ti = &any_in[i];
  const PrecTestOutputType *expected = &add_out[i];
  if (BIGDECCAP < ti->num1.len || BIGDECCAP < ti->num2.len)
   continue;

  BigDecimal128 a = bigdecimal128_ctor_cstream(ti->num1.str, ti->num1.len);
  BigDecimal128 b = bigdecimal128_ctor_cstream(ti->num2.str, ti->num2.len);

  BigDecimal128 sum = bigdecimal128_add(&a, &b);

  buint_size_t len = bigdecimal128_print(&sum, buffer, sizeof(buffer) / sizeof(char) - 1);
  buffer[len] = 0;

  int result = strcmp(expected->num.str, buffer);

  if (result != 0) {
   fprintf(stderr, "input: (%s + %s); expected output: [%s], actual [%s]\n",
    ti->num1.str, ti->num2.str, expected->num.str, buffer);
   fail = true;
  }
 }
 return !fail;
}

bool test_sub0() {
 bool fail = false;
 char buffer[BUFLEN + 1];
 for (int i = 0; i < input_len; ++i) {
  const PrecTestInputType *ti = &any_in[i];
  const PrecTestOutputType *expected = &sub_out[i];
  if (BIGDECCAP < ti->num1.len || BIGDECCAP < ti->num2.len)
   continue;

  BigDecimal128 a = bigdecimal128_ctor_cstream(ti->num1.str, ti->num1.len);
  BigDecimal128 b = bigdecimal128_ctor_cstream(ti->num2.str, ti->num2.len);

  BigDecimal128 diff = bigdecimal128_sub(&a, &b);

  buint_size_t len = bigdecimal128_print(&diff, buffer, sizeof(buffer) / sizeof(char) - 1);
  buffer[len] = 0;

  int result = strcmp(expected->num.str, buffer);

  if (result != 0) {
   fprintf(stderr, "input: (%s + %s); expected output: [%s], actual [%s]\n",
    ti->num1.str, ti->num2.str, expected->num.str, buffer);
   fail = true;
  }
 }
 return !fail;
}

int main(int argc, char **argv) {

 assert(test_add0());
 assert(test_sub0());

 return 0;
}

