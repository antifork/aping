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

/* SONAR PLUGIN */

/* local header */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define PACKAGE_INFO	"A plugin for blind users"
#include "config.h"
#include "../plugin.h"

#include "locale.h"

#ifdef HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>
#else
#ifdef HAVE_LINUX_SOUNDCARD_H
#include <linux/soundcard.h>
#else
#ifdef HAVE_SOUNDCARD_H
#include <soundcard.h>
#else
#error "FIXME: OSS support not found."
#endif
#endif
#endif

int
playsound (char *sound, int len)
{
    int           speed = 11025;
    int           channels = 1;
    int           format = AFMT_S16_LE;
    int           bit = 8;
    int           dsp = 0;

    dsp = open ("/dev/dsp", O_WRONLY);
    if (dsp == -1) {
	FATAL ("open(\"/dev/dsp\")");
    }

    if (ioctl (dsp, SNDCTL_DSP_SAMPLESIZE, &bit) == -1) {
	FATAL ("unable to set the sample size\n");
    }

    if (ioctl (dsp, SNDCTL_DSP_SETFMT, &format) == -1) {
	FATAL ("ioctl(\"SNDCTL_DSP_SETFMT\")");
    }

    if (ioctl (dsp, SNDCTL_DSP_STEREO, &channels) == -1) {
	FATAL ("ioctl(\"SNDCTL_DSP_CHANNELS\")");
    }

    if (ioctl (dsp, SNDCTL_DSP_SPEED, &speed) == -1) {
	FATAL ("ioctl(\"SNDCTL_DSP_SPEED\")");
    }

    write (dsp, sound, len);
    close (dsp);

    return 0;
}


char         *sound;
int           size;

void
sonar_exit ()
{
    printf ("[sonar::exit]\n");
    playsound (sound, size);
}


void
sonar_init ()
{

    struct stat   f_stat;
    int           fd;

    if (stat (PINGWAV, &f_stat) == -1)
	FATAL ("%s: file not found", PINGWAV);

    size = (int) f_stat.st_size;
    sound = (char *) malloc (size);

    if ((fd = open (PINGWAV, O_RDONLY)) == -1)
	FATAL ("%s: file not found", PINGWAV);

    if (read (fd, sound, size) == -1)
	FATAL ("err: read() sound error.");

    close (fd);

    printf ("[sonar::init]\n");

    playsound (sound, size);

}
