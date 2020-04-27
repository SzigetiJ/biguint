lib_LIBRARIES = libbiguint.a

libbiguint_a_SOURCES = biguint128.c uint.c intio.c

nodist_libbiguint_a_SOURCES = biguint256.c

CLEANFILES = biguint256.h biguint256.c

BUILT_SOURCES = biguint256.h biguint256.c

biguint256.c: biguint128.c
	$(SED) 's/128/256/g' < $< > $@

biguint256.h: biguint128.h
	$(SED) 's/128/256/g' < $< > $@

