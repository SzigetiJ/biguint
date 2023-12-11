#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "biguint128.h"
#include "perf_common128.h"

#define LOOPS (1<<20) // 1M loops

// conventional div
void exec_div1000_v1(const BigUInt128 *ainit, const BigUInt128 *astep) {
 uint32_t loop_cnt;
 clock_t t0, t1;

 BigUInt128 bdiv = biguint128_value_of_uint(1000);
 BigUInt128 bx = biguint128_ctor_copy(ainit);
 BigUInt128 bsum = biguint128_ctor_default();

 t0 = clock();
 for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
  BigUIntPair128 res = biguint128_div(&bx, &bdiv);
  biguint128_add_assign(&bsum, &res.first);
  biguint128_add_assign(&bx, astep);
 }
 t1 = clock();

 print_exec_summary(t0, t1, "div(x,1000)", LOOPS, &bsum, 1);
}

// div1000(x)
void exec_div1000_v2(const BigUInt128 *ainit, const BigUInt128 *astep) {
 uint32_t loop_cnt;
 clock_t t0, t1;

 BigUInt128 bx = biguint128_ctor_copy(ainit);
 BigUInt128 bsum = biguint128_ctor_default();

 t0 = clock();
 for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
  BigUIntPair128 res = biguint128_div1000(&bx);
  biguint128_add_assign(&bsum, &res.first);
  biguint128_add_assign(&bx, astep);
 }
 t1 = clock();

 print_exec_summary(t0, t1, "div1000(x)", LOOPS, &bsum, 1);
}

int main() {
 BigUInt128 blow = biguint128_ctor_default();
 BigUInt128 bmed = biguint128_ctor_default();
 BigUInt128 bhigh = biguint128_ctor_default();
 bmed.dat[BIGUINT128_CELLS/2] = 1;
 bhigh.dat[BIGUINT128_CELLS-1] = 1;
 BigUInt128 bstep = biguint128_value_of_uint(29);

 fprintf(stdout, "*** Dividing low numbers ***\n");
 // #1.1: conventional div(x,1000) on low numbers
 exec_div1000_v1(&blow, &bstep);
 // #1.2: div1000(x) on low numbers
 exec_div1000_v2(&blow, &bstep);

 fprintf(stdout, "*** Dividing medium numbers ***\n");
 // #2.1: conventional div(x,1000) on med numbers
 exec_div1000_v1(&bmed, &bstep);
 // #2.2: div1000(x) on medium numbers
 exec_div1000_v2(&bmed, &bstep);

 fprintf(stdout, "*** Dividing high numbers ***\n");
 // #3.1: conventional div(x,1000) on high numbers
 exec_div1000_v1(&bhigh, &bstep);
 // #3.2: div1000(x) on hogh numbers
 exec_div1000_v2(&bhigh, &bstep);

 return 0;
}
