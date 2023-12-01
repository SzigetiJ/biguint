
noinst_PROGRAMS = \
 exprtree \
 factorial128 \
 calcdecimal128

AM_CPPFLAGS = -I${top_srcdir}/src -I../src
LDADD = ../src/libbiguint.a

CLEANFILES =

if WITH_BIGUINT256
 noinst_PROGRAMS += powers10001 powers3 factorial@bits256@ calcdecimal@bits256@
 CLEANFILES += factorial@bits256@.c calcdecimal@bits256@.c
 nodist_factorial@bits256@_SOURCES = factorial@bits256@.c
 nodist_calcdecimal@bits256@_SOURCES = calcdecimal@bits256@.c

factorial256.c: factorial128.c
	$(SED) 's/128/256/g' < $< > $@

calcdecimal256.c: calcdecimal128.c
	$(SED) 's/128/256/g' < $< > $@
endif


if WITH_BIGUINT384
 noinst_PROGRAMS +=  fibratio fibratio_dec factorial@bits384@
 CLEANFILES += factorial@bits384@.c
 nodist_factorial@bits384@_SOURCES = factorial@bits384@.c

factorial384.c: factorial128.c
	$(SED) 's/128/384/g' < $< > $@
endif

if WITH_BIGUINT512
 noinst_PROGRAMS += factorial@bits512@
 CLEANFILES += factorial@bits512@.c
 nodist_factorial@bits512@_SOURCES = factorial@bits512@.c

factorial512.c: factorial128.c
	$(SED) 's/128/512/g' < $< > $@
endif


BUILT_SOURCES = $(CLEANFILES)
