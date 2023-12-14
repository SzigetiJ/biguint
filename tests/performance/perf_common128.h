#ifndef _PERF_COMMON_H_
#define _PERF_COMMON_H_

#include <stdio.h>
#include <time.h>

#include "biguint128.h"

#define UINT_BITS (8U * sizeof(UInt))
#define BIGUINT_BITS 128
#define DEC_BIGUINTLEN ((BIGUINT_BITS / 10 + 1) * 3 + 1) // a good approximation, since 2^10 >~ 10^3

typedef struct {
 char *name[3];
 BigUInt128 val[3];
} PerfTestInitValues;

static inline void print_exec_time(clock_t t_begin, clock_t t_end, const char *op, int cnt) {
 clock_t dt = t_end - t_begin;
 fprintf(stdout, "=== %d BigUInt128 %s operations ===\n", cnt, op);
 fprintf(stdout, "Elapsed time: %ld us,\t%.1f op/s\n", dt, (1000000.0 * cnt) / dt);
}

static inline void print_exec_summary(clock_t t_begin, clock_t t_end, const char *op, int cnt, const BigUInt128 *val, int valnum) {
 char buf[DEC_BIGUINTLEN];
 print_exec_time(t_begin, t_end, op, cnt);
 for (int i=0; i<valnum; ++i) {
  buf[biguint128_print_dec(val, &buf[i], DEC_BIGUINTLEN-1)] = 0;
  fprintf(stdout, " check value #%d: %s\n", i+1, buf);
 }
}

static inline PerfTestInitValues get_std_initvalues() {
 PerfTestInitValues retv = {
  {"low","medium","high"}
 };
 for (int i = 0; i<3; ++i) {
  retv.val[i]=biguint128_ctor_default();
 }
 biguint128_sbit(&retv.val[1], UINT_BITS * (BIGUINT128_CELLS / 2));
 biguint128_sbit(&retv.val[2], UINT_BITS * (BIGUINT128_CELLS - 1));
 return retv;
}

#undef UINT_BITS
#undef BIGUINT_BITS
#undef DEC_BIGUINTLEN
#endif

