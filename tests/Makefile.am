TESTS = \
	biguint128_io_test \
	biguint256_io_test

check_PROGRAMS = \
	biguint128_io_test \
	biguint256_io_test

CLEANFILES = biguint256_io_test.c

BUILT_SOURCES = biguint256_io_test.c

biguint256_io_test.c: biguint128_io_test.c
	$(SED) 's/128/256/g' < $< > $@

AM_CFLAGS = -I../src
LDADD = ../src/libbiguint.a

