#ifndef _PERF_COMMON_H_
#define _PERF_COMMON_H_

#include <stdio.h>

#include "biguint128.h"

#define UINT_BITS (8U * sizeof(UInt))
#define BIGUINT_BITS 128
#define DEC_BIGUINTLEN ((BIGUINT_BITS / 10 + 1) * 3 + 1) // a good approximation, since 2^10 >~ 10^3

typedef struct {
 char *name[3];
 BigUInt128 val[3];
} PerfTestInitValues;

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

static inline BigUInt128 get_value_by_level(unsigned int level, unsigned int max) {
 BigUInt128 retv = biguint128_ctor_unit();
 unsigned int shift = (level * BIGUINT_BITS) / max;
 return biguint128_shl(&retv, shift);
}

static inline void inc_operands_v1(BigUInt128 *a, BigUInt128 *b, UInt ainc, UInt binc) {
 biguint128_add_tiny(a, ainc);
 biguint128_add_tiny(b, binc);
}

/**
 * 1st variant of processing the result: accumulate something of a in b.
 * @param a Input: retrieve some information from this value.
 * @param b Input/output: modify somhow this value.
 */
static inline void process_result_v1(BigUInt128 *a, UInt *b) {
 static buint_size_t idx = 0;
 *b+=a->dat[idx];
 if (++idx == BIGUINT128_CELLS) idx = 0;
}

static inline void process_result_v2(const char *buf, unsigned int buflen, UInt *b) {
 for (unsigned int i=0; i<buflen; ++i) {
  *b+=buf[i];
 }
}

static inline void process_result_v3(UInt *a, UInt *b) {
 *b+=*a;
}

#undef UINT_BITS
#undef BIGUINT_BITS
#undef DEC_BIGUINTLEN
#endif

