#include "test_common128.h"
#include "test_common.h"

// Note: these values rely on add/sub functions.
// They can be used only after basic add/del tests passed.
BigUInt128 zero;
BigUInt128 one;
BigUInt128 two;
BigUInt128 uintmax;
BigUInt128 uintoflow;
BigUInt128 max;
BigUInt128 maxbutone;

void init_testvalues() {
 zero= biguint128_ctor_default();
 one= biguint128_ctor_unit();
 two= biguint128_add(&one, &one);
 uintmax= biguint128_ctor_hexcstream("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", 2 * sizeof(UInt));
 uintoflow= biguint128_add(&uintmax, &one);
 max= biguint128_sub(&zero, &one);
 maxbutone= biguint128_sub(&max, &one);
}

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


void fprint_funres_buint128_x_bsz_buint128(
        FILE *out,
        const char *funname,
        const BigUInt128 *a, buint_size_t b,
        const BigUInt128 *expected, const BigUInt128 *actual
) {
    char buffer[3][HEX_BIGUINTLEN + 1];
    buffer[0][biguint128_print_hex(a, buffer[0], HEX_BIGUINTLEN)]=0;
    buffer[1][biguint128_print_hex(expected, buffer[1], HEX_BIGUINTLEN)]=0;
    buffer[2][biguint128_print_hex(actual, buffer[2], HEX_BIGUINTLEN)]=0;
    fprintf(out, "%s(%s,%" PRIbuint_size_t ") -- expected: [%s], actual [%s]\n",
            funname, buffer[0], b, buffer[1], buffer[2]);
}

void fprint_funres_buint128_x_bsz_bb(
        FILE *out,
        const char *funname,
        const BigUInt128 *a, buint_size_t b,
        buint_bool expected, buint_bool actual
) {
    char buffer[1][HEX_BIGUINTLEN + 1];
    buffer[0][biguint128_print_hex(a, buffer[0], HEX_BIGUINTLEN)]=0;
    fprintf(out, "%s(%s,%" PRIbuint_size_t ") -- expected: [%u], actual [%u]\n",
            funname, buffer[0], b, expected, actual);
}