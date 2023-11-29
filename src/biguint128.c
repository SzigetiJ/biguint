/*****************************************************************************

    Copyright 2020 SZIGETI János

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
#include "biguint128.h"
#include "intio.h"
#include "string.h"
#include "uint.h"

// Local definitions (cleaned up at end of file)
#define FOREACHCELL(i) for (buint_size_t i=0u; i<BIGUINT128_CELLS; ++i)
#define UINT_BYTES sizeof(UInt)
#define UINT_BITS (8 * UINT_BYTES)
#define MINUS_SIGN '-'

// Assertions
// SIZEOF_UINT checked by configure script
#if (SIZEOF_UINT < 2)
#error Current implementation requires UInt basic storage type being at least 2 bytes wide.
#endif

// static function declarations
static inline buint_size_p bitpos_(buint_size_t a);
static inline buint_bool is_bigint_negative_(const BigUInt128 *a);
static inline BigUInt128 *clrall_(BigUInt128 *a);

static inline BigUIntPair128 d1024_(const BigUInt128 *a);
static inline BigUIntPair128 d1000_(const BigUInt128 *a);
static inline BigUInt128 m10_(const BigUInt128 *a);

static buint_size_t biguint128_print_dec_anywhere_(const BigUInt128 *a, char *buf, buint_size_t buf_len, buint_size_t *offset);

// implementations

/////////////////////
// internal functions

/**
 * Splits a bit position into pair of byte index [0..UINT_BYTES) and internal bit position [0,8)
 * @param a Bit position in BigUInt.
 * @return byte_sel: byte index, bit_sel: bit position inside the indexed byte.
 */
static inline buint_size_p bitpos_(buint_size_t a) {
 return (buint_size_p){a/UINT_BITS,a%UINT_BITS};
}

/**
 * Is the signed value than 0?
 * @param a Subject of check.
 * @return a is less than 0 (sign bit set).
 */
static inline buint_bool is_bigint_negative_(const BigUInt128 *a) {
 return biguint128_gbit(a, 128u - 1u);
}

/**
 * Clears all data (set to 0).
 * @param a Subject of operation;
 * @return a.
 */
static inline BigUInt128 *clrall_(BigUInt128 *a) {
 FOREACHCELL(i) {
  a->dat[i]=0;
 }
 return a;
}

/**
 * Division by 1024. Division by 1000 relies on this function.
 * @param a Divident.
 * @return Pair of quotient and remainder.
 */
static inline BigUIntPair128 d1024_(const BigUInt128 *a) {
 static const BigUInt128 x1023={{0x3FF}};

 BigUIntPair128 retv;
 retv.first= biguint128_shr(a, 10);
 retv.second= biguint128_and(a, &x1023);
 return retv;
}

/**
 * Division by 1000. Definitely faster than biguint128_div().
 * The decimal printer function HEX->DEC conversion,
 * and without this function it would take a long time.
 * Maybe this function will be promoted to the interface,
 * since div/mod by 1000 is called frequently
 * (e.g., ms->s, mm->m, etc. conversions).
 * @param a Divident.
 * @return Pair of quotient and remainder.
 */
static inline BigUIntPair128 d1000_(const BigUInt128 *a) {
 // The procedure goes like this:
 // We have to containers (retv.first, retv.second), and at the end
 // these will store the quotient and the remainder, respectively.
 // Initially, retv.first is empty and retv.second stores the whole amount of a.
 // Step-by-step retv.first is increased while retv.second is decreased.
 // Note that 1000 * retv.first + retv.second = *a remains invariant
 // during the whole process.
 // In phase #1 we exploit that
 // a = 1024*b + c = (1000*b) + (24*b+c)
 // and reduce retv.second iteratively until it gets small enough.
 // In phase #2 we just subtract 1000 from the remainder if it is still too high.
 // Well, the while loop is an overkill for this limit (2000).
 static const BigUInt128 x1={{1}};
 static const BigUInt128 x1000={{0x3E8}};
 static const BigUInt128 x2000={{0x7D0}};

 BigUIntPair128 retv= {biguint128_ctor_default(), biguint128_ctor_copy(a)};
 // Phase 1:
 while (biguint128_lt(&x2000, &retv.second)) {
  BigUIntPair128 x= d1024_(&retv.second);
  biguint128_add_assign(&retv.first, &x.first);
  BigUInt128 d_mul8= biguint128_shl(&x.first, 3);
  BigUInt128 d_mul16= biguint128_shl(&x.first, 4);
  retv.second = biguint128_add(&x.second, &d_mul16);
  biguint128_add_assign(&retv.second, &d_mul8);
 }
 // Phase 2:
 while (!biguint128_lt(&retv.second,&x1000)) {
  biguint128_add_assign(&retv.first,&x1);
  biguint128_sub_assign(&retv.second,&x1000);
 }
 return retv;
}

