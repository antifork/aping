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

# checking for ns hack and pthread
AC_MSG_CHECKING(whether to activate timeout on ns library)
case "$target_os" in
*linux*)
    APING_DEFS="-DNS_HACK"
    ;;
*freebsd*)
    APING_DEFS="-DNS_HACK"
    ;;
*openbsd*)
    ;;
*netbsd*|*bsdi*|*)
    ;;
esac
if test -z "$APING_DEFS" ; then
        AC_MSG_RESULT(no)
else
        AC_MSG_RESULT(yes)
fi

# Checks for libraries.
AC_CHECK_LIB([pcap], [pcap_open_live], [ have_pcap=yes ; LIBS="-lpcap $LIBS" ], have_pcap=no)
if test $have_pcap = no ; then
        AC_MSG_WARN([this os doesn't support pcap library])
        rm -Rf *.cache *.log
        exit
fi

AC_CHECK_LIB([pthread], [pthread_create], [ have_pthread=yes ; LIBS="-lpthread $LIBS" ], have_pthread=no)
AC_CHECK_LIB([c_r],     [pthread_create], [ have_libc_r=yes ;  LIBS="-pthread $LIBS" ], have_libc_r=no)
if test $have_pthread = no && test $have_libc_r = no ; then
        AC_MSG_WARN([this os doesn't support pthread])
        AC_MSG_WARN([contact the author reporting what happened.])
        rm -Rf *.cache *.log
        exit
fi

# Checks for header files.

AC_HEADER_DIRENT
AC_HEADER_STDC
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
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_CHECK_FUNCS([pcap_setnonblock pcap_getnonblock alarm atexit gethostbyaddr gethostbyname gettimeofday inet_ntoa memset socket strchr strdup strerror strstr strtol])

AC_REPLACE_FUNCS([strlcpy strlcat])

AC_CONFIG_SUBDIRS(plugin)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

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
