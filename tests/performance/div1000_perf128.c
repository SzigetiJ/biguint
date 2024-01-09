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

const char *funname[] = {
 "div(x,1000)",
 "div1000(x)"
};

static unsigned int exec_function_loop_(unsigned int ai, unsigned int fun, const StandardArgs *args, UInt *chkval) {
 BigUInt128 bdiv = biguint128_value_of_uint(1000);
 BigUInt128 a = get_value_by_level(ai, args->levels);
 BigUIntPair128 res;
 uint32_t loop_cnt;

 for (loop_cnt = 0; loop_cnt < args->loops; ++loop_cnt) {
  res = (fun==VARIANT_DIV1000_X)?
   biguint128_div1000(&a):
   biguint128_div(&a, &bdiv);
  process_result_v1(&res.first, &chkval[0]);
  process_result_v3(&res.second.dat[0], &chkval[1]);
  biguint128_add_tiny(&a, (UInt)args->diff[0]);
 }
 return loop_cnt;
}

int main(int argc, const char *argv[]) {
 return fun1_main(argc, argv, 128, ARGS_DEFAULT, &LIMITS, ARRAYSIZE(funname), funname, &exec_function_loop_, 2);
}
