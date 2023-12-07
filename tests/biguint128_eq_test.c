#include "biguint128.h"
#include "test_common.h"
#include "test_common128.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef enum {
 BIGUINT_LT,
 BIGUINT_EQ,
 BIGUINT_GT
} BigUIntSortRelation;

typedef struct {
 CStr left;
 CStr right;
 BigUIntSortRelation rel_pp;
 BigUIntSortRelation rel_np;
 BigUIntSortRelation rel_pn;
 BigUIntSortRelation rel_nn;
} BigUIntSortRelTestVector;

const BigUIntSortRelTestVector samples[]={
 {STR("0"),STR("0"),BIGUINT_EQ,BIGUINT_EQ,BIGUINT_EQ,BIGUINT_EQ},
 {STR("0"),STR("1"),BIGUINT_LT,BIGUINT_LT,BIGUINT_GT,BIGUINT_GT},
 {STR("FFFFFFFF"),STR("FFFFFFFF"),BIGUINT_EQ,BIGUINT_LT,BIGUINT_GT,BIGUINT_EQ},
 {STR("FFFFFFFF"),STR("100000000"),BIGUINT_LT,BIGUINT_LT,BIGUINT_GT,BIGUINT_GT},
 {STR("0"),STR("100000000"),BIGUINT_LT,BIGUINT_LT,BIGUINT_GT,BIGUINT_GT},
 {STR("0"),STR("10000000000000000"),BIGUINT_LT,BIGUINT_LT,BIGUINT_GT,BIGUINT_GT},
 {STR("0"),STR("100000000000000000000000000000000"),BIGUINT_LT,BIGUINT_LT,BIGUINT_GT,BIGUINT_GT},
 {STR("0"),STR("10000000000000000000000000000000000000000000000000000000000000000"),BIGUINT_LT,BIGUINT_LT,BIGUINT_GT,BIGUINT_GT},
 {STR("1"),STR("100000001"),BIGUINT_LT,BIGUINT_LT,BIGUINT_GT,BIGUINT_GT},
 {STR("100000001"),STR("100000001"),BIGUINT_EQ,BIGUINT_LT,BIGUINT_GT,BIGUINT_EQ},
 {STR("FFFFFFFFF00000000"),STR("FFFFFFFF00000000FFFFFFFF"),BIGUINT_LT,BIGUINT_LT,BIGUINT_GT,BIGUINT_GT},
 {STR("A5A5A5A5A5A5A5A5"),STR("5A5A5A5A5A5A5A5A"),BIGUINT_GT,BIGUINT_LT,BIGUINT_GT,BIGUINT_LT},
 {STR("A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5"),STR("5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A"),BIGUINT_GT,BIGUINT_LT,BIGUINT_GT,BIGUINT_LT},
 {STR("A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5"),STR("5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A"),BIGUINT_GT,BIGUINT_LT,BIGUINT_GT,BIGUINT_LT},
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
  const CStr *sample_left = &(samples[i].left);
  const CStr *sample_right = &(samples[i].right);
  if (HEX_BIGUINTLEN < sample_left->len || HEX_BIGUINTLEN < sample_right->len)
   continue;
  BigUInt128 a = biguint128_ctor_hexcstream(sample_left->str, sample_left->len);
  BigUInt128 b = biguint128_ctor_hexcstream(sample_right->str, sample_right->len);

  // eval
  buint_bool result_lt = biguint128_lt(&a, &b);
  buint_bool result_eq = biguint128_eq(&a, &b);
  buint_bool result_lt_rev = biguint128_lt(&b, &a);
  buint_bool result_eq_rev = biguint128_eq(&b, &a);

  // process result
  if (!!result_lt != (samples[i].rel_pp==BIGUINT_LT)) {
   fprintf(stderr, "[%s < %s] expected: [%s], actual [%s]\n", sample_left->str, sample_right->str, bool_to_str(samples[i].rel_pp==BIGUINT_LT), bool_to_str(result_lt));
   fail = 1;
  }
  if (!!result_eq != (samples[i].rel_pp==BIGUINT_EQ)) {
   fprintf(stderr, "[%s == %s] expected: [%s], actual [%s]\n", sample_left->str, sample_right->str, bool_to_str(samples[i].rel_pp==BIGUINT_EQ), bool_to_str(result_eq));
   fail = 1;
  }
  if (!!result_lt_rev != (samples[i].rel_pp==BIGUINT_GT)) {
   fprintf(stderr, "[%s < %s] expected: [%s], actual [%s]\n", sample_right->str, sample_left->str, bool_to_str(samples[i].rel_pp==BIGUINT_GT), bool_to_str(result_lt));
   fail = 1;
  }
  if (!!result_eq_rev != (samples[i].rel_pp==BIGUINT_EQ)) {
   fprintf(stderr, "[%s == %s] expected: [%s], actual [%s]\n", sample_right->str, sample_left->str, bool_to_str(samples[i].rel_pp==BIGUINT_EQ), bool_to_str(result_eq));
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
  const CStr *sample_left = &samples[i].left;
  const CStr *sample_right = &samples[i].right;
  if (HEX_BIGUINTLEN < sample_left->len || HEX_BIGUINTLEN < sample_right->len)
   continue;
  BigUInt128 a = biguint128_ctor_hexcstream(sample_left->str, sample_left->len);
  BigUInt128 b = biguint128_ctor_hexcstream(sample_right->str, sample_right->len);
  if (biguint128_gbit(&a, 128 - 1) || biguint128_gbit(&b, 128 - 1))
   continue; // cannot handle signed value

  BigUInt128 a_neg = negate_bigint128(&a);
  BigUInt128 b_neg = negate_bigint128(&b);
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
  // neg(a) vs. b
  if (!!result_lt_1 != (samples[i].rel_np==BIGUINT_LT)) {
   fprintf(stderr, "[neg(%s) < %s] expected: [%s], actual [%s]\n", sample_left->str, sample_right->str, bool_to_str(samples[i].rel_np==BIGUINT_LT), bool_to_str(result_lt_1));
   fail = 1;
  }
  if (!!result_gt_1 != (samples[i].rel_np==BIGUINT_GT)) { // a not negative value cannot be less than a not positive value
   fprintf(stderr, "[%s < neg(%s)] expected: [%s], actual [%s]\n", sample_right->str, sample_left->str, bool_to_str(samples[i].rel_np==BIGUINT_GT), bool_to_str(result_gt_1));
   fail = 1;
  }

  // a vs. neg(b)
  if (!!result_lt_2 != (samples[i].rel_pn==BIGUINT_LT)) {
   fprintf(stderr, "[%s < neg(%s)] expected: [%s], actual [%s]\n", sample_left->str, sample_right->str, bool_to_str(samples[i].rel_pn==BIGUINT_LT), bool_to_str(result_lt_2));
   fail = 1;
  }
  if (!!result_gt_2 != (samples[i].rel_pn==BIGUINT_GT)) {
   fprintf(stderr, "[neg(%s) < %s] expected: [%s], actual [%s]\n", sample_right->str, sample_left->str, bool_to_str(samples[i].rel_pn==BIGUINT_GT), bool_to_str(result_gt_2));
   fail = 1;
  }

  // neg(a) vs. neg(b)
  if (!!result_lt_3 != (samples[i].rel_nn==BIGUINT_LT)) {
   fprintf(stderr, "[neg(%s) < neg(%s)] expected: [%s], actual [%s]\n", sample_left->str, sample_right->str, bool_to_str(samples[i].rel_nn==BIGUINT_LT), bool_to_str(result_lt_3));
   fail = 1;
  }
  if (!!result_gt_3 != (samples[i].rel_nn==BIGUINT_GT)) {
   fprintf(stderr, "[neg(%s) < neg(%s)] expected: [%s], actual [%s]\n", sample_right->str, sample_left->str, bool_to_str(samples[i].rel_nn==BIGUINT_GT), bool_to_str(result_gt_3));
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

