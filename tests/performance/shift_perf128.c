#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "biguint128.h"
#include "perf_common128.h"

#define LOOPS (1<<20) // 1M loops
#define UINT_BITS (8U * sizeof(UInt))

typedef enum {
 VARIANT_SHL,
 VARIANT_SHL_OR_NEW,
 VARIANT_SHL_TINY,
 VARIANT_SHR,
 VARIANT_SHR_ASSIGN_CP,
 VARIANT_ROL,
 VARIANT_ROR,
 VARIANT_SHL_OR,
 VARIANT_SHR_ASSIGN,
 VARIANT_SHR_TINY
} ShiftVariant;
void exec_shift(const BigUInt128 *ainit, const BigUInt128 *astep, ShiftVariant v) {
 uint32_t loop_cnt;
 clock_t t0, t1;

 BigUInt128 bx = biguint128_ctor_copy(ainit);
 BigUInt128 bsum = biguint128_ctor_default();

 t0 = clock();
 for (loop_cnt = 0; loop_cnt < LOOPS;) {
  if (v == VARIANT_SHL) {
   for (buint_size_t i = 0; i < 128U; ++i) {
    BigUInt128 res = biguint128_shl(&bx, i);
    biguint128_add_tiny(&bsum, res.dat[BIGUINT128_CELLS-1]>>(UINT_BITS-1));
    ++loop_cnt;
   }
  } else if (v == VARIANT_SHL_OR_NEW) {
   for (buint_size_t i = 0; i < 128U; ++i) {
    BigUInt128 res = biguint128_ctor_default();
    biguint128_shl_or(&res, &bx, i);
    biguint128_add_tiny(&bsum, res.dat[BIGUINT128_CELLS-1]>>(UINT_BITS-1));
    ++loop_cnt;
   }
  } else if (v == VARIANT_SHL_TINY) {
   for (buint_size_t i = 0; i < 128U; ++i) {
    BigUInt128 res = biguint128_ctor_copy(&bx);
    biguint128_shl_tiny(&res, i);
    biguint128_add_tiny(&bsum, res.dat[BIGUINT128_CELLS-1]>>(UINT_BITS-1));
    ++loop_cnt;
   }
  } else if (v == VARIANT_SHR) {
   for (buint_size_t i = 0; i < 128U; ++i) {
    BigUInt128 res = biguint128_shr(&bx, i);
    biguint128_add_tiny(&bsum, res.dat[0]&1);
    ++loop_cnt;
   }
  } else if (v == VARIANT_SHR_ASSIGN_CP) {
   for (buint_size_t i = 0; i < 128U; ++i) {
    BigUInt128 res = biguint128_ctor_copy(&bx);
    biguint128_shr_assign(&res, i);
    biguint128_add_tiny(&bsum, res.dat[0]&1);
    ++loop_cnt;
   }
  } else if (v == VARIANT_SHR_TINY) {
   for (buint_size_t i = 0; i < 128U; ++i) {
    BigUInt128 res = biguint128_ctor_copy(&bx);
    biguint128_shr_tiny(&res, i);
    biguint128_add_tiny(&bsum, res.dat[0]&1);
    ++loop_cnt;
   }
  } else if (v == VARIANT_ROL) {
   for (buint_size_t i = 0; i < 128U; ++i) {
    BigUInt128 res = biguint128_rol(&bx, i);
    biguint128_add_tiny(&bsum, res.dat[0]&1);
    ++loop_cnt;
   }
  } else if (v == VARIANT_ROR) {
   for (buint_size_t i = 0; i < 128U; ++i) {
    BigUInt128 res = biguint128_ror(&bx, i);
    biguint128_add_tiny(&bsum, res.dat[0]&1);
    ++loop_cnt;
   }
  } else if (v == VARIANT_SHL_OR) {
   BigUInt128 res = biguint128_ctor_default();
   for (buint_size_t i = 0; i < 128U; ++i) {
    biguint128_shl_or(&res, &bx, i);
    biguint128_add_tiny(&bsum, res.dat[BIGUINT128_CELLS-1]>>(UINT_BITS-1));
    ++loop_cnt;
   }
  } else if (v == VARIANT_SHR_ASSIGN) {
   BigUInt128 res = biguint128_ctor_copy(&bx);
   for (buint_size_t i = 0; i < 128U; ++i) {
    biguint128_shr_assign(&res, i);
    biguint128_add_tiny(&bsum, res.dat[0]&1);
    ++loop_cnt;
   }
  }
  biguint128_add_assign(&bx, astep);
 }
 t1 = clock();

 print_exec_summary(t0, t1,
  v==VARIANT_SHL?"shl(x,i)":
  v==VARIANT_SHL_OR_NEW?"shl_or(new(),x,i)":
  v==VARIANT_SHL_TINY?"shl_tiny(copy(x),i)":
  v==VARIANT_SHR?"shr(x,i)":
  v==VARIANT_SHR_ASSIGN_CP?"shr_assign(copy(x),i)":
  v==VARIANT_SHR_TINY?"shr_tiny(copy(x),i)":
  v==VARIANT_ROL?"rol(x,i)":
  v==VARIANT_ROR?"ror(x,i)":
  v==VARIANT_SHL_OR?"shl_or(x,i)":
  "shr_assign(x,i)",
  loop_cnt, &bsum, 1);
}

int main() {
 PerfTestInitValues b = get_std_initvalues();
 BigUInt128 bstep = biguint128_value_of_uint(29);
 ShiftVariant variant[] = {
  VARIANT_SHL,
  VARIANT_SHL_OR_NEW,
  VARIANT_SHL_TINY,
  VARIANT_SHR,
  VARIANT_SHR_ASSIGN_CP,
  VARIANT_SHR_TINY,
  VARIANT_ROL,
  VARIANT_ROR,
  VARIANT_SHL_OR,
  VARIANT_SHR_ASSIGN
 };

 for (int i=0; i<3; ++i) {
  fprintf(stdout, "*** Shifting %s numbers ***\n", b.name[i]);
  for (int j=0; j<sizeof(variant)/sizeof(variant[0]); ++j) {
   exec_shift(&b.val[i], &bstep, variant[j]);
  }
 }

 return 0;
}
