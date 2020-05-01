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

