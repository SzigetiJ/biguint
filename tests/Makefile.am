TESTS = \
	intio_test \
	uint_test \
	biguint128_ctor_test \
	biguint128_io_test \
	biguint128_eq_test \
	biguint128_bit_test \
	biguint128_add_test \
	biguint128_mul_test \
	bigdecimal128_io_test \
	bigdecimal128_add_test \
	bigdecimal128_mul_test \
	bigdecimal128_eq_test \
	bigdecimal128_pval_test \
	bigdecimal128_oom_test \
	bigdecimal128_prec_test

check_PROGRAMS = $(TESTS)

## Not sure this is the best solution
EXTRA_DIST = test_common.c test_common.h test_common128.c test_common128.h
DISTCLEANFILES = $(DEPDIR)/test_common.Po $(DEPDIR)/test_common128.Po $(DEPDIR)/test_common256.Po


if WITH_BIGUINT256
 ADDITIONAL256 = \
	biguint@bits256@_ctor_test \
	biguint@bits256@_io_test \
	biguint@bits256@_eq_test \
	biguint@bits256@_bit_test \
	biguint@bits256@_add_test \
	biguint@bits256@_mul_test \
	bigdecimal@bits256@_io_test \
	bigdecimal@bits256@_oom_test

 TESTS += $(ADDITIONAL256)
 check_PROGRAMS += $(ADDITIONAL256)
 nodist_biguint@bits256@_ctor_test_SOURCES = biguint@bits256@_ctor_test.c
 nodist_biguint@bits256@_io_test_SOURCES = biguint@bits256@_io_test.c
 nodist_biguint@bits256@_eq_test_SOURCES = biguint@bits256@_eq_test.c
 nodist_biguint@bits256@_bit_test_SOURCES = biguint@bits256@_bit_test.c
 nodist_biguint@bits256@_add_test_SOURCES = biguint@bits256@_add_test.c
 nodist_biguint@bits256@_mul_test_SOURCES = biguint@bits256@_mul_test.c
 nodist_bigdecimal@bits256@_io_test_SOURCES = bigdecimal@bits256@_io_test.c

 CLEANFILES = \
	biguint@bits256@_ctor_test.c \
	biguint@bits256@_io_test.c \
	biguint@bits256@_eq_test.c \
	biguint@bits256@_bit_test.c \
	biguint@bits256@_add_test.c \
	biguint@bits256@_mul_test.c \
	bigdecimal@bits256@_io_test.c \
	test_common@bits256@.c \
	test_common@bits256@.h
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

bigdecimal256_io_test.c: bigdecimal128_io_test.c
	$(SED) 's/128/256/g' < $< > $@

bigdecimal256_oom_test.c: bigdecimal128_oom_test.c
	$(SED) 's/128/256/g' < $< > $@

test_common256.c: test_common128.c
	$(SED) 's/128/256/g' < $< > $@

test_common256.h: test_common128.h
	$(SED) 's/128/256/g' < $< > $@

 biguint@bits256@_io_test_LDADD = test_common.o test_common256.o ../src/libbiguint.a
 biguint@bits256@_eq_test_LDADD = test_common.o test_common256.o ../src/libbiguint.a
 biguint@bits256@_mul_test_LDADD = test_common.o test_common256.o ../src/libbiguint.a
 biguint@bits256@_add_test_LDADD = test_common.o test_common256.o ../src/libbiguint.a
 biguint@bits256@_bit_test_LDADD = test_common.o test_common256.o ../src/libbiguint.a
 bigdecimal@bits256@_io_test_LDADD = test_common.o ../src/libbiguint.a
 bigdecimal@bits256@_oom_test_LDADD = test_common.o ../src/libbiguint.a

 DISTCLEANFILES += $(DEPDIR)/test_common256.Po
endif

AM_CPPFLAGS = -I${top_srcdir}/src -I../src -I${top_srcdir}/tests -I../tests

## Temporary arrangement --
## the goal is to move test_common*.o objs from test-specific LDADDs to the generic LDADD.
LDADD = ../src/libbiguint.a
biguint128_io_test_LDADD = test_common.o test_common128.o ../src/libbiguint.a
biguint128_eq_test_LDADD = test_common.o test_common128.o ../src/libbiguint.a
biguint128_mul_test_LDADD = test_common.o test_common128.o ../src/libbiguint.a
biguint128_add_test_LDADD = test_common.o test_common128.o ../src/libbiguint.a
biguint128_bit_test_LDADD = test_common.o test_common128.o ../src/libbiguint.a
bigdecimal128_io_test_LDADD = test_common.o ../src/libbiguint.a
bigdecimal128_oom_test_LDADD = test_common.o ../src/libbiguint.a
bigdecimal128_prec_test_LDADD = test_common.o ../src/libbiguint.a

SUBDIRS=performance
