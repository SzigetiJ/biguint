SRCDIR=../src
TESTDIR=../tests

CC=gcc
CFLAGS=-Wall -Wpedantic -O0 -std=c99 -fprofile-arcs -ftest-coverage -I$(SRCDIR)
LDFLAGS=-fprofile-arcs -ftest-coverage

TEST_X0 = ctor io eq bit add mul
TEST_X1 = intio uint

INFO_FILE=biguint.info

TEST_X0_PROGS = $(TEST_X0:%=biguint128_%_test)
TEST_X0_OBJS = $(TEST_X0_PROGS:%=%.o)
TEST_X0_SOURCES = $(TEST_X0_PROGS:%=$(TESTDIR)/%.c)
TEST_X0_COVS = $(TEST_X0_PROGS:%=%.gcda)
TEST_X0_GCNO = $(TEST_X0_PROGS:%=%.gcno)

TEST_X1_PROGS = $(TEST_X1:%=%_test)
TEST_X1_OBJS = $(TEST_X1_PROGS:%=%.o)
TEST_X1_SOURCES = $(TEST_X1_PROGS:%=$(TESTDIR)/%.c)
TEST_X1_COVS = $(TEST_X1_PROGS:%=%.gcda)
TEST_X1_GCNO = $(TEST_X1_PROGS:%=%.gcno)


LIB_OBJS = \
 l_biguint128.o \
 l_intio.o \
 l_uint.o

LIB_SOURCES = $(LIB_OBJS:l_%.o=$(SRCDIR)/%.c)

all: $(INFO_FILE)

$(INFO_FILE): $(TEST_X0_COVS) $(TEST_X1_COVS)
	lcov -c --directory . --output-file $@

%.gcda: %
	./$<

#biguint128_%_test: biguint128_%_test.o $(LIB_OBJS)
#	$(CC) $(LDFLAGS) -o $@ $^
#
#biguint128_%_test.o: $(TESTDIR)/biguint128_%_test.c
#	$(CC) -c $(CFLAGS) -o $@ $<
#

%_test: %_test.o $(LIB_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%_test.o: $(TESTDIR)/%_test.c
	$(CC) -c $(CFLAGS) -o $@ $<

l_%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

html: $(INFO_FILE)
	genhtml $< --output-directory html

clean:
	lcov -z --directory .
	rm -f $(INFO_FILE)
	rm -f $(LIB_OBJS)
	rm -f $(LIB_OBJS:%.o=%.gcda)
	rm -f $(LIB_OBJS:%.o=%.gcno)
	rm -f $(TEST_X0_PROGS)
	rm -f $(TEST_X0_OBJS)
	rm -f $(TEST_X0_COVS)
	rm -f $(TEST_X0_GCNO)
	rm -f $(TEST_X1_PROGS)
	rm -f $(TEST_X1_OBJS)
	rm -f $(TEST_X1_COVS)
	rm -f $(TEST_X1_GCNO)
	rm -rf html

.PHONY: clean html

