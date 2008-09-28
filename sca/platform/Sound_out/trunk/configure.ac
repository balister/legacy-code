AC_INIT(Sound_out, 0.6.0)
AM_INIT_AUTOMAKE

AC_PREFIX_DEFAULT("/sdr")

AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_HEADER_SYS_WAIT

AC_FUNC_FORK

AC_CHECK_HEADERS([pulse/simple.h], [], [AC_MSG_ERROR([cannot find pulse/simple.h])])
AC_CHECK_HEADERS([pulse/error.h], [], [AC_MSG_ERROR([cannot find pulse/error.h])])
AC_CHECK_LIB([pulse-simple], [pa_simple_new], [], [AC_MSG_ERROR([cannot find libpulse-simple])])

AC_LANG_PUSH([C++])

AC_CHECK_LIB([omniORB4], [main], [], [AC_MSG_ERROR([cannot find omniORBi4 library])])
AC_CHECK_LIB([omniDynamic4], [main], [], [AC_MSG_ERROR([cannot find omniDynamic4 library])])
AC_CHECK_HEADERS([omniORB4/CORBA.h], [], [AC_MSG_ERROR([cannot find omniORB4 header files])])

AC_CHECK_LIB([standardInterfaces], [main], [], [AC_MSG_ERROR([cannot find standardInterfaces])])
AC_CHECK_HEADERS([standardinterfaces/complexShort.h], [], [AC_MSG_ERROR([cannot find standardInterfaces header files])])

AC_LANG_POP

export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig"
PKG_CHECK_MODULES(OSSIE, ossie >= 0.6.0,,exit)
CXXFLAGS="$CXXFLAGS $OSSIE_CFLAGS"
LIBS="$LIBS $OSSIE_LIBS"
IDL_FLAGS="$OSSIE_CFLAGS"
AC_SUBST(IDL_FLAGS)

m4_pattern_allow([PKG_CONFIG_SYSROOT_DIR])
PKG_CHECK_MODULES(SI, standardInterfaces >= 0.6.0,,exit)
SI_PATH=$PKG_CONFIG_SYSROOT_DIR/`pkg-config --variable=includedir standardInterfaces`
AC_SUBST(SI_PATH)

AC_CONFIG_FILES(Makefile)

AC_OUTPUT
