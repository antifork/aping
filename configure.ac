# Process this file with autoconf to produce a configure script.
AC_INIT([aping],[ALPHA],[Bonelli Nicola <bonelli@blackhats.it>])
AC_CONFIG_SRCDIR([aping.c])
AC_CONFIG_HEADER([config.h])

AC_SUBST(optimize_def)
AC_SUBST(INCLS)

version=`cat ./VERSION`
echo "========================"
echo " aping $version"
echo "========================"

# Checks for programs.
AC_CANONICAL_HOST
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_CHECK_PROGS(SHTOOL, shtool, ./shtool)

# Check and set OS specific parameters.

CFLAGS=""

case "$host_os" in
#aix4.*)    optimize_def="-O2"
#           CC=${CC-"xlc_r"}
#           CPPFLAGS="-D_THREAD_SAFE $CPPFLAGS"
#bsdi3*)    CC=${CC-"shlicc2"}
#           optimize_def="-O2"
#           LIBS="-lipc $LIBS";;
#bsdi*)     optimize_def="-O2";;
openbsd*)   optimize_def="-O2"
            CPPFLAGS="-D_USE_NSHACK $CPPFLAGS"
            LIBS="-pthread";;
freebsd*)   optimize_def="-O2"
            CPPFLAGS="-D_THREAD_SAFE -D_USE_NSHACK $CPPFLAGS"
            LIBS="-pthread";;
hpux*)      CPPFLAGS="-D_REENTRANT $CPPFLAGS";;
#irix*)     optimize_def="-O2"
#           CPPFLAGS="-D_SGI_MP_SOURCE $CPPFLAGS";;
linux*)     optimize_def="-O2"
            CFLAGS="-D_GNU_SOURCE $CFLAGS"
            CPPFLAGS="-D_REENTRANT -D_USE_NSHACK $CPPFLAGS"
            LIBS="-lpthread";;
#mpeix*)    CPPFLAGS="-D_POSIX_SOURCE -D_SOCKET_SOURCE $CPPFLAGS"
#           LIBS="-lsocket -lsvipc $LIBS";;
#osf*)      CPPFLAGS="-D_REENTRANT $CPPFLAGS";;
#sco3.2v4*) CC=${CC-"cc -belf"}
#           LIBS="-lsocket -lnsl_s $LIBS";;
#sco*)      CC=${CC-"cc -belf"}
#           LIBS="-lsocket -lnsl $LIBS";;
solaris*)   CPPFLAGS="-D_REENTRANT -D_PTHREADS $CPPFLAGS";;
*) 	    AC_MSG_ERROR($target_os is not supported yet);;
esac

# Checks for libraries.

AC_CHECK_LIB([pcap], [pcap_open_live], [ have_pcap=yes ; LIBS="-lpcap $LIBS" ], have_pcap=no)
if test $have_pcap = no ; then
	rm -Rf *.cache *.log
        AC_MSG_ERROR([this os doesn't support pcap library])
fi

# Checks for header files.

AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_PCAP_HEADER_CHECK(INCLS)
AC_CHECK_HEADERS([pcap-int.h arpa/inet.h fcntl.h netdb.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h termios.h unistd.h])
INCLS="-I. $INCLS"

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
echo "The aping package has been configured with the following options:"
echo "  compiler      : $CC"
echo "  optimize_def  : $optimize_def"
echo "  host_cpu      : $host_cpu"
echo "  host_vendor   : $host_vendor"
echo "  host_os       : $host_os"
echo "  CFLAGS        : $CFLAGS"
echo "  CPPFLAGS      : $CPPFLAGS"
echo "  lib           : $LIBS"
echo "  defs          : $DEFS"
echo "  include       : $INCLS"
echo "  prefix        : $prefix"
echo "  exec_prefix   : $exec_prefix"
echo "  bindir        : $bindir"
echo "  mandir        : $mandir"
echo "  datadir       : $datadir"
