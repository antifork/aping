/*
 * $Id$
 *
 * New aping.
 * 
 * Copyright (c) 2002 Nicola Bonelli <bonelli@antifork.org>
 *                    Roberto Ferranti <sbirish@sbirish.net>
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. 2.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * 
 */


#include "header.h"
#include "aping.h"

#include "typedef.h"
#include "prototype.h"

#include "macro.h"
#include "statistic.h"
#include "global.h"
#include "statistic.h"


#include <termios.h>
#include <unistd.h>


void
incr_tcp_tos (int delta)
{
    out_burst = 0;
    max_burst = 0;

    E (&mean_burst, 0, -1);
    lp_FIR (-1, 0);

    traffic_tos += delta;
    traffic_tos &= 0x0f;

    PUTS ("%s: avrg size %ld bytes\n", tcpip_lenght[traffic_tos].type, tcpip_lenght[traffic_tos].lenght);
}


int
getchar_raw ()
{
    int           c;
    struct termios tty,
                  otty;

    tcgetattr (0, &otty);
    tty = otty;			/* saving termios */

    tty.c_lflag &= ~(ECHO | ECHOK | ICANON);
    tty.c_cc[VTIME] = 1;

    tcsetattr (0, TCSANOW, &tty);
    c = getchar ();
    tcsetattr (0, TCSANOW, &otty);

    return (c);

}

void
keystroke ()
{
    int           c;

    while (c = getchar_raw ())

	switch (c) {

	 case 10:
	     n_pause = 0;
	     break;

	 case '0':
	     SET_VAL (detail, 0);
	     break;
	 case '1':
	     SET_VAL (detail, 1);
	     break;
	 case '2':
	     SET_VAL (detail, 2);
	     break;
	 case '3':
	     SET_VAL (detail, 3);
	     break;
	 case '4':
             SET_VAL (detail, 4);
             break;
	 case ' ':
	     n_pause ^= 1;
	     if (n_pause) {
		 PUTS ("--pause--\n");
	     }
	     break;

	 case '+':
	 case '=':
	     incr_tcp_tos (1);
	     break;
	 case '-':
	 case '_':
	     incr_tcp_tos (-1);
	     break;
	 case 'e':
             reset_stat();
	     SWITCH (endian_bug);
	     break;
	 case 'd':
	     reset_stat();
	     SWITCH (diff_ipid);
	     break;
	}

}
