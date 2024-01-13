#include "biguint128.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "assert.h"

#define BIGUINT_BITS 128U
#define UINT_BITS (8U * sizeof(UInt))
#define BIGUINT_CELLS (BIGUINT_BITS / UINT_BITS)
#define BIGUINT_SIZE (BIGUINT_BITS / 8)
#define UINT_NEG_UNIT ((UInt)-1)

static UInt val_uint[] = {
 0,	// no bit set
 0xA5A5,	// some bits set
 ((UInt)1<<(UINT_BITS-1))-1, // only MSB is not set
 ((UInt)1<<(UINT_BITS-1)),	// only MSB is set
 ((UInt)1<<(UINT_BITS-1)) | 0xA5A5,	// MSB and other bits are set
 UINT_NEG_UNIT	// all bits set
};
static size_t val_uint_n = sizeof(val_uint)/sizeof(val_uint[0]);

static UInt exp_fill_unsigned[] = {0, 0, 0, 0, 0, 0};
static UInt exp_fill_signed[] = {0, 0, 0, UINT_NEG_UNIT, UINT_NEG_UNIT, UINT_NEG_UNIT};

static inline bool check_dat_array(const char *fun, const UInt *actual, const UInt *expected) {
 bool pass = true;
 for (unsigned int i=0; i < BIGUINT_CELLS; ++i) {
  if (actual[i] != expected[i]) {
   fprintf(stderr, "%s -- cell #%u assertion failed, expected: [%" PRIuint "], actual: [%" PRIuint "]\n",
    fun, i, expected[i], actual[i]);
   pass=false;
  }
 }
 return pass;
}


bool test_ctor_default() {
 UInt expected[BIGUINT_CELLS];
 for (unsigned int i=0; i < BIGUINT_CELLS; ++i) {
  expected[i]=0;
 }

 BigUInt128 a = biguint128_ctor_default();

 return check_dat_array("ctor_default",a.dat, expected);
}

bool test_ctor_unit() {
 UInt expected[BIGUINT_CELLS];
 expected[0]=1;
 for (unsigned int i=1; i < BIGUINT_CELLS; ++i) {
  expected[i]=0;
 }

 BigUInt128 a = biguint128_ctor_unit();

 return check_dat_array("ctor_unit",a.dat, expected);
}

bool test_ctor_standard() {
 UInt dat[BIGUINT_CELLS];
 for (unsigned int i=0; i < BIGUINT_CELLS; ++i) {
  dat[i]=i;
 }

 BigUInt128 a = biguint128_ctor_standard(dat);

 return check_dat_array("ctor_standard",a.dat, dat);
}

bool test_ctor_uint(bool fun_signed, size_t len, UInt *tin, UInt *fillexp) {
 UInt expected[BIGUINT_CELLS];
 bool pass = true;
 for (unsigned int j = 0; j < len; ++j) {
  expected[0] = tin[j];
  for (unsigned int i=1; i < BIGUINT_CELLS; ++i) {
   expected[i]=fillexp[j];
  }

  BigUInt128 a = fun_signed?
   bigint128_value_of_uint(tin[j]):
   biguint128_value_of_uint(tin[j]);

  pass&=check_dat_array(fun_signed?"value_of_uint (signed)":"value_of_uint", a.dat, expected);
 }
 return pass;
}

bool test_ctor_copy() {
 UInt dat[BIGUINT_CELLS];
 UInt zdat[BIGUINT_CELLS];
 for (unsigned int i=0; i < BIGUINT_CELLS; ++i) {
  dat[i]=i;
  zdat[i]=0U;
 }
 BigUInt128 a = biguint128_ctor_standard(dat);
 BigUInt128 b = biguint128_ctor_copy(&a);
 bool pass = true;
 // check that `b' is written
 pass&=check_dat_array("ctor_copy#1",b.dat, dat);
 // overwrite `b'
 for (unsigned int i=0; i < BIGUINT_CELLS; ++i) {
  b.dat[i]=0;
 }
 // then check that `a' is not overwritten
 pass&=check_dat_array("ctor_copy#2",a.dat, dat);

 // check that `b' is all 0
 pass&=check_dat_array("ctor_copy#3",b.dat, zdat);
 return pass;
}

bool test_import() {
 char dat[BIGUINT_SIZE];
 for (unsigned int i=0; i < BIGUINT_SIZE; ++i) {
  dat[i]=(char)(i&0xFF);
 }
 BigUInt128 a = biguint128_ctor_default();
 buint_size_t result = biguint128_import(&a,dat);

 bool pass = (result == BIGUINT_SIZE);
 pass&=check_dat_array("import", a.dat, (UInt*)dat);

 return pass;
}

bool test_export(bool type) {
 BigUInt128 a = biguint128_ctor_default();
 for (unsigned int i=0; i < BIGUINT_SIZE; ++i) {
  ((char*)a.dat)[i]=(char)(i&0xFF);
 }

 // test action phase
 char dump[BIGUINT_SIZE];
#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
 buint_size_t result = type?biguint128_export(&a,dump):biguint128_exportv(a,dump);
#else
 buint_size_t result = biguint128_export(&a,dump);
#endif
 // check phase
 bool pass = result == BIGUINT_SIZE;
 pass&=check_dat_array("export", (UInt*)dump, a.dat);
 return pass;
}

int main() {

 assert(test_ctor_default());
 assert(test_ctor_unit());
 assert(test_ctor_standard());
 assert(test_ctor_copy());
 assert(test_ctor_uint(false, val_uint_n, val_uint, exp_fill_unsigned));
 assert(test_ctor_uint(true, val_uint_n, val_uint, exp_fill_signed));
 assert(test_import());
 assert(test_export(true));
#ifndef WITHOUT_PASS_BY_VALUE_FUNCTIONS
 assert(test_export(false));
#endif
 return 0;
}
