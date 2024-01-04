#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "bigdecimal128.h"
#include "test_common.h"
#include "test_common128.h"

const CStr dec_input[] = {
 STR(""),
 STR("-"),
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
 STR("0"),
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
 char buffer[BIGDECLEN_HI + 1];
 for (int i = 0; i < dec_input_len; ++i) {
  const CStr *input = &dec_input[i];
  const CStr *expected = &dec_printed[i];
  if (DEC_BIGUINTLEN_LO < input->len)
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

bool test_io_lowbuf() {
 bool pass = true;
 char buffer[BIGDECLEN_HI + 1];

 for (int i = 0; i < dec_input_len; ++i) {
  const CStr *input = &dec_input[i];
  size_t exp_len = dec_printed[i].len;
  if (DEC_BIGUINTLEN_LO < input->len)
   continue;
  BigDecimal128 a = bigdecimal128_ctor_cstream(input->str, input->len);
  for (unsigned int len_dec = 1; len_dec <= 2; ++len_dec) {
   if (exp_len < len_dec) continue;
   buint_size_t len = bigdecimal128_print(&a, buffer, exp_len - len_dec);
   if (len != 0) {
    fprintf(stderr, "bigdecimal128_print() error on low buffer size. Params: (%s,buf,%"PRIbuint_size_t") expected: [0], actual: [%"PRIbuint_size_t"] (len_dec: %u)\n", input->str, exp_len - len_dec, len, len_dec);
    pass = false;
   }
  }
 }


 return pass;
}

int main(int argc, char **argv) {

 assert(test_io_dec0());
 assert(test_io_lowbuf());

 return 0;
}

