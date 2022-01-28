lib_LIBRARIES = libbiguint.a

include_HEADERS = biguint128.h uint_types.h
nodist_include_HEADERS =

libbiguint_a_SOURCES = biguint128.c uint.c intio.c intio.h uint.h
nodist_libbiguint_a_SOURCES =
CLEANFILES =

if WITH_BIGUINT256
 nodist_include_HEADERS += biguint@bits256@.h
 nodist_libbiguint_a_SOURCES += biguint@bits256@.c
 CLEANFILES += biguint@bits256@.h biguint@bits256@.c

biguint256.c: biguint128.c
	$(SED) 's/128/256/g' < $< > $@

biguint256.h: biguint128.h
	$(SED) 's/128/256/g' < $< > $@
endif

if WITH_BIGUINT384
nodist_include_HEADERS += biguint@bits384@.h
nodist_libbiguint_a_SOURCES += biguint@bits384@.c
CLEANFILES += biguint@bits384@.h biguint@bits384@.c

biguint384.c: biguint128.c
	$(SED) 's/128/384/g' < $< > $@

biguint384.h: biguint128.h
	$(SED) 's/128/384/g' < $< > $@
endif

if WITH_BIGUINT512
 nodist_include_HEADERS += biguint@bits512@.h
 nodist_libbiguint_a_SOURCES += biguint@bits512@.c
 CLEANFILES += biguint@bits512@.h biguint@bits512@.c

biguint512.c: biguint128.c
	$(SED) 's/128/512/g' < $< > $@

biguint512.h: biguint128.h
	$(SED) 's/128/512/g' < $< > $@
endif

if EXTRA_BITLEN
 nodist_libbiguint_a_SOURCES += biguint@userdef_bits@.c
 nodist_include_HEADERS += biguint@userdef_bits@.h
 CLEANFILES += biguint@userdef_bits@.h biguint@userdef_bits@.c

biguint@userdef_bits@.c: biguint128.c
	$(SED) 's/128/@userdef_bits@/g' < $< > $@

biguint@userdef_bits@.h: biguint128.h
	$(SED) 's/128/@userdef_bits@/g' < $< > $@
endif

BUILT_SOURCES = $(CLEANFILES)

