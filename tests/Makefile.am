TESTS = \
	biguint128_io_test \
	biguint256_io_test \
	biguint128_add_test \
	biguint256_add_test

check_PROGRAMS = \
	biguint128_io_test \
	biguint256_io_test \
	biguint128_add_test \
	biguint256_add_test

CLEANFILES = \
	biguint256_io_test.c \
	biguint256_add_test.c

BUILT_SOURCES = \
	biguint256_io_test.c \
	biguint256_add_test.c

biguint256_io_test.c: biguint128_io_test.c
	$(SED) 's/128/256/g' < $< > $@

biguint256_add_test.c: biguint128_add_test.c
	$(SED) 's/128/256/g' < $< > $@

AM_CFLAGS = -I${top_srcdir}/src
LDADD = ../src/libbiguint.a

