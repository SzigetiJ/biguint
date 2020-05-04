#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "uint.h"

bool test_uint_add() {
 UInt samples[] = {
  1,0,-1
 };
 unsigned int samples_len=sizeof(samples)/sizeof(UInt);
 UInt sum_expects[][3] = {
  {2,1,0},
  {1,0,-1},
  {0,-1,-2}
 };
 buint_bool carry_expects[][3] = {
  {false, false, true},
  {false, false, false},
  {true, false, true}
 };
 bool fail = false;

 for (unsigned int a_i=0; a_i<samples_len;++a_i) {
  for (unsigned int b_i=0; b_i<samples_len;++b_i) {
   buint_bool carry = false;
   UInt sum = uint_add(samples[a_i], samples[b_i], &carry);

   if (sum!=sum_expects[a_i][b_i]) {
    fprintf(stderr, "%s: uint_add(%u,%u,&carry) return value expected: [%u] actual: [%u]\n",
     __func__, samples[a_i], samples[b_i], sum_expects[a_i][b_i], sum);
     fail = true;
   }
   if (!!carry!=!!carry_expects[a_i][b_i]) {
    fprintf(stderr, "%s: uint_add(%u,%u,&carry) carry expected: [%d] actual: [%d]\n",
     __func__, samples[a_i], samples[b_i], !!carry_expects[a_i][b_i], !!carry);
     fail = true;
   }
  }
 }
 return !fail;
}

bool test_uint_sub() {
 UInt samples[] = {
  1,0,-1
 };
 unsigned int samples_len=sizeof(samples)/sizeof(UInt);
 UInt diff_expects[][3] = {
  {0,1,2},
  {-1,0,1},
  {-2,-1,0}
 };
 buint_bool carry_expects[][3] = {
  {false, false, true},
  {true, false, true},
  {false, false, false}
 };
 bool fail = false;

 for (unsigned int a_i=0; a_i<samples_len;++a_i) {
  for (unsigned int b_i=0; b_i<samples_len;++b_i) {
   buint_bool carry = false;
   UInt diff = uint_sub(samples[a_i], samples[b_i], &carry);

   if (diff!=diff_expects[a_i][b_i]) {
    fprintf(stderr, "%s: uint_sub(%u,%u,&carry) return value expected: [%u] actual: [%u]\n",
     __func__, samples[a_i], samples[b_i], diff_expects[a_i][b_i], diff);
     fail = true;
   }
   if (!!carry!=!!carry_expects[a_i][b_i]) {
    fprintf(stderr, "%s: uint_sub(%u,%u,&carry) carry expected: [%d] actual: [%d]\n",
     __func__, samples[a_i], samples[b_i], !!carry_expects[a_i][b_i], !!carry);
     fail = true;
   }
  }
 }
 return !fail;
}

bool test_uint_mul() {
 const UInt MAX = -1;
 const UInt SPEC_0 = (1<<sizeof(UInt)*4+1)-1;
 const UInt SPEC_1 = MAX;
 UInt samples_a[] = {
  0,1,2,SPEC_0,SPEC_1
 };
 UInt samples_b[] = {
  0,1,3,MAX
 };
 unsigned int samples_len_a=sizeof(samples_a)/sizeof(UInt);
 unsigned int samples_len_b=sizeof(samples_b)/sizeof(UInt);
 UInt prod_hi_expects[][5] = {
  {0,0,0,0,0},
  {0,0,0,0,0},
  {0,0,0,0,2},
  {0,0,1,SPEC_0-1,SPEC_1-1}
 };
 UInt prod_lo_expects[][5] = {
  {0,0,0,0,0},
  {0,1,2,SPEC_0,SPEC_1},
  {0,3,6,3*SPEC_0,3*SPEC_1},
  {0,-1,-2,0-SPEC_0,0-SPEC_1}
 };
 bool fail = false;

 for (unsigned int a_i=0; a_i<samples_len_a;++a_i) {
  for (unsigned int b_i=0; b_i<samples_len_b;++b_i) {
   UIntPair prod_expect = {prod_hi_expects[b_i][a_i], prod_lo_expects[b_i][a_i]};
   UIntPair prod = uint_mul(samples_a[a_i], samples_b[b_i]);

   if (!(prod.first==prod_expect.first && prod.second==prod_expect.second)) {
    fprintf(stderr, "%s: uint_mul(%u,%u) return value expected: [%u,%u] actual: [%u,%u]\n",
     __func__, samples_a[a_i], samples_b[b_i], prod_expect.first, prod_expect.second, prod.first, prod.second);
     fail = true;
   }
  }
 }
 return !fail;
}

int main(int argc, char **argv) {
 assert(test_uint_add());
 assert(test_uint_sub());
 assert(test_uint_mul());
}