/**
 * Multiplication by 10.
 */
static inline BigUInt128 m10_(const BigUInt128 *a) {
 BigUInt128 a2 = biguint128_shl(a,1);
 BigUInt128 a8 = biguint128_shl(a,3);
 biguint128_add_assign(&a8, &a2);
 return a8;
}

// END internal functions
/////////////////////

/////////////////////
// public functions

BigUInt128 biguint128_ctor_default() {
 BigUInt128 retv;
 return *clrall_(&retv);
}

BigUInt128 biguint128_ctor_unit() {
 BigUInt128 retv = biguint128_ctor_default();
 retv.dat[0]=1;
 return retv;
}

BigUInt128 biguint128_ctor_standard(const UInt *dat) {
 BigUInt128 retv;
 FOREACHCELL(i) {
  retv.dat[i]=dat[i];
 }
 return retv;
}

BigUInt128 biguint128_ctor_copy(const BigUInt128 *orig) {
 return biguint128_ctor_standard(orig->dat);
}

BigUInt128 biguint128_ctor_hexcstream(const char *hex_digits, buint_size_t len) {
 BigUInt128 retv=biguint128_ctor_default();
 buint_bool ready = 0;
 for (buint_size_t i = 0; !ready && i < BIGUINT128_CELLS; ++i) {
  for (buint_size_t j = 0; !ready && j < UINT_BYTES; ++j) {
   buint_size_t offset = 2 * (UINT_BYTES * i + j);
   if (offset < len) {
    unsigned char x;
    if (offset + 1 < len) {
     get_hexbyte(hex_digits + len - offset - 2, &x);
    } else {
     get_digit(hex_digits[len - offset - 1], 16, &x);
    }
    retv.dat[i]|= (UInt)x << 8*j;
   } else {
    ready = 1;
   }
  }
 }
 return retv;
}

BigUInt128 biguint128_ctor_deccstream(const char *dec_digits, buint_size_t len) {
 BigUInt128 retv=biguint128_ctor_default();
 for (buint_size_t i = 0; i < len; ++i) {
  unsigned char d;
  get_digit(dec_digits[i], 10, &d);
  BigUInt128 dx = biguint128_value_of_uint(d);
  retv = m10_(&retv);
  biguint128_add_assign(&retv, &dx);
 }
 return retv;
}

BigUInt128 bigint128_ctor_deccstream(const char *dec_digits, buint_size_t len) {
 if (len && dec_digits[0] == MINUS_SIGN) {
  BigUInt128 zero = biguint128_ctor_default();
  BigUInt128 val = biguint128_ctor_deccstream(dec_digits+1, len-1);
  return biguint128_sub(&zero, &val);
 } else {
  return biguint128_ctor_deccstream(dec_digits, len);
 }
}

BigUInt128 biguint128_value_of_uint(UInt value) {
 BigUInt128 retv = biguint128_ctor_default();
 retv.dat[0] = value;
 return retv;
}

buint_size_t biguint128_import(BigUInt128 *dest, const char *src) {
 memcpy(&dest->dat, src, BIGUINT128_CELLS * UINT_BYTES);
 return BIGUINT128_CELLS * UINT_BYTES;
}


///////////////
// BEGIN ADD //

BigUInt128 biguint128_add(const BigUInt128 *a, const BigUInt128 *b) {
 BigUInt128 retv;
 biguint128_add_replace(&retv, a, b);
 return retv;
}

BigUInt128 *biguint128_add_assign(BigUInt128 *a, const BigUInt128 *b) {
 biguint128_add_replace(a,a,b);
 return a;
}

/**
 * Adds *a and *b and stores the result in *dest.
 * @param dest
 * @param a
 * @param b
 */
