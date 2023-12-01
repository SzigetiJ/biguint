#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "bigdecimal128.h"
#include "test_common.h"

#define BIGUINT_BITS 128
#define BIGDECLEN ((BIGUINT_BITS / 10 + 1) * 3 + 4)


const CStr dec_input[] = {
 STR("0"),
 STR("0.0"),
 STR("-0"),
 STR("-0.0"),
 STR("+0.0"),
 STR("1.1"),
 STR("0.1"),
 STR("-0.1"),
 STR("0.00000000000000000000000001"),
 STR("-0.00000000000000000000000001"),
 STR("10000000000000.0000000000001"),
 STR("-10000000000000.0000000000001")
};

const CStr dec_printed[] = {
 STR("0"),
 STR("0.0"),
 STR("0"),
 STR("0.0"),
 STR("0.0"),
 STR("1.1"),
 STR("0.1"),
 STR("-0.1"),
 STR("0.00000000000000000000000001"),
 STR("-0.00000000000000000000000001"),
 STR("10000000000000.0000000000001"),
 STR("-10000000000000.0000000000001")
};
int dec_input_len = sizeof(dec_input) / sizeof(dec_input[0]);

// Assert print(parse(dec_str))==dec_str.
bool test_io_dec0() {
 bool fail = false;
 char buffer[BIGDECLEN + 1];
 for (int i = 0; i < dec_input_len; ++i) {
  const CStr *input = &dec_input[i];
  const CStr *expected = &dec_printed[i];
  if (BIGDECLEN < input->len)
   continue;
  BigDecimal128 a = bigdecimal128_ctor_cstream(input->str, input->len);
  buint_size_t len = bigdecimal128_print(&a, buffer, sizeof(buffer) / sizeof(char) - 1);
  buffer[len] = 0;
  int result = strcmp(expected->str, buffer);
  if (result != 0) {
   fprintf(stderr, "expected: [%s], actual [%s]\n", expected->str, buffer);
   fail = true;
  }
 }
 return !fail;
}

int main(int argc, char **argv) {

 assert(test_io_dec0());

 return 0;
}

