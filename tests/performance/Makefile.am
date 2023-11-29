noinst_PROGRAMS = \
	add_perf \
	uint_perf

if WITH_BIGUINT512
 noinst_PROGRAMS += parse_perf
endif


AM_CPPFLAGS = -I${top_srcdir}/src -I../../src
LDADD = ../../src/libbiguint.a

