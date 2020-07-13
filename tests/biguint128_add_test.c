#include "biguint128.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define BIGUINT_BITS 128

#define MAX_STRLEN 255

const char* const hex_samples[][3] = {
 { "0", "1", "1"},
 { "1", "0", "1"},
 { "0", "FFFFFFFF", "FFFFFFFF"},
 { "1", "FFFFFFFF", "100000000"},
 { "FFFFFFFF", "FFFFFFFF", "1FFFFFFFE"},
 { "100000000", "200000001", "300000001"},
 { "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE", "1", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"},
 { "EFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "1", "F0000000000000000000000000000000"},
 { "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE", "1", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"},
 { "EFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "1", "F000000000000000000000000000000000000000000000000000000000000000"}
};
int hex_sample_len = sizeof(hex_samples) / (sizeof(char*[3]));

bool read_hex_or_fail(const char * const hexstr, BigUInt128 *result) {
  size_t hexstrlen = strlen(hexstr);
  if (hexstrlen == 0 || BIGUINT_BITS / 4 < hexstrlen) {
    return false;
  }
  *result = biguint128_ctor_hexcstream(hexstr, hexstrlen);
  return true;
}

void fprintf_biguint128_binop_testresult(FILE *out, BigUInt128 *op0, BigUInt128 *op1, BigUInt128 *expected, BigUInt128 *actual, const char *op_str) {
   char buffer[4][BIGUINT_BITS / 4 + 1];
   BigUInt128 *v_refs[] = {op0, op1, expected, actual};
   for (int j = 0; j < 4; ++j) {
     buffer[j][biguint128_print_hex(v_refs[j], buffer[j], sizeof(buffer[j])/sizeof(char)-1)]=0;
   }
   fprintf(out, "[%s %s %s] -- expected: [%s], actual [%s]\n", buffer[0], op_str, buffer[1], buffer[2], buffer[3]);
}

int test_addsub0() {
 int fail = 0;
 for (int i=0; i<hex_sample_len; ++i) {
   BigUInt128 a, b, c;
   BigUInt128 sum, diff;
   BigUInt128 *value_refs[] = {&a, &b, &c};
   bool readOk = true;

   for (int j = 0; j<3; ++j) {
     readOk&=read_hex_or_fail(hex_samples[i][j], value_refs[j]);
   }
   if (!readOk) continue;

   // operation
  sum = biguint128_add(&a, &b);
  diff = biguint128_sub(&c, &b);

  // eval sum
  buint_bool result_sum = biguint128_eq(&c, &sum);
  if (!result_sum) {
    fprintf_biguint128_binop_testresult(stderr, &a, &b, &c, &sum, "+");
   fail = 1;
  }

  buint_bool result_diff = biguint128_eq(&a, &diff);
  if (!result_diff) {
    fprintf_biguint128_binop_testresult(stderr, &a, &b, &c, &sum, "-");
   fail = 1;
  }
 }

 return fail;
}

int main(int argc, char **argv) {

 assert(test_addsub0() == 0);

 return 0;
}

