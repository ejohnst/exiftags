/*
 * Copyright (c) 2002, Eric M. Johnston <emj@postal.net>
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
 * $Id: fuji.c,v 1.1 2002/07/10 18:04:46 ejohnst Exp $
 */

/*
 * Exif tag definitions for Fuji maker notes.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "exiftags.h"
#include "exifint.h"
#include "makers.h"


/* Maker note IFD tags. */

static struct exiftag fuji_tags[] = {
	{ 0x0000, TIFF_UNDEF, 4, ED_CAM, "FujiVersion",
	  "Maker Note Version" },
	{ 0x1000, TIFF_ASCII, 8, ED_UNK, "FujiQuality",
	  "Quality Setting" },
	{ 0x1001, TIFF_SHORT, 1, ED_IMG, "FujiSharpness",
	  "Sharpness" },
	{ 0x1002, TIFF_SHORT, 1, ED_IMG, "FujiWhiteBal",
	  "White Balance" },
	{ 0x1003, TIFF_SHORT, 1, ED_IMG, "FujiColor",
	  "Chroma Saturation" },
	{ 0x1004, TIFF_SHORT, 1, ED_IMG, "FujiTone",
	  "Contrast" },
	{ 0x1010, TIFF_SHORT, 1, ED_IMG, "FujiFlashMode",
	  "Flash Mode" },
	{ 0x1011, TIFF_SRTNL, 1, ED_UNK, "FujiFlashStrength",
	  "Flash Strength" },
	{ 0x1020, TIFF_SHORT, 1, ED_IMG, "FujiMacro",
	  "Macro Mode" },
	{ 0x1021, TIFF_SHORT, 1, ED_IMG, "FujiFocusMode",
	  "Focus Mode" },
	{ 0x1030, TIFF_SHORT, 1, ED_IMG, "FujiSlowSync",
	  "Slow Synchro Mode" },
	{ 0x1031, TIFF_SHORT, 1, ED_IMG, "FujiPicMode",
	  "Picture Mode" },
	{ 0x1100, TIFF_SHORT, 1, ED_IMG, "FujiBracket",
	  "Continuous/Bracketing Mode" },
	{ 0x1300, TIFF_SHORT, 1, ED_IMG, "FujiBlurWarn",
	  "Blur Status" },
	{ 0x1301, TIFF_SHORT, 1, ED_IMG, "FujiFocusWarn",
	  "Focus Status" },
	{ 0x1302, TIFF_SHORT, 1, ED_IMG, "FujiAEWarn",
	  "Auto Exposure Status" },
	{ 0xffff, TIFF_UNKN, 0, ED_UNK, "FujiUnknown",
	  "Fuji Unknown" },
};


/* Sharpness. */

static struct descrip fuji_sharp[] = {
	{ 1,	"Soft" },
	{ 2,	"Soft" },
	{ 3,	"Normal" },
	{ 4,	"Hard" },
	{ 5,	"Hard" },
	{ -1,	"Unknown" },
};


/* White balance. */

static struct descrip fuji_white[] = {
	{ 0,	"Auto" },
	{ 256,	"Daylight" },
	{ 512,	"Cloudy" },
	{ 768,	"Daylight Color Fluorescence" },
	{ 769,	"Daywhite Color Fluorescence" },
	{ 769,	"White Fluorescence" },
	{ 1024,	"Incandescence" },
	{ 3840,	"Custom" },
	{ -1,	"Unknown" },
};


/* Color & tone settings. */

static struct descrip fuji_color[] = {
	{ 0,	"Normal" },
	{ 256,	"High" },
	{ 512,	"Low" },
	{ -1,	"Unknown" },
};


/* Flash mode. */

static struct descrip fuji_flmode[] = {
	{ 0,	"Auto" },
	{ 1,	"On" },
	{ 2,	"Off" },
	{ 3,	"Red Eye Reduction" },
	{ -1,	"Unknown" },
};


/* Generic boolean. */

static struct descrip fuji_bool[] = {
	{ 0,	"Off" },
	{ 1,	"On" },
	{ -1,	"Unknown" },
};


/* Focus mode. */

static struct descrip fuji_focus[] = {
	{ 0,	"Auto" },
	{ 1,	"Manual" },
	{ -1,	"Unknown" },
};


