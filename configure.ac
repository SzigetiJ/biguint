AC_PREREQ([2.69])
AC_INIT([biguint],[1.1.0])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_SED

AC_DEFINE(USE_STD_TYPES)

AC_CHECK_SIZEOF([UInt],,[#include "$srcdir/src/uint_types.h"])
AS_VAR_ARITH([UINT_BITS], [$ac_cv_sizeof_UInt \* 8])

dnl ===SECTION EXTRABITLEN BEGIN===
AC_ARG_ENABLE([extrabitlen],
    AS_HELP_STRING([--enable-extrabitlen=<bits>], [Generate additional type with user-defined bitlength]))

AM_CONDITIONAL([EXTRA_BITLEN], [test x$enable_extrabitlen != x])

AM_COND_IF([EXTRA_BITLEN],
 [
  AM_CONDITIONAL([VALID_BITLEN], [! expr $enable_extrabitlen % $UINT_BITS])
  AM_CONDITIONAL([FORBIDDEN_BITLEN], [test $enable_extrabitlen -le 0 -o $enable_extrabitlen = 128])
 ],
 [
  AC_MSG_NOTICE([No extra type required])
  AM_CONDITIONAL([VALID_BITLEN],[false])
  AM_CONDITIONAL([FORBIDDEN_BITLEN],[false])
 ]
)

dnl Checking whether extrabitlen is allowed at all.
AM_COND_IF([EXTRA_BITLEN],
 [AM_COND_IF([FORBIDDEN_BITLEN],
  [AC_MSG_ERROR([Cannot define extrabitlen=$enable_extrabitlen: forbidden.])]
 )]
)

dnl Checking whether the BigUInt type with extrabitlen can be divided into UInt cells.
AM_COND_IF([EXTRA_BITLEN],
 [AM_COND_IF([VALID_BITLEN],
  [AC_MSG_NOTICE([$enable_extrabitlen is valid (can be divided by $UINT_BITS)])],
  [AC_MSG_ERROR([$enable_extrabitlen is invalid (cannot be divided by $UINT_BITS)])]
 )]
)

AC_SUBST([userdef_bits],[$enable_extrabitlen])
dnl ===SECTION EXTRABITLEN END===

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  tests/Makefile
  tests/performance/Makefile
  examples/Makefile
])

AC_OUTPUT
