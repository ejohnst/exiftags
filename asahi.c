/*
 * Copyright (c) 2004 Eric M. Johnston <emj@postal.net>
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
 * $Id: asahi.c,v 1.1 2004/02/27 19:16:57 ejohnst Exp $
 */

/*
 * Exif tag definitions for Asahi Optical Co. (Pentax) maker notes.
 * Note that they're basically the same as for Casio.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "makers.h"


/* Recording mode. */

static struct descrip asahi_record[] = {
	{ 1,	"Single Shutter" },
	{ 7,	"Panorama" },
	{ 10,	"Night Scene" },
	{ 15,	"Portrait" },
	{ 16,	"Landscape" },
	{ -1,	"Unknown" },
};


/* Quality. */

static struct descrip asahi_qual[] = {
	{ 1,	"Economy" },
	{ 2,	"Normal" },
	{ 3,	"Fine" },
	{ -1,	"Unknown" },
};


/* Focus mode. */

static struct descrip asahi_focus[] = {
	{ 2,	"Macro" },
	{ 3,	"Auto" },
	{ 4,	"Manual" },
	{ 5,	"Infinity" },
	{ -1,	"Unknown" },
};


/* Flash mode. */

static struct descrip asahi_flash[] = {
	{ 1,	"Auto" },
	{ 2,	"On" },
	{ 4,	"Off" },
	{ 5,	"Red Eye Reduction" },
	{ -1,	"Unknown" },
};


/* Flash intensity. */

static struct descrip asahi_intense[] = {
	{ 11,	"Weak" },
	{ 13,	"Normal" },
	{ 15,	"Strong" },
	{ -1,	"Unknown" },
};


/* White balance. */

static struct descrip asahi_whiteb[] = {
	{ 1,	"Auto" },
	{ 2,	"Tungsten" },
	{ 3,	"Daylight" },
	{ 4,	"Fluorescent" },
	{ 5,	"Shade" },
	{ 129,	"Manual" },
	{ -1,	"Unknown" },
};


/* Sharpness. */

static struct descrip asahi_sharp[] = {
	{ 0,	"Normal" },
	{ 1,	"Soft" },
	{ 2,	"Hard" },
	{ -1,	"Unknown" },
};


/* Contrast & saturation. */

static struct descrip asahi_range[] = {
	{ 0,	"Normal" },
	{ 1,	"Low" },
	{ 2,	"High" },
	{ -1,	"Unknown" },
};


/* Sensitivity. */

static struct descrip asahi_sensitive[] = {
	{ 64,	"Normal" },
	{ 80,	"Normal" },
	{ 100,	"High" },
	{ 125,	"+1.0" },
	{ 244,	"+3.0" },
	{ 250,	"+2.0" },
	{ -1,	"Unknown" },
};


/* Maker note IFD tags. */

static struct exiftag asahi_tags[] = {
	{ 0x0001, TIFF_SHORT, 1, ED_IMG, "AsahiRecord",
	  "Recording Mode", asahi_record },
	{ 0x0002, TIFF_SHORT, 1, ED_IMG, "AsahiQuality",
	  "Quality Setting", asahi_qual },
	{ 0x0003, TIFF_SHORT, 1, ED_IMG, "AsahiFocus",
	  "Focusing Mode", asahi_focus },
	{ 0x0004, TIFF_SHORT, 1, ED_IMG, "AsahiFlash",
	  "Flash Mode", asahi_flash },
	{ 0x0005, TIFF_SHORT, 1, ED_IMG, "AsahiIntensity",
	  "Flash Intensity", asahi_intense },
	{ 0x0006, TIFF_LONG, 1, ED_VRB, "AsahiDistance",
	  "Object Distance", NULL },
	{ 0x0007, TIFF_SHORT, 1, ED_IMG, "AsahiWhiteB",
	  "White Balance", asahi_whiteb },
	{ 0x000a, TIFF_LONG, 1, ED_UNK, "AsahiDZoom",
	  "Digital Zoom", NULL },
	{ 0x000b, TIFF_SHORT, 1, ED_IMG, "AsahiSharp",
	  "Sharpness", asahi_sharp },
	{ 0x000c, TIFF_SHORT, 1, ED_IMG, "AsahiContrast",
	  "Contrast", asahi_range },
	{ 0x000d, TIFF_SHORT, 1, ED_IMG, "AsahiSaturate",
	  "Saturation", asahi_range },
	{ 0x0014, TIFF_SHORT, 1, ED_IMG, "AsahiSensitive",
	  "Sensitivity", asahi_sensitive },
	{ 0xffff, TIFF_UNKN, 0, ED_UNK, "AsahiUnknown",
	  "Asahi Unknown", NULL },
};


/*
 * Try to read an Asahi maker note IFD.
 */
struct ifd *
asahi_ifd(u_int32_t offset, struct tiffmeta *md)
{
	struct ifd *myifd;

	/*
	 * It appears that there are two different types of maker notes
	 * for Asahi cameras: one, for older cameras, uses a standard IFD
	 * format; the other starts at offset + 6 ("AOC\0\0\0").
	 */

	if (!memcmp("AOC\0\0\0", md->btiff + offset, 6)) {
		readifd(offset + strlen("AOC") + 3, &myifd, asahi_tags, md);
	} else {
		readifd(offset, &myifd, asahi_tags, md);
		exifwarn("Asahi maker note version not supported");
	}

	return (myifd);
}
