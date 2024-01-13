#include <stdlib.h>
#include <string.h>

#include "test_common128.h"
#include "test_common.h"

#define MAX_VALUES 8U

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
 uintmax= biguint128_value_of_uint(-1);
 uintoflow= biguint128_add(&uintmax, &one);
 max= biguint128_sub(&zero, &one);
 maxbutone= biguint128_sub(&max, &one);
}

static bool read_biguint128(BigUInt128 *result, const char* const str, size_t len, Format fmt) {
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

bool read_more_cstr_biguint128(BigUInt128 *result_arr, const CStr* const a_arr, const unsigned int *idx_arr, size_t num, Format fmt) {
 bool retv = true;
 for (size_t i = 0; i < num; ++i) {
  retv &= read_cstr_biguint128(result_arr + i, a_arr + idx_arr[i], fmt);
 }
 return retv;
}

static bool read_more_cstr_genarg(GenArgU *result_arr, const CStr* const a_arr, const unsigned int *idx_arr, size_t arg_n, ArgType *atypes, Format fmt) {
 bool retv = true;
 for (size_t i = 0; i < arg_n; ++i) {
  if (atypes[i]==BUINT_XROREF || atypes[i] == BUINT_XRWREF || atypes[i] == BUINT_XVAL) {
   retv &= read_cstr_biguint128(&result_arr[i].x, a_arr + idx_arr[i], fmt);
  } else if (atypes[i] == BUINT_TSIZE) {
   result_arr[i].sz = atoi((a_arr + idx_arr[i])->str);
  } else if (atypes[i] == BUINT_TBOOL) {
   result_arr[i].b = atoi((a_arr + idx_arr[i])->str);
  } else if (atypes[i] == BUINT_CWOREF) {
   result_arr[i].str = (a_arr + idx_arr[i])->str;
  } else {
   fprintf(stderr,"errorx0");
  }
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

static inline void print_gennums(FILE *out, const GenArgU *nums, unsigned int num_n, const ArgType *numt, Format fmt) {
 BigUInt128TernaryFun fun =
  fmt == FMT_HEX?biguint128_print_hex :
  fmt == FMT_DEC?biguint128_print_dec :
  fmt == FMT_SDEC?bigint128_print_dec :
  NULL;
  char buf[DEC_BIGINTLEN_HI + 1];
 for (unsigned int i=0; i<num_n; ++i) {
  if (i!=0) fprintf(out, ", ");
  if (numt[i] == BUINT_XROREF || numt[i] == BUINT_XRWREF || numt[i] == BUINT_XVAL) {
   buf[fun(&nums[i].x, buf, DEC_BIGINTLEN_HI)]=0;
   fprintf(out, "%s", buf);
  } else if (numt[i] == BUINT_TBOOL || numt[i] == BUINT_TSIZE) {
   fprintf(out, "%u", (unsigned int)nums[i].sz);
  } else if (numt[i] == BUINT_CWOREF) {
   fprintf(out, "'%s'", nums[i].str);
  } else {
   fprintf(out,"???");
  }
 }
}

void fprintf_biguint128_genfun0_testresult(FILE *out, const char *funname, GenArgU *values, GenArgU *result, GenArgU *expected, unsigned int arg_n, ArgType *argt, unsigned int res_n, ArgType *rest, Format fmt) {
   fprintf(out, "%s(", funname);
   print_gennums(out, values, arg_n, argt, fmt);
   fprintf(out, ") -- expected: [");
   print_gennums(out, expected, res_n, rest, fmt);
   fprintf(out, "], actual [");
   print_gennums(out, result, res_n, rest, fmt);
   fprintf(out, "]\n");
}

#define FL_REL 1 // is relation
#define FL_ASG 2 // is assignment
#define FL_EXP 4 // is export
#define FL_PAIR 8 // return pair

typedef struct {
 unsigned int args;
 ArgType atypes[4];
 unsigned int flags;
} FunProperties;

FunProperties FUNPROP[]={
 {2, {BUINT_XROREF, BUINT_XVAL,   BUINT_VOID,    BUINT_VOID}, 0},        // FUN_UN0
 {2, {BUINT_XRWREF, BUINT_XRWREF, BUINT_VOID,    BUINT_VOID}, FL_ASG},   // FUN_UN0_ASG
 {3, {BUINT_XROREF, BUINT_XROREF, BUINT_XVAL,    BUINT_VOID}, 0},        // FUN_BIN0
 {3, {BUINT_XRWREF, BUINT_XROREF, BUINT_XRWREF,  BUINT_VOID}, FL_ASG},   // FUN_BIN0_ASG
 {3, {BUINT_XROREF, BUINT_TSIZE,  BUINT_XVAL,    BUINT_VOID}, 0},        // FUN_BIN1
 {3, {BUINT_XRWREF, BUINT_TSIZE,  BUINT_XRWREF,  BUINT_VOID}, FL_ASG},   // FUN_BIN1_ASG
 {4, {BUINT_XROREF, BUINT_XROREF, BUINT_XVAL,    BUINT_XVAL}, FL_PAIR},  // FUN_BIN3
 {4, {BUINT_XROREF, BUINT_CWOREF, BUINT_TSIZE,   BUINT_TSIZE}, FL_EXP},  // FUN_TER0
 {2, {BUINT_XROREF, BUINT_TBOOL,  BUINT_VOID,    BUINT_VOID}, FL_REL},   // REL_UN0
 {3, {BUINT_XROREF, BUINT_XROREF, BUINT_TBOOL,   BUINT_VOID}, FL_REL},   // REL_BIN0

 {3, {BUINT_XVAL,   BUINT_XVAL,   BUINT_XVAL,    BUINT_VOID}, 0},        // FUN_BIN0V
 {3, {BUINT_XVAL,   BUINT_TSIZE,  BUINT_XVAL,    BUINT_VOID}, 0},        // FUN_BIN1V
 {4, {BUINT_XVAL,   BUINT_XVAL,   BUINT_XVAL,    BUINT_XVAL}, FL_PAIR},  // FUN_BIN3V
 {4, {BUINT_XROREF, BUINT_CWOREF, BUINT_TSIZE,   BUINT_TSIZE}, FL_EXP},  // FUN_TER0V
 {2, {BUINT_XVAL,   BUINT_TBOOL,  BUINT_VOID,    BUINT_VOID}, FL_REL},   // REL_UN0V
 {3, {BUINT_XVAL,   BUINT_XVAL,   BUINT_TBOOL,   BUINT_VOID}, FL_REL}    // REL_BIN0V
};

int test_genfun(const CStr *samples, unsigned int sample_width, unsigned int sample_n, Format fmt, const unsigned int *param_idx, BigUInt128GenFun fun, const char *funname, ParamRelation param_valid) {
 int fail = 0;
 FunProperties prop = FUNPROP[fun.t];
 unsigned int retv_n = prop.flags & FL_PAIR ? 2 : 1;
 for (unsigned int i = 0; i < sample_n; ++i) {
  GenArgU result;
  BigUIntPair128 resultp; // exceptional case: function is returning a pair
  GenArgU values[5];
  GenArgU *expected = &values[prop.args - 1];
  GenArgU *expected_aux = &values[prop.args - retv_n];
  char buffer[2 * DEC_BIGINTLEN_HI + 10]; // big enough buffer
  if (!read_more_cstr_genarg(values, &samples[i * sample_width], param_idx, prop.args, prop.atypes, fmt)) {
   continue;
  }
  if (param_valid != NULL && !param_valid(values, prop.args)) continue;

  // operation
  switch (fun.t) {
   case FUN_UN0:
    result.x = fun.u.ufun(&values[0].x);
    break;
   case FUN_UN0_ASG:
    values[prop.args].x = values[0].x;
    result.xptr = fun.u.ufun_asg(&values[prop.args].x);
    break;
   case FUN_BIN0:
    result.x = fun.u.bfun(&values[0].x, &values[1].x);
    break;
   case FUN_BIN0_ASG:
    values[prop.args].x = values[0].x;
    result.xptr = fun.u.bfun_asg(&values[prop.args].x, &values[1].x);
    break;
   case FUN_BIN1:
    result.x = fun.u.bfun1(&values[0].x, values[1].sz);
    break;
   case FUN_BIN1_ASG:
    values[prop.args].x = values[0].x;
    result.xptr = fun.u.bfun1_asg(&values[prop.args].x, values[1].sz);
    break;
   case FUN_BIN3:
    resultp = fun.u.bfun3(&values[0].x, &values[1].x);
    break;
   case FUN_TER0:
    result.sz = fun.u.tfun(&values[0].x, buffer, values[2].sz);
    break;
   case REL_UN0:
    result.b = fun.u.urel(&values[0].x);
    break;
   case REL_BIN0:
    result.b = fun.u.brel(&values[0].x, &values[1].x);
    break;
   case FUN_BIN0V:
    result.x = fun.u.bfunv(values[0].x, values[1].x);
    break;
   case FUN_BIN1V:
    result.x = fun.u.bfun1v(values[0].x, values[1].sz);
    break;
   case FUN_BIN3V:
    resultp = fun.u.bfun3v(values[0].x, values[1].x);
    break;
   case FUN_TER0V:
    result.sz = fun.u.tfunv(values[0].x, buffer, values[2].sz);
    break;
   case REL_UN0V:
    result.b = fun.u.urelv(values[0].x);
    break;
   case REL_BIN0V:
    result.b = fun.u.brelv(values[0].x, values[1].x);
    break;
  }

  // eval
  if (prop.flags & FL_ASG) {
   buint_bool result_ok = biguint128_eq(&expected->x, &values[prop.args].x);
   bool retv_ok = (result.xptr == &values[prop.args].x);
   if (!result_ok) {
    fprintf_biguint128_genfun0_testresult(stderr, funname, values, &values[prop.args], values + (prop.args - 1), prop.args - 1, prop.atypes, 1, prop.atypes + (prop.args - 1), fmt);
    fail |= 1;
   }
   if (!retv_ok) {
    fprintf(stderr, "Operation '%s' returned invalid pointer. Expected: %p, actual: %p\n", funname,
      (void*) &values[prop.args].x, (void*) result.xptr);
    fail |= 1;
   }
  } else if (prop.flags & FL_EXP) {
   bool result_ok = (strncmp(buffer, values[1].str, result.sz) == 0);
   bool retv_ok = (result.sz == values[prop.args-1].sz);
   if (!result_ok) {
    buffer[result.sz]=0;
    GenArgU bufx = {.str = buffer};
    fprintf_biguint128_genfun0_testresult(stderr, funname, values, &bufx, values + 1, prop.args - 1, prop.atypes, 1, prop.atypes + 1, fmt);
    fail |= 1;
   }
   if (!retv_ok) {
    fprintf(stderr, "Operation '%s' returned invalid size. Expected: %"PRIbuint_size_t", actual: %"PRIbuint_size_t"\n", funname,
      values[prop.args-1].sz, result.sz);
    fail |= 1;
   }
  } else {
   buint_bool result_ok =
     (prop.flags & FL_REL) ? !!expected->b == !!result.b :
     (prop.flags & FL_PAIR) ? (biguint128_eq(&expected_aux->x, &resultp.first) && biguint128_eq(&expected->x, &resultp.second)) :
     biguint128_eq(&expected->x, &result.x);
   if (!result_ok) {
    GenArgU res_gen[] = {{.x = resultp.first}, {.x = resultp.second}};
    if (!(prop.flags & FL_PAIR)) res_gen[0] = result;
    fprintf_biguint128_genfun0_testresult(stderr, funname, values, res_gen, values + (prop.args - retv_n), prop.args - retv_n, prop.atypes, retv_n, prop.atypes + (prop.args - retv_n), fmt);
    fail |= 1;
   }

  }
 }

 return fail;
}
