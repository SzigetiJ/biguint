/*****************************************************************************

    Copyright 2023 SZIGETI János

    This file is part of biguint library (Big Unsigned Integers).

    Biguint is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Biguint is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*****************************************************************************/
#include <string.h>
#include "bigdecimal128.h"
#include "intio.h"

#define MINUS_SIGN '-'
#define PLUS_SIGN '+'
#ifndef DEC_DOT
#define DEC_DOT '.'
#endif

// INTERNAL TYPES
/**
 Auxiliary type for internal functions
 */
typedef struct {
 BigDecimal128 a;
 BigDecimal128 b;
 UInt prec;
} BigDecimalCommon128;

// STATIC FUNCTION DECLARATIONS
static inline BigDecimalCommon128 gen_common_prec_(const BigDecimal128 *a, const BigDecimal128 *b);
static inline buint_bool gen_common_xprec_safe_(UInt *aprec, UInt *bprec, UInt trg_prec, BigUInt128 *aval, BigUInt128 *bval, buint_bool *has_remainder);
static inline buint_bool gen_common_hiprec_safe_(UInt *aprec, UInt *bprec, BigUInt128 *aval, BigUInt128 *bval);
static inline buint_bool gen_common_loprec_safe_(UInt *aprec, UInt *bprec, BigUInt128 *aval, BigUInt128 *bval, buint_bool *has_remainder);
static inline UInt oom_estimation_(UInt base, UInt corr_step, UInt prec, const BigUInt128 *val);
static inline UInt oom_lb_(UInt prec, const BigUInt128 *val);
static inline UInt oom_ub_(UInt prec, const BigUInt128 *val);
static buint_bool tune_prec_(BigUInt128 *a, UInt *aprec, UInt trg_prec, buint_bool *has_remainder);
static buint_bool compare_(const BigDecimal128 *a, const BigDecimal128 *b, buint_bool lt);
static inline buint_bool addsub_safe_(BigDecimal128 *dest, const BigDecimal128 *a, const BigDecimal128 *b, buint_bool add);

// IMPLEMENTATION
// internal functions
static inline BigDecimalCommon128 gen_common_prec_(const BigDecimal128 *a, const BigDecimal128 *b) {
 UInt prec_hi = a->prec < b->prec ? b->prec : a->prec;
 return (BigDecimalCommon128) {
  bigdecimal128_ctor_prec(a, prec_hi),
  bigdecimal128_ctor_prec(b, prec_hi),
  prec_hi
 };
}

static inline buint_bool gen_common_xprec_safe_(UInt *aprec, UInt *bprec, UInt trg_prec, BigUInt128 *aval, BigUInt128 *bval, buint_bool *has_remainder) {
 buint_bool res_a = tune_prec_(aval, aprec, trg_prec, has_remainder);
 buint_bool res_b = tune_prec_(bval, bprec, trg_prec, has_remainder);
 return res_a && res_b;
}

static inline buint_bool gen_common_hiprec_safe_(UInt *aprec, UInt *bprec, BigUInt128 *aval, BigUInt128 *bval) {
 return gen_common_xprec_safe_(aprec, bprec, *aprec < *bprec ? *bprec : *aprec, aval, bval, NULL);
}

static inline buint_bool gen_common_loprec_safe_(UInt *aprec, UInt *bprec, BigUInt128 *aval, BigUInt128 *bval, buint_bool *has_remainder) {
 return gen_common_xprec_safe_(aprec, bprec, *aprec < *bprec ? *aprec : *bprec, aval, bval, has_remainder);
}

