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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "uint.h"

#define LOOPS (1<<28) 	// 256M loops
#define BUFSIZE 2048

typedef enum {
 ADD,
 ADD2,
 ADD_ASSIGN,
 ADD_ASSIGN2,
 SUB,
 SUB2,
 SUB_ASSIGN,
 SUB_ASSIGN2
} UintFunction;

static inline UInt _uint_add2(UInt a, UInt b, buint_bool *carry) {
 UInt c = a + b;
 if (*carry) ++c;
 *carry = *carry ? c <= a : c < a;
 return c;
}

static inline UInt _uint_sub2(UInt a, UInt b, buint_bool *carry) {
 UInt c = a - b;
 if (*carry) --c;
 *carry = *carry ? a <= c : a < c;
 return c;
}

static inline UInt *_uint_add_assign2(UInt *a, UInt b, buint_bool *carry) {
 *a += b;
 if (*carry) ++(*a);
 *carry = (*carry ? *a <= b : *a < b);
 return a;
}

static void print_result(clock_t t_begin, clock_t t_end, const char *op, int cnt) {
 clock_t dt = t_end - t_begin;
 fprintf(stdout, "=== %d %s() operations ===\n", cnt, op);
 fprintf(stdout, "Elapsed time: %ld us,\t%.1f op/s\n", dt, (1000000.0 * cnt) / dt);
}

static inline void exec_uint_test_(UintFunction fun, UInt buf_init, UInt buf_step) {
 clock_t t0, t1;
 uint32_t loop_cnt;
 UInt sum = (fun==SUB || fun==SUB2 || fun == SUB_ASSIGN || fun == SUB_ASSIGN2?-1:0);
 buint_bool carry = false;

 // generate some numbers
 UInt num[BUFSIZE];
 for (unsigned int i = 0U; i < BUFSIZE; ++i) {
  num[i] = buf_init + i * buf_step;
 }

 t0 = clock();
 if (fun == ADD) {
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   sum = uint_add(sum, num[loop_cnt % BUFSIZE], &carry);
  }
 } else if (fun == ADD2) {
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   sum = _uint_add2(sum, num[loop_cnt % BUFSIZE], &carry);
  }
 } else if (fun == ADD_ASSIGN) {
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   uint_add_assign(&sum, num[loop_cnt % BUFSIZE], &carry);
  }
 } else if (fun == ADD_ASSIGN2) {
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   _uint_add_assign2(&sum, num[loop_cnt % BUFSIZE], &carry);
  }
 } else if (fun == SUB){
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   sum = uint_sub(sum, num[loop_cnt % BUFSIZE], &carry);
  }
  ++sum;	// give back the (-1) init.value
  sum*=-1;
 } else if (fun == SUB2){
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   sum = _uint_sub2(sum, num[loop_cnt % BUFSIZE], &carry);
  }
  ++sum;	// give back the (-1) init.value
  sum*=-1;
 } else if (fun == SUB_ASSIGN){
  for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
   uint_sub_assign(&sum, num[loop_cnt % BUFSIZE], &carry);
  }
  ++sum;	// give back the (-1) init.value
  sum*=-1;
 } else {
 }
 sum+=!!carry;
 t1 = clock();
 print_result(t0, t1,
  fun==ADD?"uint_add":
  fun==ADD2?"uint_add2":
  fun==ADD_ASSIGN?"uint_add_assign":
  fun==ADD_ASSIGN2?"uint_add_assign2":
  fun==SUB?"uint_sub":
  fun==SUB2?"uint_sub2":
  fun==SUB_ASSIGN?"uint_sub_assign":
  "uint_sub_assign2",
  loop_cnt);
 printf("sum: %" PRIuint "\n", sum);
}



int main(int argc, const char *argv[]) {
 UInt buf_init[]={0,((UInt)1<<(4*sizeof(UInt)))-(BUFSIZE/2)*29,-1};
 UInt buf_step[]={29,29,-29};
 const char *title[]={"low","medium","high"};

 for (int i=0; i<3; ++i) {
  fprintf(stderr, "*** uint functions on %s numbers ***\n", title[i]);
  exec_uint_test_(SUB, buf_init[i], buf_step[i]);
  exec_uint_test_(SUB2, buf_init[i], buf_step[i]);
  exec_uint_test_(SUB_ASSIGN, buf_init[i], buf_step[i]);
  exec_uint_test_(ADD, buf_init[i], buf_step[i]);
  exec_uint_test_(ADD2, buf_init[i], buf_step[i]);
  exec_uint_test_(ADD_ASSIGN, buf_init[i], buf_step[i]);
  exec_uint_test_(ADD_ASSIGN2, buf_init[i], buf_step[i]);
 }
 return 0;
}