void biguint128_add_replace(BigUInt128 *dest, const BigUInt128 *a, const BigUInt128 *b) {
 buint_bool carry = 0;
 biguint128_adc_replace(dest, a, b, &carry);
}

/**
 * Adds *a *b and *carry, and stores the result in *dest and *carry.
 * @param dest
 * @param a
 * @param b
 * @param carry input/output overflow.
 */
void biguint128_adc_replace(BigUInt128 *dest, const BigUInt128 *a, const BigUInt128 *b, buint_bool *carry) {
 FOREACHCELL(i) {
  dest->dat[i] = uint_add(a->dat[i], b->dat[i], carry);
 }
}

// END ADD   //
///////////////

///////////////
// BEGIN SUB //

BigUInt128 biguint128_sub(const BigUInt128 *a, const BigUInt128 *b) {
 BigUInt128 retv;
 biguint128_sub_replace(&retv, a, b);
 return retv;
}

BigUInt128 *biguint128_sub_assign(BigUInt128 *a, const BigUInt128 *b) {
 biguint128_sub_replace(a,a,b);
 return a;
}

/**
 * Subtracts *b from *a and stores the result in *dest.
 * @param dest
 * @param a
 * @param b
 */
void biguint128_sub_replace(BigUInt128 *dest, const BigUInt128 *a, const BigUInt128 *b) {
 buint_bool carry = 0;
 biguint128_sbc_replace(dest, a, b, &carry);
}

/**
 * Subtracts *b and *carry from *a, and stores the result in *dest and *carry.
 * @param dest
 * @param a
 * @param b
 * @param carry input/output underflow.
 */
void biguint128_sbc_replace(BigUInt128 *dest, const BigUInt128 *a, const BigUInt128 *b, buint_bool *carry) {
 FOREACHCELL(i) {
  dest->dat[i] = uint_sub(a->dat[i], b->dat[i], carry);
 }
}

// END SUB   //
///////////////

BigUInt128 *biguint128_inc(BigUInt128 *a) {
 buint_bool carry= 1u;
 for (buint_size_t i= 0u; carry && i<BIGUINT128_CELLS; ++i) {
  a->dat[i]= uint_add(a->dat[i], 0u, &carry);
 }
 return a;
}

BigUInt128 *biguint128_dec(BigUInt128 *a) {
 buint_bool carry= 1u;
 for (buint_size_t i= 0u; carry && i<BIGUINT128_CELLS; ++i) {
  a->dat[i]= uint_sub(a->dat[i], 0u, &carry);
 }
 return a;
}


BigUInt128 biguint128_shl(const BigUInt128 *a, const buint_size_t shift) {
 BigUInt128 retv = biguint128_ctor_default();
 return *biguint128_shl_or(&retv, a, shift);;
}

/**
 * Shift left operator, result put into dest with OR operation.
 * dest and a must not point to the same structure.
 * @param dest Reference to result.
 * @param a Subject of the function.
 * @param shift Amount of shift.
 * @return dest.
 */
BigUInt128 *biguint128_shl_or(BigUInt128 *dest, const BigUInt128 *a, const buint_size_t shift) {
 buint_size_p shift_p = bitpos_(shift);

 FOREACHCELL(i) {
  UIntPair x = uint_split(a->dat[i], UINT_BITS - shift_p.bit_sel);
  if (i < BIGUINT128_CELLS - shift_p.byte_sel) {
   dest->dat[i + shift_p.byte_sel]|= x.second << shift_p.bit_sel;
   if (i < BIGUINT128_CELLS - shift_p.byte_sel - 1) {
    dest->dat[i + shift_p.byte_sel + 1]|= x.first >> (UINT_BITS - shift_p.bit_sel);
   }
  }
 }
 return dest;
}

BigUInt128 biguint128_shr(const BigUInt128 *a, const buint_size_t shift) {
 BigUInt128 retv = biguint128_ctor_default();
 buint_size_p shift_p = bitpos_(shift);

 FOREACHCELL(i) {
  UIntPair x = uint_split(a->dat[i], shift_p.bit_sel);
  if (shift_p.byte_sel <= i) {
   retv.dat[i - shift_p.byte_sel]|= x.first >> shift_p.bit_sel;
   if (shift_p.byte_sel + 1 <= i) {
    retv.dat[i - shift_p.byte_sel - 1]|= x.second << (UINT_BITS - shift_p.bit_sel);
   }
  }
 }
 return retv;
}

