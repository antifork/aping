# Process this file with autoconf to produce a configure script.
AC_INIT([aping],[2.0.0-ALPHA],[Bonelli Nicola <bonelli@blackhats.it>])
AC_CONFIG_SRCDIR([aping.c])
AC_CONFIG_HEADER([config.h])

AC_SUBST(APING_DEFS)
AC_SUBST(INCLS)

version=`cat ./VERSION`
echo "========================"
echo " aping $version"
echo "========================"

# Checks for programs.
AC_CANONICAL_TARGET([])
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_CHECK_PROGS(SHTOOL, shtool, ./shtool)

# Check and set OS specific parameters.

case "$target_os" in
*linux*)
    APING_DEFS="-D__USE_NSHACK"
    ;;
*freebsd*)
    APING_DEFS="-D__USE_NSHACK"
    ;;
*openbsd*)
    ;;
*)
    AC_MSG_ERROR($target_os is NOT SUPPORTED YET);
    ;;
esac

# checking for ns hack and pthread
AC_MSG_CHECKING(whether to activate timeout on ns library)

if test -z "$APING_DEFS" ; then
        AC_MSG_RESULT(no)
else
        AC_MSG_RESULT(yes)
fi

# Checks for libraries.
# Solaris

AC_CHECK_LIB([socket], [socket])
AC_CHECK_LIB([nsl], [gethostbyname])

AC_CHECK_LIB([pcap], [pcap_open_live], [ have_pcap=yes ; LIBS="-lpcap $LIBS" ], have_pcap=no)
if test $have_pcap = no ; then
	rm -Rf *.cache *.log
        AC_MSG_ERROR([this os doesn't support pcap library])
fi

AC_CHECK_LIB([pthread], [pthread_create], [ have_pthread=yes ; LIBS="-lpthread $LIBS" ], have_pthread=no)
AC_CHECK_LIB([c_r],     [pthread_create], [ have_libc_r=yes ;  LIBS="-pthread $LIBS" ], have_libc_r=no)
if test $have_pthread = no && test $have_libc_r = no ; then
	rm -Rf *.cache *.log
        AC_MSG_ERROR([this os doesn't support pthread])
fi

# Checks for header files.

AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([pcap-int.h arpa/inet.h fcntl.h netdb.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h termios.h unistd.h])

AC_PCAP_HEADER_CHECK(INCLS)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_CHECK_SA_LEN(ac_cv_sockaddr_has_sa_len)

# Checks for library functions.
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_FUNC_STRTOD
AC_CHECK_FUNCS([pcap_setnonblock pcap_getnonblock alarm atexit gethostbyaddr gethostbyname gettimeofday inet_ntoa memset socket strchr strdup strerror strstr strtol])

AC_REPLACE_FUNCS([strlcpy strlcat])

AC_CONFIG_SUBDIRS(plugin)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

ln -f -s header.h dissect.h

echo ""
echo "The aping package has been configured with the following options:"
echo "  compiler   : $CC"
echo "  CFLAGS     : $CFLAGS"
echo "  lib        : $LIBS"
echo "  defs       : $DEFS"
echo "  include    : $INCLS"
echo "  prefix     : $prefix"
echo "  exec_prefix: $exec_prefix"
echo "  bindir     : $bindir"
echo "  mandir     : $mandir"
echo "  datadir    : $datadir"
