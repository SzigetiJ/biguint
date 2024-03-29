#include "biguint128.h"
#include "perf_common.h"
#include "perf_common128.h"

// ### Constraints and default values
const StandardConstraints LIMITS = {
 8,
 (1<<26) // 64M loops
};
const StandardArgs ARGS_DEFAULT = INIT_FUN1ARGS(
 (1<<20), // 1M loops
 3U,
 29U);

typedef enum {
 VARIANT_DIV_X_10,
 VARIANT_DIV1000_MUL100_X,
 VARIANT_DIV30_MUL3_X,
 VARIANT_DIV10_X,
 VARIANT_DIV_TINY_X_10
} Div10Variant;

const char *funname[] = {
 "div(x,10)",
 "d1000(m100(x))",
 "div30(mul3(x))",
 "div10(x)",
 "div_uint(x,10)"
};

static unsigned int exec_function_loop_(unsigned int ai, unsigned int fun, const StandardArgs *args, UInt *chkval) {
 const BigUInt128 bdiv = biguint128_value_of_uint(10);
 BigUInt128 a = get_value_by_level(ai, args->levels);
 BigUInt128 res0;
 BigUIntPair128 res;
 BigUIntTinyPair128 rest;
 BigUInt128 *procref1 = fun == VARIANT_DIV10_X || fun == VARIANT_DIV_TINY_X_10?&rest.first:&res.first;
 UInt *procref2 = fun == VARIANT_DIV10_X || fun == VARIANT_DIV_TINY_X_10?&rest.second:&res.second.dat[0];
 unsigned int loop_cnt;

 for (loop_cnt = 0; loop_cnt < args->loops; ++loop_cnt) {
  if (fun == VARIANT_DIV_X_10) {
   res = biguint128_div(&a, &bdiv);
  } else if (fun == VARIANT_DIV1000_MUL100_X) {
   res0 = biguint128_mul100(&a);
   res = biguint128_div1000(&res0);
   res.second.dat[0]/=100;
  } else if (fun == VARIANT_DIV30_MUL3_X) {
   res0 = biguint128_mul3(&a);
   res = biguint128_div30(&res0);
   res.second.dat[0]/=3;
  } else if (fun == VARIANT_DIV_TINY_X_10) {
   rest = biguint128_div_uint(&a,10);
  } else {
   rest = biguint128_div10(&a);
  }
  process_result_v1(procref1, &chkval[0]);
  process_result_v3(procref2, &chkval[1]);
  biguint128_add_tiny(&a, (UInt) args->diff[0]);
 }
 return loop_cnt;
}

int main(int argc, const char *argv[]) {
 return fun1_main(argc, argv, 128, ARGS_DEFAULT, &LIMITS, ARRAYSIZE(funname), funname, &exec_function_loop_,2);
}