/**
 * Quick (negative) order of magnitude estimation frame-function.
 * Can be used to estimate (base-10*log10(x)) for any x=val*10^(-prec). Kind of dB(x).
 * @param base Choose a high enough base in order to always get positive result.
 * @param corr_step Within the estimation the -3*lzb(val) member needs a correction as lzb gets higher.
 * By means of corr_step a linear correction can be defined: if lzb(val) is greater than corr_step, the result is decreased by 1.
 * If lzb is greater than 2*corr_step, the result is decreased by 2, etc.
 * @param prec The examined value x is given by prec and val.
 * @param val The examined value x is given by prec and val.
 * @return Calculated order of magnitude descriptor. For x=1 it returns base.
 * For any x={1.0, 1.00, 1.000, etc} it return a value close to base.
 */
static inline UInt oom_estimation_(UInt base, UInt corr_step, UInt prec, const BigUInt128 *val) {
 buint_size_t lzb = biguint128_lzb(val);
 buint_size_t correction = lzb / corr_step;
 return base + 10 * prec - 3 * lzb - correction;
}

static inline UInt oom_lb_(UInt prec, const BigUInt128 *val) {
 return oom_estimation_(4 * 128, 96, prec, val);
}

/**
 * The base of oom_ub_ is increased by 3 (compared to oom_lb_).
 * That is highest n-bit value is almost as much as the lowest n+1 bit value.
 * @param prec
 * @param val
 * @return
 */
static inline UInt oom_ub_(UInt prec, const BigUInt128 *val) {
 return oom_estimation_(4 * 128 + 3, 100, prec, val);
}

/**
 * Multiplies a by 10^(trg_prec - aprec), (this can also be division), if possible.
 * Note, this function assumes that a is unsigned integer, however check if the result can fit into 128 - 1 bits.
 * @param a
 * @param aprec
 * @param trg_prec
 * @param has_remainder
 * @return
 */
static buint_bool tune_prec_(BigUInt128 *a, UInt *aprec, UInt trg_prec, buint_bool *has_remainder) {
 static BigUInt128 divs[] = {
  {{10}},
  {{100}}
 };
 if (*aprec < trg_prec) { // multiplication (may overflow)
  UInt pdiff = trg_prec - *aprec;
  buint_size_t lzb = biguint128_lzb(a);
  buint_bool low_a = lzb < 2 || !biguint128_gbit(a, lzb - 2); // low_a means a is 1 or b10xx..xxx. It counts as a third free bit
  buint_bool low2_a = low_a && (lzb < 3 || !biguint128_gbit(a, lzb - 3)); // low2_a means a is 1 or b100xx..xxx. It counts as a third free bit
  buint_size_t free_bits = 128 - 1 - lzb;

  if (free_bits * 3 + !!low_a + !!low2_a < pdiff * 10) { // rough estimation..
   return 0;
  }
  while (*aprec < trg_prec) {
   switch (trg_prec - *aprec) {
    case 1:
     *a = biguint128_mul10(a);
     ++*aprec;
     break;
    case 2:
     *a = biguint128_mul100(a);
     *aprec += 2;
     break;
    default:
     *a = biguint128_mul1000(a);
     *aprec += 3;
   }
  }
 } else {
  while (trg_prec < *aprec) { // divisions (no overflow danger)
   UInt pdiff = *aprec - trg_prec;
   BigUIntPair128 dx;
   switch (pdiff) {
    case 1:
    case 2:
     dx = biguint128_div(a, &divs[pdiff - 1]);
     *aprec -= pdiff;
     break;
    default:
     dx = biguint128_div1000(a);
     *aprec -= 3;
   }
   *a = dx.first;
   if (has_remainder != NULL) *has_remainder |= (dx.second.dat[0] != 0);
  }
 }
 return 1;

}

