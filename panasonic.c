/*
 * Copyright (c) 2004, Tom Hughes <tom@compton.nu>
 * Copyright (c) 2003, Eric M. Johnston <emj@postal.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Eric M. Johnston.
 * 4. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: panasonic.c,v 1.1 2004/04/20 21:51:19 ejohnst Exp $
 *
 */ 

/*
 * Exif tag definitions for Panasonic Lumix maker notes.
 * Tags deciphered by Tom Hughes <tom@compton.nu>.
 *
 * Tested models: DMC-FZ10.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "makers.h"


/* Image quality. */

static struct descrip panasonic_quality[] = {
	{ 2,	"Fine" },
	{ 3,	"Standard" },
	{ -1,	"Unknown" },
};


/* White balance. */

static struct descrip panasonic_whitebal[] = {
	{ 1,	"Auto" },
	{ 2,	"Daylight" },
	{ 3,	"Cloudy" },
	{ 4,	"Halogen" },
	{ 8,	"Flash" },
	{ 5,	"Manual" },
	{ -1,	"Unknown" },
};


/* Spot mode. */

static struct descrip panasonic_spot[] = {
	{ 256,	"On" },
	{ 4096,	"Off" },
	{ -1,	"Unknown" },
};


/* OIS mode. */

static struct descrip panasonic_ois[] = {
	{ 2,	"Mode 1" },
	{ 3,	"Off" },
	{ 4,	"Mode 2" },
	{ -1,	"Unknown" },
};


/* Macro. */

static struct descrip panasonic_macro[] = {
	{ 1,	"Macro" },
	{ 2,	"Normal" },
	{ -1,	"Unknown" },
};


/* Shooting mode. */

static struct descrip panasonic_shoot[] = {
	{ 2,	"Portrait" },
	{ 4,	"Sports" },
	{ 5,	"Night Portrait" },
	{ 6,	"Program" },
	{ 7,	"Aperture Priority" },
	{ 8,	"Shutter Priority" },
	{ 9,	"Macro" },
	{ 11,	"Manual" },
	{ 13,	"Panning" },
	{ -1,	"Unknown" },
};


/* Audio. */

static struct descrip panasonic_audio[] = {
	{ 1,	"Yes" },
	{ 2,	"No" },
	{ -1,	"Unknown" },
};


/* Flash bias. */

static struct descrip panasonic_fbias[] = {
	{ 0x0000,	"0 EV" },
	{ 0x0001,	"0.33 EV" },
	{ 0x0002,	"0.67 EV" },
	{ 0x0003,	"1.00 EV" },
	{ 0x0004,	"1.33 EV" },
	{ 0x0005,	"1.67 EV" },
	{ 0x0006,	"2.00 EV" },
	{ 0xffff,	"-0.33 EV" },
	{ 0xfffe,	"-0.67 EV" },
	{ 0xfffd,	"-1.00 EV" },
	{ 0xfffc,	"-1.33 EV" },
	{ 0xfffb,	"-1.67 EV" },
	{ 0xfffa,	"-2.00 EV" },
	{ -1,		"Unknown" },
};


/* White balance adjust. */

static struct descrip panasonic_wbadjust[] = {
	{ 0x0000,	"0" },
	{ 0x0001,	"+1" },
	{ 0x0002,	"+2" },
	{ 0x0003,	"+3" },
	{ 0x0004,	"+4" },
	{ 0x0005,	"+5" },
	{ 0xffff,	"-1" },
	{ 0xfffe,	"-2" },
	{ 0xfffd,	"-3" },
	{ 0xfffc,	"-4" },
	{ 0xfffb,	"-5" },
	{ -1,		"Unknown" },
};


/* Color effect. */

static struct descrip panasonic_color[] = {
	{ 1,	"Off" },
	{ 2,	"Warm" },
	{ 3,	"Cool" },
	{ 4,	"Black & White" },
	{ -1,	"Unknown" },
};


/* Maker note IFD tags. */

static struct exiftag panasonic_tags0[] = {
	{ 0x0001, TIFF_SHORT, 1, ED_IMG, "PanasonicQuality",
	  "Image Quality", panasonic_quality },
	{ 0x0003, TIFF_SHORT, 1, ED_IMG, "PanasonicWhiteB",
	  "White Balance", panasonic_whitebal },
	{ 0x000f, TIFF_BYTE, 1, ED_IMG, "PanasonicSpotMode",
	  "Spot Mode", panasonic_spot },
	{ 0x001a, TIFF_SHORT, 1, ED_IMG, "PanasonicOIS",
	  "OIS Mode", panasonic_ois },
	{ 0x001c, TIFF_SHORT, 1, ED_IMG, "PanasonicMacroMode",
	  "Macro Mode", panasonic_macro },
	{ 0x001f, TIFF_SHORT, 1, ED_IMG, "PanasonicShootMode",
	  "Shooting Mode", panasonic_shoot }, 
	{ 0x0020, TIFF_SHORT, 1, ED_IMG, "PanasonicAudio",
	  "Audio", panasonic_audio },
	{ 0x0023, TIFF_SHORT, 1, ED_IMG, "PanasonicWBAdjust",
	  "White Balance Adjust", NULL },
	{ 0x0024, TIFF_SSHORT, 1, ED_IMG, "PanasonicFlashBias",
	  "Flash Bias", panasonic_fbias },
	{ 0x0028, TIFF_SHORT, 1, ED_IMG, "PanasonicColorEffect",
	  "Color Effect", panasonic_color },
	{ 0xffff, TIFF_UNKN, 0, ED_UNK, "PanasonicUnknown",
	  "Panasonic Unknown", NULL },
};


/*
 * Try to read a Panasonic maker note IFD.
 */
struct ifd *
panasonic_ifd(u_int32_t offset, struct tiffmeta *md)
{
	struct ifd *myifd;

	if (memcmp("Panasonic\0\0\0", md->btiff + offset, 12)) {
		exifwarn("Maker note format not supported");
		return (NULL);
	}

	readifd(offset + 12, &myifd, panasonic_tags0, md);
        
	return (myifd);
}
