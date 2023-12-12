#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "biguint128.h"
#include "test_common.h"
#include "test_common128.h"

const CStr hex_samples[] = {
 STR("0"),
 STR("1"),
 STR("F"),
 STR("FE"),
 STR("1ED"),
 STR("2EDC"),
 STR("FEDCBA9876543210"),
 STR("FEDCBA987654321"),
 STR("F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF"),
 STR("F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFFF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF"),
 STR("F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFFF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFFF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFFF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF")
};
int hex_sample_len = sizeof(hex_samples) / sizeof(hex_samples[0]);

const CStr dec_samples[] = {
 STR("0"),
 STR("1"),
 STR("9"),
 STR("10"),
 STR("256"),
 STR("65536"),
 STR("1234567890"),
 STR("323456789012345678901234567890123456789"),
 STR("113456789012345678901234567890123456789012345678901234567890123456789012345678"),
 STR("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234")
};
int dec_sample_len = sizeof(dec_samples) / sizeof(dec_samples[0]);

// Assert print(parse(hex_str))==hex_str.
bool test_io_hex0() {
 bool fail = false;
 char buffer[HEX_BIGUINTLEN + 1];

 for (int i = 0; i < hex_sample_len; ++i) {
  const CStr *sample = &hex_samples[i];
  if (HEX_BIGUINTLEN < sample->len)
   continue;
  BigUInt128 a = biguint128_ctor_hexcstream(sample->str, sample->len);
  buint_size_t len = biguint128_print_hex(&a, buffer, sizeof(buffer) / sizeof(char) - 1);
  buffer[len] = 0;
  int result = strcmp(sample->str, buffer);
  if (result != 0) {
   fprintf(stderr, "expected: [%s], actual [%s]\n", sample->str, buffer);
   fail = true;
  }
 }
 return !fail;
}

// Assert (hex) print error, if buffer length is less than required.
bool test_io_hex1() {
 bool fail = false;
 char buffer[HEX_BIGUINTLEN + 1];
 for (int i = 0; i < hex_sample_len; ++i) {
  const CStr *sample = &hex_samples[i];
  if (HEX_BIGUINTLEN < sample->len)
   continue;
  BigUInt128 a = biguint128_ctor_hexcstream(sample->str, sample->len);
  buint_size_t len = biguint128_print_hex(&a, buffer, sample->len - 1);
  buffer[len] = 0;

  if (len != 0) {
   fprintf(stderr, "expected: [] (insufficient buffer capacity), actual [%s]\n", buffer);
   fail = true;
  }
 }
 return !fail;
}

// Assert print(parse(dec_str))==dec_str.
bool test_io_dec0() {
 bool fail = false;
 char buffer[DEC_BIGUINTLEN_HI + 1];
 for (int i = 0; i < dec_sample_len; ++i) {
  const CStr *sample = &dec_samples[i];
  if (DEC_BIGUINTLEN_LO < sample->len)
   continue;
  BigUInt128 a = biguint128_ctor_deccstream(sample->str, sample->len);
  buint_size_t len = biguint128_print_dec(&a, buffer, sizeof(buffer) / sizeof(char) - 1);
  buffer[len] = 0;
  int result = strcmp(sample->str, buffer);
  if (result != 0) {
   fprintf(stderr, "expected: [%s], actual [%s]\n", sample->str, buffer);
   fail = true;
  }
 }
 return !fail;
}

// print signed value, check string
// next, read that string and check value
// depends on biguint128_sub
bool test_io_dec1() {
 bool fail = false;

 char buffer[DEC_BIGINTLEN_HI + 1];
 char sgnsample[DEC_BIGINTLEN_HI + 1];

 for (int i = 0; i < dec_sample_len; ++i) {
  const CStr *sample = &dec_samples[i];
  if (DEC_BIGUINTLEN_LO < sample->len)
   continue;
  if (!negate_strcpy(sgnsample,sizeof(sgnsample), sample->str, sample->len))
   continue;
  BigUInt128 a = biguint128_ctor_deccstream(sample->str, sample->len);
  if (biguint128_gbit(&a, BIGUINT_BITS - 1)) { // if MSB is set, value is OOR for bigint
   continue;
  }
  // here we introduce dependency on biguint128_sub
  BigUInt128 minus_a = negate_bigint128(&a);

  // note: prefix of print is bigint128_
  buint_size_t buflen = bigint128_print_dec(&minus_a, buffer, sizeof(buffer) / sizeof(char) - 1);
  buffer[buflen] = 0;

  // note: prefix of ctor is bigint128_
  BigUInt128 reminus_a = bigint128_ctor_deccstream(buffer, buflen);

  int result = strcmp(sgnsample, buffer);
  buint_bool re_result = biguint128_eq(&minus_a, &reminus_a);
  if (result != 0) {
   fprintf(stderr, "Print failed. Expected: [%s], actual [%s]\n", sgnsample, buffer);
   fail = true;
  }
  if (!re_result) {
   // here we depend on bigint128_print_dec..
   buint_size_t len = bigint128_print_dec(&reminus_a, buffer, sizeof(buffer) / sizeof(char) - 1);
   buffer[len] = 0;
   fprintf(stderr, "Read failed. Expected: [%s], actual [%s]\n", sgnsample, buffer);
   fail = true;
  }
 }
 return !fail;
}

int main(int argc, char **argv) {

 assert(test_io_hex0());
 assert(test_io_hex1());
 assert(test_io_dec0());
 assert(test_io_dec1());

 return 0;
}

