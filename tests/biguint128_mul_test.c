#include "biguint128.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define BIGUINT_BITS 128
#define BIGUINT_STRLEN_MAX (BIGUINT_BITS / 4)

const char* const hex_samples[][3] = {
 { "0", "1", "0"},
 { "1", "1", "1"},
 { "1", "FFFFFFFF", "FFFFFFFF"},
 { "2", "FFFFFFFF", "1FFFFFFFE"},
 { "FFFFFFFF", "FFFFFFFF", "FFFFFFFE00000001"},
 { "100000001", "200000001", "20000000300000001"},
 { "400000005", "600000003", "180000002A0000000F"}
};
int hex_sample_len = sizeof(hex_samples) / (sizeof(char*[3]));

bool test_mul0() {
 bool fail = false;
 for (int i=0; i<hex_sample_len; ++i) {
  const char * const sample_a = hex_samples[i][0];
  const char * const sample_b = hex_samples[i][1];
  const char * const sample_c = hex_samples[i][2];
  if (BIGUINT_STRLEN_MAX < strlen(sample_a)
   || BIGUINT_STRLEN_MAX < strlen(sample_b)
   || BIGUINT_STRLEN_MAX < strlen(sample_c)
  )
   continue;
  BigUInt128 a = biguint128_ctor_hexcstream(sample_a, strlen(sample_a));
  BigUInt128 b = biguint128_ctor_hexcstream(sample_b, strlen(sample_b));
  BigUInt128 c = biguint128_ctor_hexcstream(sample_c, strlen(sample_c));

  BigUInt128 prod = biguint128_mul(&a, &b);
  BigUInt128 prod_rev = biguint128_mul(&b, &a);

  buint_bool result_eq = biguint128_eq(&c, &prod);
  buint_bool result_eq_rev = biguint128_eq(&c, &prod_rev);

  if (!result_eq) {
   char buffer[4][BIGUINT_STRLEN_MAX + 1];
   buffer[0][biguint128_print_hex(&a, buffer[0], sizeof(buffer[0])/sizeof(char)-1)]=0;
   buffer[1][biguint128_print_hex(&b, buffer[1], sizeof(buffer[1])/sizeof(char)-1)]=0;
   buffer[2][biguint128_print_hex(&c, buffer[2], sizeof(buffer[2])/sizeof(char)-1)]=0;
   buffer[3][biguint128_print_hex(&prod, buffer[3], sizeof(buffer[3])/sizeof(char)-1)]=0;
   fprintf(stderr, "[%s * %s] -- expected: [%s], actual [%s]\n", buffer[0], buffer[1], buffer[2], buffer[3]);
   fail = true;
  }

  if (!result_eq_rev) {
   char buffer[4][BIGUINT_STRLEN_MAX + 1];
   buffer[0][biguint128_print_hex(&b, buffer[0], sizeof(buffer[0])/sizeof(char)-1)]=0;
   buffer[1][biguint128_print_hex(&a, buffer[1], sizeof(buffer[1])/sizeof(char)-1)]=0;
   buffer[2][biguint128_print_hex(&c, buffer[2], sizeof(buffer[2])/sizeof(char)-1)]=0;
   buffer[3][biguint128_print_hex(&prod_rev, buffer[3], sizeof(buffer[3])/sizeof(char)-1)]=0;
   fprintf(stderr, "[%s * %s] -- expected: [%s], actual [%s]\n", buffer[0], buffer[1], buffer[2], buffer[3]);
   fail = true;
  }
 }

 return !fail;
}

bool test_mul1() {
 bool fail = false;
 BigUInt128 a0 = biguint128_ctor_default(); 
 BigUInt128 a1 = biguint128_ctor_unit(); 
 BigUInt128 max = biguint128_sub(&a0, &a1);
 char max_str[BIGUINT_STRLEN_MAX + 1];
 max_str[biguint128_print_hex(&max, max_str, sizeof(max_str)/sizeof(max_str[0])-1)]=0;

 for (buint_size_t i = 0; i < BIGUINT_STRLEN_MAX; ++i) {
  char buffer[BIGUINT_STRLEN_MAX + 1];
  char expected[BIGUINT_STRLEN_MAX + 1];
  
  BigUInt128 mult = biguint128_shl(&a1, 4*i);
  BigUInt128 prod = biguint128_mul(&max, &mult);

  for (buint_size_t digit = 0; digit < BIGUINT_STRLEN_MAX; ++digit) {
   expected[digit] = digit + i < BIGUINT_STRLEN_MAX ? 'F' : '0';
  }
  expected[BIGUINT_STRLEN_MAX]=0;

  buffer[biguint128_print_hex(&prod, buffer, sizeof(buffer)/sizeof(buffer[0])-1)]=0;
  
  if (strcmp(expected,buffer)!=0) {
   char mult_str[BIGUINT_STRLEN_MAX + 1];
   mult_str[biguint128_print_hex(&mult, mult_str, sizeof(mult_str)/sizeof(mult_str[0])-1)]=0;
   fprintf(stderr, "[%s * %s] -- expected: [%s], actual [%s]\n", max_str, mult_str, expected, buffer);
   fail = true;
  }  
 }
 return !fail;
}

bool test_mul2() {
 bool fail = false;
 BigUInt128 a0 = biguint128_ctor_default(); 
 BigUInt128 a1 = biguint128_ctor_unit(); 
 BigUInt128 max = biguint128_sub(&a0, &a1);
 BigUInt128 max2 = biguint128_mul(&max, &max);

 if (!biguint128_eq(&max2, &a1)) {
  char max_str[BIGUINT_STRLEN_MAX + 1];
  max_str[biguint128_print_hex(&max, max_str, sizeof(max_str)/sizeof(max_str[0])-1)]=0;
  char max2_str[BIGUINT_STRLEN_MAX + 1];
  max2_str[biguint128_print_hex(&max2, max2_str, sizeof(max2_str)/sizeof(max2_str[0])-1)]=0;
  fprintf(stderr, "[%s * %s] -- expected: [%s], actual [%s]\n", max_str, max_str, "1", max2_str);
  fail = true;
 }
 return !fail;
}

int main(int argc, char **argv) {

 assert(test_mul0());
 assert(test_mul1());
 assert(test_mul2());

 return 0;
}