BigUInt128 *biguint128_shr_assign(BigUInt128 *a, const buint_size_t shift) {
 const buint_size_p shift_p = bitpos_(shift);
 UInt carry=uint_split_shift(a->dat[shift_p.byte_sel],shift_p.bit_sel).first;

 // There are 2 ranges in the produced a->dat:
 //  [0, BIGUINT128_CELLS-shift_p.byte_sel]: these values are derived from equal and higher indexed a->dat entries;
 //  [BIGUINT128_CELLS-shift_p.byte_sel, BIGUINT128_CELLS): these values are cleared
 // the special entry is dat[BIGUINT128_CELLS-shift_p.byte_sel]: low part is copied, high part is cleared.
 FOREACHCELL(i) {
  if (shift_p.byte_sel + i + 1 < BIGUINT128_CELLS){
   UIntPair x= uint_split_shift(a->dat[shift_p.byte_sel + 1 + i], shift_p.bit_sel);
   a->dat[i]= x.second | carry;
   carry= x.first;
  } else {
    a->dat[i]= carry;
    carry= 0;
  }
 }
 return a;
}

BigUInt128 biguint128_ror(const BigUInt128 *a, const buint_size_t shift) {
 BigUInt128 retv = biguint128_ctor_default();
 BigUInt128 *dest = &retv;
 buint_size_p shift_p = bitpos_(shift);

 FOREACHCELL(i) {
  UIntPair x = uint_split_shift(a->dat[i], shift_p.bit_sel);
  dest->dat[(i - shift_p.byte_sel) % BIGUINT128_CELLS]|= x.first;
  dest->dat[(i - shift_p.byte_sel - 1) % BIGUINT128_CELLS]|= x.second;
 }
 return retv;
}

BigUInt128 biguint128_rol(const BigUInt128 *a, const buint_size_t shift) {
 return biguint128_ror(a, 128u - shift);
}

// AND
BigUInt128 biguint128_and(const BigUInt128 *a, const BigUInt128 *b) {
 BigUInt128 retv;
 FOREACHCELL(i) {
  retv.dat[i] = a->dat[i] & b->dat[i];
 }
 return retv;
}

BigUInt128 *biguint128_and_assign(BigUInt128 *a, const BigUInt128 *b) {
 FOREACHCELL(i) {
  a->dat[i] &= b->dat[i];
 }
 return a;
}

// OR
BigUInt128 biguint128_or(const BigUInt128 *a, const BigUInt128 *b) {
 BigUInt128 retv;
 FOREACHCELL(i) {
  retv.dat[i] = a->dat[i] | b->dat[i];
 }
 return retv;
}

BigUInt128 *biguint128_or_assign(BigUInt128 *a, const BigUInt128 *b) {
 FOREACHCELL(i) {
  a->dat[i] |= b->dat[i];
 }
 return a;
}

// NOT
BigUInt128 biguint128_not(const BigUInt128 *a) {
 BigUInt128 retv;
 FOREACHCELL(i) {
  retv.dat[i] = ~a->dat[i];
 }
 return retv;
}

BigUInt128 *biguint128_not_assign(BigUInt128 *a) {
 FOREACHCELL(i) {
  a->dat[i] = ~a->dat[i];
 }
 return a;
}

// XOR
BigUInt128 biguint128_xor(const BigUInt128 *a, const BigUInt128 *b) {
 BigUInt128 retv;
 FOREACHCELL(i) {
  retv.dat[i] = a->dat[i] ^ b->dat[i];
 }
 return retv;
}

BigUInt128 *biguint128_xor_assign(BigUInt128 *a, const BigUInt128 *b) {
 FOREACHCELL(i) {
  a->dat[i] ^= b->dat[i];
 }
 return a;
}

