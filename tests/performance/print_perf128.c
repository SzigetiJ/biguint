/* This performance test executes
 biguint128 print routines multiple
 times with different values.
 Inputs:
  [argv]: execute this program with -h
 * (help) for information.
 Outputs:
  <stderr>: statistics.
*/
#include "biguint128.h"
#include "perf_common.h"
#include "perf_common128.h"


// ### Constraints and default values
#define BUFLEN (3*128/10)+10	// More than the approx. max. usage of 128 bit long dec string
const StandardConstraints LIMITS = {
 8,
 (1<<20) // 1M loops
};
const StandardArgs ARGS_DEFAULT = INIT_FUN1ARGS(
 (1<<17), // 128K loops
 3U,
 37U);

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

// ### Internal functions
static unsigned int exec_function_loop_(unsigned int ai, unsigned int fun, const StandardArgs *args, UInt *chkval) {
 BigUInt128 a = get_value_by_level(ai, args->levels);
 char pbuf[BUFLEN + 1];
 buint_size_t plen;

 for (unsigned int i = 0; i < args->loops; ++i) {
   if (fun == FUN_PRINT_HEX) {
    plen = biguint128_print_hex(&a, pbuf, BUFLEN);
   } else if (fun == FUN_PRINT_DEC) {
    plen = biguint128_print_dec(&a, pbuf, BUFLEN);
   }
  process_result_v2(pbuf, plen, chkval);
  biguint128_add_tiny(&a, (UInt)args->diff_a);
 }
 return args->loops;
}

// ### Main function
int main(int argc, const char *argv[]) {
 return fun1_main(argc, argv, 128, ARGS_DEFAULT, &LIMITS, fun_n, funname, &exec_function_loop_, 1);
}
