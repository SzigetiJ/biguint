#include "biguint128.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"

#define BIGUINT_BITS 128

typedef enum {
 BIGUINT_LT,
 BIGUINT_EQ,
 BIGUINT_GT
} BigUIntSortRelation;

typedef struct {
 char *left;
 char *right;
 BigUIntSortRelation rel;
} BigUIntSortRelTestVector;

const BigUIntSortRelTestVector samples[]={
 {"0","0",BIGUINT_EQ},
 {"0","1",BIGUINT_LT},
 {"FFFFFFFF","FFFFFFFF",BIGUINT_EQ},
 {"FFFFFFFF","100000000",BIGUINT_LT},
 {"0","100000000",BIGUINT_LT},
 {"0","10000000000000000",BIGUINT_LT},
 {"0","100000000000000000000000000000000",BIGUINT_LT},
 {"0","10000000000000000000000000000000000000000000000000000000000000000",BIGUINT_LT},
 {"1","100000001",BIGUINT_LT},
 {"100000001","100000001",BIGUINT_EQ},
 {"FFFFFFFFF00000000","FFFFFFFF00000000FFFFFFFF",BIGUINT_LT},
 {"A5A5A5A5A5A5A5A5","5A5A5A5A5A5A5A5A",BIGUINT_GT},
 {"A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5","5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A",BIGUINT_GT},
 {"A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5","5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A",BIGUINT_GT},
};
int sample_len = sizeof(samples) / sizeof(BigUIntSortRelTestVector);

const char *bool_to_str(int a) {
 return a?"TRUE":"FALSE";
}

int test_sortrel0() {
 int fail = 0;
 int run_cnt = 0;

 for (int i=0; i<sample_len; ++i) {
  // prepare
  char *sample_left = samples[i].left;
  char *sample_right = samples[i].right;
  if (BIGUINT_BITS / 4 < strlen(sample_left) || BIGUINT_BITS / 4 < strlen(sample_right))
   continue;
  BigUInt128 a = biguint128_ctor_hexcstream(sample_left, strlen(sample_left));
  BigUInt128 b = biguint128_ctor_hexcstream(sample_right, strlen(sample_right));

  // eval
  buint_bool result_lt = biguint128_lt(&a, &b);
  buint_bool result_eq = biguint128_eq(&a, &b);
  buint_bool result_lt_rev = biguint128_lt(&b, &a);
  buint_bool result_eq_rev = biguint128_eq(&b, &a);

  // process result
  if (!!result_lt != (samples[i].rel==BIGUINT_LT)) {
   fprintf(stderr, "[%s < %s] expected: [%s], actual [%s]\n", sample_left, sample_right, bool_to_str(samples[i].rel==BIGUINT_LT), bool_to_str(result_lt));
   fail = 1;
  }
  if (!!result_eq != (samples[i].rel==BIGUINT_EQ)) {
   fprintf(stderr, "[%s == %s] expected: [%s], actual [%s]\n", sample_left, sample_right, bool_to_str(samples[i].rel==BIGUINT_EQ), bool_to_str(result_eq));
   fail = 1;
  }
  if (!!result_lt_rev != (samples[i].rel==BIGUINT_GT)) {
   fprintf(stderr, "[%s < %s] expected: [%s], actual [%s]\n", sample_right, sample_left, bool_to_str(samples[i].rel==BIGUINT_GT), bool_to_str(result_lt));
   fail = 1;
  }
  if (!!result_eq_rev != (samples[i].rel==BIGUINT_EQ)) {
   fprintf(stderr, "[%s == %s] expected: [%s], actual [%s]\n", sample_right, sample_left, bool_to_str(samples[i].rel==BIGUINT_EQ), bool_to_str(result_eq));
   fail = 1;
  }
  ++run_cnt;
 }
 fprintf(stdout, "Run %d tests.\n", run_cnt);
 return fail;
}

