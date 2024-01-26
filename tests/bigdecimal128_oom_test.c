/**********************************************
 * Checks the oom estimation on consecutive
 * numbers with many different precision values.
 * We estimate oom(a) with a range: oomrng(a)
 * (lower and upper bound).
 * There are two properties to assure:
 *  - if a is lower than b, oomrng(a) must not be lower
 *    than oomrng(b).
 *  - if a equals to b (but has different precision),
 *    oomrng(a) must not be lower than oomrng(b) and
 *    oomrng(a) must not be higher than oomrng(b).
 * Here, 'must not be lower' means that the upper bound of
 * 'a' is not allowed to be lower than the lower bound of 'b'.
 * 'Must not be higher' is defined in a similar way.
 * This test accepts command line arguments:
 * [$1]: test range (given in bits, default: 16,
 *       i.e., 2^16 numbers are tested).
 * [$2]: precision range (starting at 0,
 *       default: ~max valid precision).
 * [$3]: test range begin point (given in bits,
 *       default: 0, i.e., test begins with value '1').
 **********************************************/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "bigdecimal128.h"
#include "test_common.h"

#define DEFAULT_LIMIT_BITS 16
#define DEFAULT_LIMIT_PREC ((128 - DEFAULT_LIMIT_BITS) * 3 / 10)   // altogether we are still below biguint limit

// copied from bigdecimal128.c
static inline UInt oom_estimation_(UInt base, UInt corr_step, UInt prec, const BigUInt128 *val) {
 buint_size_t lzb = biguint128_lzb(val);
 buint_size_t correction = lzb / corr_step;
 return base + 10 * prec - 3 * lzb - correction;
}

static inline UInt oom_lb_(UInt prec, const BigUInt128 *val) {
 return oom_estimation_(4 * 128, 96, prec, val);
}

static inline UInt oom_ub_(UInt prec, const BigUInt128 *val) {
 return oom_estimation_(4 * 128 + 3, 100, prec, val);
}
// copy end

bool test_oom(const BigUInt128 *a, int maxprec, bool has_predecessor, UInt *lowest_ub, UInt *highest_lb) {
 bool pass = true;
 UInt lowest_ub_val;
 int lowest_ub_at;
 UInt highest_lb_val;
 int highest_lb_at;
 BigUInt128 ai = *a;
 for (int i = 0; i < maxprec; ++i) {
  UInt lbi = oom_lb_(i, &ai);
  UInt ubi = oom_ub_(i, &ai);
  if (i == 0 || highest_lb_val < lbi) {
   highest_lb_at = i;
   highest_lb_val = lbi;
  }
  if (i == 0 || ubi < lowest_ub_val) {
   lowest_ub_at = i;
   lowest_ub_val = ubi;
  }
  ai = biguint128_mul10(&ai);
 }
 if (lowest_ub_val < highest_lb_val) {
  char buf[128 + 1];
  buf[biguint128_print_dec(a, buf, 128)] = 0;
  fprintf(stderr, "OOM failed for %s. ub@%d: %d, lb@%d: %d\n", buf, lowest_ub_at, (int) lowest_ub_val, highest_lb_at, (int) highest_lb_val);
  pass = false;
 }
 if (has_predecessor) { // check relation to neighbor oom value
  // current number is higher than the neighbor, i.e., current oom cannot be higher.
  if (*lowest_ub < highest_lb_val) {
   char buf[128 + 1];
   buf[biguint128_print_dec(a, buf, 128)] = 0;
   fprintf(stderr, "OOM failed for %s. ub@%d: %d, former lb: %d\n", buf, lowest_ub_at, (int) lowest_ub_val, (int) *highest_lb);
   pass = false;
  }
 }

 *lowest_ub = lowest_ub_val;
 *highest_lb = highest_lb_val;

 return pass;
}

int main(int argc, const char *argv[]) {
 int limit_bits = DEFAULT_LIMIT_BITS;
 int limit_prec = DEFAULT_LIMIT_PREC;
 BigUInt128 a = biguint128_ctor_unit();
 if (1 < argc) { // test range given in power of 2.
  limit_bits = atoi(argv[1]);
 }
 if (2 < argc) { // precision depth
  limit_prec = atoi(argv[2]);
 }
 if (3 < argc) { // ai offset given in power of 2.
  a = biguint128_shl(&a, atoi(argv[3]));
 }

 BigUInt128 lim = biguint128_ctor_unit();
 biguint128_shl_tiny(&lim, limit_bits);
 biguint128_add_assign(&lim, &a);
 UInt lowest_ub;
 UInt highest_lb;
 int i = 0;
 while (biguint128_lt(&a, &lim)) {
  if ((i & 0xFFFF) == 0) { // show progress
   fprintf(stderr, ".");
  }
  assert(test_oom(&a, limit_prec, i != 0, &lowest_ub, &highest_lb));
  biguint128_inc(&a);
  ++i;
 }

 return 0;
}
