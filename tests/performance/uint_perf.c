/***************************************
 This program performs multiple (LOOPS)
 UInt additions / subtraction.
 The main purpose of this program is to
 check the performance of functions
 (uint_add() / uint_add_assign() /
 uint_sub()) as external and as inline
 functions.
***************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "uint.h"

#define LOOPS (1<<28) 	// 256M loops
#define BUFSIZE 2048

static void print_result(clock_t t_begin, clock_t t_end, const char *op, int cnt) {
 clock_t dt = t_end - t_begin;
 fprintf(stdout, "=== %d %s() operations ===\n", cnt, op);
 fprintf(stdout, "Elapsed time: %ld us,\t%.1f op/s\n", dt, (1000000.0 * cnt) / dt);
}


static inline UInt _uint_add(UInt a, UInt b, buint_bool *carry) {
 UInt c = a + b + !!(*carry);
 *carry = *carry ? c <= a : c < a;
 return c;
}

static inline UInt *_uint_add_assign(UInt *a, UInt b, buint_bool *carry) {
  *a += b + !!(*carry);
  *carry = (*carry ? *a <= b : *a < b);
  return a;
 }

static inline UInt _uint_sub(UInt a, UInt b, buint_bool *carry) {
 UInt c = a - b - !!(*carry);
 *carry = *carry ? a <= c : a < c;
 return c;
}


int main(int argc, const char *argv[]) {
 uint32_t loop_cnt;
 clock_t t0, t1;

 // generate some numbers
 UInt num[BUFSIZE];
 for (unsigned int i = 0U; i < BUFSIZE; ++i) {
  num[i] = i * 29;
 }

 {
  UInt sum1 = 0U;
  buint_bool carry1 = false;
  t0 = clock();
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   sum1 = _uint_add(sum1, num[loop_cnt % BUFSIZE], &carry1);
  }
  t1 = clock();
  print_result(t0, t1, "inline uint_add", LOOPS);
  sum1+=!!carry1;
  printf("sum1: %" PRIuint "\n", sum1);
 }

 {
  UInt sum2 = 0U;
  buint_bool carry2 = false;
  t0 = clock();
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   _uint_add_assign(&sum2, num[loop_cnt % BUFSIZE], &carry2);
  }
  t1 = clock();
  print_result(t0, t1, "inline uint_add_assign", LOOPS);
  sum2+=!!carry2;
  printf("sum2: %" PRIuint "\n", sum2);
 } 

 {
  UInt sum3 = -1;	// (== MAX_UINT) start at the top to avoid carry bit set after the 1st call
  buint_bool carry3 = false;
  t0 = clock();
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   sum3=_uint_sub(sum3, num[loop_cnt % BUFSIZE], &carry3);
  }
  t1 = clock();
  print_result(t0, t1, "inline uint_sub", LOOPS);
  ++sum3;	// give back the (-1) init.value
  sum3-=!!carry3;
  printf("sum3: %" PRIuint "\n", -sum3);
 }

 {
  UInt sum1 = 0U;
  buint_bool carry1 = false;
  t0 = clock();
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   sum1 = uint_add(sum1, num[loop_cnt % BUFSIZE], &carry1);
  }
  t1 = clock();
  print_result(t0, t1, "uint_add", LOOPS);
  sum1+=!!carry1;
  printf("sum1: %" PRIuint "\n", sum1);
 }

 {
  UInt sum2 = 0U;
  buint_bool carry2 = false;
  t0 = clock();
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   uint_add_assign(&sum2, num[loop_cnt % BUFSIZE], &carry2);
  }
  t1 = clock();
  print_result(t0, t1, "uint_add_assign", LOOPS);
  sum2+=!!carry2;
  printf("sum2: %" PRIuint "\n", sum2);
 } 

 {
  UInt sum3 = -1;	// (== MAX_UINT) start at the top to avoid carry bit set after the 1st call
  buint_bool carry3 = false;
  t0 = clock();
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   sum3=uint_sub(sum3, num[loop_cnt % BUFSIZE], &carry3);
  }
  t1 = clock();
  print_result(t0, t1, "uint_sub", LOOPS);
  ++sum3;	// give back the (-1) init.value
  sum3-=!!carry3;
  printf("sum3: %" PRIuint "\n", -sum3);
 }

 return 0;
}

