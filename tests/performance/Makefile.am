UINT_PROGS = add_uperf
BUINT128_PROGS = \
    add_perf128 \
    mul_perf128 \
	div10_perf128 \
	div1000_perf128 \
	parse_perf128 \
	print_perf128 \
	shift_perf128

noinst_PROGRAMS = $(UINT_PROGS) $(BUINT128_PROGS)

CLEANFILES =

BUINT256_PROGS = \
    add_perf256 \
    mul_perf256 \
	div10_perf256 \
	div1000_perf256 \
	parse_perf256 \
	print_perf256 \
	shift_perf256

BUINT512_PROGS = \
    add_perf512 \
    mul_perf512 \
	div10_perf512 \
	div1000_perf512 \
	parse_perf512 \
	print_perf512 \
	shift_perf512

BUINTEXTRA_PROGS = \
    add_perf@userdef_bits@ \
    mul_perf@userdef_bits@ \
	div10_perf@userdef_bits@ \
	div1000_perf@userdef_bits@ \
	parse_perf@userdef_bits@ \
	print_perf@userdef_bits@ \
	shift_perf@userdef_bits@

if WITH_BIGUINT256
 noinst_PROGRAMS += $(BUINT256_PROGS)

 CLEANFILES += perf_common256.h \
    add_perf256.c \
    mul_perf256.c \
	div10_perf256.c \
	div1000_perf256.c \
	parse_perf256.c \
	print_perf256.c \
	shift_perf256.c

 nodist_add_perf256_SOURCES = add_perf256.c
 nodist_mul_perf256_SOURCES = mul_perf256.c
 nodist_div10_perf256_SOURCES = div10_perf256.c
 nodist_div1000_perf256_SOURCES = div1000_perf256.c
 nodist_parse_perf256_SOURCES = parse_perf256.c
 nodist_print_perf256_SOURCES = print_perf256.c
 nodist_shift_perf256_SOURCES = shift_perf256.c

 %perf256.c: %perf128.c
	$(SED) 's/128/256/g' < $< > $@

 %common256.h: %common128.h
	$(SED) 's/128/256/g' < $< > $@

endif

if WITH_BIGUINT512
 noinst_PROGRAMS += $(BUINT512_PROGS)

 CLEANFILES += perf_common512.h \
    add_perf512.c \
    mul_perf512.c \
	div10_perf512.c \
	div1000_perf512.c \
	parse_perf512.c \
	print_perf512.c \
	shift_perf512.c

 nodist_add_perf512_SOURCES = add_perf512.c
 nodist_mul_perf512_SOURCES = mul_perf512.c
 nodist_div10_perf512_SOURCES = div10_perf512.c
 nodist_div1000_perf512_SOURCES = div1000_perf512.c
 nodist_parse_perf512_SOURCES = parse_perf512.c
 nodist_print_perf512_SOURCES = print_perf512.c
 nodist_shift_perf512_SOURCES = shift_perf512.c

 %perf512.c: %perf128.c
	$(SED) 's/128/512/g' < $< > $@

 %common512.h: %common128.h
	$(SED) 's/128/512/g' < $< > $@

endif

if EXTRA_BITLEN
 noinst_PROGRAMS += $(BUINTEXTRA_PROGS)

 CLEANFILES += perf_common@userdef_bits@.h \
    add_perf@userdef_bits@.c \
    mul_perf@userdef_bits@.c \
	div10_perf@userdef_bits@.c \
	div1000_perf@userdef_bits@.c \
	parse_perf@userdef_bits@.c \
	print_perf@userdef_bits@.c \
	shift_perf@userdef_bits@.c

 nodist_add_perf@userdef_bits@_SOURCES = add_perf@userdef_bits@.c
 nodist_mul_perf@userdef_bits@_SOURCES = mul_perf@userdef_bits@.c
 nodist_div10_perf@userdef_bits@_SOURCES = div10_perf@userdef_bits@.c
 nodist_div1000_perf@userdef_bits@_SOURCES = div1000_perf@userdef_bits@.c
 nodist_parse_perf@userdef_bits@_SOURCES = parse_perf@userdef_bits@.c
 nodist_print_perf@userdef_bits@_SOURCES = print_perf@userdef_bits@.c
 nodist_shift_perf@userdef_bits@_SOURCES = shift_perf@userdef_bits@.c

 %perf@userdef_bits@.c: %perf128.c
	$(SED) 's/128/@userdef_bits@/g' < $< > $@

 %common@userdef_bits@.h: %common128.h
	$(SED) 's/128/@userdef_bits@/g' < $< > $@

endif

AM_CPPFLAGS = -I${top_srcdir}/src -I../../src
LDADD = perf_common.o ../../src/libbiguint.a

EXTRA_DIST = perf_common128.h perf_common.h perf_common.c
DISTCLEANFILES = $(DEPDIR)/perf_common.Po

BUILT_SOURCES = $(CLEANFILES)