/* Picture mode. */

static struct descrip fuji_picture[] = {
	{ 0,	"Auto" },
	{ 1,	"Portrait" },
	{ 2,	"Landscape" },
	{ 4,	"Sports Scene" },
	{ 5,	"Night Scene" },
	{ 6,	"Program AE" },
	{ 256,	"Aperture Prior AE" },
	{ 512,	"Shutter Prior AE" },
	{ 768,	"Manual Exposure" },
	{ -1,	"Unknown" },
};


/* Blur warning. */

static struct descrip fuji_blurw[] = {
	{ 0,	"OK" },
	{ 1,	"Blur Warning" },
	{ -1,	"Unknown" },
};


/* Focus warning. */

static struct descrip fuji_focusw[] = {
	{ 0,	"OK" },
	{ 1,	"Out of Focus" },
	{ -1,	"Unknown" },
};


/* Auto exposure warning. */

static struct descrip fuji_aew[] = {
	{ 0,	"OK" },
	{ 1,	"Over Exposed" },
	{ -1,	"Unknown" },
};


/* Process Fuji maker note tags. */

void
fuji_prop(struct exifprop *prop, struct exiftags *t)
{
	int i;

	/*
	 * Don't process properties we've created while looking at other
	 * maker note tags.
	 */

	if (prop->tag == EXIF_T_UNKNOWN)
		return;

	/* Lookup the field name (if known). */

	for (i = 0; fuji_tags[i].tag < EXIF_T_UNKNOWN &&
	    fuji_tags[i].tag != prop->tag; i++);
	prop->name = fuji_tags[i].name;
	prop->descr = fuji_tags[i].descr;
	prop->lvl = fuji_tags[i].lvl;

	if (debug) {
		static int once = 0;	/* XXX Breaks on multiple files. */

		if (!once) {
			printf("Processing Fuji Maker Note\n");
			once = 1;
		}

	        for (i = 0; ftypes[i].type &&
		    ftypes[i].type != prop->type; i++);
		printf("   %s (0x%04X): %s, %d, %d\n", prop->name, prop->tag,
		    ftypes[i].name, prop->count, prop->value);
	}

	switch (prop->tag) {

	/* Maker note version. */

	case 0x0000:
		if (!(prop->str = (char *)malloc(prop->count + 1)))
			exifdie((const char *)strerror(errno));
		strncpy(prop->str, (const char*)(&prop->value), prop->count);
		prop->str[prop->count] = '\0';
		break;

	case 0x1001:
		prop->str = finddescr(fuji_sharp, prop->value);
		break;

	case 0x1002:
		prop->str = finddescr(fuji_white, prop->value);
		break;

	case 0x1003:
	case 0x1004:
		prop->str = finddescr(fuji_color, prop->value);
		break;

	case 0x1010:
		prop->str = finddescr(fuji_flmode, prop->value);
		break;

	case 0x1020:
	case 0x1030:
	case 0x1100:
		prop->str = finddescr(fuji_bool, prop->value);
		break;

	case 0x1021:
		prop->str = finddescr(fuji_focus, prop->value);
		break;

	case 0x1031:
		prop->str = finddescr(fuji_picture, prop->value);
		break;

	case 0x1300:
		prop->str = finddescr(fuji_blurw, prop->value);
		break;

	case 0x1301:
		prop->str = finddescr(fuji_focusw, prop->value);
		break;

	case 0x1302:
		prop->str = finddescr(fuji_aew, prop->value);
		break;
	}
}


/* Try to read a Fuji maker note IFD. */

struct ifd *
fuji_ifd(u_int32_t offset, struct exiftags *t)
{
	struct ifd *myifd;
	int fujilen, fujioff;

	fujilen = strlen("FUJIFILM");

	/*
	 * Seems that Fuji maker notes start with an ID string, followed by
	 * an IFD offset relative to the MakerNote tag.
	 */

	if (!strncmp(t->btiff + offset, "FUJIFILM", fujilen)) {
		fujioff = exif2byte(t->btiff + offset + fujilen, t->tifforder);
		readifd(t->btiff + offset + fujioff, &myifd, t);
	} else
		readifd(t->btiff + offset, &myifd, t);

	return (myifd);
}
