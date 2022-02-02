SRCDIR=../src
TESTDIR=../tests

CC=gcc
## progs
GENHTML=genhtml
GCOV=gcov
LCOV=lcov

## we employ two versions of CFLAGS:
## 1st -- here we want to measure coverage
CFLAGS=-Wall -Wpedantic -O0 -std=c99 -fprofile-arcs -ftest-coverage -I$(SRCDIR) -DSIZEOF_UINT=4
## 2nd -- we do not measure coverage
CFLAGS_X=-Wall -Wpedantic -O0 -std=c99 -I$(SRCDIR) -DSIZEOF_UINT=4

LDFLAGS=-fprofile-arcs -ftest-coverage

TEST_X0 = ctor io eq bit add mul
TEST_X1 = intio uint

INFO_FILE=biguint.info
HTML_DIR=html

TEST_X0_PROGS = $(TEST_X0:%=biguint128_%_test)
TEST_X0_OBJS = $(TEST_X0_PROGS:%=%.o)
TEST_X0_SOURCES = $(TEST_X0_PROGS:%=$(TESTDIR)/%.c)

TEST_X1_PROGS = $(TEST_X1:%=%_test)
TEST_X1_OBJS = $(TEST_X1_PROGS:%=%.o)
TEST_X1_SOURCES = $(TEST_X1_PROGS:%=$(TESTDIR)/%.c)

TEST_COMMON_OBJS = test_common.o test_common128.o

LIB_OBJS = \
 l_biguint128.o \
 l_intio.o \
 l_uint.o

LIB_SOURCES = $(LIB_OBJS:l_%.o=$(SRCDIR)/%.c)
LIB_GCDA = $(LIB_OBJS:%.o=%.gcda)
LIB_GCNO = $(LIB_OBJS:%.o=%.gcno)
LIB_GCOV = $(LIB_OBJS:l_%.o=%.c.gcov)

## main target
all: $(INFO_FILE)

## not main, still popular target#1
html: $(HTML_DIR)/index.html

$(HTML_DIR)/index.html: $(INFO_FILE)
	$(GENHTML) $< --branch-coverage --output-directory $(HTML_DIR)

## not main, still popular target#2
gcov: $(LIB_GCOV)

%.c.gcov: l_%.gcda
	$(GCOV) -abcfu -o $< $(@:.gcov=)

$(INFO_FILE): $(LIB_GCDA)
	$(LCOV) --rc lcov_branch_coverage=1 -c --directory . --output-file $@

%.gcda: $(TEST_X0_PROGS) $(TEST_X1_PROGS)
	$(foreach x,$^,./$(x);)

$(TEST_X0_PROGS): $(TEST_COMMON_OBJS)

%_test: %_test.o $(LIB_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%_test.o: $(TESTDIR)/%_test.c
	$(CC) -c $(CFLAGS_X) -o $@ $<

test_commo%.o: $(TESTDIR)/test_commo%.c
	$(CC) -c $(CFLAGS_X) -o $@ $<

l_%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

## other targets
clean:
	$(LCOV) -z --directory .
	rm -f $(INFO_FILE)
	rm -f $(LIB_OBJS)
	rm -f $(LIB_GCDA)
	rm -f $(LIB_GCNO)
	rm -f $(LIB_GCOV)
	rm -f $(TEST_X0_PROGS)
	rm -f $(TEST_X0_OBJS)
	rm -f $(TEST_X1_PROGS)
	rm -f $(TEST_X1_OBJS)
	rm -f $(TEST_COMMON_OBJS)
	rm -rf $(HTML_DIR)

.PHONY: clean html gcov

## keep everything
.PRECIOUS: $(TEST_X1_PROGS) $(TEST_X0_PROGS) $(TEST_X1_OBJS) $(TEST_X0_OBJS) $(TEST_COMMON_OBJS) $(LIB_OBJS)