BigUInt128 biguint128_mul(const BigUInt128 *a, const BigUInt128 *b) {
 BigUInt128 retv = biguint128_ctor_default();
 BigUInt128 carry = biguint128_ctor_default();
 FOREACHCELL(i) {
  FOREACHCELL(j) {
   buint_size_t k = i + j;
   if (k < BIGUINT128_CELLS) {
    UIntPair kmul = uint_mul(a->dat[i], b->dat[j]);
    buint_bool cx0 = 0;
    buint_bool cx1 = 0;
    retv.dat[k] = uint_add(retv.dat[k], kmul.second, &cx0);
    if (k + 1 < BIGUINT128_CELLS) {
     if (cx0) {
      ++carry.dat[k + 1];
     }
     retv.dat[k + 1] = uint_add(retv.dat[k + 1], kmul.first, &cx1);
     if (cx1 && k + 2 < BIGUINT128_CELLS) {
      ++carry.dat[k + 2];
     }
    }
   }
  }
 }
 BigUInt128 rx = biguint128_add(&retv, &carry);
 return rx;
}

BigUIntPair128 biguint128_dmul(const BigUInt128 *a, const BigUInt128 *b) {
 BigUIntPair128 result= {biguint128_ctor_default(), biguint128_ctor_default()};
 BigUIntPair128 carry= {biguint128_ctor_default(), biguint128_ctor_default()};
 UInt *res=result.first.dat;
 UInt *crr=carry.first.dat;

 FOREACHCELL(i) { // iterate over a
  FOREACHCELL(j) { // iterate over b
   buint_size_t k = i + j;

   UIntPair kmul = uint_mul(a->dat[i], b->dat[j]);
   buint_bool cx0 = 0;
   res[k] = uint_add(res[k], kmul.second, &cx0);
   if (k + 1 < 2 * BIGUINT128_CELLS) {
    res[k+1]=uint_add(res[k+1],kmul.first, &cx0);
    if (cx0 && k + 2 < 2 * BIGUINT128_CELLS) { // highest cell will not overflow, still we check for OOR error.
     ++crr[k + 2];
    }
   }
  }
 }
 buint_bool cx=0;
 biguint128_adc_replace(&result.first, &result.first, &carry.first, &cx);
 biguint128_adc_replace(&result.second, &result.second, &carry.second, &cx);
 return result;
}

BigUIntPair128 biguint128_div(const BigUInt128 *a, const BigUInt128 *b) {
 BigUInt128 div = biguint128_ctor_default();
 BigUIntPair128 retv;
 retv.first = div;
 buint_size_t msb_a = biguint128_msb(a);
 buint_size_t msb_b = biguint128_msb(b);
 if (msb_a < msb_b) {
  retv.second = biguint128_ctor_copy(a);
 } else {
  buint_size_t xbit = msb_a - msb_b;
  BigUInt128 xobj = biguint128_ctor_copy(a);
  BigUInt128 xdiv = biguint128_shl(b, xbit);
  for (buint_size_t i=0; i <= xbit; ++i) {
   if (!biguint128_lt(&xobj, &xdiv)) {
    biguint128_sbit(&retv.first, xbit - i);
    xobj = biguint128_sub(&xobj, &xdiv);
   }
   biguint128_shr_assign(&xdiv, 1);
  }
  retv.second = xobj;
 }
 return retv;
}

buint_bool biguint128_lt(const BigUInt128 *a, const BigUInt128 *b) {
 FOREACHCELL(i) {
  buint_size_t j = BIGUINT128_CELLS - i - 1;
  if (a->dat[j]<b->dat[j]) return 1;
  if (b->dat[j]<a->dat[j]) return 0;
 }
 return 0;
}

buint_bool bigint128_lt(const BigUInt128 *a, const BigUInt128 *b) {
 buint_bool neg_a = is_bigint_negative_(a);
 buint_bool neg_b = is_bigint_negative_(b);
 return (neg_a == neg_b) ?
  biguint128_lt(a,b): // compare them in the usual way
  neg_a; // if only one of them is negative, a must be negative to be lower than b.
}

buint_bool biguint128_eq(const BigUInt128 *a, const BigUInt128 *b) {
 FOREACHCELL(i) {
  if (a->dat[i]!=b->dat[i]) return 0;
 }
 return 1;
}

buint_size_t biguint128_msb(const BigUInt128 *a) {
 buint_size_t j;
 buint_bool found = 0;
 FOREACHCELL(i) {
  j = BIGUINT128_CELLS - i - 1;
  if (0 < a->dat[j]) {
   found = 1;
   break;
  }
 }
 if (!found) {
  return 0;
 }
 return j * UINT_BITS + uint_msb(a->dat[j]);
}

