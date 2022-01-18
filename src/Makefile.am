lib_LIBRARIES = libbiguint.a

include_HEADERS = biguint128.h biguint256.h biguint384.h biguint512.h uint_types.h

libbiguint_a_SOURCES = biguint128.c uint.c intio.c intio.h uint.h

nodist_libbiguint_a_SOURCES = biguint256.c biguint384.c biguint512.c

CLEANFILES = \
 biguint256.h biguint256.c \
 biguint384.h biguint384.c \
 biguint512.h biguint512.c

BUILT_SOURCES = $(CLEANFILES)

biguint256.c: biguint128.c
	$(SED) 's/128/256/g' < $< > $@

biguint256.h: biguint128.h
	$(SED) 's/128/256/g' < $< > $@

biguint384.c: biguint128.c
	$(SED) 's/128/384/g' < $< > $@

biguint384.h: biguint128.h
	$(SED) 's/128/384/g' < $< > $@

biguint512.c: biguint128.c
	$(SED) 's/128/512/g' < $< > $@

biguint512.h: biguint128.h
	$(SED) 's/128/512/g' < $< > $@

if EXTRA_BITLEN
 nodist_libbiguint_a_SOURCES += biguint@userdef_bits@.c
 include_HEADERS += biguint@userdef_bits@.h
 CLEANFILES += \
  biguint@userdef_bits@.h biguint@userdef_bits@.c
 BUILT_SOURCES += \
  biguint@userdef_bits@.h biguint@userdef_bits@.c

biguint@userdef_bits@.c: biguint128.c
	$(SED) 's/128/@userdef_bits@/g' < $< > $@

biguint@userdef_bits@.h: biguint128.h
	$(SED) 's/128/@userdef_bits@/g' < $< > $@
endif

