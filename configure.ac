AC_INIT(Windstille, 0.1.0)
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/windstille_main.cxx])
AM_INIT_AUTOMAKE(dist-bzip2)

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_RANLIB
AC_LANG(C++)

dnl Checking for libraries
AC_CHECK_LIB(qthreads, main)
AC_CHECK_LIB(guile, scm_boot_guile)
AC_CHECK_HEADER(guile/gh.h)

REQUIRED_CLANLIB_VERSION="0.8.0"

PKG_CHECK_MODULES(WINDSTILLE_DEPS,
 [
  clanCore-0.8    >= $REQUIRED_CLANLIB_VERSION
  clanApp-0.8     >= $REQUIRED_CLANLIB_VERSION
  clanDisplay-0.8 >= $REQUIRED_CLANLIB_VERSION
  clanGL-0.8      >= $REQUIRED_CLANLIB_VERSION
dnl clanSound   >= $REQUIRED_CLANLIB_VERSION
dnl clanMikMod  >= $REQUIRED_CLANLIB_VERSION
dnl clanVorbis  >= $REQUIRED_CLANLIB_VERSION
  ],
 [CXXFLAGS="$CXXFLAGS $WINDSTILLE_DEPS_CFLAGS"
  LIBS="$LIBS $WINDSTILLE_DEPS_LIBS"])

AC_CONFIG_FILES( 
    Makefile
    data/Makefile
    src/Makefile
    src/guile/Makefile
    src/editor/Makefile
)
AC_OUTPUT

dnl EOF dnl
