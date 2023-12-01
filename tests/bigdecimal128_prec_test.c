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

int input_len = sizeof(input) / sizeof(input[0]);

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

int main(int argc, char **argv) {

 assert(test_prec0());

 return 0;
}

