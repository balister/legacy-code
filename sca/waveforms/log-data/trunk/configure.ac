AC_INIT(Log_Data, 0.6.0)
AM_INIT_AUTOMAKE

AC_PREFIX_DEFAULT("/sdr")

AC_PROG_INSTALL
AC_PROG_MAKE_SET

export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig"
PKG_CHECK_MODULES(OSSIE, ossie >= 0.6.0,,exit)
CXXFLAGS="$CXXFLAGS $OSSIE_CFLAGS"
IDL_FLAGS="$OSSIE_CFLAGS"
AC_SUBST(IDL_FLAGS)

PKG_CHECK_MODULES(SI, standardInterfaces >= 0.6.0,,exit)
SI_PATH=`pkg-config --variable=includedir standardInterfaces`

LIBS="$LIBS $OSSIE_LIBS $SI_LIBS"

AC_SUBST(SI_PATH)

AC_CONFIG_FILES(Makefile)

AC_OUTPUT
