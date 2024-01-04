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

// Local definitions
#define FORRANGE(i, LO, HI) for (buint_size_t i=(LO); i<(HI); ++i)
#define FORRANGEREV(i, X) for (buint_size_t i=(X)-1; i!=(buint_size_t)-1; --i)

#define FOREACHCELL(i) FORRANGE(i, 0U, BIGUINT128_CELLS)
#define FOREACHCELLREV(i) FORRANGEREV(i,BIGUINT128_CELLS)

#define UINT_BYTES sizeof(UInt)
#define UINT_BITS (8 * UINT_BYTES)
#define MINUS_SIGN '-'

// Assertions
// SIZEOF_UINT checked by configure script
#if (SIZEOF_UINT < 2)
#error Current implementation requires UInt basic storage type being at least 2 bytes wide.
#endif

// Local types
typedef struct {
 buint_bool ready;
 BigUInt128 val;
 BigUInt128 invpre;
 BigUInt128 inv;
} BigUInt128DivAsMul;

// Static function declarations
static inline buint_size_p bitpos_(buint_size_t a);
static inline buint_bool is_bigint_negative_(const BigUInt128 *a);
static inline BigUInt128 *negate_(BigUInt128 *a);
static inline BigUInt128 *clrall_(BigUInt128 *a);
static inline void fast_div_phase2_(BigUIntPair128 *res, UInt divisor);
static inline BigUInt128 divmax_tiny_(UInt divisor);
static inline void init_divasmul_(UInt divisor, BigUInt128DivAsMul *a);
static inline void tune_remainder_(BigUIntTinyPair128 *res, const BigUInt128DivAsMul *a);
static inline buint_size_t msbnxt_(const BigUInt128 *a, buint_size_t last);
static inline BigUInt128 *shr_tiny_crng_(BigUInt128 *a, buint_size_t shift, buint_size_t clo, buint_size_t chi);

static inline BigUInt128 *sbc_crng_(BigUInt128 *a, const BigUInt128 *b, buint_size_t clo, buint_size_t chi, buint_bool *carry);
static inline BigUInt128 *sub_carry_crng_(BigUInt128 *a, buint_size_t clo, buint_bool *carry);


static buint_size_t print_dec_anywhere_(const BigUInt128 *a, char *buf, buint_size_t buf_len, buint_size_t *offset);
static BigUIntTinyPair128 div_special_tiny_(const BigUInt128 *a, UInt b);
static BigUInt128 *shr_tiny_brng_(BigUInt128 *a, buint_size_t shift, buint_size_t lsb, buint_size_t msb);
static BigUInt128 *sub_assign_brng_(BigUInt128 *a, const BigUInt128 *b, buint_size_t lsb, buint_size_t msb);
static buint_bool lt_brng_(const BigUInt128 *a, const BigUInt128 *b, buint_size_t msb);
static void max_uint_10pow_(UInt *val, buint_size_t *pow);
static buint_size_t print_dec_anywhere_(const BigUInt128 *a, char *buf, buint_size_t buf_len, buint_size_t *offset);

// Implementations

/////////////////////
// internal functions

/**
 * Splits a bit position into pair of byte index [0..UINT_BYTES) and internal bit position [0,8)
 * @param a Bit position in BigUInt.
 * @return byte_sel: byte index, bit_sel: bit position inside the indexed byte.
 */
