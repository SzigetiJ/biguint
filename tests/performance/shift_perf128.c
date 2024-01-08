#include <time.h>

#include "biguint128.h"
#include "perf_common.h"
#include "perf_common128.h"

#define UINT_BITS (8U * sizeof(UInt))
const StandardConstraints LIMITS = {
 8,
 (1<<24) // 16M loops
};
const StandardArgs ARGS_DEFAULT = INIT_FUN1ARGS(
 (1<<20), // 1M loops
 3U,
 37U);

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

const char *funname[]={
  "shl(x,i)",
  "shl_or(new(),x,i)",
  "shl_tiny(copy(x),i)",
  "shr(x,i)",
  "shr_assign(copy(x),i)",
  "rol(x,i)",
  "ror(x,i)",
  "shl_or(x,i)",
  "shr_assign(x,i)",
  "shr_tiny(copy(x),i)"
};
const unsigned int fun_n = ARRAYSIZE(funname);

// ### Internal functions
static void exec_function_loop_(unsigned int ai, unsigned int fun, const StandardArgs *args) {
 BigUInt128 a = get_value_by_level(ai, args->levels);
 BigUInt128 chkval = biguint128_ctor_default();
 clock_t t0, t1;
 uint32_t loop_cnt;

 t0 = clock();
 for (loop_cnt = 0; loop_cnt < args->loops;) {
  if (fun == VARIANT_SHL) {
   for (buint_size_t i = 0; i < 128U; ++i) {
    BigUInt128 res = biguint128_shl(&a, i);
    biguint128_add_tiny(&chkval, res.dat[BIGUINT128_CELLS-1]>>(UINT_BITS-1));
    ++loop_cnt;
   }
  } else if (fun == VARIANT_SHL_OR_NEW) {
   for (buint_size_t i = 0; i < 128U; ++i) {
    BigUInt128 res = biguint128_ctor_default();
    biguint128_shl_or(&res, &a, i);
    biguint128_add_tiny(&chkval, res.dat[BIGUINT128_CELLS-1]>>(UINT_BITS-1));
    ++loop_cnt;
   }
  } else if (fun == VARIANT_SHL_TINY) {
   for (buint_size_t i = 0; i < 128U; ++i) {
    BigUInt128 res = biguint128_ctor_copy(&a);
    biguint128_shl_tiny(&res, i);
    biguint128_add_tiny(&chkval, res.dat[BIGUINT128_CELLS-1]>>(UINT_BITS-1));
    ++loop_cnt;
   }
  } else if (fun == VARIANT_SHR) {
   for (buint_size_t i = 0; i < 128U; ++i) {
    BigUInt128 res = biguint128_shr(&a, i);
    biguint128_add_tiny(&chkval, res.dat[0]&1);
    ++loop_cnt;
   }
  } else if (fun == VARIANT_SHR_ASSIGN_CP) {
   for (buint_size_t i = 0; i < 128U; ++i) {
    BigUInt128 res = biguint128_ctor_copy(&a);
    biguint128_shr_assign(&res, i);
    biguint128_add_tiny(&chkval, res.dat[0]&1);
    ++loop_cnt;
   }
  } else if (fun == VARIANT_SHR_TINY) {
   for (buint_size_t i = 0; i < 128U; ++i) {
    BigUInt128 res = biguint128_ctor_copy(&a);
    biguint128_shr_tiny(&res, i);
    biguint128_add_tiny(&chkval, res.dat[0]&1);
    ++loop_cnt;
   }
  } else if (fun == VARIANT_ROL) {
   for (buint_size_t i = 0; i < 128U; ++i) {
    BigUInt128 res = biguint128_rol(&a, i);
    biguint128_add_tiny(&chkval, res.dat[0]&1);
    ++loop_cnt;
   }
  } else if (fun == VARIANT_ROR) {
   for (buint_size_t i = 0; i < 128U; ++i) {
    BigUInt128 res = biguint128_ror(&a, i);
    biguint128_add_tiny(&chkval, res.dat[0]&1);
    ++loop_cnt;
   }
  } else if (fun == VARIANT_SHL_OR) {
   BigUInt128 res = biguint128_ctor_default();
   for (buint_size_t i = 0; i < 128U; ++i) {
    biguint128_shl_or(&res, &a, i);
    biguint128_add_tiny(&chkval, res.dat[BIGUINT128_CELLS-1]>>(UINT_BITS-1));
    ++loop_cnt;
   }
  } else if (fun == VARIANT_SHR_ASSIGN) {
   BigUInt128 res = biguint128_ctor_copy(&a);
   buint_size_t lzca = biguint128_lzc(&a);
   buint_size_t xc = lzca==0?0:lzca-1;
   for (buint_size_t i = 0; i < 128U; ++i) {
    res.dat[xc]|=1;   // fill it with something similar to a
    biguint128_shr_assign(&res, i);
    biguint128_add_tiny(&chkval, res.dat[0]&1);
    ++loop_cnt;
   }
  }
  biguint128_add_tiny(&a, (UInt)args->diff_a);
 }
 t1 = clock();

 print_exec_summary(t0, t1,funname[fun], loop_cnt, &chkval, 1);
}

// ### Main function
int main(int argc, const char *argv[]) {
 return fun1_main(argc, argv, 128, ARGS_DEFAULT, &LIMITS, fun_n, funname, &exec_function_loop_);
}
