#include "biguint128.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "assert.h"

#define BIGUINT_BITS 128
#define BIGUINT_CELLS BIGUINT_BITS / (8*sizeof(UInt))

bool test_ctor_default() {
 BigUInt128 a = biguint128_ctor_default();
 bool pass = true;
 for (int i=0; i < BIGUINT_CELLS; ++i) {
  UInt cell = a.dat[i];
  if (cell != 0) {
   fprintf(stderr, "%s -- cell #%d assertion failed, expected: [0], actual: [%u]\n", __func__, i, cell);
   pass=false;
  }
 }
 return pass;
}

bool test_ctor_unit() {
 BigUInt128 a = biguint128_ctor_unit();
 bool pass = true;
 for (int i=0; i < BIGUINT_CELLS; ++i) {
  UInt cell = a.dat[i];
  if (cell != (i==0?1:0)) {
   fprintf(stderr, "%s -- cell #%d assertion failed, expected: [%d], actual: [%u]\n", __func__, i, (i==0?1:0), cell);
   pass=false;
  }
 }
 return pass;
}

bool test_ctor_standard() {
 UInt dat[BIGUINT_CELLS];
 for (int i=0; i < BIGUINT_CELLS; ++i) {
  dat[i]=i;
 }
 BigUInt128 a = biguint128_ctor_standard(dat);
 bool pass = true;
 for (int i=0; i < BIGUINT_CELLS; ++i) {
  UInt cell = a.dat[i];
  if (cell != dat[i]) {
   fprintf(stderr, "%s -- cell #%d assertion failed, expected: [%d], actual: [%u]\n", __func__, i, dat[i], cell);
   pass=false;
  }
 }
 return pass;
}

bool test_ctor_copy() {
 UInt dat[BIGUINT_CELLS];
 for (int i=0; i < BIGUINT_CELLS; ++i) {
  dat[i]=i;
 }
 BigUInt128 a = biguint128_ctor_standard(dat);
 BigUInt128 b = biguint128_ctor_copy(&a);
 bool pass = true;
 // check that `b' is written
 for (int i=0; i < BIGUINT_CELLS; ++i) {
  UInt cell = b.dat[i];
  if (cell != dat[i]) {
   fprintf(stderr, "%s -- variable b cell #%d assertion failed, expected: [%d], actual: [%u]\n", __func__, i, dat[i], cell);
   pass=false;
  }
 }
 // overwrite `b'
 for (int i=0; i < BIGUINT_CELLS; ++i) {
  b.dat[i]=0;
 }
 // then check that `a' is not overwritten
 for (int i=0; i < BIGUINT_CELLS; ++i) {
  UInt cell = a.dat[i];
  if (cell != dat[i]) {
   fprintf(stderr, "%s -- variable a cell #%d assertion failed, expected: [%d], actual: [%u]\n", __func__, i, dat[i], cell);
   pass=false;
  }
 }
 return pass;
}

int main(int argc, char **argv) {

 assert(test_ctor_default());
 assert(test_ctor_unit());
 assert(test_ctor_standard());
 assert(test_ctor_copy());

 return 0;
}

