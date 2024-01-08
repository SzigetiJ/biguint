#ifndef PERF_COMMON_H
#define PERF_COMMON_H
#include <stdbool.h>

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

#define ARGMASK_LOOPS 1U
#define ARGMASK_LEVELS 2U
#define ARGMASK_LMASKA 4U
#define ARGMASK_LMASKB 8U
#define ARGMASK_DIFFA 16U
#define ARGMASK_DIFFB 32U
#define ARGMASK_FMASK 64U
#define ARGMASK_FEXMASK 128U
#define ARGMASK_ALL 255U

StandardArgs parse_args(int argc, const char *argv[], const StandardArgs res_init);
void print_help(const char *prgname, unsigned int argmask);
void print_help_all(const char *prgname, unsigned int bits, unsigned int argmask, unsigned int fun_n, const char *funname[]);
int fun2_main(int argc, const char *argv[],
        unsigned int bits, const StandardArgs args_init,
        unsigned int max_levels, unsigned int max_loops,
        unsigned int fun_n, const char *funname[],
        void (*internal_loop)(unsigned int ai, unsigned int bi, unsigned int funidx, const StandardArgs *args));
int fun1_main(int argc, const char *argv[],
        unsigned int bits, const StandardArgs args_init,
        unsigned int max_levels, unsigned int max_loops,
        unsigned int fun_n, const char *funname[],
        void (*internal_loop)(unsigned int ai, unsigned int funidx, const StandardArgs *args));

#endif /* PERF_COMMON_H */

