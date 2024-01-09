#include "biguint128.h"
#include "perf_common.h"
#include "perf_common128.h"


// ### Constraints and default values
const StandardConstraints LIMITS = {
 8,
 (1<<26) // 64M loops
};
const StandardArgs ARGS_DEFAULT = INIT_FUN2ARGS(
 (1<<20), // 1M loops
 3U,
 37U,
 29U);

typedef enum {
 FUN_MUL = 0,
 FUN_DMUL,
 FUN_DIV
} MultiplicativeFun;

const char *funname[]={
 "mul",
 "dmul",
 "div"
};

static unsigned int exec_function_loop_(unsigned int ai, unsigned int bi, unsigned int fun, const StandardArgs *args, UInt *chkval) {
 BigUInt128 a = get_value_by_level(ai, args->levels);
 BigUInt128 b = get_value_by_level(bi, args->levels);
 BigUInt128 res;
 BigUIntPair128 resp;
 BigUInt128 *procref1 = fun == FUN_MUL ? &res : &resp.first;
 BigUInt128 *procref2 = fun == FUN_MUL ? &res : &resp.second;

 for (unsigned int i = 0; i < args->loops; ++i) {
  if (fun == FUN_MUL) {
   res = biguint128_mul(&a, &b);
  } else if (fun == FUN_DMUL) {
   resp = biguint128_dmul(&a, &b);
  } else if (fun == FUN_DIV) {
   resp = biguint128_div(&a, &b);
  }
  process_result_v1(procref2, chkval+0);
  process_result_v1(procref1, chkval+1);
  inc_operands_v1(&a, &b, (UInt) args->diff[0], (UInt) args->diff[1]);
 }
 return args->loops;
}

int main(int argc, const char *argv[]) {
 return fun2_main(argc, argv, 128, ARGS_DEFAULT, &LIMITS, ARRAYSIZE(funname), funname, &exec_function_loop_, 2);
}
