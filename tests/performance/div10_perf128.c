#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "biguint128.h"
#include "perf_common128.h"

#define LOOPS (1<<20) // 1M loops

typedef enum {
 VARIANT_DIV_X_10,
 VARIANT_DIV1000_MUL100_X,
 VARIANT_DIV30_MUL3_X,
 VARIANT_DIV10_X
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
  } else if (v == VARIANT_DIV30_MUL3_X){
   res0 = biguint128_mul3(&bx);
   res = biguint128_div30(&res0).first;
  } else {
   res = biguint128_div10(&bx).first;
  }
  biguint128_add_assign(&bsum, &res);
  biguint128_add_assign(&bx, astep);
 }
 t1 = clock();

 print_exec_summary(t0, t1,
  v == VARIANT_DIV_X_10 ? "div(x,10)" :
  v == VARIANT_DIV1000_MUL100_X ? "div1K(m100(x))" :
  v == VARIANT_DIV30_MUL3_X ? "div30(mul3(x))":
  "div10(x)",
  loop_cnt, &bsum, 1);
}

int main() {
 PerfTestInitValues b = get_std_initvalues();
 BigUInt128 bstep = biguint128_value_of_uint(29);
 Div10Variant variant[] = {
  VARIANT_DIV_X_10,
  VARIANT_DIV1000_MUL100_X,
  VARIANT_DIV30_MUL3_X,
  VARIANT_DIV10_X
 };

 for (int i=0; i<3; ++i) {
  fprintf(stdout, "*** Dividing %s numbers ***\n", b.name[i]);
  for (int j=0; j< sizeof(variant)/sizeof(variant[0]); ++j) {
   exec_div10(&b.val[i], &bstep, variant[j]);
  }
 }

 return 0;
}
