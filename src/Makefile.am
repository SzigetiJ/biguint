lib_LIBRARIES = libbiguint.a

include_HEADERS = biguint128.h bigdecimal128.h uint_types.h
nodist_include_HEADERS =

libbiguint_a_SOURCES = biguint128.c bigdecimal128.c uint.c intio.c intio.h uint.h
nodist_libbiguint_a_SOURCES =
CLEANFILES =

if WITH_BIGUINT256
 nodist_include_HEADERS += biguint@bits256@.h bigdecimal@bits256@.h
 nodist_libbiguint_a_SOURCES += biguint@bits256@.c bigdecimal@bits256@.c
 CLEANFILES += biguint@bits256@.h biguint@bits256@.c bigdecimal@bits256@.h bigdecimal@bits256@.c

big%256.c: big%128.c
	$(SED) 's/128/256/g' < $< > $@

big%256.h: big%128.h
	$(SED) 's/128/256/g' < $< > $@
endif

if WITH_BIGUINT384
nodist_include_HEADERS += biguint@bits384@.h bigdecimal@bits384@.h
nodist_libbiguint_a_SOURCES += biguint@bits384@.c bigdecimal@bits384@.c
CLEANFILES += biguint@bits384@.h biguint@bits384@.c bigdecimal@bits384@.h bigdecimal@bits384@.c

big%384.c: big%128.c
	$(SED) 's/128/384/g' < $< > $@

big%384.h: big%128.h
	$(SED) 's/128/384/g' < $< > $@
endif

if WITH_BIGUINT512
 nodist_include_HEADERS += biguint@bits512@.h bigdecimal@bits512@.h
 nodist_libbiguint_a_SOURCES += biguint@bits512@.c bigdecimal@bits512@.c
 CLEANFILES += biguint@bits512@.h biguint@bits512@.c bigdecimal@bits512@.h bigdecimal@bits512@.c

big%512.c: big%128.c
	$(SED) 's/128/512/g' < $< > $@

big%512.h: big%128.h
	$(SED) 's/128/512/g' < $< > $@
endif

if EXTRA_BITLEN
 nodist_libbiguint_a_SOURCES += biguint@userdef_bits@.c bigdecimal@userdef_bits@.c
 nodist_include_HEADERS += biguint@userdef_bits@.h bigdecimal@userdef_bits@.h
 CLEANFILES += biguint@userdef_bits@.h biguint@userdef_bits@.c bigdecimal@userdef_bits@.h bigdecimal@userdef_bits@.c

big%@userdef_bits@.c: big%128.c
	$(SED) 's/128/@userdef_bits@/g' < $< > $@

big%@userdef_bits@.h: big%128.h
	$(SED) 's/128/@userdef_bits@/g' < $< > $@
endif

BUILT_SOURCES = $(CLEANFILES)

