AC_PREREQ([2.69])
AC_INIT([biguint],[1.1.0])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_SED

AC_DEFINE(USE_STD_TYPES)

AC_CHECK_SIZEOF([UInt],,[#include "$srcdir/src/uint_types.h"])

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  tests/Makefile
  tests/performance/Makefile
  examples/Makefile
])

AC_OUTPUT
