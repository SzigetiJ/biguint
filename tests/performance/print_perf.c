#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "biguint128.h"

#define LOOPS 100000

#define BUFLEN (3*512/10)+10	// Approx. max. usage of 512 bits

static void print_result(clock_t t_begin, clock_t t_end, const char *op, int cnt) {
 clock_t dt = t_end - t_begin;
 fprintf(stdout, "=== %d BigUInt128 %s operations ===\n", cnt, op);
 fprintf(stdout, "Elapsed time: %ld us,\t%.1f op/s\n", dt, (1000000.0 * cnt) / dt);
}

int main() {
 BigUInt128 b0 = biguint128_ctor_default();
 uint32_t loop_cnt;
 clock_t t0, t1;
 char buf[BUFLEN];
 unsigned int sum = 0;

 // #1: decimal print
 {
  t0 = clock();
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   biguint128_dec(&b0);
   buf[biguint128_print_dec(&b0, buf, BUFLEN)] = 0;
   // just do something with the printed string
   for (unsigned int i = 0; i < BUFLEN; ++i) {
    sum+=buf[i];
   }
  }
  t1 = clock();
  print_result(t0, t1, "print_dec", LOOPS);
 }

 // #2: hexadecimal print
 {
  t0 = clock();
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   biguint128_dec(&b0);
   buf[biguint128_print_hex(&b0, buf, BUFLEN)] = 0;
   // just do something with the printed string
   for (unsigned int i = 0; i < BUFLEN; ++i) {
    sum+=buf[i];
   }
  }
  t1 = clock();
  print_result(t0, t1, "print_hex", LOOPS);
 }

 return 0;
}