void biguint128_sbit(BigUInt128 *a, buint_size_t bit) {
 buint_size_p bit_p = bitpos_(bit);
 a->dat[bit_p.byte_sel]|= (UInt)1 << bit_p.bit_sel;
}
void biguint128_cbit(BigUInt128 *a, buint_size_t bit) {
 buint_size_p bit_p = bitpos_(bit);
 a->dat[bit_p.byte_sel]&= ~((UInt)1 << bit_p.bit_sel);
}
void biguint128_obit(BigUInt128 *a, buint_size_t bit, buint_bool value) {
 value?
  biguint128_sbit(a,bit):
  biguint128_cbit(a,bit);
}
buint_bool biguint128_gbit(const BigUInt128 *a, buint_size_t bit) {
 buint_size_p bit_p = bitpos_(bit);
 return 0 < (a->dat[bit_p.byte_sel] & ((UInt)1 << bit_p.bit_sel));
}

buint_size_t biguint128_print_hex(const BigUInt128 *a, char *buf, buint_size_t buf_len) {
 buint_size_t retv = biguint128_msb(a)/4 + 1;
 if (buf_len < retv) {
  return 0;
 }
 buint_bool ready = 0;
 for (buint_size_t i=0; !ready && i<BIGUINT128_CELLS; ++i) {
  for (buint_size_t j=0; !ready && j<UINT_BYTES; ++j) {
   buint_size_t offset = 2 * (i * UINT_BYTES + j);
   if (retv <= offset) {
    ready = 1;
    break;
   }
   if (offset + 1 < retv) {
    set_hexbyte(buf + retv - offset - 2, a->dat[i]>>8*j & 0xFF);
   } else {
    set_hexdigit(buf + retv - offset - 1, a->dat[i]>>8*j & 0xF);
   }
  }
 }
 return retv;
}

static buint_size_t biguint128_print_dec_anywhere_(const BigUInt128 *a, char *buf, buint_size_t buf_len, buint_size_t *offset) {
 buint_bool ready = 0;
 BigUInt128 zero = biguint128_value_of_uint(0);
 BigUInt128 temp = biguint128_ctor_copy(a);

 for (buint_size_t i= 0; !ready && i < buf_len; i+= 3) {
  BigUIntPair128 res= d1000_(&temp);

  buint_size_t buf_idx0= buf_len - i - 1;
  buint_size_t buf_idx1= buf_len - i - 2;
  buint_size_t buf_idx2= buf_len - i - 3;

  UInt d= res.second.dat[0];
  char d0= d%10;
  char d1= (d/10)%10;
  char d2= (d/10)/10;

  set_decdigit(buf + buf_idx0, d0);
  *offset= buf_idx0;
  if (buf_idx0 != 0) {
   set_decdigit(buf + buf_idx1, d1);
   if (d1) *offset= buf_idx1;
   if (buf_idx1 != 0) {
    set_decdigit(buf + buf_idx2, d2);
    if (d2) *offset= buf_idx2;
   }
  }
  ready = biguint128_eq(&zero, &res.first);
  temp = res.first;
 }
 return buf_len - *offset;
}
buint_size_t biguint128_print_dec(const BigUInt128 *a, char *buf, buint_size_t buf_len) {
 buint_size_t offset;
 buint_size_t size = biguint128_print_dec_anywhere_(a, buf, buf_len, &offset);
 for (buint_size_t i = 0; i < size; ++i) {
  buf[i] = buf[offset + i];
 }
 return size;
}

buint_size_t bigint128_print_dec(const BigUInt128 *a, char *buf, buint_size_t buf_len) {
 buint_size_t retv=0;

 if (is_bigint_negative_(a)) {
  BigUInt128 zero = biguint128_ctor_default();
  BigUInt128 val = biguint128_sub(&zero,a);

  if (buf_len) {
   buf[0] = MINUS_SIGN;
   retv = 1 + biguint128_print_dec(&val, buf+1, buf_len-1);
  }
 } else {
  retv = biguint128_print_dec(a, buf, buf_len);
 }
 return retv;
}

buint_size_t biguint128_export(const BigUInt128 *a, char *dest) {
 memcpy(dest, &a->dat, BIGUINT128_CELLS * UINT_BYTES);
 return BIGUINT128_CELLS * UINT_BYTES;
}

// cleanup
#undef FOREACHCELL
#undef UINT_BYTES
#undef UINT_BITS
