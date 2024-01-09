#ifndef _PERF_COMMON_H_
#define _PERF_COMMON_H_

#include "biguint128.h"

#define BIGUINT_BITS 128

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

#undef BIGUINT_BITS
#endif

