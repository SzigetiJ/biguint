#include "biguint128.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define BIGUINT_BITS 128

const char* const hex_samples[]={
 "0",
 "1",
 "F",
 "FE",
 "1ED",
 "2EDC",
 "FEDCBA9876543210",
 "FEDCBA987654321",
 "F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF",
 "F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFFF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF",
 "F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFFF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFFF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFFF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF"
};
int hex_sample_len = sizeof(hex_samples) / sizeof(char*);

const char* const dec_samples[]={
 "0",
 "1",
 "9",
 "10",
 "256",
 "65536",
 "1234567890",
 "323456789012345678901234567890123456789",
 "113456789012345678901234567890123456789012345678901234567890123456789012345678",
 "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234"
};
int dec_sample_len = sizeof(dec_samples) / sizeof(char*);

bool test_io_hex0() {
 bool fail = false;
 char buffer[BIGUINT_BITS / 4 + 1];
 for (int i=0; i<hex_sample_len; ++i) {
  const char* const sample = hex_samples[i];
  if (BIGUINT_BITS / 4 < strlen(sample))
   continue;
  BigUInt128 a = biguint128_ctor_hexcstream(sample, strlen(sample));
  buint_size_t len = biguint128_print_hex(&a, buffer, sizeof(buffer) / sizeof(char)-1);
  buffer[len]=0;
  int result = strcmp(sample, buffer);
  if (result!=0) {
   fprintf(stderr, "expected: [%s], actual [%s]\n", sample, buffer);
   fail = true;
  }
 }
 return !fail;
}

bool test_io_hex1() {
 bool fail = false;
 char buffer[BIGUINT_BITS / 4 + 1];
 for (int i=0; i<hex_sample_len; ++i) {
  const char* const sample = hex_samples[i];
  if (BIGUINT_BITS / 4 < strlen(sample))
   continue;
  BigUInt128 a = biguint128_ctor_hexcstream(sample, strlen(sample));
  buint_size_t len = biguint128_print_hex(&a, buffer, strlen(sample)-1);
  buffer[len]=0;

  if (len!=0) {
   fprintf(stderr, "expected: [] (insufficient buffer capacity), actual [%s]\n", buffer);
   fail = true;
  }
 }
 return !fail;
}


bool test_io_dec0() {
 bool fail = false;
 char buffer[(BIGUINT_BITS / 10 + 1) * 3 + 2];
 for (int i=0; i<dec_sample_len; ++i) {
  const char* const sample = dec_samples[i];
  if (((BIGUINT_BITS / 10 + 1) * 3 + 1) < strlen(sample))
   continue;
  BigUInt128 a = biguint128_ctor_deccstream(sample, strlen(sample));
  buint_size_t len = biguint128_print_dec(&a, buffer, sizeof(buffer) / sizeof(char)-1);
  buffer[len]=0;
  int result = strcmp(sample, buffer);
  if (result!=0) {
   fprintf(stderr, "expected: [%s], actual [%s]\n", sample, buffer);
   fail = true;
  }
 }
 return !fail;
}

int main(int argc, char **argv) {

 assert(test_io_hex0());
 assert(test_io_hex1());
 assert(test_io_dec0());

 return 0;
}

