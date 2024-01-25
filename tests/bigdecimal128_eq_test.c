#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "bigdecimal128.h"
#include "test_common.h"

#define BIGUINT_BITS 128
#define BIGDECCAP ((BIGUINT_BITS / 10 + 1) * 3 + 1)
#define BUFLEN (BIGDECCAP + 3)

typedef struct {
 CStr num1;
 CStr num2;
} PrecTestInputType;

typedef struct {
 buint_bool rel;
} PrecTestOutputType;

const PrecTestInputType any_in[] = {
 {STR("20"),STR("20")},
 {STR("0.2"),STR("-0.2")},
 {STR("0.0"),STR("-0.0")},
 {STR("+0.0"),STR("0.0")},
 {STR("0.0"),STR("-0.00")},
 {STR("10.0"),STR("10")},
 {STR("10.0"),STR("10.00")},
 {STR("10.0"),STR("10.0")},
 {STR("10.0"),STR("9")},
 {STR("10.0"),STR("9.9")},
 {STR("10.0"),STR("11")},
 {STR("10.0"),STR("10.0000001")},
 {STR("20"),STR("0.5")},
 {STR("0.2"),STR("0.6")},
 {STR("10"),STR("30")},
 {STR("-1.5"),STR("1.5")},
 {STR("2.0"),STR("-0.5")},
 {STR("-10"),STR("0.001")},
 {STR("0.000"),STR("10.00")},
 {STR("0.1"),STR("0.099999999999999999999999999999999")},
 {STR("0.1"),STR("0.1000000000000000000000000000000001")},
 {STR("-0.2"),STR("-0.19999999999999999999999999999999")},
 {STR("-0.2"),STR("-0.2000000000000000000000000000001")},
 {STR("0.3"),STR("0.31")},
 {STR("0.3"),STR("0.3001")},
 {STR("0.3"),STR("0.30001")},
 {STR("0.3"),STR("0.30000000001")},
 {STR("0.3"),STR("0.300000000000000000000001")},
 {STR("0.3"),STR("0.300000000000000000000000000000001")},
 {STR("0.4"),STR("0.399999999999999999999999999999999")},
 {STR("0.4"),STR("0.4000000000000000000000000000000001")}
};

const PrecTestOutputType eq_out[] = {
 {1},
 {0},
 {1},
 {1},
 {1},
 {1},
 {1},
 {1},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0}
};

const PrecTestOutputType lt_out[] = {
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {0},
 {1},
 {1},
 {0},
 {1},
 {1},
 {1},
 {0},
 {1},
 {1},
 {0},
 {1},
 {1},
 {0},
 {1},
 {1},
 {1},
 {1},
 {1},
 {1},
 {0},
 {1}
};
int input_len = ARRAYSIZE(any_in);

BigUInt128 bint_store[6];
const BigUInt128 *MAX_BINT = &bint_store[0];
const BigUInt128 *MIN_BINT = &bint_store[1];
const BigUInt128 *MAX_BINTD10 = &bint_store[2];
const BigUInt128 *MIN_BINTD10 = &bint_store[3];
const BigUInt128 *MAX_BINTR10 = &bint_store[4];
const BigUInt128 *MIN_BINTR10 = &bint_store[5];

static void init_bint_store() {
 BigUInt128 max_bint = bigint128_value_of_uint(-1);
 biguint128_shr_tiny(&max_bint, 1); // maxbint
 bint_store[0] = max_bint;
 BigUInt128 min_bint = max_bint;
 biguint128_inc(&min_bint);
 bint_store[1] = min_bint;
 BigUInt128 d10 = biguint128_value_of_uint(10);
 BigUIntPair128 maxd10 = bigint128_div(&max_bint, &d10);
 bint_store[2] = maxd10.first;
 bint_store[3] = bigint128_negate(MAX_BINTD10);
 bint_store[4] = biguint128_mul10(MAX_BINTD10);
 bint_store[5] = bigint128_negate(MAX_BINTR10);
}

static void print_relation_result(FILE *out, const char *fnname, const BigDecimal128 *a, const BigDecimal128 *b, buint_bool expected, buint_bool actual) {
 char bufa[BUFLEN + 1];
 char bufb[BUFLEN + 1];
 bufa[bigdecimal128_print(a, bufa, BUFLEN)] = 0;
 bufb[bigdecimal128_print(b, bufb, BUFLEN)] = 0;
 fprintf(out, "%s with\n\ta: %s\n\tb: %s\n\texpected: %s, actual: %s\n", fnname, bufa, bufb, bool_to_str(expected), bool_to_str(actual));
}

bool test_eq0() {
 bool pass = true;
 for (int i = 0; i < input_len; ++i) {
  const PrecTestInputType *ti = &any_in[i];
  const PrecTestOutputType *expected = &eq_out[i];
  if (BIGDECCAP < ti->num1.len || BIGDECCAP < ti->num2.len)
   continue;

  BigDecimal128 a = bigdecimal128_ctor_cstream(ti->num1.str, ti->num1.len);
  BigDecimal128 b = bigdecimal128_ctor_cstream(ti->num2.str, ti->num2.len);

  buint_bool eq = bigdecimal128_eq(&a, &b);
  buint_bool eqrev = bigdecimal128_eq(&b, &a);

  int result = (!!eq != !!expected->rel);
  int resultrev = (!!eqrev != !!expected->rel);

  if (result != 0) {
   print_relation_result(stderr, "eq(a,b)", &a, &b, expected->rel, eq);
   pass = false;
  }
  if (resultrev != 0) {
   print_relation_result(stderr, "eq(a,b)", &b, &a, expected->rel, eqrev);
   pass = false;
  }
 }
 return pass;
}

