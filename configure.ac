AC_PREREQ([2.69])
AC_INIT([biguint],[1.2.1])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_SED

AC_DEFINE(USE_STD_TYPES)

## ax_gcc_builtin extracted
## snippet taken from Libgcrypt
## (slightly modified)
AC_CACHE_CHECK(for __builtin_clz,
       [buint_cv_have_builtin_clz],
       [buint_cv_have_builtin_clz=no
        AC_LINK_IFELSE([AC_LANG_PROGRAM([],
          [unsigned int x = 0; int y = __builtin_clz(x); return y;])],
          [buint_cv_have_builtin_clz=yes])])
if test "$buint_cv_have_builtin_clz" = "yes" ; then
   AC_DEFINE(HAVE_BUILTIN_CLZ, 1,
             [Defined if compiler has '__builtin_clz' intrinsic])
fi

AC_CHECK_SIZEOF([UInt],,[#include "$srcdir/src/uint_types.h"])
AS_VAR_ARITH([UINT_BITS], [$ac_cv_sizeof_UInt \* 8])

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

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  tests/Makefile
  tests/performance/Makefile
  examples/Makefile
])

AC_OUTPUT
