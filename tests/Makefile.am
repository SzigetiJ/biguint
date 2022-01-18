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

check_PROGRAMS = $(TESTS)

CLEANFILES = \
	biguint256_ctor_test.c \
	biguint256_io_test.c \
	biguint256_eq_test.c \
	biguint256_bit_test.c \
	biguint256_add_test.c \
	biguint256_mul_test.c \
	test_common256.c \
	test_common256.h

BUILT_SOURCES = $(CLEANFILES)

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

test_common256.c: test_common128.c
	$(SED) 's/128/256/g' < $< > $@

test_common256.h: test_common128.h
	$(SED) 's/128/256/g' < $< > $@

## Not sure this is the best solution
EXTRA_DIST = test_common.c test_common.h test_common128.c test_common128.h
DISTCLEANFILES = $(DEPDIR)/test_common.Po $(DEPDIR)/test_common128.Po $(DEPDIR)/test_common256.Po

AM_CPPFLAGS = -I${top_srcdir}/src -I../src -I${top_srcdir}/tests -I../tests

## Temporary arrangement --
## the goal is to move test_common*.o objs from test-specific LDADDs to the generic LDADD.
LDADD = ../src/libbiguint.a
biguint128_io_test_LDADD = test_common.o test_common128.o ../src/libbiguint.a
biguint256_io_test_LDADD = test_common.o test_common256.o ../src/libbiguint.a
biguint128_eq_test_LDADD = test_common.o test_common128.o ../src/libbiguint.a
biguint256_eq_test_LDADD = test_common.o test_common256.o ../src/libbiguint.a
biguint128_mul_test_LDADD = test_common.o test_common128.o ../src/libbiguint.a
biguint256_mul_test_LDADD = test_common.o test_common256.o ../src/libbiguint.a
biguint128_add_test_LDADD = test_common.o test_common128.o ../src/libbiguint.a
biguint256_add_test_LDADD = test_common.o test_common256.o ../src/libbiguint.a

SUBDIRS=performance