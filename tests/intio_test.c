#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "intio.h"

bool test_get_digit() {
 const unsigned char bases[] = {2,8,10,16,32};
 const unsigned int bases_len = sizeof(bases)/sizeof(unsigned char);
 const char samples[] = {
  '0','1','2','3','4','5','6','7','8','9',
  'a','b','c','d','e','f',
  'A','B','C','D','E','F',
  'g','n','o','v','w',
  'G','N','O','v','W',
  '.','?','!','/','\0'
  };
 const unsigned int samples_len = sizeof(samples)/sizeof(char);
 const unsigned char sample_values[] = {
  0,1,2,3,4,5,6,7,8,9,
  10,11,12,13,14,15,
  10,11,12,13,14,15,
  16,23,24,31,32,
  16,23,24,31,32,
  255,255,255,255,255
  };
 const unsigned int sample_values_len = sizeof(sample_values)/sizeof(unsigned char);

 bool fail = false;

 for (int base_i=0; base_i<bases_len; ++base_i) {
  for (int sample_i=0; sample_i<samples_len; ++sample_i) {
   const char sample = samples[sample_i];
   const unsigned char base = bases[base_i];
   const IntIoError expected_result = sample_values[sample_i] < base ? INTIO_NO_ERROR:INTIO_ERROR_OUTOFRANGE;
   const unsigned char expected_digit = sample_values[sample_i];

   IntIoError result;
   unsigned char digit;

   result = get_digit(sample, base, &digit);

   if (result != expected_result) {
    fprintf(stderr, "%s: get_digit('%c',%d,&digit) return value expected: [%d] actual: [%d]\n", __func__, sample, base, expected_result, result);
    fail = true;
   }
   if (expected_result == INTIO_NO_ERROR) {
    if (digit != expected_digit) {
     fprintf(stderr, "%s: get_digit('%c',%d,&digit) digit expected: [%u] actual: [%u]\n", __func__, sample, base, expected_digit, digit);
     fail = true;
    }
   }
  }
 }
 return !fail;
}

bool test_get_hexbyte_valid() {
 const char samples[]="00" "01" "02" "0F" "10" "11" "5A" "7F" "80" "A5" "F0" "FF";
 const unsigned int samples_len = ((sizeof(samples) / sizeof(char)) - 1) / 2;
 const unsigned char values[] = {0,1,2,15,16,17,90,127,128,165,240,255};

 bool fail = false;

 for (int sample_i = 0; sample_i<samples_len; ++sample_i) {
  unsigned char digit;
  IntIoError result = get_hexbyte(samples+(2*sample_i), &digit);

  if (result != INTIO_NO_ERROR) {
   fprintf(stderr, "%s: get_hexbyte(\"%c%c\",&digit) return value expected: [%d] actual: [%d]\n", __func__, samples[2*sample_i], samples[2*sample_i+1], INTIO_NO_ERROR, result);
   fail = true;
  }
  if (digit != values[sample_i]) {
   fprintf(stderr, "%s: get_hexbyte(\"%c%c\",&digit) digit expected: [%u] actual: [%u]\n", __func__, samples[2*sample_i], samples[2*sample_i+1], values[sample_i], digit);
   fail = true;
  }
 }
 return !fail;
}

int main(int argc, char **argv) {
 assert(test_get_digit());
 assert(test_get_hexbyte_valid());
}