// check 1: -a vs. b; 2: a vs. -b and 3: -a vs. -b
int test_sortrel1() {
 int fail = 0;
 int run_cnt = 0;
 BigUInt128 zero = biguint128_ctor_default();

 for (int i=0; i<sample_len; ++i) {
  // prepare
  char *sample_left = samples[i].left;
  char *sample_right = samples[i].right;
  if (BIGUINT_BITS / 4 < strlen(sample_left) || BIGUINT_BITS / 4 < strlen(sample_right))
   continue;
  BigUInt128 a = biguint128_ctor_hexcstream(sample_left, strlen(sample_left));
  BigUInt128 b = biguint128_ctor_hexcstream(sample_right, strlen(sample_right));
  if (biguint128_gbit(&a, 128 - 1) || biguint128_gbit(&b, 128 - 1))
   continue; // cannot handle signed value

  BigUInt128 a_neg = biguint128_sub(&zero, &a);
  BigUInt128 b_neg = biguint128_sub(&zero, &b);
  buint_bool za = biguint128_eq(&a, &zero);
  buint_bool zb = biguint128_eq(&b, &zero);

  // eval
  buint_bool result_lt_1 = bigint128_lt(&a_neg, &b);
  buint_bool result_gt_1 = bigint128_lt(&b, &a_neg);
  buint_bool result_lt_2 = bigint128_lt(&a, &b_neg);
  buint_bool result_gt_2 = bigint128_lt(&b_neg, &a);
  buint_bool result_lt_3 = bigint128_lt(&a_neg, &b_neg);
  buint_bool result_gt_3 = bigint128_lt(&b_neg, &a_neg);

  // process result
  if (!result_lt_1 && !(za && zb)) { // a not positive value is less than a not negative value, unless they both are 0
   fprintf(stderr, "[neg(%s) < %s] expected: [%s], actual [%s]\n", sample_left, sample_right, bool_to_str(true), bool_to_str(result_lt_1));
   fail = 1;
  }
  if (result_gt_1) { // a not negative value cannot be less than a not positive value
   fprintf(stderr, "[%s < neg(%s)] expected: [%s], actual [%s]\n", sample_right, sample_left, bool_to_str(false), bool_to_str(result_gt_1));
   fail = 1;
  }

  if (result_lt_2) { // a not negative value cannot be less than a not positive value
   fprintf(stderr, "[%s < neg(%s)] expected: [%s], actual [%s]\n", sample_left, sample_right, bool_to_str(false), bool_to_str(result_lt_2));
   fail = 1;
  }
  if (!result_gt_2 && !(za && zb)) { // a not positive value is less than a not negative value, unless they both are 0
   fprintf(stderr, "[neg(%s) < %s] expected: [%s], actual [%s]\n", sample_right, sample_left, bool_to_str(true), bool_to_str(result_gt_2));
   fail = 1;
  }

  // original
  if (!!result_lt_3 != (samples[i].rel==BIGUINT_GT)) { // if a gt b, then neg(a) lt neg(b)
   fprintf(stderr, "[neg(%s) < neg(%s)] expected: [%s], actual [%s]\n", sample_left, sample_right, bool_to_str(samples[i].rel==BIGUINT_LT), bool_to_str(result_lt_3));
   fail = 1;
  }
  if (!!result_gt_3 != (samples[i].rel==BIGUINT_LT)) { // if a lt b, then neg(a) gt neg(b)
   fprintf(stderr, "[neg(%s) < neg(%s)] expected: [%s], actual [%s]\n", sample_left, sample_right, bool_to_str(samples[i].rel==BIGUINT_GT), bool_to_str(result_gt_3));
   fail = 1;
  }
  ++run_cnt;
 }
 fprintf(stdout, "Run %d tests.\n", run_cnt);
 return fail;
}

int main(int argc, char **argv) {

 assert(test_sortrel0() == 0);
 assert(test_sortrel1() == 0);

 return 0;
}