static inline buint_size_p bitpos_(buint_size_t a) {
 return (buint_size_p){a/UINT_BITS, a%UINT_BITS};
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
 * Negates a signed value.
 * @param a Subject of operation.
 * @return a.
 */
static inline BigUInt128 *negate_(BigUInt128 *a) {
 biguint128_dec(a);
 biguint128_not_assign(a);
 return a;
}

/**
 * Clears all data (set to 0).
 * @param a Subject of operation;
 * @return a.
 */
static inline BigUInt128 *clrall_(BigUInt128 *a) {
 FOREACHCELL(i) {
  a->dat[i] = 0;
 }
 memset(&a->dat, 0, BIGUINT128_CELLS * UINT_BYTES);
 return a;
}

/**
 * The fast division algorithms have two phases.
 * The second phase is invoked when the dividend fits into UInt.
 * @param res res->first contains the dividend and the quotient is written there. res->second will store the remainder.
 * @param divisor
 */
static inline void fast_div_phase2_(BigUIntPair128 *res, UInt divisor) {
 UInt d = res->second.dat[0] / divisor;
 UInt m = res->second.dat[0] % divisor;
 biguint128_add_tiny(&res->first, d);
 res->second = biguint128_value_of_uint(m);
}

/**
 * Performs division on the maximal BigUInt128 number.
 * Only the quotient is returned, i.e., we do not store the remainder.
 * @param divisor
 * @return Quotient.
 */
static inline BigUInt128 divmax_tiny_(UInt divisor) {
 BigUInt128 max = biguint128_ctor_default();
 biguint128_dec(&max);
 BigUInt128 d = biguint128_value_of_uint(divisor);
 return biguint128_div(&max, &d).first;
}

/**
 * If we know the modular multiplicative inverse number (inv_a) of a certain number a,
 * the division with a can be carried out as multiplication with inv_a.
 * In order to do multiplication with mod mul inv. value,
 *  we have to define some values (and store them in BigUInt128DivAsMul struct).
 * This method finds the mod mul inv value for numbers that are divisors of the
 * maximal BigUInt128 value (e.g., 3, 5, 15, etc.).
 * @param divisor
 * @param a Mod mul inv and other useful values are written here.
 */
static inline void init_divasmul_(UInt divisor, BigUInt128DivAsMul *a) {
 a->val = biguint128_value_of_uint(divisor);
 a->invpre = divmax_tiny_(divisor);
 a->inv = bigint128_negate(&a->invpre);
 a->ready = 1;
}

/**
 * Division carried out as multiplication with the mod mul inv value does
 * not give the quotient and the remainder directly.
 * This method transforms the result of the multiplication into quotient and remainder.
 * @param res Subject of operation. res->first contains the raw result,
 * res->second should be the initial remainder (0).
 * As output, res->first will contain the quotient, wherease res->second will contain the remainder.
 * @param a Mod mul inv values and others.
 */
static inline void tune_remainder_(BigUIntTinyPair128 *res, const BigUInt128DivAsMul *a) {
 while (biguint128_lt(&a->invpre, &res->first)) {
  ++res->second;
  biguint128_sub_assign(&res->first, &a->inv);
 }
}

/**
 * Accelerated search for most significant set bit.
 * We assume that all the bits above last are 0.
 * @param a Subject of operation.
 * @param last begin bit of search.
 * @return MSB not hiher than last.
 */
static inline buint_size_t msbnxt_(const BigUInt128 *a, buint_size_t last) {
 for (buint_size_t i = last; 0 < i; --i) {
  if (biguint128_gbit(a, i-1)) return i-1;
 }
 return 0;
}

/**
 * Shift right in a range of cells.
 * This function works as SHR_TINY if the cells outside the range are all 0.
 * @param a Subject of operation
 * @param shift Amount of shifting (less than UINT_BITS).
 * @param clo Lower bound of cell range (inclusive).
 * @param chi Upper bound of cell range (exclusive).
 * @return a.
 */
static inline BigUInt128 *shr_tiny_crng_(BigUInt128 *a, buint_size_t shift, buint_size_t clo, buint_size_t chi) {
 FORRANGE(i,clo, chi) {
  UIntPair x = uint_split_shift(a->dat[i], shift);
  if (0 < i) a->dat[i - 1] |= x.second;
  a->dat[i] = x.first;
 }
 return a;
}

/**
 * Shift right in a range of bits.
 * This function works as SHR_TINY if the bits outside the range are all 0.
 * @param a Subject of operation
 * @param shift Amount of shifting (less than UINT_BITS).
 * @param lsb Lower bound of bit range (inclusive).
 * @param msb Upper bound of bit range (inclusive!).
 * @return a.
 */
static BigUInt128 *shr_tiny_brng_(BigUInt128 *a, buint_size_t shift, buint_size_t lsb, buint_size_t msb) {
 buint_size_t clo = bitpos_(lsb).byte_sel;
 buint_size_t chi = bitpos_(msb).byte_sel + 1;
 return shr_tiny_crng_(a, shift, clo, chi);
}

/**
 * Subtraction with carry in a cell range.
 * @param a Subject of the operation.
 * @param b This number will be (partially) subtracted from a.
 * @param clo Lower bound of cell range (inclusive).
 * @param chi Upper bound of cell range (exclusive).
 * @param carry carry flag (input/output)
 * @return a.
 */
static inline BigUInt128 *sbc_crng_(BigUInt128 *a, const BigUInt128 *b, buint_size_t clo, buint_size_t chi, buint_bool *carry){
 FORRANGE(i, clo, chi) {
  uint_sub_assign(&a->dat[i], b->dat[i], carry);
 }
 return a;
}

/**
 * Optionally decrement a BigUInt128 value beginning at a given cell.
 * @param a Subject of operation.
 * @param clo Decrement this cell (if carry flag is active), and cascade to higher cells if required.
 * @param carry carry flag (input/output).
 * @return a.
 */
static inline BigUInt128 *sub_carry_crng_(BigUInt128 *a, buint_size_t clo, buint_bool *carry) {
 for (buint_size_t i=clo; *carry && i<BIGUINT128_CELLS; ++i) {
  a->dat[i]= uint_sub(a->dat[i], 0u, carry);
 }
 return a;
}

/**
 * Performs subtraction with assignment in a given bit range.
 * This functions gives the same result as biguint128_sub_assign
 * if the bits of b are 0 outside the interval [lsb, msb].
 * @param a
 * @param b
 * @param lsb Lower bound of bit range (inclusive).
 * @param msb Upper bound of bit range (inclusive).
 * @return a.
 */
static BigUInt128 *sub_assign_brng_(BigUInt128 *a, const BigUInt128 *b, buint_size_t lsb, buint_size_t msb){
 buint_size_t clo = bitpos_(lsb).byte_sel;
 buint_size_t chi = bitpos_(msb).byte_sel + 1;
 buint_bool carry = 0;

 sbc_crng_(a,b,clo,chi,&carry);
 return sub_carry_crng_(a,chi,&carry);
}

/**
 * Constrainted ``less than'' check.
 * It assumes that a end b equal at bits higher than msb.
 * @param a Reference to left hand side argument of the operator.
 * @param b Reference to right hand side argument of the operator.
 * @param msb MSB to check.
 * @return *a is less than *b in bit range [0,msb].
 */
static buint_bool lt_brng_(const BigUInt128 *a, const BigUInt128 *b, buint_size_t msb) {
 FORRANGEREV(j, msb / UINT_BITS+1) {
  if (a->dat[j] < b->dat[j]) return 1;
  if (b->dat[j] < a->dat[j]) return 0;
 }
 return 0;
}

/**
 * Returns the highest power of 10 that can be represented in UInt type.
 * @param val Output: the highest power of 10.
 * @param pow Output: the power itself.
 */
static void max_uint_10pow_(UInt *val, buint_size_t *pow) {
 // some constants
 static const UInt m10 = 10U;
 static UInt limit = ((UInt)-1) / m10;
 // init
 *val = 1;
 *pow = 0;
 // iterate
 while (1) {
  *val*= m10;
  ++*pow;
  if (limit < *val) return;
 }
}

/**
 * It is complicated to determine in advance the number of digits of a BigUInt128 value.
 * This function writes the decimal digits of the value to the end of the given buffer,
 * beginning with the least significant digit at the very end of the buffer.
 * @param a Value to print.
 * @param buf Output: Here to write the value.
 * @param buf_len length of buf. The value must be written in the range [buf, buf+buf_len).
 * @param offset Output: the written digits can be read out from the buffer beginning at this offset.
 * @return Number of digits that can be read out of the buffer beginning at buffer[offset].
 * If the value does not fit into the buffer, 0 is returned.
 */
static buint_size_t print_dec_anywhere_(const BigUInt128 *a, char *buf, buint_size_t buf_len, buint_size_t *offset) {
 // static values
 static const UInt BASE = 10U;
 static buint_bool fstrun = 1;
 static UInt div10u;
 static buint_size_t pow;
 static BigUInt128 div10pow;
 if (fstrun) {
  max_uint_10pow_(&div10u, &pow);
  div10pow = biguint128_value_of_uint(div10u);
  fstrun = 0;
 }

 // variables
 buint_bool ready = 0;
 BigUInt128 zero = biguint128_value_of_uint(0);
 BigUInt128 temp = biguint128_ctor_copy(a);

 for (buint_size_t i= 0; !ready && i < buf_len; i+= pow) {
  BigUIntPair128 res= biguint128_div(&temp, &div10pow);
  UInt d= res.second.dat[0];
  buint_bool no_more_div10pow = (biguint128_eq(&zero, &res.first));
  for (buint_size_t pi=0; !ready && pi < pow; ++pi) {
   buint_size_t buf_idx_pi= buf_len - i - pi - 1;
   char di = d%BASE;
   set_decdigit(buf + buf_idx_pi, di);
   d/= BASE;
   if (no_more_div10pow && d == 0) {
    *offset = buf_idx_pi;
    ready = 1;
   }
   if (buf_idx_pi==0) {	// this was the last writeable digit
    break;
   }
  }
  temp = res.first;
 }
 if (!ready) *offset = buf_len;
 return buf_len - *offset;
}

/**
 * Division as Multiplication with modular multiplicative inverse is
 * supported for two tiny values: 3 and 5.
 * This function performs this kind of division.
 * @param a Reference to the value to divide.
 * @param b Divisor, either 3 or 5. If b!=3, here we assume, that b==5.
 * Note: internal function, no input validation...
 * @return Pair of quotient and remainder.
 */
static BigUIntTinyPair128 div_special_tiny_(const BigUInt128 *a, UInt b) {
 static BigUInt128DivAsMul v[]={{0},{0}};	// not ready
 if (!v[0].ready) {
  init_divasmul_(3, &v[0]);
  init_divasmul_(5, &v[1]);
 }
 size_t idx = (b==3? 0: 1);
 BigUIntTinyPair128 retv = {biguint128_mul(a, &v[idx].inv), 0};
 tune_remainder_(&retv, &v[idx]);
 return retv;
}

// END internal functions
/////////////////////

/////////////////////
// Public functions

// ### Section CTOR
BigUInt128 biguint128_ctor_default() {
 BigUInt128 retv;
 return *clrall_(&retv);
}

BigUInt128 biguint128_ctor_unit() {
 BigUInt128 retv = biguint128_ctor_default();
 retv.dat[0]= 1;
 return retv;
}

BigUInt128 biguint128_ctor_standard(const UInt *dat) {
 BigUInt128 retv;
 FOREACHCELL(i) {
  retv.dat[i]= dat[i];
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
  retv = biguint128_mul10(&retv);
  biguint128_add_tiny(&retv, d);
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

BigUInt128 bigint128_value_of_uint(UInt value) {
 BigUInt128 retv = biguint128_ctor_default();
 if (value & (((UInt)1)<<(UINT_BITS-1))) {
  FOREACHCELL(i) {
   retv.dat[i]= (UInt)-1;
  }
 }
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
 buint_bool carry = 0;
 FOREACHCELL(i) {
  uint_add_assign(&a->dat[i], b->dat[i], &carry);
 }
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
 buint_bool carry = 0;
 return sbc_crng_(a,b,0,BIGUINT128_CELLS,&carry);
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

BigUInt128 bigint128_negate(const BigUInt128 *a) {
 BigUInt128 retv = biguint128_ctor_copy(a);
 negate_(&retv);
 return retv;
}

// END SUB   //
///////////////

// Tiny ADD/SUB
BigUInt128 *biguint128_add_tiny(BigUInt128 *a, const UInt b) {
 buint_bool carry= 0U;
 a->dat[0]= uint_add(a->dat[0], b, &carry);
 for (buint_size_t i= 1u; carry && i<BIGUINT128_CELLS; ++i) {
  a->dat[i]= uint_add(a->dat[i], 0u, &carry);
 }
 return a;
}

BigUInt128 *biguint128_sub_tiny(BigUInt128 *a, const UInt b) {
 buint_bool carry= 0U;
 a->dat[0]= uint_sub(a->dat[0], b, &carry);
 return sub_carry_crng_(a,1,&carry);
}

// #### Subsection INC / DEC
BigUInt128 *biguint128_inc(BigUInt128 *a) {
 biguint128_add_tiny(a, 1U);
 return a;
}

BigUInt128 *biguint128_dec(BigUInt128 *a) {
 biguint128_sub_tiny(a, 1U);
 return a;
}

// ### Section SHIFT / ROTATE
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
BigUInt128 *biguint128_shl_or(BigUInt128 *restrict dest, const BigUInt128 *restrict a, const buint_size_t shift) {
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

BigUInt128 *biguint128_shl_tiny(BigUInt128 *a, const buint_size_t shift) {
 buint_size_t bitshift = UINT_BITS - bitpos_(shift).bit_sel;

 FOREACHCELL(i) {
  UIntPair x = uint_split_shift(a->dat[BIGUINT128_CELLS - 1 - i], bitshift);
  if (0 < i) a->dat[BIGUINT128_CELLS - i] |= x.first;
  a->dat[BIGUINT128_CELLS - 1 - i] = x.second;
 }
 return a;
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

BigUInt128 *biguint128_shr_tiny(BigUInt128 *a, const buint_size_t shift) {
 buint_size_p shift_p = bitpos_(shift);
 return shr_tiny_crng_(a, shift_p.bit_sel, 0, BIGUINT128_CELLS);
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

// ### Section Bitwise Logic
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

// ### Section MUL/DIV
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
 BigUIntPair128 retv = {biguint128_ctor_default(), biguint128_ctor_copy(a)};
 buint_size_t msb_a = biguint128_msb(a);
 buint_size_t msb_b = biguint128_msb(b);

 if (!(msb_a < msb_b)) {
  buint_size_t xbit = msb_a - msb_b;
  BigUInt128 xdiv = biguint128_shl(b, xbit);
  buint_size_t msb_x=msb_a;
  for (buint_size_t i = 0; i <= xbit;) {
   UInt idiff = 1;
   if (!lt_brng_(&retv.second, &xdiv, msb_x)) {
    biguint128_sbit(&retv.first, xbit - i);
    sub_assign_brng_(&retv.second, &xdiv, xbit - i, xbit - i + msb_b);
    msb_x = msbnxt_(&retv.second, msb_x);
    idiff = msb_a - msb_x - i;
    if (!idiff) idiff = 1;
   }
   idiff<UINT_BITS?
    shr_tiny_brng_(&xdiv, idiff, xbit - i, xbit - i + msb_b):
    biguint128_shr_assign(&xdiv, idiff);
   i += idiff;
  }
 }
 return retv;
}

BigUIntPair128 bigint128_div(const BigUInt128 *a, const BigUInt128 *b) {
 // work with copies
 BigUInt128 ab[] = {biguint128_ctor_copy(a), biguint128_ctor_copy(b)};
 // check signs
 buint_bool neg[] = {is_bigint_negative_(a), is_bigint_negative_(b)};
 // convert
 for (unsigned int i = 0U; i<2; ++i) {
  if (neg[i]) {
   biguint128_dec(&ab[i]);
   biguint128_not_assign(&ab[i]);
  }
 }

 // calling the wrapped function
 BigUIntPair128 retv = biguint128_div(&ab[0], &ab[1]);

 // afterwork
 if (!neg[0] != !neg[1]) {
  negate_(&retv.first);
 }
 if (neg[0]) {
  negate_(&retv.second);
 }
 return retv;
}

// ### Section COMPARISON
buint_bool biguint128_lt(const BigUInt128 *a, const BigUInt128 *b) {
 FOREACHCELLREV(j) {
  if (a->dat[j]<b->dat[j]) return 1;
  if (b->dat[j]<a->dat[j]) return 0;
 }
 return 0;
}

buint_bool bigint128_ltz(const BigUInt128 *a) {
 return is_bigint_negative_(a);
}

buint_bool bigint128_lt(const BigUInt128 *a, const BigUInt128 *b) {
 buint_bool neg_a = is_bigint_negative_(a);
 buint_bool neg_b = is_bigint_negative_(b);
 return (!!neg_a == !!neg_b) ?
  biguint128_lt(a,b): // compare them in the usual way
  neg_a; // if only one of them is negative, a must be negative to be lower than b.
}

buint_bool biguint128_eq(const BigUInt128 *a, const BigUInt128 *b) {
 FOREACHCELL(i) {
  if (a->dat[i]!=b->dat[i]) return 0;
 }
 return 1;
}

buint_bool biguint128_eqz(const BigUInt128 *a) {
 FOREACHCELL(i) {
  if (a->dat[i]!=0) return 0;
 }
 return 1;
}

// ### Section BIT level
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

// ### Section PRINT
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

buint_size_t biguint128_print_dec(const BigUInt128 *a, char *buf, buint_size_t buf_len) {
 buint_size_t offset;
 buint_size_t size = print_dec_anywhere_(a, buf, buf_len, &offset);
 for (buint_size_t i = 0; i < size; ++i) {
  buf[i] = buf[offset + i];
 }
 return size;
}

buint_size_t bigint128_print_dec(const BigUInt128 *a, char *buf, buint_size_t buf_len) {
 buint_size_t retv=0;

 if (is_bigint_negative_(a)) {
  BigUInt128 val = bigint128_negate(a);

  if (buf_len) {
   buf[0] = MINUS_SIGN;
   buint_size_t len = biguint128_print_dec(&val, buf+1, buf_len-1);
   if (len) {
    retv = len + 1U;
   }
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


// ### Section special DIV/MUL
// #### Subsection MUL as combined SHL/ADD
BigUInt128 biguint128_mul3(const BigUInt128 *a) {
 BigUInt128 a1 = biguint128_shl(a,1);
 biguint128_add_assign(&a1, a);
 return a1;
}

BigUInt128 biguint128_mul10(const BigUInt128 *a) {
 BigUInt128 a3 = biguint128_shl(a,3);
 BigUInt128 a1 = biguint128_shl(a,1);
 biguint128_add_assign(&a3, &a1);
 return a3;
}

BigUInt128 biguint128_mul100(const BigUInt128 *a) {
 BigUInt128 a6 = biguint128_shl(a,6);
 BigUInt128 ax = biguint128_shl(a,2);
 biguint128_add_assign(&a6, &ax);
 biguint128_shl_tiny(&ax,3);
 biguint128_add_assign(&a6, &ax);
 return a6;
}

BigUInt128 biguint128_mul1000(const BigUInt128 *a) {
 BigUInt128 a10 = biguint128_shl(a,10);
 BigUInt128 ax = biguint128_shl(a,3);
 biguint128_sub_assign(&a10, &ax);
 biguint128_shl_tiny(&ax,1);
 biguint128_sub_assign(&a10, &ax);
 return a10;
}

// #### Subsection DIV as SHR / MOD as AND
BigUIntTinyPair128 biguint128_div32(const BigUInt128 *a) {
 BigUIntTinyPair128 retv;
 retv.first= biguint128_shr(a, 5);
 retv.second= (a->dat[0]) & (UInt)0x1F;
 return retv;
}

BigUIntTinyPair128 biguint128_div1024(const BigUInt128 *a) {
 BigUIntTinyPair128 retv;
 retv.first= biguint128_shr(a, 10);
 retv.second= (a->dat[0]) & (UInt)0x3FF;
 return retv;
}

// #### Subsection DIV with divisor close to 2^n.
BigUIntPair128 biguint128_div1000(const BigUInt128 *a) {
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
 // In phase #2 we just do a simple UInt division.
 static const UInt DIVISOR = 1000U;
 static const BigUInt128 PH1LIMIT = {{-1}};

 BigUIntPair128 retv= {biguint128_ctor_default(), biguint128_ctor_copy(a)};
 // Phase 1:
 while (biguint128_lt(&PH1LIMIT, &retv.second)) {
  BigUIntTinyPair128 x= biguint128_div1024(&retv.second);
  biguint128_add_assign(&retv.first, &x.first);
  biguint128_shl_tiny(&x.first, 3);
  BigUInt128 d_mul16= biguint128_shl(&x.first, 1);
  retv.second = biguint128_add(&x.first, &d_mul16);
  biguint128_add_tiny(&retv.second, x.second);
 }
 // Phase 2:
 fast_div_phase2_(&retv, DIVISOR);
 return retv;
}

BigUIntPair128 bigint128_div1000(const BigUInt128 *a) {
 if (is_bigint_negative_(a)) {
  BigUInt128 ac = *a;
  negate_(&ac);
  BigUIntPair128 retv = biguint128_div1000(&ac);
  negate_(&retv.first);
  negate_(&retv.second);
  return retv;
 } else {
  return biguint128_div1000(a);
 }
}


BigUIntPair128 biguint128_div30(const BigUInt128 *a) {
 // In phase #1 we exploit that
 // a = 32*b + c = (30*b) + (2*b+c)
 // and reduce retv.second iteratively until it gets small enough.
 // In phase #2 we just do a simple UInt division.
 static const UInt DIVISOR = 30U;
 static const BigUInt128 PH1LIMIT = {{-1}};

 BigUIntTinyPair128 x={biguint128_ctor_copy(a),0U};
 BigUIntPair128 div = {biguint128_ctor_default(),biguint128_ctor_default()};
 while (biguint128_lt(&PH1LIMIT, &x.first)) {
  x= biguint128_div32(&x.first);
  biguint128_add_assign(&div.first, &x.first);
  biguint128_shl_tiny(&x.first, 1);
  biguint128_add_tiny(&x.first, x.second);
 }
 div.second = x.first;
 // Phase 2:
// fast_div_phase2_(&retv, DIVISOR);
 fast_div_phase2_(&div, DIVISOR);
 return div;
}

// #### Subsection DIV as MUL with mod mul inv
BigUIntTinyPair128 biguint128_div3(const BigUInt128 *a) {
 return div_special_tiny_(a, 3);
}

BigUIntTinyPair128 biguint128_div5(const BigUInt128 *a) {
 return div_special_tiny_(a, 5);
}

BigUIntTinyPair128 biguint128_div10(const BigUInt128 *a) {
 BigUIntTinyPair128 retv = biguint128_div5(a);
 if (retv.first.dat[0]&1) {
  retv.second+=5;
 }
 biguint128_shr_tiny(&retv.first, 1);
 return retv;
}