static buint_bool compare_(const BigDecimal128 *a, const BigDecimal128 *b, buint_bool lt) {
 UInt pdiff = b->prec - a->prec;

 // check #0: a and b are of the same precision
 if (pdiff == 0) {
  return lt ? bigint128_lt(&a->val, &b->val) : biguint128_eq(&a->val, &b->val);
 }

 // check #1: sign
 buint_bool aeqz = biguint128_eqz(&a->val);
 buint_bool beqz = biguint128_eqz(&b->val);
 buint_bool altz = bigint128_ltz(&a->val);
 buint_bool bltz = bigint128_ltz(&b->val);
 // math is as follows:
 // !xltz: (-inf,-1] -> 0; [0,inf) -> 1
 // !xeqz: (-inf,-1] -> 1; 0 -> 0; [1,inf) -> 1
 // !xeqz + 2 * !xltz: (-inf,-1]-> 1; 0 -> 2; [1,inf) -> 3
 if (!aeqz + 2 * !altz < !beqz + 2 * !bltz) return lt; // == (lt?1:0)
 if (!beqz + 2 * !bltz < !aeqz + 2 * !altz) return 0;
 if (aeqz && beqz) return !lt; // == (lt?0:1)
 // at this point ltz(a)==ltz(b) is sure.

 // check #2: order of magnitude
 BigUInt128 av = bigint128_abs(&a->val, NULL);
 BigUInt128 bv = bigint128_abs(&b->val, NULL);

 UInt ooma_lo = oom_lb_(a->prec, &av);
 UInt ooma_hi = oom_ub_(a->prec, &av);
 UInt oomb_lo = oom_lb_(b->prec, &bv);
 UInt oomb_hi = oom_ub_(b->prec, &bv);
 if (ooma_hi < oomb_lo) return lt && altz; // va gt vb. If the numbers were negative that means a lt b
 if (oomb_hi < ooma_lo) return lt && !altz;
 // at this point a and b are near the same order of magnitude

 // check #3: multiplication is possible
 UInt aprec = a->prec;
 UInt bprec = b->prec;
 buint_bool hiprec_ok = gen_common_hiprec_safe_(&aprec, &bprec, &av, &bv);
 if (hiprec_ok) {
  return lt ? biguint128_lt(&av, &bv) != altz : biguint128_eq(&av, &bv);
 }

 // check #4: division by 10 with remainder check
 buint_bool has_remainder = 0;
 gen_common_loprec_safe_(&aprec, &bprec, &av, &bv, &has_remainder);

 return !lt ? biguint128_eq(&av, &bv) && !has_remainder :
   biguint128_lt(&av, &bv) ? !altz :
   biguint128_lt(&bv, &av) ? altz :
   has_remainder && // at this point we know that av == bv. Still, a lt b is possible if the truncated digits were not zero
   ((!altz && a->prec == aprec) || // either b was truncated and became less - for positive values this means a lt b,
   (altz && b->prec == bprec)); // or a was truncated and for negative values this means that a became greater, thus originally a lt b.
}

static inline buint_bool addsub_safe_(BigDecimal128 *dest, const BigDecimal128 *a, const BigDecimal128 *b, buint_bool add) {
 buint_bool carry = 0;
 buint_bool retv = 1;
 static void (*fun[])(BigUInt128 *dest, const BigUInt128 *a, const BigUInt128 *b, buint_bool * carry) = {
  biguint128_adc_replace,
  biguint128_sbc_replace
 };
 if (a->prec == b->prec) {
  fun[!add](&dest->val, &a->val, &b->val, &carry);
  dest->prec = a->prec;
 } else {
  buint_bool altz;
  buint_bool bltz;
  BigDecimal128 av = {bigint128_abs(&a->val, &altz),a->prec};
  BigDecimal128 bv = {bigint128_abs(&b->val, &bltz),b->prec};

  retv = gen_common_hiprec_safe_(&av.prec, &bv.prec, &av.val, &bv.val);
  fun[!add != (altz != bltz)](&dest->val, &av.val, &bv.val, &carry);
  if (altz) {
   bigint128_negate_assign(&dest->val);
  }
  dest->prec = av.prec;
 }
 return retv;
}

// interface functions
BigDecimal128 bigdecimal128_ctor_default() {
 return (BigDecimal128){biguint128_ctor_default(), 0};
}

