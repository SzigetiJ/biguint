UINT_PROGS = add_uperf
BUINT128_PROGS = \
    add_perf128 \
	div10_perf128 \
	div1000_perf128 \
	parse_perf128 \
	print_perf128

noinst_PROGRAMS = $(UINT_PROGS) $(BUINT128_PROGS)

CLEANFILES =

BUINT512_PROGS = \
    add_perf512 \
	div10_perf512 \
	div1000_perf512 \
	parse_perf512 \
	print_perf512


if WITH_BIGUINT512
 noinst_PROGRAMS += $(BUINT512_PROGS)

 CLEANFILES += perf_common512.h \
    add_perf512.c \
	div10_perf512.c \
	div1000_perf512.c \
	parse_perf512.c \
	print_perf512.c

 nodist_add_perf512_SOURCES = add_perf512.c
 nodist_div10_perf512_SOURCES = div10_perf512.c
 nodist_div1000_perf512_SOURCES = div1000_perf512.c
 nodist_parse_perf512_SOURCES = parse_perf512.c
 nodist_print_perf512_SOURCES = print_perf512.c

%perf512.c: %perf128.c
	$(SED) 's/128/512/g' < $< > $@

%common512.h: %common128.h
	$(SED) 's/128/512/g' < $< > $@

endif

AM_CPPFLAGS = -I${top_srcdir}/src -I../../src
LDADD = ../../src/libbiguint.a

EXTRA_DIST = perf_common128.h

BUILT_SOURCES = $(CLEANFILES)

