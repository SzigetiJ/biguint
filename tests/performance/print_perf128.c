/* This performance test executes
 biguint128 print routines multiple
 times with different values.
 Inputs:
  [argv]: execute this program with -h
 * (help) for information.
 Outputs:
  <stderr>: statistics.
*/
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
#define MAX_LOOPS (1<<20) // 1M loops
#define DEFAULT_LOOPS (1<<17) // 128K loops
#define BUFLEN (3*128/10)+10	// More than the approx. max. usage of 128 bit long dec string
#define FBUFLEN 40 // for full function names
#define INC_A 37U
#define INC_B 29U

// ### Local types
typedef enum {
 FUN_PRINT_HEX = 0,
 FUN_PRINT_DEC
} PrintFun;

// ### Constants
const char *funname[]={
 "print_hex",
 "print_dec"
};
const unsigned int fun_n = sizeof(funname) / sizeof(funname[0]);

const StandardArgs ARGS_DEFAULT = {
 DEFAULT_LOOPS, false, false,
 DEFAULT_LEVELS, -1, -1,
 INC_A, INC_B,
 -1, 0
};

// ### Internal functions
static void exec_function_loop_(unsigned int ai, unsigned int fun, const StandardArgs *args) {
 BigUInt128 a = get_value_by_level(ai, args->levels);
 BigUInt128 chkval = biguint128_ctor_default();
 char pbuf[BUFLEN + 1];
 buint_size_t plen;
 clock_t t0, t1;
 char fnamebuf[FBUFLEN];

 t0 = clock();
 for (unsigned int i = 0; i < args->loops; ++i) {
   if (fun == FUN_PRINT_HEX) {
    plen = biguint128_print_hex(&a, pbuf, BUFLEN);
   } else if (fun == FUN_PRINT_DEC) {
    plen = biguint128_print_dec(&a, pbuf, BUFLEN);
   }
  process_result_v2(pbuf, plen, &chkval.dat[0]);
  biguint128_add_tiny(&a, (UInt)args->diff_a);
 }
 t1 = clock();
 snprintf(fnamebuf, FBUFLEN, "%s + 2*add_tiny(C)", funname[fun]);
 print_exec_summary(t0, t1, fnamebuf, args->loops, &chkval, 1);
}

// ### Main function
int main(int argc, const char *argv[]) {
 return fun1_main(argc, argv, 128, ARGS_DEFAULT, MAX_LEVELS, MAX_LOOPS, fun_n, funname, &exec_function_loop_);
}