BigDecimal128 bigdecimal128_ctor_prec(const BigDecimal128 *a, UInt prec) {
 BigDecimal128 retv = *a;
 while (retv.prec != prec) {
  int precdiff = (((int)retv.prec - (int)prec) % 3 + 3) % 3;	// FIXME: dirty code..
  if (precdiff || retv.prec < prec) {
   switch (precdiff) {
    case 2:
     retv.val = biguint128_mul10(&retv.val);
     ++retv.prec;
     break;
    case 1:
     retv.val = biguint128_mul100(&retv.val);
     retv.prec+=2;
     break;
    default:
     retv.val = biguint128_mul1000(&retv.val);
     retv.prec+=3;
   }
  } else {
   retv.val = bigint128_div1000(&retv.val).first;
   retv.prec-=3;
  }
 }
 return retv;
}

buint_bool bigdecimal128_prec_safe(BigDecimal128 *dest, const BigDecimal128 *a, UInt prec) {
 buint_bool altz;
 dest->val = bigint128_abs(&a->val, &altz);
 dest->prec = a->prec;
 buint_bool retv = tune_prec_(&dest->val, &dest->prec, prec, NULL);
 if (altz) {
  bigint128_negate_assign(&dest->val);
 }
 return retv;
}

BigUIntPair128 bigdecimal128_trunc(const BigDecimal128 *a) {
 buint_bool altz;
 BigUIntTinyPair128 atmp = {bigint128_abs(&a->val, &altz), 0};
 BigUInt128 afrac = biguint128_ctor_default();
 BigUInt128 m10i = biguint128_ctor_unit();
 UInt pi = a->prec;
 while (pi) {
  atmp = biguint128_div10(&atmp.first);
  BigUInt128 ifrac = biguint128_value_of_uint(atmp.second);
  ifrac = biguint128_mul(&ifrac, &m10i);
  biguint128_add_assign(&afrac, &ifrac);
  --pi;
  m10i = biguint128_mul10(&m10i);
 }
 if (altz) {
  bigint128_negate_assign(&atmp.first);
  bigint128_negate_assign(&afrac);
 }
 return (BigUIntPair128){atmp.first, afrac};
}

BigDecimal128 bigdecimal128_add(const BigDecimal128 *a, const BigDecimal128 *b) {
 BigDecimalCommon128 cp = gen_common_prec_(a,b);

 return (BigDecimal128){biguint128_add(&cp.a.val, &cp.b.val), cp.prec};
}

buint_bool bigdecimal128_add_safe(BigDecimal128 *dest, const BigDecimal128 *a, const BigDecimal128 *b) {
 return addsub_safe_(dest, a, b, 1);
}

BigDecimal128 bigdecimal128_sub(const BigDecimal128 *a, const BigDecimal128 *b) {
 BigDecimalCommon128 cp = gen_common_prec_(a,b);

 return (BigDecimal128){biguint128_sub(&cp.a.val, &cp.b.val), cp.prec};
}

buint_bool bigdecimal128_sub_safe(BigDecimal128 *dest, const BigDecimal128 *a, const BigDecimal128 *b) {
 return addsub_safe_(dest, a, b, 0);
}

BigDecimal128 bigdecimal128_mul(const BigDecimal128 *a, const BigDecimal128 *b) {
 return (BigDecimal128){biguint128_mul(&a->val, &b->val), a->prec + b->prec};
}

buint_bool bigdecimal128_mul_safe(BigDecimal128 *dest, const BigDecimal128 *a, const BigDecimal128 *b) {
 buint_bool inv[2];
 BigUInt128 par[] = {bigint128_abs(&a->val, &inv[0]),bigint128_abs(&b->val, &inv[1])};
 BigUIntPair128 dmul = biguint128_dmul(&par[0], &par[1]);
 buint_bool valid = biguint128_eqz(&dmul.second);
 if (!!inv[0] != !!inv[1]) { // result should be not positive
  bigint128_negate_assign(&dmul.first);
  valid &= bigint128_ltz(&dmul.first) || biguint128_eqz(&dmul.first);
 } else {
  valid &= !bigint128_ltz(&dmul.first);
 }
 dest->val = dmul.first;
 dest->prec = a->prec + b->prec;
 return valid;
}

