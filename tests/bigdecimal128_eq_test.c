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
 buint_bool rel;
} PrecTestOutputType;

const PrecTestInputType any_in[] = {
 {STR("20"),STR("20")},
 {STR("0.2"),STR("-0.2")},
 {STR("0.0"),STR("-0.0")},
 {STR("+0.0"),STR("0.0")},
 {STR("0.0"),STR("-0.00")},
 {STR("10.0"),STR("10")},
 {STR("10.0"),STR("10.00")},
 {STR("10.0"),STR("10.0")},
 {STR("10.0"),STR("9")},
 {STR("10.0"),STR("9.9")},
 {STR("10.0"),STR("11")},
 {STR("10.0"),STR("10.0000001")}
};

const PrecTestOutputType eq_out[] = {
 {1},
 {0},
 {1},
 {1},
 {1},
 {1},
 {1},
 {1},
 {0},
 {0},
 {0},
 {0}
};

const PrecTestOutputType lt_out[] = {
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {1},
 {1}
};

int input_len = sizeof(any_in) / sizeof(any_in[0]);

bool test_eq0() {
 bool fail = false;
 for (int i = 0; i < input_len; ++i) {
  const PrecTestInputType *ti = &any_in[i];
  const PrecTestOutputType *expected = &eq_out[i];
  if (BIGDECCAP < ti->num1.len || BIGDECCAP < ti->num2.len)
   continue;

  BigDecimal128 a = bigdecimal128_ctor_cstream(ti->num1.str, ti->num1.len);
  BigDecimal128 b = bigdecimal128_ctor_cstream(ti->num2.str, ti->num2.len);

  buint_bool eq = bigdecimal128_eq(&a, &b);

  int result = (!!eq != !!expected->rel);

  if (result != 0) {
   fprintf(stderr, "input: (%s == %s); expected output: [%s], actual [%s]\n",
    ti->num1.str, ti->num2.str, bool_to_str(expected->rel), bool_to_str(eq));
   fail = true;
  }
 }
 return !fail;
}

bool test_eq1() {
 bool fail = false;

 BigDecimal128 a = bigdecimal128_ctor_cstream("0",1);
 BigDecimal128 b = bigdecimal128_ctor_default();

 buint_bool eq = bigdecimal128_eq(&a, &b);

 int result = (!eq);

 if (result != 0) {
  char buffer[BUFLEN + 1];
  buint_size_t len = bigdecimal128_print(&b, buffer, sizeof(buffer) / sizeof(char) - 1);
  buffer[len] = 0;
  fprintf(stderr, "input: (default ctor); expected output: [%s], actual [%s]\n",
   "0", buffer);
  fail = true;
 }

 return !fail;
}


bool test_lt0() {
 bool fail = false;
 for (int i = 0; i < input_len; ++i) {
  const PrecTestInputType *ti = &any_in[i];
  const PrecTestOutputType *expected = &lt_out[i];
  if (BIGDECCAP < ti->num1.len || BIGDECCAP < ti->num2.len)
   continue;

  BigDecimal128 a = bigdecimal128_ctor_cstream(ti->num1.str, ti->num1.len);
  BigDecimal128 b = bigdecimal128_ctor_cstream(ti->num2.str, ti->num2.len);

  buint_bool lt = bigdecimal128_lt(&a, &b);

  int result = (!!lt != !!expected->rel);

  if (result != 0) {
   fprintf(stderr, "input: (%s < %s); expected output: [%s], actual [%s]\n",
    ti->num1.str, ti->num2.str, bool_to_str(expected->rel), bool_to_str(lt));
   fail = true;
  }
 }
 return !fail;
}

int main(int argc, char **argv) {

 assert(test_eq0());
 assert(test_lt0());
 assert(test_eq1());

 return 0;
}

