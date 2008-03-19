AC_INIT(ossie, 0.7.0.0)
AC_PREREQ(2.59)

AM_INIT_AUTOMAKE(nostdinc)

AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

AC_LANG_PUSH([C++])

AC_CHECK_LIB([omniORB4], [main], [], [AC_MSG_ERROR([cannot find omniORB library])])
AC_CHECK_HEADERS([omniORB4/CORBA.h], [], [AC_MSG_ERROR([cannot find omniORB4 header files])])
AC_CHECK_PROG([IDL], [omniidl], [omniidl], [AC_MSG_ERROR([cannot find omniidl prgram])])

AC_CHECK_HEADERS([boost/filesystem/operations.hpp], [], [AC_MSG_ERROR([Boost filesystem library headers not installed.])])
AC_CHECK_LIB([boost_filesystem], [main], [], [AC_MSG_ERROR([cannot find boost_filesystem library.])])

AC_LANG_POP

AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(time.h)
AC_HEADER_DIRENT

AC_FUNC_FORK

if test "$ac_cv_func_fork_works" != yes; then
   AC_MSG_ERROR([working fork test failed. Make sure /usr/local/lib is in /etc/ld.so.conf and run /sbin/ldconfig.])
fi

AC_CONFIG_FILES(Makefile ossie.pc idl/Makefile parser/Makefile framework/Makefile include/Makefile include/ossie/Makefile)
AC_OUTPUT