BigDecimal128 bigdecimal128_div_fast(const BigDecimal128 *a, const BigDecimal128 *b, UInt prec) {
 UInt ac_prec = a->prec + b->prec + prec;
 BigDecimal128 ac = bigdecimal128_ctor_prec(a, ac_prec);
 BigDecimal128 qc = {bigint128_div(&ac.val, &b->val).first, ac_prec - b->prec};
 return bigdecimal128_ctor_prec(&qc, prec);
}

BigDecimal128 bigdecimal128_div(const BigDecimal128 *a, const BigDecimal128 *b, UInt prec) {
 BigDecimalCommon128 cp = gen_common_prec_(a,b);

 BigUInt128 aa = cp.a.val;
 BigUInt128 bb = cp.b.val;
 BigDecimal128 retv = {biguint128_ctor_default(), 0};
 UInt curprec = 0;
 while (1) {
  BigUIntPair128 qq = bigint128_div(&aa, &bb);
  biguint128_add_assign(&retv.val, &qq.first);
  aa = qq.second;
  if (curprec == prec) break;
  ++curprec;
  retv.val = biguint128_mul10(&retv.val);
  aa = biguint128_mul10(&aa);
 }
 retv.prec = curprec;
 return retv;
}

buint_bool bigdecimal128_div_safe(BigDecimal128 *dest, const BigDecimal128 *a, const BigDecimal128 *b, UInt prec) {
 static buint_bool first = 1;
 static BigUInt128 max;
 if (first) {
  BigUInt128 minusone = bigint128_value_of_uint(-1);
  BigUInt128 maxx = biguint128_div5(&minusone).first;
  max = biguint128_shr(&maxx, 2);
 first = 0;
 }
 buint_bool ainv, binv;
 BigUInt128 av = bigint128_abs(&a->val, &ainv);
 BigUInt128 bv = bigint128_abs(&b->val, &binv);

 // div.prec = a.prec - b.prec
 // mul10 iterations: prec - div.prec = prec + b.prec - a.prec
 // 10 / 0.3 = 33 -- a.prec=0, b.prec=1
 // 100.00 / 3 = 33(.33) -- a.prec=2, b.prec=0
 if (a->prec < prec + b->prec) {
  UInt i = a->prec;
  dest->val = biguint128_ctor_default();
  while (1) {
   BigUIntPair128 qq = biguint128_div(&av, &bv);
   biguint128_add_assign(&dest->val, &qq.first);
   av = qq.second;
   if (i == prec + b->prec) break;
   ++i;
   if (biguint128_lt(&max, &dest->val)) return 0;
   dest->val = biguint128_mul10(&dest->val);
   av = biguint128_mul10(&av);
  }
  for (UInt i = a->prec; i < prec + b->prec; ++i) {

  }
 } else {
  for (UInt i = prec + b->prec; i < a->prec; ++i) {
   av = biguint128_div10(&av).first;
  }
  dest->val = biguint128_div(&av, &bv).first;
 }
 dest->prec = prec;
 if (ainv != binv) {
  bigint128_negate_assign(&dest->val);
 }
 return 1;
}

// I/O

