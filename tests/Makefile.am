TESTS = \
	intio_test \
	uint_test \
	biguint128_ctor_test \
	biguint256_ctor_test \
	biguint128_io_test \
	biguint256_io_test \
	biguint128_eq_test \
	biguint256_eq_test \
	biguint128_bit_test \
	biguint256_bit_test \
	biguint128_add_test \
	biguint256_add_test \
	biguint128_mul_test \
	biguint256_mul_test

check_PROGRAMS = \
	intio_test \
	uint_test \
	biguint128_ctor_test \
	biguint256_ctor_test \
	biguint128_io_test \
	biguint256_io_test \
	biguint128_eq_test \
	biguint256_eq_test \
	biguint128_bit_test \
	biguint256_bit_test \
	biguint128_add_test \
	biguint256_add_test \
	biguint128_mul_test \
	biguint256_mul_test

CLEANFILES = \
	biguint256_ctor_test.c \
	biguint256_io_test.c \
	biguint256_eq_test.c \
	biguint256_bit_test.c \
	biguint256_add_test.c \
	biguint256_mul_test.c

BUILT_SOURCES = \
	biguint256_ctor_test.c \
	biguint256_io_test.c \
	biguint256_eq_test.c \
	biguint256_bit_test.c \
	biguint256_add_test.c \
	biguint256_mul_test.c

biguint256_ctor_test.c: biguint128_ctor_test.c
	$(SED) 's/128/256/g' < $< > $@

biguint256_io_test.c: biguint128_io_test.c
	$(SED) 's/128/256/g' < $< > $@

biguint256_eq_test.c: biguint128_eq_test.c
	$(SED) 's/128/256/g' < $< > $@

biguint256_bit_test.c: biguint128_bit_test.c
	$(SED) 's/128/256/g' < $< > $@

biguint256_add_test.c: biguint128_add_test.c
	$(SED) 's/128/256/g' < $< > $@

biguint256_mul_test.c: biguint128_mul_test.c
	$(SED) 's/128/256/g' < $< > $@

AM_CFLAGS = -I${top_srcdir}/src -I../src
LDADD = ../src/libbiguint.a

SUBDIRS=performance