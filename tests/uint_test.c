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
    fprintf(stderr, "%s: uint_add(%" PRIuint ",%" PRIuint ",&carry) return value expected: [%" PRIuint "] actual: [%" PRIuint "]\n",
     __func__, samples[a_i], samples[b_i], sum_expects[a_i][b_i], sum);
     fail = true;
   }
   if (!!carry!=!!carry_expects[a_i][b_i]) {
    fprintf(stderr, "%s: uint_add(%" PRIuint ",%" PRIuint ",&carry) carry expected: [%d] actual: [%d]\n",
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
    fprintf(stderr, "%s: uint_sub(%" PRIuint ",%" PRIuint ",&carry) return value expected: [%" PRIuint "] actual: [%" PRIuint "]\n",
     __func__, samples[a_i], samples[b_i], diff_expects[a_i][b_i], diff);
     fail = true;
   }
   if (!!carry!=!!carry_expects[a_i][b_i]) {
    fprintf(stderr, "%s: uint_sub(%" PRIuint ",%" PRIuint ",&carry) carry expected: [%d] actual: [%d]\n",
     __func__, samples[a_i], samples[b_i], !!carry_expects[a_i][b_i], !!carry);
     fail = true;
   }
  }
 }
 return !fail;
}

bool test_uint_mul() {
 const UInt MAX = -1;
 const UInt SPEC_0 = ((UInt)1 << (sizeof(UInt)*4+1))-1;
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
    fprintf(stderr, "%s: uint_mul(%" PRIuint ",%" PRIuint ") return value expected: [%" PRIuint ",%" PRIuint "] actual: [%" PRIuint ",%" PRIuint "]\n",
     __func__, samples_a[a_i], samples_b[b_i], prod_expect.first, prod_expect.second, prod.first, prod.second);
     fail = true;
   }
  }
 }
 return !fail;
}

