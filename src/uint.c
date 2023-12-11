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
#include "uint.h"

UIntPair uint_mul(UInt a, UInt b) {
 UIntPair retv;
 buint_size_t uint_mbits = 4*sizeof(UInt);

 // split
 UIntPair ap = uint_split(a, uint_mbits);
 ap.first>>=uint_mbits;
 UIntPair bp = uint_split(b, uint_mbits);
 bp.first>>=uint_mbits;

 // calculating the products
 retv.first = ap.first * bp.first;
 retv.second = ap.second * bp.second;

 UInt c = ap.first * bp.second;
 UInt d = ap.second * bp.first;
 UIntPair cp = uint_split(c, uint_mbits);
 cp.first>>=uint_mbits;
 cp.second<<=uint_mbits;
 UIntPair dp = uint_split(d, uint_mbits);
 dp.first>>=uint_mbits;
 dp.second<<=uint_mbits;

 // summing
 // Note: retv.first will not overflow,
 // thus no need to check the carry bit.
 UInt retv_lo0 = retv.second + cp.second;
 if (retv_lo0 < retv.second) {
  ++retv.first;
 }
 retv.second = retv_lo0 + dp.second;
 if (retv.second < retv_lo0) {
  ++retv.first;
 }
 retv.first+=cp.first;
 retv.first+=dp.first;
 return retv;
}

buint_size_t uint_msb(UInt a) {
 // kind of binary search..
 buint_size_t n = 4 * sizeof(UInt);
 buint_size_t inf = 0;
 for (buint_size_t s = n / 2; 0 < s; s/=2) {
  if (a < (UInt)1 << n) {
   n-=s;    // same as n=inf+s
  } else {
   inf = n;
   n+=s;
  }
 }
 return a < (UInt)1 << n ? inf : n;
}

