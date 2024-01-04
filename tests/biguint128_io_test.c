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
unsigned int hex_sample_len = sizeof(hex_samples) / sizeof(hex_samples[0]);

const CStr dec_samples[] = {
 STR("0"),
 STR("1"),
 STR("9"),
 STR("10"),
 STR("256"),
 STR("4444"),
 STR("65536"),
 STR("666666"),
 STR("7777777"),
 STR("88888888"),
 STR("999999999"),
 STR("1234567890"),
 STR("101"),
 STR("2002"),
 STR("30003"),
 STR("400004"),
 STR("5000005"),
 STR("60000006"),
 STR("700000007"),
 STR("1000000000"),
 STR("2000000000"),
 STR("3000000000"),
 STR("4000000000"),
 STR("4294967295"),
 STR("4294967296"),
 STR("4294967297"),
 STR("8000000008"),
 STR("90000000009"),
 STR("323456789012345678901234567890123456789"),
 STR("113456789012345678901234567890123456789012345678901234567890123456789012345678"),
 STR("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234")
};
unsigned int dec_sample_len = sizeof(dec_samples) / sizeof(dec_samples[0]);

static inline buint_size_t parse_and_print(const CStr *sample, char *buffer, size_t buf_len, Format fmt) {
 BigUInt128 a =
  fmt == FMT_DEC? biguint128_ctor_deccstream(sample->str, sample->len):
  fmt == FMT_SDEC? bigint128_ctor_deccstream(sample->str, sample->len):
  biguint128_ctor_hexcstream(sample->str, sample->len);
 buint_size_t len =
  fmt == FMT_DEC?biguint128_print_dec(&a, buffer, buf_len):
  fmt == FMT_SDEC?bigint128_print_dec(&a, buffer, buf_len):
  biguint128_print_hex(&a, buffer, buf_len);
 buffer[len] = 0;
 return len;
}

// Assert print(parse(hex_str))==hex_str / print(parse(dec_str))==dec_str.
bool test_io_gen0(const CStr *samples, size_t nsamples, Format fmt) {
 bool fail = false;
 char buffer[DEC_BIGUINTLEN_HI + 1];	// DEC is never less than HEX str

 for (unsigned int i = 0; i < nsamples; ++i) {
  const CStr *sample = &samples[i];
  if ((fmt!=FMT_HEX? DEC_BIGUINTLEN_LO:HEX_BIGUINTLEN) < sample->len)
   continue;

  parse_and_print(sample, buffer, sizeof(buffer) / sizeof(char) - 1, fmt);
  int result = strcmp(sample->str, buffer);

  if (result != 0) {
   fprintf(stderr, "expected: [%s], actual [%s]\n", sample->str, buffer);
   fail = true;
  }
 }
 return !fail;
}

// Assert print error, if buffer length is less than required.
bool test_io_gen1(const CStr *samples, size_t nsamples, Format fmt) {
 bool fail = false;
 char buffer[DEC_BIGUINTLEN_HI + 1];	// DEC is never less than HEX str

 for (unsigned int i = 0; i < nsamples; ++i) {
  const CStr *sample = &samples[i];
  if ((fmt!=FMT_HEX? DEC_BIGUINTLEN_LO:HEX_BIGUINTLEN) < sample->len)
   continue;

  buint_size_t len = parse_and_print(sample, buffer, sample->len - 1, fmt);

  if (len != 0) {
   fprintf(stderr, "expected: [] (insufficient buffer capacity), actual [%s]\n", buffer);
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

 for (unsigned int i = 0; i < dec_sample_len; ++i) {
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
  BigUInt128 minus_a = bigint128_negate(&a);

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

bool test_io_parse_zbuf(Format fmt) {
 bool pass = true;
 char buf[DEC_BIGINTLEN_HI + 1];

 bool only_sdec[]={
  false,
  false,
  true,
  true
 };
 const CStr tin[]={
  {NULL,0},
  STR("12"),
  STR("-12"),
  STR("-12")
 };
 size_t buflen[]={
  sizeof(buf)-1,
  0,	// this input is redundant (see test_io_gen1)
  0,	// this test is designed for SDEC print
  1	// this test is designed for SDEC print as well
 };
 const CStr exp[]= {
  {"0",1},
  {"",0},
  {"",0},
  {"",0}
 };
 const char *fun[]={
  "print(parse({NULL},0),buf,buflen)",
  "print(parse(\"12\",2),buf,0)",
  "print(parse(\"-12\",2),buf,0)",
  "print(parse(\"-12\",2),buf,1)"
 };

 for (size_t i= 0; i<sizeof(tin)/sizeof(tin[0]);++i) {
  if (only_sdec[i] && fmt !=FMT_SDEC) continue;
  buint_size_t len = parse_and_print(&tin[i], buf, buflen[i], fmt);
  pass &= check_cstr(fun[i], &exp[i], len, buf);
 }

 return pass;
}

int main(int argc, char **argv) {

 assert(test_io_gen0(hex_samples, hex_sample_len, FMT_HEX));
 assert(test_io_gen1(hex_samples, hex_sample_len, FMT_HEX));
 assert(test_io_gen0(dec_samples, dec_sample_len, FMT_DEC));
 assert(test_io_gen1(dec_samples, dec_sample_len, FMT_DEC));
 assert(test_io_gen1(dec_samples, dec_sample_len, FMT_SDEC));
 assert(test_io_dec1());
 assert(test_io_parse_zbuf(FMT_DEC));
 assert(test_io_parse_zbuf(FMT_HEX));
 assert(test_io_parse_zbuf(FMT_SDEC));

 return 0;
}

