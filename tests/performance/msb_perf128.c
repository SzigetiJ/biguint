#include "biguint128.h"
#include "perf_common.h"
#include "perf_common128.h"

// ### Constraints and default values
const StandardConstraints LIMITS = {
 64,
 (1<<24) // 16M loops
};
const StandardArgs ARGS_DEFAULT = INIT_FUN1ARGS(
 (1<<20), // 1M loops
 8U,
 3U);

typedef enum {
 FUN_MSB = 0,
 FUN_LZB,
 FUN_LZC
} MsbFun;

const char *funname[]={
 "msb",
 "lzb",
 "lzc"
};

static unsigned int exec_function_loop_(unsigned int ai, unsigned int fun, const StandardArgs *args, UInt *chkval) {
 BigUInt128 a = get_value_by_level(ai, args->levels);
 buint_size_t res;

 for (unsigned int i = 0; i < args->loops; ++i) {
  if (fun == FUN_MSB) {
   res = biguint128_msb(&a);
  } else if (fun == FUN_LZB) {
   res = biguint128_lzb(&a);
  } else if (fun == FUN_LZC) {
   res = biguint128_lzc(&a);
  }
  *chkval+=res;
  biguint128_add_tiny(&a, args->diff[0]);
 }
 return args->loops;
}

int main(int argc, const char *argv[]) {
 return fun1_main(argc, argv, 128, ARGS_DEFAULT, &LIMITS, ARRAYSIZE(funname), funname, &exec_function_loop_, 1);
}
