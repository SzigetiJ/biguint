#include "biguint128.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "assert.h"

#define BIGUINT_BITS 128
#define UINT_BITS (8*sizeof(UInt))
#define BIGUINT_CELLS BIGUINT_BITS / UINT_BITS

static inline bool check_dat_array(const char *fun, const UInt *actual, const UInt *expected) {
 bool pass = true;
 for (int i=0; i < BIGUINT_CELLS; ++i) {
  if (actual[i] != expected[i]) {
   fprintf(stderr, "%s -- cell #%d assertion failed, expected: [%" PRIuint "], actual: [%" PRIuint "]\n",
    fun, i, expected[i], actual[i]);
   pass=false;
  }
 }
 return pass;
}

bool test_ctor_default() {
 UInt expected[BIGUINT_CELLS];
 for (int i=0; i < BIGUINT_CELLS; ++i) {
  expected[i]=0;
 }

 BigUInt128 a = biguint128_ctor_default();

 return check_dat_array("ctor_default",a.dat, expected);
}

bool test_ctor_unit() {
 UInt expected[BIGUINT_CELLS];
 expected[0]=1;
 for (int i=1; i < BIGUINT_CELLS; ++i) {
  expected[i]=0;
 }

 BigUInt128 a = biguint128_ctor_unit();

 return check_dat_array("ctor_unit",a.dat, expected);
}

bool test_ctor_standard() {
 UInt dat[BIGUINT_CELLS];
 for (int i=0; i < BIGUINT_CELLS; ++i) {
  dat[i]=i;
 }

 BigUInt128 a = biguint128_ctor_standard(dat);

 return check_dat_array("ctor_standard",a.dat, dat);
}

bool test_ctor_uint() {
 UInt init_dat[] = {0x01, 0xFF, 0xFFFFFFFF, (UInt)(-1)};
 UInt expected[BIGUINT_CELLS];
 bool pass = true;
 for (int j = 0; j<sizeof(init_dat)/sizeof(UInt); ++j) {
  expected[0]=init_dat[j];
  for (int i=1; i < BIGUINT_CELLS; ++i) {
   expected[i]=0;
  }

  BigUInt128 a = biguint128_value_of_uint(init_dat[j]);

  pass&=check_dat_array("value_of_uint",a.dat, expected);
 }
 return pass;
}

bool test_ctor_uint_signed() {
 UInt init_dat[] = {0, ((UInt)1<<(UINT_BITS-1))-1, ((UInt)1<<(UINT_BITS-1)), (UInt)(-1)};
 UInt exp_fill[] = {0,0,(UInt)-1, (UInt)-1};
 UInt expected[BIGUINT_CELLS];
 bool pass = true;
 for (int j = 0; j<sizeof(init_dat)/sizeof(UInt); ++j) {
  expected[0]=init_dat[j];
  for (int i=1; i < BIGUINT_CELLS; ++i) {
   expected[i]=exp_fill[j];
  }

  BigUInt128 a = bigint128_value_of_uint(init_dat[j]);

  pass&=check_dat_array("value_of_uint",a.dat, expected);
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
 pass&=check_dat_array("ctor_copy#1",b.dat, dat);
 // overwrite `b'
 for (int i=0; i < BIGUINT_CELLS; ++i) {
  b.dat[i]=0;
 }
 // then check that `a' is not overwritten
 pass&=check_dat_array("ctor_copy#2",a.dat, dat);

 return pass;
}

bool test_import() {
 char dat[BIGUINT_BITS / 8];
 for (int i=0; i < sizeof(dat); ++i) {
  dat[i]=(char)(i&0xFF);
 }
 BigUInt128 a = biguint128_ctor_default();
 buint_size_t result = biguint128_import(&a,dat);

 bool pass = result == BIGUINT_BITS / 8;
 pass&=check_dat_array("import", a.dat, (UInt*)dat);

 return pass;
}

bool test_export() {
 // init phase: import
 char dat[BIGUINT_BITS / 8];
 for (int i=0; i < sizeof(dat); ++i) {
  dat[i]=(char)(i&0xFF);
 }
 BigUInt128 a = biguint128_ctor_default();
 biguint128_import(&a,dat);

 // test action phase
 char dump[BIGUINT_BITS / 8];
 buint_size_t result = biguint128_export(&a,dump);

 // check phase
 bool pass = result == BIGUINT_BITS / 8;
 pass&=check_dat_array("export", (UInt*)dump, a.dat);
 return pass;
}


int main(int argc, char **argv) {

 assert(test_ctor_default());
 assert(test_ctor_unit());
 assert(test_ctor_standard());
 assert(test_ctor_copy());
 assert(test_ctor_uint());
 assert(test_ctor_uint_signed());
 assert(test_import());
 assert(test_export());

 return 0;
}

