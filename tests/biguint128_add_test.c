#include "biguint128.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"

#define BIGUINT_BITS 128

char *hex_samples[][3]={
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
int hex_sample_len = sizeof(hex_samples) / (3 * sizeof(char*));

int test_addsub0() {
 int fail = 0;
 for (int i=0; i<hex_sample_len; ++i) {
  char *sample_a = hex_samples[i][0];
  char *sample_b = hex_samples[i][1];
  char *sample_c = hex_samples[i][2];
  if (BIGUINT_BITS / 4 < strlen(sample_a) || BIGUINT_BITS / 4 < strlen(sample_b) || BIGUINT_BITS / 4 < strlen(sample_c))
   continue;
  BigUInt128 a = biguint128_ctor_hexcstream(sample_a, strlen(sample_a));
  BigUInt128 b = biguint128_ctor_hexcstream(sample_b, strlen(sample_b));
  BigUInt128 c = biguint128_ctor_hexcstream(sample_c, strlen(sample_c));
  BigUInt128 sum = biguint128_add(&a, &b);
  BigUInt128 diff = biguint128_sub(&c, &b);
  buint_bool result_sum = biguint128_eq(&c, &sum);
  if (!result_sum) {
   char buffer[4][BIGUINT_BITS / 4 + 1];
   buffer[0][biguint128_print_hex(&a, buffer[0], sizeof(buffer[0])/sizeof(char))]=0;
   buffer[1][biguint128_print_hex(&b, buffer[1], sizeof(buffer[1])/sizeof(char))]=0;
   buffer[2][biguint128_print_hex(&c, buffer[2], sizeof(buffer[2])/sizeof(char))]=0;
   buffer[3][biguint128_print_hex(&sum, buffer[3], sizeof(buffer[3])/sizeof(char))]=0;
   fprintf(stderr, "[%s + %s] -- expected: [%s], actual [%s]\n", buffer[0], buffer[1], buffer[2], buffer[3]);
   fail = 1;
  }
  buint_bool result_diff = biguint128_eq(&a, &diff);
  if (!result_diff) {
   char buffer[4][BIGUINT_BITS / 4 + 1];
   buffer[0][biguint128_print_hex(&c, buffer[0], sizeof(buffer[0])/sizeof(char))]=0;
   buffer[1][biguint128_print_hex(&b, buffer[1], sizeof(buffer[1])/sizeof(char))]=0;
   buffer[2][biguint128_print_hex(&a, buffer[2], sizeof(buffer[2])/sizeof(char))]=0;
   buffer[3][biguint128_print_hex(&diff, buffer[3], sizeof(buffer[3])/sizeof(char))]=0;
   fprintf(stderr, "[%s - %s] -- expected: [%s], actual [%s]\n", buffer[0], buffer[1], buffer[2], buffer[3]);
   fail = 1;
  }
 }

 return fail;
}

int main(int argc, char **argv) {

 assert(test_addsub0() == 0);

 return 0;
};