BigDecimal128 bigdecimal128_ctor_cstream(const char *dec_digits, buint_size_t len) {
 BigDecimal128 retv;
 retv.val = biguint128_ctor_default();

 // find sign
 buint_size_t zpos = 0U;
 if (dec_digits[0] == MINUS_SIGN || dec_digits[0] == PLUS_SIGN) {
  ++zpos;
 }
 buint_bool neg = zpos && (dec_digits[0] == MINUS_SIGN);

 // find decimal point
 buint_bool has_decdot = 0U;
 buint_size_t dpos = 0U;
 for (buint_size_t i = zpos; !has_decdot && i < len; ++i) {
  if (dec_digits[i] == DEC_DOT) {
   has_decdot = 1U;
   dpos = i;
  }
 }

 // read digit-by-digit
 for (buint_size_t i = zpos; i < len; ++i) {
  if (has_decdot && i == dpos) continue;
  unsigned char d;
  get_digit(dec_digits[i], 10, &d);
  retv.val = biguint128_mul10(&retv.val);
  biguint128_add_tiny(&retv.val, d);
 }

 // set precision
 retv.prec = has_decdot ? len - dpos - 1 : 0;

 // flip negative number
 if (neg) {
  BigUInt128 zero = biguint128_ctor_default();
  retv.val = biguint128_sub(&zero, &retv.val);
 }
 return retv;
}

buint_size_t bigdecimal128_print(const BigDecimal128 *a, char *buf, buint_size_t buf_len) {
 buint_size_t aepos = bigint128_print_dec(&a->val, buf, buf_len);
 buint_size_t abpos = (aepos && buf[0] == MINUS_SIGN) ? 1 : 0;
 buint_size_t alen = aepos - abpos;
 if (alen == 0) return 0;
 if (a->prec != 0) { // decimal dot must be inserted
  if (alen <= a->prec) { // leading 0s must be inserted
   buint_size_t ashift = a->prec - alen + 1;
   if (buf_len < aepos + ashift) return 0;	// hard break
   memmove(&buf[abpos + ashift], &buf[abpos], alen);
   memset(&buf[abpos], '0', ashift);	// insert
   aepos += ashift;
  }
  // insert dot
  if (buf_len < aepos + 1) return 0;	// hard break
  for (buint_size_t i = 0; i < a->prec; ++i) {	// shift
   buf[aepos - i] = buf[aepos - 1 - i];
  }
  buf[aepos - a->prec]= DEC_DOT;	// insert
  ++aepos;
 }
 return aepos;
}

buint_bool bigdecimal128_lt(const BigDecimal128 *a, const BigDecimal128 *b) {
 return compare_(a,b,1);
}

buint_bool bigdecimal128_eq(const BigDecimal128 *a, const BigDecimal128 *b) {
 return compare_(a,b,0);
}

#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
BigDecimal128 bigdecimal128_ctor_precv(const BigDecimal128 a, UInt prec) {
 return bigdecimal128_ctor_prec(&a, prec);
}
BigDecimal128 bigdecimal128_addv(const BigDecimal128 a, const BigDecimal128 b) {
 return bigdecimal128_add(&a, &b);
}
BigDecimal128 bigdecimal128_subv(const BigDecimal128 a, const BigDecimal128 b) {
 return bigdecimal128_sub(&a, &b);
}
BigDecimal128 bigdecimal128_mulv(const BigDecimal128 a, const BigDecimal128 b) {
 return bigdecimal128_mul(&a, &b);
}
BigDecimal128 bigdecimal128_divv(const BigDecimal128 a, const BigDecimal128 b, UInt prec) {
 return bigdecimal128_div(&a, &b, prec);
}

BigDecimal128 bigdecimal128_absv(const BigDecimal128 a, buint_bool *result_inv) {
 return (BigDecimal128){bigint128_absv(a.val, result_inv), a.prec};
}

buint_bool bigdecimal128_ltv(const BigDecimal128 a, const BigDecimal128 b) {
 return bigdecimal128_lt(&a, &b);
}
buint_bool bigdecimal128_eqv(const BigDecimal128 a, const BigDecimal128 b) {
 return bigdecimal128_eq(&a, &b);
}
buint_size_t bigdecimal128_printv(const BigDecimal128 a, char *buf, buint_size_t buf_len) {
 return bigdecimal128_print(&a, buf, buf_len);
}
#endif