/* This performance test executes
 biguint128 print routines multiple
 times with different values.
 First, decimal prints are done,
 next, hexadecimal prints.
 There is a default number of print iterations,
 however, this number can be overriden
 with command line argument.
 Inputs:
  [argv(1)]: number of print calls.
 Outputs:
  <stderr>: statistics.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "biguint128.h"
#include "perf_common128.h"

#define LOOPS 200000U	// default number of print cycles

#define BUFLEN (3*128/10)+10	// More than the approx. max. usage of 128 bit long dec string

unsigned int exec_print_loop(bool decimal, uint32_t loops) {
 BigUInt128 b0 = biguint128_ctor_default();
 clock_t t0, t1;
 char buf[BUFLEN];
 unsigned int sum = 0;
 uint32_t loop_cnt;

 t0 = clock();
 for (loop_cnt = 0; loop_cnt < loops; ++loop_cnt) {
  biguint128_dec(&b0);
  buint_size_t len = decimal?
   biguint128_print_dec(&b0, buf, BUFLEN-1):
   biguint128_print_hex(&b0, buf, BUFLEN-1);
  buf[len] = 0;
  // just do something with the printed string
  for (unsigned int i = 0; i < len; ++i) {
   sum+= buf[i];
  }
 }
 t1 = clock();

 print_exec_time(t0, t1, decimal?"print_dec":"print_hex", loop_cnt);
 fprintf(stderr,"sum: %u\n", sum);
 return sum;
}

int main(int argc, const char *argv[]) {
 uint32_t loops = LOOPS;
 if (1<argc){
  loops = atoi(argv[1]);
 }

 // #1: decimal print
 exec_print_loop(true, loops);

 // #2: hexadecimal print
 exec_print_loop(false, loops);

 return 0;
}
