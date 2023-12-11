#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "biguint128.h"
#include "perf_common128.h"

#define LOOPS (1<<20) // 1M loops

// conventional div
void exec_div10_v1(const BigUInt128 *ainit, const BigUInt128 *astep) {
 uint32_t loop_cnt;
 clock_t t0, t1;

 BigUInt128 bdiv = biguint128_value_of_uint(10);
 BigUInt128 bx = biguint128_ctor_copy(ainit);
 BigUInt128 bsum = biguint128_ctor_default();

 t0 = clock();
 for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
  BigUInt128 res = biguint128_div(&bx, &bdiv).first;
  biguint128_add_assign(&bsum, &res);
  biguint128_add_assign(&bx, astep);
 }
 t1 = clock();

 print_exec_summary(t0, t1, "div(x,10)", LOOPS, &bsum, 1);
}

// x/10 as (x*3)/30
void exec_div10_v2(const BigUInt128 *ainit, const BigUInt128 *astep) {
 uint32_t loop_cnt;
 clock_t t0, t1;

 BigUInt128 bx = biguint128_ctor_copy(ainit);
 BigUInt128 bsum = biguint128_ctor_default();

 t0 = clock();
 for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
  BigUInt128 res0 = biguint128_mul3(&bx);
  BigUInt128 res = biguint128_div30(&res0).first;
  biguint128_add_assign(&bsum, &res);
  biguint128_add_assign(&bx, astep);
 }
 t1 = clock();

 print_exec_summary(t0, t1, "div30(mul3(x))", LOOPS, &bsum, 1);
}

// x/10 as (x*100)/1000
void exec_div10_v3(const BigUInt128 *ainit, const BigUInt128 *astep) {
 uint32_t loop_cnt;
 clock_t t0, t1;

 BigUInt128 bx = biguint128_ctor_copy(ainit);
 BigUInt128 bsum = biguint128_ctor_default();

 t0 = clock();
 for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
  BigUInt128 res0 = biguint128_mul100(&bx);
  BigUInt128 res = biguint128_div1000(&res0).first;
  biguint128_add_assign(&bsum, &res);
  biguint128_add_assign(&bx, astep);
 }
 t1 = clock();

 print_exec_summary(t0, t1, "div1000(mul100(x))", LOOPS, &bsum, 1);
}


int main() {
 BigUInt128 blow = biguint128_ctor_default();
 BigUInt128 bmed = biguint128_ctor_default();
 BigUInt128 bhigh = biguint128_ctor_default();
 bmed.dat[BIGUINT128_CELLS/2] = 1;
 bhigh.dat[BIGUINT128_CELLS-1] = 1;
 BigUInt128 bstep = biguint128_value_of_uint(29);

 fprintf(stdout, "*** Dividing low numbers ***\n");
 // #1.1: conventional div 10 on low numbers
 exec_div10_v1(&blow, &bstep);
 // #1.2: conventional mul3->div30 on low numbers
 exec_div10_v2(&blow, &bstep);
 // #1.3: conventional mul100->div1000 on low numbers
 exec_div10_v3(&blow, &bstep);

 fprintf(stdout, "*** Dividing medium numbers ***\n");
 // #2.1: conventional div 10 on medium numbers
 exec_div10_v1(&bmed, &bstep);
 // #2.2: conventional mul3->div30 on medium numbers
 exec_div10_v2(&bmed, &bstep);
 // #2.3: conventional mul100->div1000 on medium numbers
 exec_div10_v3(&bmed, &bstep);

 fprintf(stdout, "*** Dividing high numbers ***\n");
 // #3.1: conventional div 10 on high numbers
 exec_div10_v1(&bhigh, &bstep);
 // #3.2: conventional mul3->div30 on high numbers
 exec_div10_v2(&bhigh, &bstep);
 // #3.3: conventional mul100->div1000 on high numbers
 exec_div10_v3(&bhigh, &bstep);

 return 0;
}
