# aclocal.m4 generated automatically by aclocal 1.6.2 -*- Autoconf -*-

# Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002
# Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

dnl
dnl Look for pcap headers directories
dnl
dnl usage:
dnl
dnl     AC_PCAP_HEADER_CHECK(incls)
dnl
dnl
AC_DEFUN(AC_PCAP_HEADER_CHECK,
[
        if test -z "$pcap_dir"
        then
          AC_MSG_CHECKING(for pcap header no regular directories)
          for pcap_dir in /usr/include/pcap /usr/local/include/pcap $prefix/include
          do
            if test -d $pcap_dir ; then
                p_incls="$p_incls -I$pcap_dir"
            fi
          done

          if test -z "$p_incls" ; then
            AC_MSG_RESULT(not found)
          else
            AC_MSG_RESULT(found $p_incls)
          fi
        else
          p_incls="$p_incls -I$pcap_dir/include"
        fi

        CPPFLAGS="$CPPFLAGS $p_incls"
        # Pcap header checks

        AC_CHECK_HEADER(net/bpf.h,,
            AC_MSG_ERROR([[header file net/bpf.h not found]]))
        AC_CHECK_HEADER(pcap.h,, AC_MSG_ERROR(Header file pcap.h not found.))

        $1=$p_incls
])


dnl
dnl Checks to see if the sockaddr struct has the 4.4 BSD sa_len member
dnl
dnl usage:
dnl
dnl     AC_CHECK_SA_LEN(var)
dnl
dnl results:
dnl
dnl     HAVE_SOCKADDR_SA_LEN (defined)
dnl
AC_DEFUN(AC_CHECK_SA_LEN, [
        AC_MSG_CHECKING(if sockaddr struct has sa_len member)
        AC_CACHE_VAL($1,
        AC_TRY_COMPILE([
#               include <sys/types.h>
#               include <sys/socket.h>],
                [u_int i = sizeof(((struct sockaddr *)0)->sa_len)],
                $1=yes,
                $1=no))
        AC_MSG_RESULT($$1)
                if test $$1 = yes ; then
                        AC_DEFINE(HAVE_SOCKADDR_SA_LEN,1,Define to 1 if you have 4.4BSD sa_len member in sockaddr structure")
        fi
])


