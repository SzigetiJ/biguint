#include <time.h>

#include "biguint128.h"
#include "perf_common.h"
#include "perf_common128.h"

const StandardConstraints LIMITS = {
 8,
 (1<<26) // 64M loops
};
const StandardArgs ARGS_DEFAULT = INIT_FUN1ARGS(
 (1<<20), // 1M loops
 3U,
 29U);

typedef enum {
 VARIANT_DIV_X_1000,
 VARIANT_DIV1000_X
} Div1000Variant;

// ### Constants
const char *funname[] = {
 "div(x,1000)",
 "div1000(x)"
};
const unsigned int fun_n = sizeof(funname) / sizeof(funname[0]);

static void exec_function_loop_(unsigned int ai, unsigned int fun, const StandardArgs *args) {
 BigUInt128 a = get_value_by_level(ai, args->levels);
 BigUInt128 chkval = biguint128_ctor_default();
 uint32_t loop_cnt;
 clock_t t0, t1;

 BigUInt128 bdiv = biguint128_value_of_uint(1000);
 BigUIntPair128 res;

 t0 = clock();
 for (loop_cnt = 0; loop_cnt < args->loops; ++loop_cnt) {
  res = (fun==VARIANT_DIV1000_X)?
   biguint128_div1000(&a):
   biguint128_div(&a, &bdiv);
  process_result_v1(&res.first, &chkval.dat[0]);
  process_result_v1(&res.second, &chkval.dat[0]);
  biguint128_add_tiny(&a, (UInt)args->diff_a);
 }
 t1 = clock();

 print_exec_summary(t0, t1, funname[fun], loop_cnt, &chkval, 1);
}

int main(int argc, const char *argv[]) {
 return fun1_main(argc, argv, 128, ARGS_DEFAULT, &LIMITS, fun_n, funname, &exec_function_loop_);
}
