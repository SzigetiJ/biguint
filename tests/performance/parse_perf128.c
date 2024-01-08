/***************************************
 This example performs multiple (CYCLES)
 string parsing.
 The main purpose of this program is to
 check the performance of the parser
 function (biguint128_ctor_deccstream)
 with different input stream length.
 Input:
  [args]: see option -h
 Output:
  stdout: The sum of the last cells
   of the parsed values.
***************************************/

#include <string.h>
#include "biguint128.h"
#include "perf_common.h"
#include "perf_common128.h"

const StandardConstraints LIMITS = {
 8,
 (1<<20) // 1M loops
};
const StandardArgs ARGS_DEFAULT = INIT_FUN1ARGS(
 (1<<16), // 64K loops
 3U,
 37U);

#define DECDIGITS (3*128/10)	// A value with that many decimal digits must fit into 128 bits
#define HEXDIGITS (128/4)	// A value with that many hexadecimal digits must fit into 128 bits

typedef enum {
 FUN_PARSE_HEX = 0,
 FUN_PARSE_DEC
} ParseFun;

const char *funname[]={
 "ctor_hexcstream",
 "ctor_deccstream"
};
const unsigned int numwidth[] = {
 HEXDIGITS,
 DECDIGITS
};
const unsigned int fun_n = ARRAYSIZE(funname);


// ### Internal functions
static void exec_function_loop_(unsigned int ai, unsigned int fun, const StandardArgs *args) {
 char str[DECDIGITS];
 memset(str, '1', DECDIGITS);
 buint_size_t str_n = (numwidth[fun] * (ai + 1)) / args->levels;
 BigUInt128 chkval = biguint128_ctor_default();
 BigUInt128 res;
 clock_t t0, t1;

 t0 = clock();
 for (unsigned int i = 0; i < args->loops; ++i) {
  if (fun == FUN_PARSE_HEX) {
   res = biguint128_ctor_hexcstream(str, str_n);
  } else if (fun == FUN_PARSE_DEC) {
   res = biguint128_ctor_deccstream(str, str_n);
  }
  process_result_v1(&res, &chkval.dat[0]);
  str[i % str_n] = '0' + ((i * args->diff_a) % 10);
 }
 t1 = clock();
 print_exec_summary(t0, t1, funname[fun], args->loops, &chkval, 1);
}

// ### Main function
int main(int argc, const char *argv[]) {
 return fun1_main(argc, argv, 128, ARGS_DEFAULT, &LIMITS, fun_n, funname, &exec_function_loop_);
}
