#include "uint.h"

UInt uint_add(UInt a, UInt b, buint_bool *carry) {
 UInt cx = *carry?1:0;
 UInt c = a + b + cx;
 *carry = *carry ? c <= a : c < a;
 return c; 
}

UInt uint_sub(UInt a, UInt b, buint_bool *carry) {
 UInt cx = *carry?1:0;
 UInt c = a - b - cx;
 *carry = *carry ? a <= c : a < c;
 return c; 
}

UIntPair uint_split(UInt a, buint_size_t lsb) {
 UIntPair retv;
 UInt mask = (lsb<8*sizeof(a)?((UInt)1 << lsb):0) - 1;
 retv.first = a & ~mask;
 retv.second = a & mask;
 return retv; 
}

UIntPair uint_mul(UInt a, UInt b) {
 UIntPair retv;
 buint_size_t uint_mbits = 4*sizeof(UInt);

 UIntPair ap = uint_split(a, uint_mbits);
 ap.first>>=uint_mbits;
 UIntPair bp = uint_split(b, uint_mbits);
 bp.first>>=uint_mbits;

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
 buint_size_t n = 4 * sizeof(UInt);
 buint_size_t inf = 0;
 for (buint_size_t s = n / 2; 0 < s; s/=2) {
  if (a < (UInt)1 <<n) {
   n-=s;
  } else {
   inf = n;
   n+=s;
  }
 }
 return a < (UInt)1 << n ? inf : n;
}

