#ifndef PERF_COMMON_H
#define PERF_COMMON_H
#include <stdbool.h>
#include "uint_types.h"

typedef struct {
 unsigned int levels;
 unsigned int loops;
} StandardConstraints;

typedef struct {
 unsigned int loops;
 bool error;
 bool help;
 unsigned char levels;
 unsigned int lmask_a;
 unsigned int lmask_b;
 int diff_a;
 int diff_b;
 unsigned int fmask;
 unsigned int fexmask;
} StandardArgs;

typedef unsigned int (*LoopFunction2)(unsigned int ai, unsigned int bi, unsigned int funidx, const StandardArgs *args, UInt *chkval);
typedef unsigned int (*LoopFunction1)(unsigned int ai, unsigned int funidx, const StandardArgs *args, UInt *chkval);
typedef unsigned int (*LoopFunctionN)(unsigned int *xi, unsigned int funidx, const StandardArgs *args, UInt *chkval);

#define ARGMASK_LOOPS 1U
#define ARGMASK_LEVELS 2U
#define ARGMASK_LMASKA 4U
#define ARGMASK_LMASKB 8U
#define ARGMASK_DIFFA 16U
#define ARGMASK_DIFFB 32U
#define ARGMASK_FMASK 64U
#define ARGMASK_FEXMASK 128U
#define ARGMASK_ALL 255U

#define INIT_FUN1ARGS(LOOPS,LEVELS,INCA) (StandardArgs){LOOPS, false, false, LEVELS, -1, 0, INCA, 0, -1, 0}
#define INIT_FUN2ARGS(LOOPS,LEVELS,INCA,INCB) (StandardArgs){LOOPS, false, false, LEVELS, -1, -1, INCA, INCB, -1, 0}
#define ARRAYSIZE(X) (sizeof(X)/sizeof(X[0]))

StandardArgs parse_args(int argc, const char *argv[], const StandardArgs res_init);
void print_help(const char *prgname, unsigned int argmask);
void print_help_all(const char *prgname, unsigned int bits, unsigned int argmask, unsigned int fun_n, const char *funname[]);
int fun2_main(int argc, const char *argv[],
        unsigned int bits, const StandardArgs args_init, const StandardConstraints *max,
        unsigned int fun_n, const char *funname[],
        LoopFunction2 internal_loop,
        unsigned int chkval_n);
int fun1_main(int argc, const char *argv[],
        unsigned int bits, const StandardArgs args_init, const StandardConstraints *max,
        unsigned int fun_n, const char *funname[],
        LoopFunction1 internal_loop,
        unsigned int chkval_n);
#endif /* PERF_COMMON_H */

