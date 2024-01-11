#include "test_common128.h"
#include "test_common.h"

// maximum string length by format. If the length of the
// input string is higher than this number, the parsing
// can fail.
static buint_size_t maxparselen[] = {
 HEX_BIGUINTLEN,
 DEC_BIGUINTLEN_LO,
 DEC_BIGUINTLEN_LO
};

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

bool read_biguint128(BigUInt128 *result, const char* const str, size_t len, Format fmt) {
 if (len == 0 || 2<fmt || maxparselen[fmt] < len) {
  return false;
 }
 *result =
   fmt==FMT_HEX?biguint128_ctor_hexcstream(str, len):
   fmt==FMT_DEC?biguint128_ctor_deccstream(str, len):
   bigint128_ctor_deccstream(str, len);
 return true;
}

bool read_cstr_biguint128(BigUInt128 *result, const CStr* const a, Format fmt) {
 return read_biguint128(result, a->str, a->len, fmt);
}

bool read_more_cstr_biguint128(BigUInt128 *result_arr, const CStr* const a_arr, size_t num, Format fmt) {
 bool retv = true;
 for (size_t i = 0; i < num; ++i) {
  retv &= read_cstr_biguint128(result_arr + i, a_arr + i, fmt);
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

static inline void print_numbers(FILE *out, int size, BigUInt128 *dat, Format fmt) {
 buint_size_t (*fun)(const BigUInt128 *a, char *buf, buint_size_t buf_len) =
  fmt == FMT_HEX?biguint128_print_hex :
  fmt == FMT_DEC?biguint128_print_dec :
  fmt == FMT_SDEC?bigint128_print_dec :
  NULL;
  char buf[DEC_BIGINTLEN_HI + 1];
 for (int i=0; i<size; ++i) {
  if (i!=0) fprintf(out, ", ");
  buf[fun(&dat[i], buf, DEC_BIGINTLEN_HI)]=0;
  fprintf(out, "%s", buf);
 }
}

void fprintf_biguint128_genfun_testresult(FILE *out, const char *funname, int parnum, BigUInt128 *param, int resnum, BigUInt128 *expected, BigUInt128 *actual, Format fmt) {
   fprintf(out, "%s(", funname);
   print_numbers(out, parnum, param, fmt);
   fprintf(out, ") -- expected: [");
   print_numbers(out, resnum, expected, fmt);
   fprintf(out, "], actual [");
   print_numbers(out, resnum, actual, fmt);
   fprintf(out, "]\n");
}

void fprintf_biguint128_binop_testresult(FILE *out, BigUInt128 *op0, BigUInt128 *op1, BigUInt128 *expected, BigUInt128 *actual, const char *op_str) {
 char buffer[4][HEX_BIGUINTLEN + 1];
 BigUInt128 * v_refs[] = {op0, op1, expected, actual};
 for (int j = 0; j < 4; ++j) {
  buffer[j][biguint128_print_hex(v_refs[j], buffer[j], HEX_BIGUINTLEN)] = 0;
 }
 fprintf(out, "[%s %s %s] -- expected: [%s], actual [%s]\n", buffer[0], op_str, buffer[1], buffer[2], buffer[3]);
}

void fprintf_biguint128_unop_testresult(FILE *out, const BigUInt128 *op0, const BigUInt128 *expected, const BigUInt128 *actual, const char *op_str) {
 char buffer[3][HEX_BIGUINTLEN + 1];
 const BigUInt128 * v_refs[] = {op0, expected, actual};
 for (int j = 0; j < 3; ++j) {
  buffer[j][biguint128_print_hex(v_refs[j], buffer[j], HEX_BIGUINTLEN)] = 0;
 }
 fprintf(out, "[%s%s] -- expected: [%s], actual [%s]\n", op_str, buffer[0], buffer[1], buffer[2]);
}

int test_binop0(const CStr *samples, unsigned int sample_len, unsigned int sample_n, Format fmt, unsigned int *param_idx, BigUInt128BinaryFun fun, const char *op_str) {
 int fail = 0;
 for (int i = 0; i < sample_n; ++i) {
  BigUInt128 result;
  BigUInt128 values[3];
  BigUInt128 *a = &values[param_idx[0]];
  BigUInt128 *b = &values[param_idx[1]];
  BigUInt128 *c = &values[param_idx[2]];

  if (!read_more_cstr_biguint128(values, &samples[i * sample_len], 3, fmt)) {
   continue;
  }

  // operation
  result = fun(a, b);

  // eval
  buint_bool result_ok = biguint128_eq(c, &result);
  if (!result_ok) {
   fprintf_biguint128_binop_testresult(stderr, a, b, c, &result, op_str);
   fail |= 1;
  }
 }

 return fail;
}

int test_binop0_assign(const CStr *samples, unsigned int sample_len, unsigned int sample_n, Format fmt, unsigned int *param_idx, BigUInt128BinaryAsgnFun fun, const char *op_str) {
 int fail = 0;
 for (int i = 0; i < sample_n; ++i) {
  BigUInt128 values[3];
  BigUInt128 *a = &values[param_idx[0]];
  BigUInt128 *b = &values[param_idx[1]];
  BigUInt128 *c = &values[param_idx[2]];

  if (!read_more_cstr_biguint128(values, &samples[i * sample_len], 3, fmt)) {
   continue;
  }

  // operation
  BigUInt128 acopy = biguint128_ctor_copy(a);
  BigUInt128 *result = fun(&acopy, b);

  // eval
  buint_bool result_ok = biguint128_eq(c, result);
  buint_bool retv_ok = (result == &acopy);
  if (!result_ok) {
   fprintf_biguint128_binop_testresult(stderr, a, b, c, result, op_str);
   fail |= 1;
  }
  if (!retv_ok) {
   fprintf(stderr, "Operation '%s' returned invalid pointer. Expected: %p, actual: %p\n", op_str,
     (void*) &acopy, (void*) result);
   fail |= 1;
  }
 }

 return fail;
}

int test_binop0v(const CStr *samples, unsigned int sample_len, unsigned int sample_n, Format fmt, unsigned int *param_idx, BigUInt128BinaryVFun fun, const char *op_str) {
 int fail = 0;
 for (int i = 0; i < sample_n; ++i) {
  BigUInt128 values[3];
  BigUInt128 *a = &values[param_idx[0]];
  BigUInt128 *b = &values[param_idx[1]];
  BigUInt128 *c = &values[param_idx[2]];

  if (!read_more_cstr_biguint128(values, &samples[i * sample_len], 3, fmt)) {
   continue;
  }

  // operation
  BigUInt128 result = fun(*a, *b);

  // eval
  buint_bool result_ok = biguint128_eq(c, &result);
  if (!result_ok) {
   fprintf_biguint128_binop_testresult(stderr, a, b, c, &result, op_str);
   fail |= 1;
  }
 }

 return fail;
}
