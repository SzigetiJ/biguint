#include "test_common128.h"
#include "test_common.h"

BigUInt128 negate_bigint128(const BigUInt128 *src) {
 BigUInt128 zero = biguint128_ctor_default();
 return biguint128_sub(&zero, src);
}

bool readhex_biguint128(BigUInt128 *result, const char* const hexstr, size_t hexlen) {
 if (hexlen == 0 || HEX_BIGUINTLEN < hexlen) {
  return false;
 }
 *result = biguint128_ctor_hexcstream(hexstr, hexlen);
 return true;
}

bool readhex_cstr_biguint128(BigUInt128 *result, const CStr* const hex) {
 return readhex_biguint128(result, hex->str, hex->len);
}

bool readhex_more_cstr_biguint128(BigUInt128 *result_arr, const CStr* const hex_arr, size_t num) {
 bool retv = true;
 for (size_t i = 0; i < num; ++i) {
  retv &= readhex_cstr_biguint128(result_arr + i, hex_arr + i);
 }
 return retv;
}
