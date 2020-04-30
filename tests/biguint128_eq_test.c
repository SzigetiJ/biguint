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
 BigUIntSortRelation rel
} BigUIntSortRelTestVector;

BigUIntSortRelTestVector samples[]={
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

int main(int argc, char **argv) {

 assert(test_sortrel0() == 0);

 return 0;
};

