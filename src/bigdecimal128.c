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

BigDecimal128 bigdecimal128_add(const BigDecimal128 *a, const BigDecimal128 *b) {
 BigDecimalCommon128 cp = gen_common_prec_(a,b);

 return (BigDecimal128){biguint128_add(&cp.a.val, &cp.b.val), cp.prec};
}

BigDecimal128 bigdecimal128_sub(const BigDecimal128 *a, const BigDecimal128 *b) {
 BigDecimalCommon128 cp = gen_common_prec_(a,b);

 return (BigDecimal128){biguint128_sub(&cp.a.val, &cp.b.val), cp.prec};
}

BigDecimal128 bigdecimal128_mul(const BigDecimal128 *a, const BigDecimal128 *b) {
 return (BigDecimal128){biguint128_mul(&a->val, &b->val), a->prec + b->prec};
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
 BigDecimalCommon128 cp = gen_common_prec_(a,b);

 return bigint128_lt(&cp.a.val, &cp.b.val);
}

buint_bool bigdecimal128_eq(const BigDecimal128 *a, const BigDecimal128 *b) {
 BigDecimalCommon128 cp = gen_common_prec_(a,b);

 return biguint128_eq(&cp.a.val, &cp.b.val);
}