// This test assumes 32bit wide UInt type
bool test_uint_spsh32() {
 if (sizeof(UInt)!=4) {
  return true;
 }

 bool fail=false;
 UInt samples_a[]= {
  0,
  0xFFFFFFFF,
  0xAA555555
 };
 buint_size_t samples_lsb[]={
  0,1,16,31,32,64
 };
 unsigned int samples_len_a=sizeof(samples_a)/sizeof(UInt);
 unsigned int samples_len_lsb=sizeof(samples_lsb)/sizeof(buint_size_p);

 UIntPair splt_expects[][6]={
  {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
  {{0xFFFFFFFF,0}, {0xFFFFFFFE,0x00000001}, {0xFFFF0000,0x0000FFFF}, {0x80000000,0x7FFFFFFF}, {0,0xFFFFFFFF},{0,0xFFFFFFFF}},
  {{0xAA555555,0}, {0xAA555554,0x00000001}, {0xAA550000,0x00005555}, {0x80000000,0x2A555555}, {0,0xAA555555},{0,0xAA555555}}
 };

 UIntPair spsh_expects[][6]={
  {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
  {{0xFFFFFFFF,0}, {0x7FFFFFFF,0x80000000},{0xFFFF,0xFFFF0000},{0x1,0xFFFFFFFE},{0,0xFFFFFFFF},{0,0}},
  {{0xAA555555,0}, {0x552AAAAA,0x80000000}, {0xAA55,0x55550000}, {0x1,0x54AAAA}, {0,0xAA555555},{0,0}}
 };

 for (unsigned int a_i=0; a_i<samples_len_a;++a_i) {
  for (unsigned int lsb_i=0; lsb_i<samples_len_lsb;++lsb_i) {
   UIntPair spsh_expect = spsh_expects[a_i][lsb_i];
   UIntPair spsh_actual = uint_split_shift(samples_a[a_i], samples_lsb[lsb_i]);

   if (!(spsh_actual.first==spsh_expect.first
           && spsh_actual.second==spsh_expect.second)) {
    fprintf(stderr, "%s: uint_split_shift(%08" PRIuintX ",%" PRIbuint_size_t ") return value expected: [%08" PRIuintX ",%08" PRIuintX "] actual: [%08" PRIuintX ",%08" PRIuintX "]\n",
     __func__, samples_a[a_i], samples_lsb[lsb_i],
            spsh_expect.first, spsh_expect.second,
            spsh_actual.first, spsh_actual.second);
     fail = true;
   }

   UIntPair splt_expect = splt_expects[a_i][lsb_i];
   UIntPair splt_actual = uint_split(samples_a[a_i], samples_lsb[lsb_i]);

   if (!(splt_actual.first==splt_expect.first
           && splt_actual.second==splt_expect.second)) {
    fprintf(stderr, "%s: uint_split(%08" PRIuintX ",%" PRIbuint_size_t ") return value expected: [%08" PRIuintX ",%08" PRIuintX "] actual: [%08" PRIuintX ",%08" PRIuintX "]\n",
     __func__, samples_a[a_i], samples_lsb[lsb_i],
            splt_expect.first, splt_expect.second,
            splt_actual.first, splt_actual.second);
     fail = true;
   }
  }
 }

 return !fail;
}

#ifdef USE_UINT64_T
// This test assumes 64bit wide UInt type
bool test_uint_spsh64() {
 if (sizeof(UInt)!=8) {
  return true;
 }

 bool fail=false;
 UInt samples_a[]= {
  0,
  0xFFFFFFFFFFFFFFFF,
  0xAA55555555555555
 };
 buint_size_t samples_lsb[]={
  0,1,16,31,32,63,64
 };
 unsigned int samples_len_a=sizeof(samples_a)/sizeof(UInt);
 unsigned int samples_len_lsb=sizeof(samples_lsb)/sizeof(buint_size_p);

 UIntPair splt_expects[][7]={
  {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
  {
   {0xFFFFFFFFFFFFFFFF,0},
   {0xFFFFFFFFFFFFFFFE,0x00000001},
   {0xFFFFFFFFFFFF0000,0x0000FFFF},
   {0xFFFFFFFF80000000,0x7FFFFFFF},
   {0xFFFFFFFF00000000,0xFFFFFFFF},
   {0x8000000000000000,0x7FFFFFFFFFFFFFFF},
   {0,0xFFFFFFFFFFFFFFFF}
  },{
   {0xAA55555555555555,0},
   {0xAA55555555555554,0x00000001},
   {0xAA55555555550000,0x00005555},
   {0xAA55555540000000,0x15555555},
   {0xAA55555500000000,0x55555555},
   {0x8000000000000000,0x2A55555555555555},
   {0,0xAA55555555555555}
  }
 };

 UIntPair spsh_expects[][7]={
  {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
  {
   {0xFFFFFFFFFFFFFFFF,0},
   {0x7FFFFFFFFFFFFFFF,0x8000000000000000},
   {0xFFFFFFFFFFFF,0xFFFF000000000000},
   {0x1FFFFFFFF,0xFFFFFFFE00000000},
   {0xFFFFFFFF,0xFFFFFFFF00000000},
   {0x1,0xFFFFFFFFFFFFFE},
   {0,0xFFFFFFFFFFFFFF}
  },{
   {0xAA55555555555555,0},
   {0x552AAAAAAAAAAAAA,0x8000000000000000},
   {0xAA5555555555,0x5555000000000000},
   {0x1AAAAAAAA,0x54AAAA00000000},
   {0x55555555,0xAA555555},
   {0x1,0x54AAAAAAAAAAAAAA},
   {0,0xAA55555555555555}
  }
 };

 for (unsigned int a_i=0; a_i<samples_len_a;++a_i) {
  for (unsigned int lsb_i=0; lsb_i<samples_len_lsb;++lsb_i) {
   UIntPair spsh_expect = spsh_expects[a_i][lsb_i];
   UIntPair spsh_actual = uint_split_shift(samples_a[a_i], samples_lsb[lsb_i]);

   if (!(spsh_actual.first==spsh_expect.first
           && spsh_actual.second==spsh_expect.second)) {
    fprintf(stderr, "%s: uint_split_shift(%08" PRIuintX ",%" PRIbuint_size_t ") return value expected: [%08" PRIuintX ",%08" PRIuintX "] actual: [%08" PRIuintX ",%08" PRIuintX "]\n",
     __func__, samples_a[a_i], samples_lsb[lsb_i],
            spsh_expect.first, spsh_expect.second,
            spsh_actual.first, spsh_actual.second);
     fail = true;
   }

   UIntPair splt_expect = splt_expects[a_i][lsb_i];
   UIntPair splt_actual = uint_split(samples_a[a_i], samples_lsb[lsb_i]);

   if (!(splt_actual.first==splt_expect.first
           && splt_actual.second==splt_expect.second)) {
    fprintf(stderr, "%s: uint_split(%08" PRIuintX ",%" PRIbuint_size_t ") return value expected: [%08" PRIuintX ",%08" PRIuintX "] actual: [%08" PRIuintX ",%08" PRIuintX "]\n",
     __func__, samples_a[a_i], samples_lsb[lsb_i],
            splt_expect.first, splt_expect.second,
            splt_actual.first, splt_actual.second);
     fail = true;
   }
  }
 }

 return !fail;
}
#endif

int main(int argc, char **argv) {
 assert(test_uint_add());
 assert(test_uint_sub());
 assert(test_uint_mul());
 assert(test_uint_spsh32());
#ifdef USE_UINT64_T
 assert(test_uint_spsh64());
#endif
}

