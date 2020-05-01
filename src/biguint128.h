#ifndef _BIGUINT128_H_
#define _BIGUINT128_H_

#include "uint_types.h"
#define BIGUINT128_CELLS (128u / (8u * sizeof(UInt)))

typedef struct {
 UInt dat[ BIGUINT128_CELLS ];
} BigUInt128;

typedef struct {
 BigUInt128 first;
 BigUInt128 second;
} BigUIntPair128;

// constructors
BigUInt128 biguint128_ctor_default();
BigUInt128 biguint128_ctor_unit();
BigUInt128 biguint128_ctor_standard(const UInt*);
BigUInt128 biguint128_ctor_copy(const BigUInt128 *);
BigUInt128 biguint128_value_of_uint(UInt a);
BigUInt128 biguint128_ctor_hexcstream(const char*, buint_size_t);
BigUInt128 biguint128_ctor_deccstream(const char*, buint_size_t);

// addition / substraction
BigUInt128 biguint128_add(const BigUInt128 *, const BigUInt128 *);
BigUInt128 biguint128_sub(const BigUInt128 *, const BigUInt128 *);

// shift operations
BigUInt128 biguint128_shl(const BigUInt128 *, const buint_size_t);
BigUInt128 biguint128_shr(const BigUInt128 *, const buint_size_t);

// bitwise functions
BigUInt128 biguint128_and(const BigUInt128 *, const BigUInt128 *);
BigUInt128 biguint128_or(const BigUInt128 *, const BigUInt128 *);
BigUInt128 biguint128_not(const BigUInt128 *);
BigUInt128 biguint128_xor(const BigUInt128 *, const BigUInt128 *);

// multiplication / division
BigUInt128 biguint128_mul(const BigUInt128 *, const BigUInt128 *);
BigUIntPair128 biguint128_div(const BigUInt128 *, const BigUInt128 *);

// comparison
buint_bool biguint128_lt(const BigUInt128 *, const BigUInt128 *);
buint_bool biguint128_eq(const BigUInt128 *, const BigUInt128 *);

// misc.
buint_size_t biguint128_msb(const BigUInt128 *);
void biguint128_sbit(BigUInt128 *, buint_size_t);

// out
buint_size_t biguint128_print_hex(BigUInt128 *, char*, buint_size_t);
buint_size_t biguint128_print_dec(BigUInt128 *, char*, buint_size_t);

#endif

