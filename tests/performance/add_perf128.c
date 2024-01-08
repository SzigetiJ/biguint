#include <stdio.h>
#include <time.h>

#include "biguint128.h"
#include "perf_common.h"
#include "perf_common128.h"


// ### Constraints and default values
#define BUFLEN 40 // for full function names
const StandardConstraints LIMITS = {
 8,
 (1<<28) // 256M loops
};
const StandardArgs ARGS_DEFAULT = INIT_FUN2ARGS(
 (1<<26), // 64M loops
 3U,
 37U,
 29U);

// ### Local types
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

// ### Constants
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
const unsigned int fun_n = sizeof(funname) / sizeof(funname[0]);

// ### Internal functions
static void exec_function_loop_(unsigned int ai, unsigned int bi, unsigned int fun, const StandardArgs *args) {
 BigUInt128 a = get_value_by_level(ai, args->levels);
 BigUInt128 b = get_value_by_level(bi, args->levels);
 BigUInt128 chkval = biguint128_ctor_default();
 BigUInt128 res;
 BigUInt128 *procref = (fun & 1) ? &a : &res; // note, every second function is an assignment operation
 clock_t t0, t1;
 char fnamebuf[BUFLEN];

 t0 = clock();
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
  process_result_v1(procref, &chkval.dat[0]);
  inc_operands_v1(&a, &b, args->diff_a, args->diff_b);
 }
 t1 = clock();
 snprintf(fnamebuf, BUFLEN, "%s + 2*add_tiny(C)", funname[fun]);
 print_exec_summary(t0, t1, fnamebuf, args->loops, &chkval, 1);
}

// ### Main function
int main(int argc, const char *argv[]) {
 return fun2_main(argc, argv, 128, ARGS_DEFAULT, &LIMITS, fun_n, funname, &exec_function_loop_);
}
