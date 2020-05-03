#include "biguint128.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define BIGUINT_BITS 128

bool test_and_or_not0() {
 const char *samples_a[]={
  "0",
  "1",
  "7FFFFFFF",
  "100000000",
  "A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5"
 };
 const unsigned int samples_a_len = sizeof(samples_a) / sizeof(char*);
 const char *samples_b[]={
  "3",
  "300000000",
  "30000000000000000",
  "3000000000000000000000000"
 };
 const unsigned int samples_b_len = sizeof(samples_b) / sizeof(char*);
 
 const char *intersections[][4]={
  {"0","0","0","0"},
  {"1","0","0","0"},
  {"3","0","0","0"},
  {"0","100000000","0","0"},
  {"1","100000000","10000000000000000","1000000000000000000000000"}
 };
 const char *unions[][4]={
  {"3","300000000","30000000000000000","3000000000000000000000000"},
  {"3","300000001","30000000000000001","3000000000000000000000001"},
  {"7FFFFFFF","37FFFFFFF","3000000007FFFFFFF","300000000000000007FFFFFFF"},
  {"100000003","300000000","30000000100000000","3000000000000000100000000"},
  {"A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A7","A5A5A5A5A5A5A5A5A5A5A5A7A5A5A5A5","A5A5A5A5A5A5A5A7A5A5A5A5A5A5A5A5","A5A5A5A7A5A5A5A5A5A5A5A5A5A5A5A5"}
 };

 const char *sym_diffs[][4]={
  {"3","300000000","30000000000000000","3000000000000000000000000"},
  {"2","300000001","30000000000000001","3000000000000000000000001"},
  {"7FFFFFFC","37FFFFFFF","3000000007FFFFFFF","300000000000000007FFFFFFF"},
  {"100000003","200000000","30000000100000000","3000000000000000100000000"},
  {"A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A6","A5A5A5A5A5A5A5A5A5A5A5A6A5A5A5A5","A5A5A5A5A5A5A5A6A5A5A5A5A5A5A5A5","A5A5A5A6A5A5A5A5A5A5A5A5A5A5A5A5"}
 };

 bool fail = false;

 for (unsigned int a_i = 0; a_i < samples_a_len; ++a_i) {
  BigUInt128 a = biguint128_ctor_hexcstream(samples_a[a_i], strlen(samples_a[a_i]));
  BigUInt128 not_a = biguint128_not(&a);
  for (unsigned int b_i = 0; b_i < samples_b_len; ++b_i) {
   BigUInt128 b = biguint128_ctor_hexcstream(samples_b[b_i], strlen(samples_b[b_i]));
   BigUInt128 not_b = biguint128_not(&b);

   BigUInt128 expected_and = biguint128_ctor_hexcstream(intersections[a_i][b_i], strlen(intersections[a_i][b_i]));
   BigUInt128 expected_or = biguint128_ctor_hexcstream(unions[a_i][b_i], strlen(unions[a_i][b_i]));
   BigUInt128 expected_xor = biguint128_ctor_hexcstream(sym_diffs[a_i][b_i], strlen(sym_diffs[a_i][b_i]));


   BigUInt128 result_and = biguint128_and(&a, &b);
   BigUInt128 result_or = biguint128_or(&a, &b);
   BigUInt128 tmp_or = biguint128_or(&not_a, &not_b);
   BigUInt128 result_and2 = biguint128_not(&tmp_or);
   BigUInt128 result_xor = biguint128_xor(&a, &b);

   buint_bool eq_and = biguint128_eq(&expected_and, &result_and);
   buint_bool eq_and2 = biguint128_eq(&expected_and, &result_and2);
   buint_bool eq_or = biguint128_eq(&expected_or, &result_or);
   buint_bool eq_xor = biguint128_eq(&expected_xor, &result_xor);

   if (!eq_and) {
    char buffer[BIGUINT_BITS / 4 + 1];
    buffer[biguint128_print_hex(&result_and, buffer, sizeof(buffer)/sizeof(char)-1)]=0;
    fprintf(stderr, "[%s & %s] -- expected: [%s], actual [%s]\n", samples_a[a_i], samples_b[b_i], intersections[a_i][b_i], buffer);
    fail = true;
   }

   if (!eq_and2) {
    char buffer[BIGUINT_BITS / 4 + 1];
    buffer[biguint128_print_hex(&result_and2, buffer, sizeof(buffer)/sizeof(char)-1)]=0;
    fprintf(stderr, "[~(~%s | ~%s)] -- expected: [%s], actual [%s]\n", samples_a[a_i], samples_b[b_i], intersections[a_i][b_i], buffer);
    fail = true;
   }

   if (!eq_or) {
    char buffer[BIGUINT_BITS / 4 + 1];
    buffer[biguint128_print_hex(&result_or, buffer, sizeof(buffer)/sizeof(char)-1)]=0;
    fprintf(stderr, "[%s | %s] -- expected: [%s], actual [%s]\n", samples_a[a_i], samples_b[b_i], unions[a_i][b_i], buffer);
    fail = true;
   }

   if (!eq_xor) {
    char buffer[BIGUINT_BITS / 4 + 1];
    buffer[biguint128_print_hex(&result_xor, buffer, sizeof(buffer)/sizeof(char)-1)]=0;
    fprintf(stderr, "[%s ^ %s] -- expected: [%s], actual [%s]\n", samples_a[a_i], samples_b[b_i], sym_diffs[a_i][b_i], buffer);
    fail = true;
   }
  }
 }

 return !fail;
}

int main(int argc, char **argv) {

 assert(test_and_or_not0());

 return 0;
}

