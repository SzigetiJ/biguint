#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "biguint128.h"
#include "perf_common128.h"

#define LOOPS (1<<20) // 1M loops
#define UINT_BITS (8U * sizeof(UInt))

typedef enum {
 VARIANT_DIV_X_1000,
 VARIANT_DIV1000_X
} Div1000Variant;

void exec_div1000(const BigUInt128 *ainit, const BigUInt128 *astep, Div1000Variant v) {
 uint32_t loop_cnt;
 clock_t t0, t1;

 BigUInt128 bdiv = biguint128_value_of_uint(1000);
 BigUInt128 bx = biguint128_ctor_copy(ainit);
 BigUInt128 bsum = biguint128_ctor_default();

 t0 = clock();
 for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
  BigUIntPair128 res = (v==VARIANT_DIV1000_X)?
   biguint128_div1000(&bx):
   biguint128_div(&bx, &bdiv);
  biguint128_add_assign(&bsum, &res.first);
  biguint128_add_assign(&bx, astep);
 }
 t1 = clock();

 print_exec_summary(t0, t1, v==VARIANT_DIV1000_X?"div1000(x)":"div(x,1000)", loop_cnt, &bsum, 1);
}

int main() {
 const char *bstr[]={"low","medium","high"};
 BigUInt128 binit[3];
 for (int i = 0; i<3; ++i) {
  binit[i]=biguint128_ctor_default();
 }
 biguint128_sbit(&binit[1], UINT_BITS * (BIGUINT128_CELLS / 2));
 biguint128_sbit(&binit[2], UINT_BITS * (BIGUINT128_CELLS - 1));

 BigUInt128 bstep = biguint128_value_of_uint(29);
 Div1000Variant variant[] = {
  VARIANT_DIV_X_1000,
  VARIANT_DIV1000_X
 };

 for (int i=0; i<3; ++i) {
  fprintf(stdout, "*** Dividing %s numbers ***\n", bstr[i]);
  for (int j=0; j<2; ++j) {
   exec_div1000(&binit[i], &bstep, variant[j]);
  }
 }

 return 0;
}
