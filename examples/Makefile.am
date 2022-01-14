
bin_PROGRAMS = powers10001 powers3 fibratio \
 exprtree \
 factorial128 \
 factorial256 \
 factorial384 \
 factorial512

AM_CFLAGS = -I${top_srcdir}/src -I../src
LDADD = ../src/libbiguint.a

CLEANFILES = \
	factorial256.c \
	factorial384.c \
	factorial512.c

BUILT_SOURCES = \
	factorial256.c \
	factorial384.c \
	factorial512.c

factorial256.c: factorial128.c
	$(SED) 's/128/256/g' < $< > $@

factorial384.c: factorial128.c
	$(SED) 's/128/384/g' < $< > $@

factorial512.c: factorial128.c
	$(SED) 's/128/512/g' < $< > $@
