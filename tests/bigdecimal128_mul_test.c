#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "bigdecimal128.h"
#include "test_common.h"

#define BIGUINT_BITS 128
#define BIGDECCAP ((BIGUINT_BITS / 10 + 1) * 3 + 1)
#define BUFLEN (BIGDECCAP + 3)

// INTERNAL TYPES
typedef BigDecimal128(*BigDecimalDivFun)(const BigDecimal128 *a, const BigDecimal128 *b, UInt prec);
typedef BigDecimal128(*BigDecimalBinaryFun)(const BigDecimal128 *a, const BigDecimal128 *b);

typedef union {
 BigDecimalDivFun d;
 BigDecimalBinaryFun m;
} BigDecimalXFun;

// TEST DATA
const CStr any_in[][3] = {
 {STR("20"),STR("0.5"),STR("0")},
 {STR("0.2"),STR("0.6"),STR("3")},
 {STR("10"),STR("30"),STR("5")},
 {STR("-1.5"),STR("1.5"),STR("3")},
 {STR("2.0"),STR("-0.5"),STR("1")},
 {STR("-10"),STR("0.001"),STR("2")},
 {STR("100.00"),STR("3"),STR("0")},
 {STR("-200.00"),STR("7"),STR("1")},
 {STR("200.0"),STR("-9.00"),STR("0")},
 {STR("-300"),STR("-7.00"),STR("3")},
 {STR("0.000"),STR("10.00"),STR("4")}
};

const CStr mul_out[] = {
 STR("10.0"),
 STR("0.12"),
 STR("300"),
 STR("-2.25"),
 STR("-1.00"),
 STR("-0.010"),
 STR("300.00"),
 STR("-1400.00"),
 STR("-1800.000"),
 STR("2100.00"),
 STR("0.00000")
};

const CStr div_out[] = {
 STR("40"),
 STR("0.333"),
 STR("0.33333"),
 STR("-1.000"),
 STR("-4.0"),
 STR("-10000.00"),
 STR("33"),
 STR("-28.5"),
 STR("-22"),
 STR("42.857"),
 STR("0.0000")
};

// INTERNAL FUNCTIONS

static inline bool eval_xtestcase_(const CStr *tin, const CStr *expected, bool is_mul, BigDecimalXFun fun, const char *funstr) {
 bool pass = true;
 char buffer[BUFLEN + 1];
 if (BIGDECCAP < tin[0].len || BIGDECCAP < tin[1].len)
  return false;

 BigDecimal128 a = bigdecimal128_ctor_cstream(tin[0].str, tin[0].len);
 BigDecimal128 b = bigdecimal128_ctor_cstream(tin[1].str, tin[1].len);
 UInt p = (UInt) atoi(tin[2].str);

 BigDecimal128 result = is_mul ? fun.m(&a, &b) : fun.d(&a, &b, p);
 buint_size_t len = bigdecimal128_print(&result, buffer, sizeof (buffer) / sizeof (char) - 1);
 buffer[len] = 0;

 int res_cmp = strcmp(expected->str, buffer);

 if (res_cmp != 0) {
  if (is_mul) {
   fprintf(stderr, "input: %s(%s, %s); expected output: [%s], actual [%s]\n",
     funstr, tin[0].str, tin[1].str, expected->str, buffer);
  } else {
   fprintf(stderr, "input: %s(%s, %s , %s); expected output: [%s], actual [%s]\n",
     funstr, tin[0].str, tin[1].str, tin[2].str, expected->str, buffer);
  }
  pass = false;
 }
 return pass;
}

// WRAPPER FUNCTIONS
BigDecimal128 div_safe_(const BigDecimal128 *a, const BigDecimal128 *b, UInt prec) {
 BigDecimal128 dest;
 bigdecimal128_div_safe(&dest, a, b, prec);
 return dest;
}

BigDecimal128 mul_safe_(const BigDecimal128 *a, const BigDecimal128 *b) {
 BigDecimal128 dest;
 bigdecimal128_mul_safe(&dest, a, b);
 return dest;
}

bool test_x_regular(BigDecimalXFun fun, bool is_mul, const char *funname) {
 bool pass = true;
 for (unsigned int i = 0; i < ARRAYSIZE(any_in); ++i) {
  pass &=eval_xtestcase_(&any_in[i][0], &(is_mul?mul_out :div_out)[i], is_mul, fun, funname);
 }
 return pass;
}

bool test_div_oor_prec(UInt prec_a, UInt prec_b) {
 bool pass = true;

 BigDecimal128 a = {biguint128_value_of_uint(1), prec_a};
 BigDecimal128 b = {biguint128_value_of_uint(3), prec_b};
 BigDecimal128 q;
 for (UInt pi = 0; pi < 128; ++pi) {
  buint_bool exp_res;
  if (pi + prec_b < prec_a + (128 * 3) / 10 + 128/100) exp_res = 1;
  else if (prec_a + (128 * 3) / 10 + 128/96 + 1 < pi + prec_b) exp_res = 0;
  else continue;
  buint_bool res = bigdecimal128_div_safe(&q, &a, &b, pi);
  if (!!exp_res != !!res) {
   fprintf(stderr, "div_safe failed at prec %u\n", (unsigned int) pi);
   pass = false;
  }
 }
 return pass;
}

bool test_mul_oor_prec(unsigned int zeroes) {
 bool pass = true;

 BigDecimal128 a = {biguint128_ctor_default(), 0};
 biguint128_sbit(&a.val, 128 - 1 - zeroes);
 BigDecimal128 p_ab; // product a*b
 BigDecimal128 p_ac; // product a*c

 for (UInt bi = 0; bi < 128 - 1; ++bi) {
  BigDecimal128 b = {biguint128_ctor_default(), 0};
  biguint128_sbit(&b.val, bi);
  BigDecimal128 c = {bigint128_negate(&b.val), 0};
  buint_bool exp_res_ab = (bi < zeroes);
  buint_bool exp_res_ac = (bi < zeroes + 1);
  buint_bool res_ab = bigdecimal128_mul_safe(&p_ab, &a, &b);
  buint_bool res_ac = bigdecimal128_mul_safe(&p_ac, &a, &c);

  if (!!exp_res_ab != !!res_ab) {
   fprintf(stderr, "positive mul_safe failed at bit %u\n", (unsigned int) bi);
   pass = false;
  }
  if (!!exp_res_ac != !!res_ac) {
   fprintf(stderr, "negative mul_safe failed at bit %u\n", (unsigned int) bi);
   pass = false;
  }
 }
 return pass;
}

int main(int argc, char **argv) {
 if (1 < argc) {
  fprintf(stderr,"%s does not require any arguments\n",argv[0]);
 }

 assert(test_x_regular((BigDecimalXFun){.m=bigdecimal128_mul}, true, "mul"));
 assert(test_x_regular((BigDecimalXFun){.m=mul_safe_}, true, "mul_safe"));
 assert(test_x_regular((BigDecimalXFun){.d=bigdecimal128_div}, false, "div"));
 assert(test_x_regular((BigDecimalXFun){.d=bigdecimal128_div_fast}, false, "div_fast"));
 assert(test_x_regular((BigDecimalXFun){.d=div_safe_}, false, "div_safe"));

 assert(test_div_oor_prec(0, 0));
 assert(test_div_oor_prec(2, 0));
 assert(test_div_oor_prec(0, 2));
 assert(test_mul_oor_prec(4));
 return 0;
}

