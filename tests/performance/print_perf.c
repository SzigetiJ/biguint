#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "biguint128.h"
#include "perf_common128.h"

#define LOOPS 200000

#define BUFLEN (3*512/10)+10	// Approx. max. usage of 512 bits

int main() {
 BigUInt128 b0 = biguint128_ctor_default();
 uint32_t loop_cnt;
 clock_t t0, t1;
 char buf[BUFLEN];

 // #1: decimal print
 {
  unsigned int sum = 0;
  t0 = clock();
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   biguint128_dec(&b0);
   buf[biguint128_print_dec(&b0, buf, BUFLEN)] = 0;
   // just do something with the printed string
   for (unsigned int i = 0; i < BUFLEN && buf[i]!=0; ++i) {
    sum+= buf[i];
   }
  }
  t1 = clock();
  print_exec_time(t0, t1, "print_dec", LOOPS);
  fprintf(stderr,"sum: %u\n", sum);
 }

 // #2: hexadecimal print
 {
  unsigned int sum = 0;
  t0 = clock();
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   biguint128_dec(&b0);
   buf[biguint128_print_hex(&b0, buf, BUFLEN)] = 0;
   // just do something with the printed string
   for (unsigned int i = 0; i < BUFLEN && buf[i]!=0; ++i) {
    sum+= buf[i];
   }
  }
  t1 = clock();
  print_exec_time(t0, t1, "print_hex", LOOPS);
  // do something with sum
  fprintf(stderr,"sum: %u\n", sum);
 }

 return 0;
}
