noinst_PROGRAMS = \
	add_perf \
	uint_perf \
	div10_perf \
	div1000_perf \
	print_perf

CLEANFILES =

if WITH_BIGUINT512
 ADDITIONAL512 = \
	div10_@bits512@_perf \
	div1000_@bits512@_perf
 noinst_PROGRAMS += parse_perf $(ADDITIONAL512)

 nodist_div10_@bits512@_perf_SOURCES = div10_@bits512@_perf.c perf_common@bits512@.h
 nodist_div1000_@bits512@_perf_SOURCES = div1000_@bits512@_perf.c perf_common@bits512@.h

 CLEANFILES += \
	div10_@bits512@_perf.c \
	div1000_@bits512@_perf.c \
    perf_common@bits512@.h

div10_512_perf.c: div10_perf.c
	$(SED) 's/128/512/g' < $< > $@

div1000_512_perf.c: div1000_perf.c
	$(SED) 's/128/512/g' < $< > $@

perf_common512.h: perf_common128.h
	$(SED) 's/128/512/g' < $< > $@

endif

if WITH_BIGUINT256
 ADDITIONAL256 = \
	div10_@bits256@_perf \
	div1000_@bits256@_perf \
	print_@bits256@_perf

 noinst_PROGRAMS += $(ADDITIONAL256)

 nodist_div10_@bits256@_perf_SOURCES = div10_@bits256@_perf.c perf_common@bits256@.h
 nodist_div1000_@bits256@_perf_SOURCES = div1000_@bits256@_perf.c perf_common@bits256@.h
 nodist_print_@bits256@_perf_SOURCES = print_@bits256@_perf.c perf_common@bits256@.h

 CLEANFILES += \
	div10_@bits256@_perf.c \
	div1000_@bits256@_perf.c \
	print_@bits256@_perf.c \
    perf_common@bits256@.h


div10_256_perf.c: div10_perf.c
	$(SED) 's/128/256/g' < $< > $@

div1000_256_perf.c: div1000_perf.c
	$(SED) 's/128/256/g' < $< > $@

print_256_perf.c: print_perf.c
	$(SED) 's/128/256/g' < $< > $@

perf_common256.h: perf_common128.h
	$(SED) 's/128/256/g' < $< > $@

endif

AM_CPPFLAGS = -I${top_srcdir}/src -I../../src
LDADD = ../../src/libbiguint.a

EXTRA_DIST = perf_common128.h

BUILT_SOURCES = $(CLEANFILES)

