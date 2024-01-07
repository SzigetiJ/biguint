#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "biguint128.h"
#include "perf_common.h"
#include "perf_common128.h"


// ### Constraints and default values
#define MAX_LEVELS 8U
#define DEFAULT_LEVELS 3U
#define MAX_LOOPS (1<<26) // 64M loops
#define DEFAULT_LOOPS (1<<20) // 1M loops
#define BUFLEN 40 // for full function names
#define INC_A 37U
#define INC_B 29U

// ### Local types
typedef enum {
 FUN_MUL = 0,
 FUN_DMUL,
 FUN_DIV
} MultiplicativeFun;

// ### Constants
const char *funname[]={
 "mul",
 "dmul",
 "div"
};
const unsigned int fun_n = sizeof(funname) / sizeof(funname[0]);

const StandardArgs ARGS_DEFAULT = {
 DEFAULT_LOOPS, false, false,
 DEFAULT_LEVELS, -1, -1,
 INC_A, INC_B,
 -1, 0
};


// ### Internal functions
static void exec_function_loop_(unsigned int ai, unsigned int bi, unsigned int fun, const StandardArgs *args) {
 BigUInt128 a = get_value_by_level(ai, args->levels);
 BigUInt128 b = get_value_by_level(bi, args->levels);
 BigUInt128 chkval = biguint128_ctor_default();
 BigUInt128 res;
 BigUIntPair128 resp;
 BigUInt128 *procref1 = fun==FUN_MUL?&res:&resp.first;
 BigUInt128 *procref2 = fun==FUN_MUL?&res:&resp.second;
 clock_t t0, t1;
 char fnamebuf[BUFLEN];

 t0 = clock();
 for (unsigned int i = 0; i < args->loops; ++i) {
  if (fun == FUN_MUL) {
   res = biguint128_mul(&a, &b);
  } else if (fun == FUN_DMUL) {
   resp = biguint128_dmul(&a, &b);
  } else if (fun == FUN_DIV) {
   resp = biguint128_div(&a, &b);
  }
  process_result_v1(procref2, &chkval.dat[0]);
  process_result_v1(procref1, &chkval.dat[0]);
  inc_operands_v1(&a, &b, (UInt) args->diff_a, (UInt) args->diff_b);
 }
 t1 = clock();
 snprintf(fnamebuf, BUFLEN, "%s + 2*add_tiny(C)", funname[fun]);
 print_exec_summary(t0, t1, fnamebuf, args->loops, &chkval, 1);
}

// ### Main function
int main(int argc, const char *argv[]) {
 return fun2_main(argc, argv, 128, ARGS_DEFAULT, MAX_LEVELS, MAX_LOOPS, fun_n, funname, &exec_function_loop_);
}
