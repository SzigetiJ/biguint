#ifndef _UINT_H_
#define _UINT_H_

#include "uint_types.h"

typedef struct {
 UInt first;
 UInt second;
} UIntPair;

UInt uint_add(UInt, UInt, buint_bool*);
UInt uint_sub(UInt, UInt, buint_bool*);
UIntPair uint_split(UInt, buint_size_t);
UIntPair uint_mul(UInt, UInt);
buint_size_t uint_msb(UInt);

#endif

