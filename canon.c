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
 * $Id: canon.c,v 1.3 2002/06/30 08:47:22 ejohnst Exp $
 */

/*
 * Exif tag definitions for Canon maker notes.
 * Developed from http://www.burren.cx/david/canon.html.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "exifdump.h"
#include "exif.h"


/* Maker note IFD tags. */

static struct exiftag canon_tags[] = {
	{ 0x0001, TIFF_SHORT, 0,  ED_VRB, "Canon1Tag", "Canon Tag1 Offset" },
	{ 0x0004, TIFF_SHORT, 0,  ED_VRB, "Canon4Tag", "Canon Tag4 Offset" },
	{ 0x0006, TIFF_ASCII, 32, ED_VRB, "ImageType", "Image Type" },
	{ 0x0007, TIFF_ASCII, 24, ED_CAM, "FirmwareVer", "Firmware Version" },
	{ 0x0008, TIFF_LONG,  1,  ED_IMG, "ImgNum", "Image Number" },
	{ 0x0009, TIFF_ASCII, 32, ED_CAM, "OwnerName", "Owner Name" },
	{ 0x000c, TIFF_LONG,  1,  ED_CAM, "Serial", "Serial Number" },
	{ 0xffff, TIFF_UNKN,  0,  ED_UNK, "Unknown", "Canon Unknown" },
};


/* Fields under tag 0x0001. */

static struct exiftag canon_tags1[] = {
	{ 0,  TIFF_SHORT, 0, ED_VRB, "Canon1Len", "Canon Tag1 Length" },
	{ 1,  TIFF_SHORT, 0, ED_IMG, "CanonMacroMode", "Macro Mode" },
	{ 2,  TIFF_SHORT, 0, ED_VRB, "CanonTimerLen", "Self-Timer Length" },
	{ 3,  TIFF_SHORT, 0, ED_IMG, "CanonQuality", "Compression Setting" },
	{ 4,  TIFF_SHORT, 0, ED_IMG, "CanonFlashMode", "Flash Mode" },
	{ 5,  TIFF_SHORT, 0, ED_IMG, "CanonDriveMode", "Drive Mode" },
	{ 7,  TIFF_SHORT, 0, ED_IMG, "CanonFocusMode", "Focus Mode" },
	{ 10, TIFF_SHORT, 0, ED_IMG, "CanonImageSize", "Image Size" },
	{ 11, TIFF_SHORT, 0, ED_IMG, "CanonShootMode", "Shooting Mode" },
	{ 12, TIFF_SHORT, 0, ED_VRB, "CanonDigiZoom", "Digital Zoom" },
	{ 18, TIFF_SHORT, 0, ED_IMG, "CanonFocusType", "Focus Type" },
	{ 36, TIFF_SHORT, 0, ED_VRB, "CanonDZoomRes", "Zoomed Resolution" },
	{ 37, TIFF_SHORT, 0, ED_VRB, "CanonBZoomRes", "Base Zoom Resolution" },
	{ 0xffff, TIFF_SHORT, 0, ED_UNK, "CanonUnknown", "Canon Tag1 Unknown" },
};


/* Fields under tag 0x0004. */

static struct exiftag canon_tags4[] = {
	{ 0,  TIFF_SHORT, 0, ED_VRB, "Canon4Len", "Canon Tag4 Length" },
	{ 7,  TIFF_SHORT, 0, ED_IMG, "CanonWhiteB", "White Balance" },
	{ 9,  TIFF_SHORT, 0, ED_IMG, "CanonSequence", "Sequence Number" },
	{ 0xffff, TIFF_SHORT, 0, ED_UNK, "CanonUnknown", "Canon Tag4 Unknown" },
};


/* Macro mode. */

static struct descrip canon_macro[] = {
	{ 1,	"Macro" },
	{ 2,	"Normal" },
	{ -1,	"Unknown" },
};


/* Focus type. */

static struct descrip canon_focustype[] = {
	{ 0,	"Manual" },
	{ 1,	"Auto" },
	{ 3,	"Close-Up (Macro Mode)" },
	{ 7,	"Infinity Mode" },
	{ 8,	"Locked (Pan Mode)" },
	{ -1,	"Unknown" },
};


/* Quality. */

static struct descrip canon_quality[] = {
	{ 2,	"Normal" },
	{ 3,	"Fine" },
	{ 5,	"Superfine" },
	{ -1,	"Unknown" },
};


/* Flash mode. */

static struct descrip canon_flash[] = {
	{ 0,	"Off" },
	{ 1,	"Auto" },
	{ 2,	"On" },
	{ 3,	"Red-Eye Reduction" },
	{ 4,	"Slow-Synchro" },
	{ 5,	"Red-Eye Reduction (Auto)" },
	{ 6,	"Red-Eye Reduction (On)" },
	{ 16,	"External Flash" },
	{ -1,	"Unknown" },
};


