#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "bigdecimal128.h"
#include "test_common.h"

#define BIGUINT_BITS 128
#define BIGDECCAP ((BIGUINT_BITS / 10 + 1) * 3 + 1)
#define BUFLEN (BIGDECCAP + 3)

// INTERNAL TYPES

typedef struct {
 CStr num1;
 CStr num2;
} TestInputType;

typedef struct {
 CStr num;
} TestOutputType;

typedef BigDecimal128 (*BigDecimalFun)(const BigDecimal128 *a, const BigDecimal128 *b);


// TEST DATA
const TestInputType any_in[] = {
 {STR("0"),STR("0")},
 {STR("1"),STR("0.1")},
 {STR("10"),STR("0.01")},
 {STR("10"),STR("-0.1")},
 {STR("-10"),STR("0.1")},
 {STR("-10"),STR("0.000")},
 {STR("0.000"),STR("10.00")},
 {STR("3.55"),STR("3.45")}
};

const TestOutputType add_out[] = {
 {STR("0")},
 {STR("1.1")},
 {STR("10.01")},
 {STR("9.9")},
 {STR("-9.9")},
 {STR("-10.000")},
 {STR("10.000")},
 {STR("7.00")}
};

const TestOutputType sub_out[] = {
 {STR("0")},
 {STR("0.9")},
 {STR("9.99")},
 {STR("10.1")},
 {STR("-10.1")},
 {STR("-10.000")},
 {STR("-10.000")},
 {STR("0.10")}
};

int input_len = sizeof(any_in) / sizeof(any_in[0]);

// INTERNAL FUNCTIONS
/**
 * @return true: test failed, false: test passed.
 */
static inline bool eval_testcase_(const TestInputType *tin, const TestOutputType *expected, BigDecimalFun fun, char funchr) {
 if (BIGDECCAP < tin->num1.len || BIGDECCAP < tin->num2.len) {
  return false;
 }

 char buffer[BUFLEN + 1];
 BigDecimal128 a = bigdecimal128_ctor_cstream(tin->num1.str, tin->num1.len);
 BigDecimal128 b = bigdecimal128_ctor_cstream(tin->num2.str, tin->num2.len);

 BigDecimal128 result = fun(&a, &b);

 buint_size_t len = bigdecimal128_print(&result, buffer, sizeof(buffer) / sizeof(char) - 1);
 buffer[len] = 0;

 int testresult = strcmp(expected->num.str, buffer);

 if (testresult != 0) {
  fprintf(stderr, "input: (%s %c %s); expected output: [%s], actual [%s]\n",
   tin->num1.str, funchr, tin->num2.str, expected->num.str, buffer);
  return true;
 }
 return false;
}

// TEST FUNCTIONS
bool test_add0() {
 bool fail = false;
 for (int i = 0; i < input_len; ++i) {
  bool test_failed = eval_testcase_(&any_in[i], &add_out[i], bigdecimal128_add, '+');
  fail|=test_failed;
 }
 return !fail;
}

bool test_sub0() {
 bool fail = false;
 for (int i = 0; i < input_len; ++i) {
  bool test_failed = eval_testcase_(&any_in[i], &sub_out[i], bigdecimal128_sub, '-');
  fail|=test_failed;
 }
 return !fail;
}

bool test_sub_approx0() {
 BigDecimal128 a = {
  {1}, 0};
 BigDecimal128 b = {
  {2}, 0};
 BigDecimal128 c = {
  {9}, 0};

 bool pass = true;
 for (int i = 1; pass && i < 3 * 128 / 10 + 1; ++i) {
  c.prec = i;
  b = bigdecimal128_sub(&b, &c);
  pass &= bigdecimal128_lt(&a, &b);
  if (!pass) {
   fprintf(stderr, "Failure approximating a value at prec %u", (unsigned int) i);
  }
 }
 return pass;
}

bool test_add_safe0(bool add, bool inverta, bool invertb) {
 BigDecimal128 a = {
  {1}, 0};
 BigDecimal128 b = {
  {1}, 0};
 BigDecimal128 sum;

 int loops = 128;
 int loops_ok_ub = 3 * 128 / 10 + (128 / 100);
 int loops_fail_lb = 3 * 128 / 10 + 2 + (128 / 96);

 bool pass = true;

 if (inverta) {
  bigint128_negate_assign(&a.val);
 }
 if (invertb) {
  bigint128_negate_assign(&b.val);
 }
 for (int i = 0; pass && i < loops; ++i) {
  b.prec = i;
  buint_bool res = add ?
    bigdecimal128_add_safe(&sum, &a, &b) :
    bigdecimal128_sub_safe(&sum, &a, &b);
  if (i < loops_ok_ub) {
   if (!res) {
    fprintf(stderr, "Add/sub (%s, %s, %s) safe should work with precision %u\n", bool_to_str(add), bool_to_str(inverta), bool_to_str(invertb), (unsigned int) i);
    pass = false;
   }
   if ((add != invertb) ? !bigdecimal128_lt(&a, &sum) : !bigdecimal128_lt(&sum, &a)) {
    fprintf(stderr, "Add/sub (%s, %s, %s) safe sum error at precision %u\n", bool_to_str(add), bool_to_str(inverta), bool_to_str(invertb), (unsigned int) i);
    pass = false;
   }
  }
  if (loops_fail_lb < i) {
   if (res) {
    fprintf(stderr, "Add/sub (%s, %s, %s) safe should not work with precision %u\n", bool_to_str(add), bool_to_str(inverta), bool_to_str(invertb), (unsigned int) i);
    pass = false;
   }
  }
 }
 return pass;
}

int main(int argc, char **argv) {

 if (1 < argc) {
  fprintf(stderr, "%s does not require command line arguments\n", argv[0]);
 }

 assert(test_add0());
 assert(test_sub0());

 assert(test_sub_approx0());
 for (unsigned int i = 0; i < 8; ++i) {
  assert(test_add_safe0(i & 4, i & 2, i & 1));
 }

 return 0;
}