bool test_eq1() {
 bool fail = false;

 BigDecimal128 a = bigdecimal128_ctor_cstream("0", 1);
 BigDecimal128 b = bigdecimal128_ctor_default();

 buint_bool eq = bigdecimal128_eq(&a, &b);

 int result = (!eq);

 if (result != 0) {
  char buffer[BUFLEN + 1];
  buint_size_t len = bigdecimal128_print(&b, buffer, sizeof(buffer) / sizeof(char) - 1);
  buffer[len] = 0;
  fprintf(stderr, "input: (default ctor); expected output: [%s], actual [%s]\n",
   "0", buffer);
  fail = true;
 }

 return !fail;
}

bool test_lt0() {
 bool pass = true;
 for (int i = 0; i < input_len; ++i) {
  const PrecTestInputType *ti = &any_in[i];
  const PrecTestOutputType *expected = &lt_out[i];
  const PrecTestOutputType *eqexp = &eq_out[i];
  if (BIGDECCAP < ti->num1.len || BIGDECCAP < ti->num2.len)
   continue;

  BigDecimal128 a = bigdecimal128_ctor_cstream(ti->num1.str, ti->num1.len);
  BigDecimal128 b = bigdecimal128_ctor_cstream(ti->num2.str, ti->num2.len);

  buint_bool lt = bigdecimal128_lt(&a, &b);
  buint_bool ltrev = bigdecimal128_lt(&b, &a);

  int result = (!!lt != !!expected->rel);
  int resultrev = (!!ltrev != !!(!expected->rel && !eqexp->rel));

  if (result != 0) {
   print_relation_result(stderr, "lt(a,b)", &a, &b, expected->rel, lt);
   pass = false;
  }
  if (resultrev != 0) {
   print_relation_result(stderr, "lt(a,b)", &b, &a, !expected->rel && !eqexp->rel, ltrev);
   pass = false;
  }
 }
 return pass;
}

bool test_ltx() {
 bool pass = true;

 BigDecimal128 y[][2] = {
  {
   {*MAX_BINT, 1},
   {*MAX_BINTR10, 1}
  },
  {
   {*MIN_BINT, 1},
   {*MIN_BINTR10, 1}
  }
 };

 // x[i][0]: trunc(y[i][0]), x[i][1]: trunc(y[i][0])+1, x[i][2]: trunc(y[i][0])-1
 BigDecimal128 x[][3] = {
  {
   {*MAX_BINTD10, 0},
   {*MAX_BINTD10, 0},
   {*MAX_BINTD10, 0}
  },
  {
   {*MIN_BINTD10, 0},
   {*MIN_BINTD10, 0},
   {*MIN_BINTD10, 0}
  }
 };
 biguint128_inc(&x[0][1].val);
 biguint128_inc(&x[1][1].val);
 biguint128_dec(&x[0][2].val);
 biguint128_dec(&x[1][2].val);

 buint_bool xlty[][2][3] = {
  {
   {1, 0, 1},
   {0, 0, 1}
  },
  {
   {0, 0, 1},
   {0, 0, 1}
  }
 };
 buint_bool yltx[][2][3] = {
  {
   {0, 1, 0},
   {0, 1, 0}
  },
  {
   {1, 1, 0},
   {0, 1, 0}
  }
 };

 for (int i = 0; i < 2; ++i) { // max or min
  for (int yi = 0; yi < 2; ++yi) { // exact or rounded y value
   for (int xi = 0; xi < 3; ++xi) { // which x value
    buint_bool expxy = xlty[i][yi][xi];
    buint_bool expyx = yltx[i][yi][xi];
    buint_bool resxy = bigdecimal128_lt(&x[i][xi], &y[i][yi]);
    buint_bool resyx = bigdecimal128_lt(&y[i][yi], &x[i][xi]);

    if (resxy != expxy || resyx != expyx) {
     fprintf(stderr, "i: %d, yi: %d, xi: %d\n", i, yi, xi);
     if (resxy != expxy) {
      print_relation_result(stderr, "lt(a,b)", &x[i][xi], &y[i][yi], expxy, resxy);
     }
     if (resyx != expyx) {
      print_relation_result(stderr, "lt(a,b)", &y[i][yi], &x[i][xi], expyx, resyx);
     }
     pass = false;
    }
   }
  }
 }

 return pass;
}

int main(int argc, char **argv) {

 if (1 < argc) {
  fprintf(stderr, "This test executable (%s) does not require parameters.\n", argv[0]);
 }
 init_bint_store();

 assert(test_eq0());
 assert(test_lt0());
 assert(test_eq1());

 assert(test_ltx());
 return 0;
}