/* Drive mode. */

static struct descrip canon_drive[] = {
	{ 0,	"Single" },		/* "Timed" when field 2 is > 0. */
	{ 1,	"Continuous" },
	{ -1,	"Unknown" },
};


/* Focus mode. */

static struct descrip canon_focus1[] = {
	{ 0,	"One-Shot" },
	{ 1,	"AI Servo" },
	{ 2,	"AI Focus" },
	{ 3,	"MF" },
	{ 4,	"Single" },
	{ 5,	"Continuous" },
	{ 6,	"MF" },
	{ -1,	"Unknown" },
};


/* Image size. */

static struct descrip canon_imagesz[] = {
	{ 0,	"Large" },
	{ 1,	"Medium" },
	{ 2,	"Small" },
	{ -1,	"Unknown" },
};


/* Shooting mode. */

static struct descrip canon_shoot[] = {
	{ 0,	"Full Auto" },
	{ 1,	"Manual" },
	{ 2,	"Landscape" },
	{ 3,	"Fast Shutter" },
	{ 4,	"Slow Shutter" },
	{ 5,	"Night" },
	{ 6,	"Black & White" },
	{ 7,	"Sepia" },
	{ 8,	"Portrait" },
	{ 9,	"Sports" },
	{ 10,	"Macro/Close-Up" },
	{ 11,	"Pan Focus" },
	{ -1,	"Unknown" },
};


/* Digital zoom. */

static struct descrip canon_dzoom[] = {
	{ 0,	"None" },
	{ 1,	"x2" },
	{ 2,	"x4" },
	{ -1,	"Unknown" },
};


/* White balance. */

static struct descrip canon_whitebal[] = {
	{ 0,	"Auto" },
	{ 1,	"Daylight" },
	{ 2,	"Cloudy" },
	{ 3,	"Tungsten" },
	{ 4,	"Fluorescent" },
	{ 5,	"Flash" },
	{ 6,	"Custom" },
	{ 7,	"Black & White" },
	{ -1,	"Unknown" },
};


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


/*
 * Process maker note tag 0x0004 values.
 */

static void
canon_prop4(struct exifprop *prop, char *off)
{
	int i, j;
	u_int16_t v;
	struct exifprop *aprop;

	for (i = 0; i < (int)prop->count; i++) {
		v = exif2byte(off + i * 2);

		aprop = childprop(prop);
		aprop->value = (u_int32_t)v;

		/* Lookup property name and description. */

		for (j = 0; canon_tags4[j].tag < EXIF_T_UNKNOWN &&
		    canon_tags4[j].tag != i; j++);
		aprop->name = canon_tags4[j].name;
		aprop->descr = canon_tags4[j].descr;
		aprop->lvl = canon_tags4[j].lvl;

		/* Further process known properties. */

		switch (i) {
		case 7:
			aprop->str = finddescr(canon_whitebal, v);
			break;
		case 9:
			aprop->lvl = v ? ED_IMG : ED_VRB;
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


/* Process Canon maker note tags. */

void
canon_prop(struct exifprop *prop)
{
	int i;
	char *offset;

	/*
	 * Don't process properties we've created while looking at other
	 * maker note tags.
	 */

	if (prop->tag == EXIF_T_UNKNOWN)
		return;

	/* Lookup the field name (if known). */

	for (i = 0; canon_tags[i].tag < EXIF_T_UNKNOWN &&
	    canon_tags[i].tag != prop->tag; i++);
	prop->name = canon_tags[i].name;
	prop->descr = canon_tags[i].descr;
	prop->lvl = canon_tags[i].lvl;

	if (debug) {
		static int once = 0;

		if (!once) {
			printf("Processing Canon Maker Note\n");
			once = 1;
		}

	        for (i = 0; types[i].type && types[i].type != prop->type; i++);
		printf("   %s (0x%04X): %s, %d, %d\n", prop->name, prop->tag,
		    types[i].name, prop->count, prop->value);
	}

	switch (prop->tag) {

	/* Various image data. */

	case 0x0001:
		offset = btiff + prop->value;
		if (exif2byte(offset) != 2 * prop->count) {
			exifwarn("Canon image prop 0x0001 length mismatch");
			break;
		}

		canon_prop1(prop, offset);
		break;

	case 0x0004:
		offset = btiff + prop->value;
		if (exif2byte(offset) != 2 * prop->count) {
			exifwarn("Canon image prop 0x0004 length mismatch");
			break;
		}

		canon_prop4(prop, offset);
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
