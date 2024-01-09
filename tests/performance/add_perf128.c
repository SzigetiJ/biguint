#include "biguint128.h"
#include "perf_common.h"
#include "perf_common128.h"


// ### Constraints and default values
const StandardConstraints LIMITS = {
 8,
 (1<<28) // 256M loops
};
const StandardArgs ARGS_DEFAULT = INIT_FUN2ARGS(
 (1<<26), // 64M loops
 3U,
 37U,
 29U);

typedef enum {
 FUN_ADD = 0,
 FUN_ADD_ASGN,
 FUN_ADD_REPL,
 FUN_ADD_TINY,
 FUN_SUB,
 FUN_SUB_ASGN,
 FUN_SUB_REPL,
 FUN_SUB_TINY
} AdditiveFun;

const char *funname[]={
 "add",
 "add_assign",
 "add_replace",
 "add_tiny",
 "sub",
 "sub_assign",
 "sub_replace",
 "sub_tiny"
};

static unsigned int exec_function_loop_(unsigned int ai, unsigned int bi, unsigned int fun, const StandardArgs *args, UInt *chkval) {
 BigUInt128 a = get_value_by_level(ai, args->levels);
 BigUInt128 b = get_value_by_level(bi, args->levels);
 BigUInt128 res;
 BigUInt128 *procref = (fun & 1) ? &a : &res; // note, every second function is an assignment operation

 for (unsigned int i = 0; i < args->loops; ++i) {
  if (!(fun & 4)) { // lower 4 functions
   if (fun == FUN_ADD) {
    res = biguint128_add(&a, &b);
   } else if (fun == FUN_ADD_ASGN) {
    biguint128_add_assign(&a, &b);
   } else if (fun == FUN_ADD_TINY) {
    biguint128_add_tiny(&a, b.dat[0]);
   } else if (fun == FUN_ADD_REPL) {
    biguint128_add_replace(&res, &a, &b);
   }
  } else { // upper 4 functions
   if (fun == FUN_SUB) {
    res = biguint128_sub(&a, &b);
   } else if (fun == FUN_SUB_ASGN) {
    biguint128_sub_assign(&a, &b);
   } else if (fun == FUN_SUB_TINY) {
    biguint128_sub_tiny(&a, b.dat[0]);
   } else if (fun == FUN_SUB_REPL) {
    biguint128_sub_replace(&res, &a, &b);
   }
  }
  process_result_v1(procref, chkval);
  inc_operands_v1(&a, &b, args->diff[0], args->diff[1]);
 }
 return args->loops;
}

int main(int argc, const char *argv[]) {
 return fun2_main(argc, argv, 128, ARGS_DEFAULT, &LIMITS, ARRAYSIZE(funname), funname, &exec_function_loop_, 1);
}
