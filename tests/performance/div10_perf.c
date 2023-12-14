#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "biguint128.h"
#include "perf_common128.h"

#define LOOPS (1<<20) // 1M loops
#define UINT_BITS (8U * sizeof(UInt))

typedef enum {
 VARIANT_DIV_X_10,
 VARIANT_DIV1000_MUL100_X,
 VARIANT_DIV30_MUL3_X
} Div10Variant;

void exec_div10(const BigUInt128 *ainit, const BigUInt128 *astep, Div10Variant v) {
 uint32_t loop_cnt;
 clock_t t0, t1;

 BigUInt128 bdiv = biguint128_value_of_uint(10);
 BigUInt128 bx = biguint128_ctor_copy(ainit);
 BigUInt128 bsum = biguint128_ctor_default();
 BigUInt128 res0, res;

 t0 = clock();
 for (loop_cnt = 0; loop_cnt < LOOPS; ++loop_cnt) {
  if (v == VARIANT_DIV_X_10) {
   res = biguint128_div(&bx, &bdiv).first;
  } else if (v == VARIANT_DIV1000_MUL100_X){
   res0 = biguint128_mul100(&bx);
   res = biguint128_div1000(&res0).first;
  } else {
   res0 = biguint128_mul3(&bx);
   res = biguint128_div30(&res0).first;
  }
  biguint128_add_assign(&bsum, &res);
  biguint128_add_assign(&bx, astep);
 }
 t1 = clock();

 print_exec_summary(t0, t1,
  v == VARIANT_DIV_X_10 ? "div(x,10)" :
  v == VARIANT_DIV1000_MUL100_X ? "div1000(mul100(x))" :
  "div30(mul3(x))",
  loop_cnt, &bsum, 1);
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
 Div10Variant variant[] = {
  VARIANT_DIV_X_10,
  VARIANT_DIV1000_MUL100_X,
  VARIANT_DIV30_MUL3_X
 };

 for (int i=0; i<3; ++i) {
  fprintf(stdout, "*** Dividing %s numbers ***\n", bstr[i]);
  for (int j=0; j<3; ++j) {
   exec_div10(&binit[i], &bstep, variant[j]);
  }
 }

 return 0;
}
