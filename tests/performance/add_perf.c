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

 return 0;
}