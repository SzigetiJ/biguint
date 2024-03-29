AC_PREREQ([2.69])
AC_INIT([biguint],[1.3.0])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_RANLIB
AC_PROG_SED

if test x$ac_cv_prog_cc_stdc != xno; then
 AC_DEFINE(USE_STD_TYPES)
fi

AC_CHECK_SIZEOF([unsigned long long])
AC_CHECK_SIZEOF([unsigned long])
AC_CHECK_SIZEOF([unsigned int])
AC_CHECK_SIZEOF([unsigned short])
AC_CHECK_SIZEOF([UInt],,[#include "$srcdir/src/uint_types.h"])

AS_VAR_ARITH([UINT_BITS], [$ac_cv_sizeof_UInt \* 8])
AM_CONDITIONAL([SIZEOF_UINT_LE_4], [test $ac_cv_sizeof_UInt -le 4])

dnl ===SECTION BITLENSKIP BEGIN===
AC_ARG_WITH([biguint256], AS_HELP_STRING([--without-biguint256], [Do not generate Biguint256 codes.]))
AC_ARG_WITH([biguint384], AS_HELP_STRING([--without-biguint384], [Do not generate Biguint384 codes.]))
AC_ARG_WITH([biguint512], AS_HELP_STRING([--without-biguint512], [Do not generate Biguint512 codes.]))

AM_CONDITIONAL([WITH_BIGUINT256], [test x$with_biguint256 != xno])
AM_CONDITIONAL([WITH_BIGUINT384], [test x$with_biguint384 != xno])
AM_CONDITIONAL([WITH_BIGUINT512], [test x$with_biguint512 != xno])

AM_COND_IF([WITH_BIGUINT256],[ AC_MSG_NOTICE([generate BigUInt256]) ])
AM_COND_IF([WITH_BIGUINT384],[ AC_MSG_NOTICE([generate BigUInt384]) ])
AM_COND_IF([WITH_BIGUINT512],[ AC_MSG_NOTICE([generate BigUInt512]) ])

AM_COND_IF([WITH_BIGUINT256],
 AC_SUBST([bits256],[256]),
 AC_SUBST([bits256],[256])
)
AM_COND_IF([WITH_BIGUINT384],
 AC_SUBST([bits384],[384]),
 AC_SUBST([bits384],[384])
)
AM_COND_IF([WITH_BIGUINT512],
 AC_SUBST([bits512],[512]),
 AC_SUBST([bits512],[512])
)
dnl ===SECTION BITLENSKIP BEGIN===

dnl ===SECTION EXTRABITLEN BEGIN===
AC_ARG_ENABLE([extrabitlen],
    AS_HELP_STRING([--enable-extrabitlen=<bits>], [Generate additional type with user-defined bitlength]))

AM_CONDITIONAL([EXTRA_BITLEN], [test x$enable_extrabitlen != x])

AM_COND_IF([EXTRA_BITLEN],
 [
  AS_VAR_ARITH([BITLEN_NUM], [$enable_extrabitlen + 0])
  AM_CONDITIONAL([NUMERIC_BITLEN], [test $enable_extrabitlen = $BITLEN_NUM])
  AM_CONDITIONAL([VALID_BITLEN], [! expr $enable_extrabitlen % $UINT_BITS])
  AM_CONDITIONAL([FORBIDDEN_BITLEN], [test $enable_extrabitlen -le 0 -o $enable_extrabitlen = 128])
 ],
 [
  AC_MSG_NOTICE([No extra type required])
  AM_CONDITIONAL([NUMERIC_BITLEN],[false])
  AM_CONDITIONAL([VALID_BITLEN],[false])
  AM_CONDITIONAL([FORBIDDEN_BITLEN],[false])
 ]
)

dnl Checking whether extrabitlen is numeric.
AM_COND_IF([EXTRA_BITLEN],
 [AM_COND_IF([NUMERIC_BITLEN],,
  [AC_MSG_ERROR(["$enable_extrabitlen" cannot be interpreted as number.])]
 )]
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

AM_COND_IF([EXTRA_BITLEN],
 AC_SUBST([userdef_bits],[$enable_extrabitlen]),
 AC_SUBST([userdef_bits],[512])
)
dnl ===SECTION EXTRABITLEN END===

dnl ===SECTION PASS-BY-VALUE BEGIN===
AC_ARG_ENABLE([pass-by-value],
    AS_HELP_STRING([--disable-pass-by-value], [Do not generate function with parameters passed by value.]))

AM_CONDITIONAL([WITHOUT_PASSBYVAL], [test "x$enable_pass_by_value" == xno])

if test "x$enable_pass_by_value" == xno; then
 AC_DEFINE([WITHOUT_PASS_BY_VALUE_FUNCTIONS])
fi
dnl ===SECTION PASS-BY-VALUE END===

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  tests/Makefile
  tests/performance/Makefile
  examples/Makefile
])

AC_OUTPUT
