/*
 * Copyright (c) 2001, 2002, Eric M. Johnston <emj@postal.net>
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
 * $Id: olympus.c,v 1.1 2002/06/30 09:01:41 ejohnst Exp $
 */

/*
 * Exif tag definitions for Olympus maker notes.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "exiftags.h"
#include "exif.h"


/* Maker note IFD tags. */

static struct exiftag olympus_tags[] = {
	{ 0x0200, TIFF_LONG, 3,  ED_UNK, "OlympusShootMode",
	  "Shooting Mode" },
	{ 0x0201, TIFF_SHORT, 1,  ED_UNK, "OlympusQuality",
	  "Compression Setting" },
	{ 0x0203, TIFF_SHORT, 1, ED_UNK, "OlympusMacroMode",
	  "Macro Mode" },
	{ 0x0204, TIFF_RTNL, 1, ED_UNK, "OlympusDigiZoom",
	  "Digital Zoom" },
	{ 0x0207, TIFF_ASCII, 5,  ED_UNK, "FirmwareVer",
	  "Firmware Version" },
	{ 0x0208, TIFF_ASCII, 52, ED_UNK, "OlympusPicInfo",
	  "Picture Info" },
	{ 0x0209, TIFF_UNKN,  32, ED_UNK, "OlympusCameraID",
	  "Camera ID" },
	{ 0xffff, TIFF_UNKN,  0,  ED_UNK, "Unknown",
	  "Olympus Unknown" },
};


/* Macro mode. */

static struct descrip olympus_macro[] = {
	{ 0,	"Normal" },
	{ 1,	"Macro" },
	{ -1,	"Unknown" },
};


/* Quality. */

static struct descrip olympus_quality[] = {
	{ 1,	"SQ" },
	{ 2,	"HQ" },
	{ 3,	"SHQ" },
	{ -1,	"Unknown" },
};

#if 0
/*
 * Process maker note tag 0x0001 values.
 */

static void
canon_prop1(struct exifprop *prop, char *off)
{
	int i, j;
	u_int16_t v;
	struct exifprop *aprop;

	for (i = 0; i < (int)prop->count; i++) {
		v = exif2byte(off + i * 2);

		aprop = childprop(prop);
		aprop->value = (u_int32_t)v;

		/* Lookup property name and description. */

		for (j = 0; canon_tags1[j].tag < EXIF_T_UNKNOWN &&
		    canon_tags1[j].tag != i; j++);
		aprop->name = canon_tags1[j].name;
		aprop->descr = canon_tags1[j].descr;
		aprop->lvl = canon_tags1[j].lvl;

		/* Further process known properties. */

		switch (i) {
		case 1:
			aprop->str = finddescr(canon_macro, v);
			break;
		case 2:
			aprop->lvl = v ? ED_IMG : ED_VRB;
			if (!(aprop->str = (char *)malloc(32)))
				exifdie((const char *)strerror(errno));
			snprintf(aprop->str, 31, "%d sec", v / 10);
			aprop->str[31] = '\0';
			break;
		case 3:
			aprop->str = finddescr(canon_quality, v);
			break;
		case 4:
			aprop->str = finddescr(canon_flash, v);
			break;
		case 5:
			aprop->str = finddescr(canon_drive, v);

			/* Change "Single" to "Timed" if #2 > 0. */

			if (!v && exif2byte(off + 2 * 2))
				strcpy(aprop->str, "Timed");
			break;
		case 7:
			aprop->str = finddescr(canon_focus1, v);
			break;
		case 10:
			aprop->str = finddescr(canon_imagesz, v);
			break;
		case 11:
			aprop->str = finddescr(canon_shoot, v);
			break;
		case 12:
			aprop->lvl = v ? ED_IMG : ED_VRB;

			/*
			 * Looks like we can calculate zoom level when value
			 * is 3 (ref S110).  Calculation is (2 * #37 / #36).
			 */

			if (v == 3 && prop->count > 37) {
				if (!(aprop->str = (char *)malloc(32)))
					exifdie((const char *)strerror(errno));
				snprintf(aprop->str, 31, "x%.1f", 2 *
				    (float)exif2byte(off + 37 * 2) /
				    (float)exif2byte(off + 36 * 2));
				aprop->str[31] = '\0';
			} else
				aprop->str = finddescr(canon_dzoom, v);
			break;
		case 18:
			aprop->str = finddescr(canon_focustype, v);
			break;
		default:
			if (aprop->lvl != ED_UNK)
				break;

			if (!(aprop->str = (char *)malloc(32)))
				exifdie((const char *)strerror(errno));
			snprintf(aprop->str, 31,
			    "num %02d, val 0x%04X", i, v);
			aprop->str[31] = '\0';
			break;
		}
	}
}
#endif

/* Process Olympus maker note tags. */

void
olympus_prop(struct exifprop *prop)
{
	int i;
	char *offset;
	struct exifprop *aprop;

	/*
	 * Don't process properties we've created while looking at other
	 * maker note tags.
	 */

	if (prop->tag == EXIF_T_UNKNOWN)
		return;

	/* Lookup the field name (if known). */

	for (i = 0; olympus_tags[i].tag < EXIF_T_UNKNOWN &&
	    olympus_tags[i].tag != prop->tag; i++);
	prop->name = olympus_tags[i].name;
	prop->descr = olympus_tags[i].descr;
	prop->lvl = olympus_tags[i].lvl;

	if (debug) {
		static int once = 0;	/* XXX Breaks on multiple files. */

		if (!once) {
			printf("Processing Olympus Maker Note\n");
			once = 1;
		}

	        for (i = 0; types[i].type && types[i].type != prop->type; i++);
		printf("   %s (0x%04X): %s, %d, %d\n", prop->name, prop->tag,
		    types[i].name, prop->count, prop->value);
	}

	switch (prop->tag) {

	/* Various image data. */

	case 0x0200:
		offset = btiff + prop->value;
		printf("Shoot mode: %d, %d, %d\n", exif4byte(offset),
exif4byte(offset+4), exif4byte(offset+8));

		break;

	case 0x0201:
		prop->str = finddescr(olympus_quality, prop->value);
		break;

	/* Image number. */

	case 0x0008:
		if (!(prop->str = (char *)malloc(32)))
			exifdie((const char *)strerror(errno));
		snprintf(prop->str, 31, "%03d-%04d", prop->value / 10000,
		    prop->value % 10000);
		prop->str[31] = '\0';
		break;
	}
}


/* Try to read an Olympus maker note IFD. */

struct ifd *
olympus_ifd(u_int32_t offset)
{
	struct ifd *myifd;

	/*
	 * Seems that Olympus maker notes start with an ID string.  Therefore,
	 * try reading the IFD starting at offset + 8 ("OLYMP" + 3).
	 */

	if (!strcmp(btiff + offset, "OLYMP"))
		readifd(btiff + offset + strlen("OLYMP") + 3, &myifd);
	else
		readifd(btiff + offset, &myifd);

	return (myifd);
}
