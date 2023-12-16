#include "biguint128.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LOOPS (1<<26) // 64M loops
#define BUFLEN 40 // for printsceen debugging

static void print_result(clock_t t_begin, clock_t t_end, const char *op, int cnt) {
 clock_t dt = t_end - t_begin;
 fprintf(stdout, "=== %d BigUInt128 %s operations ===\n", cnt, op);
 fprintf(stdout, "Elapsed time: %ld us,\t%.1f op/s\n", dt, (1000000.0 * cnt) / dt);
}

int main() {
 BigUInt128 b0 = biguint128_ctor_default();
 BigUInt128 b1 = biguint128_ctor_unit();
 uint32_t loop_cnt;
 clock_t t0, t1;
 char buf[BUFLEN];

 // #1: conventional add
 t0 = clock();
 for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
  b0 = biguint128_add(&b0, &b1);
 }
 t1 = clock();
 print_result(t0, t1, "add", LOOPS);
 buf[biguint128_print_dec(&b0, buf, BUFLEN)] = 0;
 fprintf(stdout, "(current sum: %s)\n", buf);

 // #2: add-assignment
 t0 = clock();
 for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
  biguint128_add_assign(&b0, &b1);
 }
 t1 = clock();
 print_result(t0, t1, "add-assignment", LOOPS);
 buf[biguint128_print_dec(&b0, buf, BUFLEN)] = 0;
 fprintf(stdout, "(current sum: %s)\n", buf);

 // #2: add-replace
 t0 = clock();
 for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
  biguint128_add_replace(&b0, &b0, &b1);
 }
 t1 = clock();
 print_result(t0, t1, "add-replace", LOOPS);
 buf[biguint128_print_dec(&b0, buf, BUFLEN)] = 0;
 fprintf(stdout, "(current sum: %s)\n", buf);

 // #3: add-assign uint as biguint
 {
  t0 = clock();
  BigUInt128 b2 = biguint128_ctor_standard(&b1.dat[0]);
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   biguint128_add_assign(&b0, &b2);
  }
  t1 = clock();
  print_result(t0, t1, "add-assign uint", LOOPS);
  buf[biguint128_print_dec(&b0, buf, BUFLEN)] = 0;
  fprintf(stdout, "(current sum: %s)\n", buf);
 }
 // #4: add-tiny
 t0 = clock();
 for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
  biguint128_add_tiny(&b0, b1.dat[0]);
 }
 t1 = clock();
 print_result(t0, t1, "add-tiny", LOOPS);
 buf[biguint128_print_dec(&b0, buf, BUFLEN)] = 0;
 fprintf(stdout, "(current sum: %s)\n", buf);

 // #5: sub-tiny
 t0 = clock();
 for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
  biguint128_sub_tiny(&b0, b1.dat[0]);
 }
 t1 = clock();
 print_result(t0, t1, "sub-tiny", LOOPS);
 buf[biguint128_print_dec(&b0, buf, BUFLEN)] = 0;
 fprintf(stdout, "(current sum: %s)\n", buf);

 // #6: sub-assign uint as biguint
 {
  t0 = clock();
  BigUInt128 b2 = biguint128_ctor_standard(&b1.dat[0]);
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   biguint128_sub_assign(&b0, &b2);
  }
  t1 = clock();
  print_result(t0, t1, "sub-assign uint", LOOPS);
  buf[biguint128_print_dec(&b0, buf, BUFLEN)] = 0;
  fprintf(stdout, "(current sum: %s)\n", buf);
 }

 // #7: sub-replace
 t0 = clock();
 for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
  biguint128_sub_replace(&b0, &b0, &b1);
 }
 t1 = clock();
 print_result(t0, t1, "sub-replace", LOOPS);
 buf[biguint128_print_dec(&b0, buf, BUFLEN)] = 0;
 fprintf(stdout, "(current sum: %s)\n", buf);

 // #8: sub-assignment
 t0 = clock();
 for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
  biguint128_sub_assign(&b0, &b1);
 }
 t1 = clock();
 print_result(t0, t1, "sub-assignment", LOOPS);
 buf[biguint128_print_dec(&b0, buf, BUFLEN)] = 0;
 fprintf(stdout, "(current sum: %s)\n", buf);

 // #9: conventional sub
 t0 = clock();
 for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
  b0 = biguint128_sub(&b0, &b1);
 }
 t1 = clock();
 print_result(t0, t1, "sub", LOOPS);
 buf[biguint128_print_dec(&b0, buf, BUFLEN)] = 0;
 fprintf(stdout, "(current sum: %s)\n", buf);

 return 0;
}
