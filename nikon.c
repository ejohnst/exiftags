/*
 * Copyright (c) 2001-2003, Eric M. Johnston <emj@postal.net>
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
 * $Id: nikon.c,v 1.18 2004/10/10 03:48:38 ejohnst Exp $
 */

/*
 * Exif tag definitions for Nikon maker notes.
 *
 * Some information for Nikon D1X support obtained from JoJoThumb, version
 * 2.7.2 (http://www.jojosoftware.de/jojothumb/).
 *
 * Updated with data from
 * http://www.ozhiker.com/electronics/pjmt/jpeg_info/nikon_mn.html.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "makers.h"


/* Quality. */

static struct descrip nikon_quality[] = {
	{ 1,	"VGA Basic" },
	{ 2,	"VGA Normal" },
	{ 3,	"VGA Fine" },
	{ 4,	"SXGA Basic" },
	{ 5,	"SXGA Normal" },
	{ 6,	"SXGA Fine" },
	{ 10,	"UXGA Basic" },
	{ 11,	"UXGA Normal" },
	{ 12,	"UXGA Fine" },
	{ -1,	"Unknown" },
};


/* Color. */

static struct descrip nikon_color[] = {
	{ 1,	"Color" },
	{ 2,	"Monochrome" },
	{ -1,	"Unknown" },
};


/* Image adjustment. */

static struct descrip nikon_adjust[] = {
	{ 0,	"Normal" },
	{ 1,	"Bright(+)" },
	{ 2,	"Bright(-)" },
	{ 3,	"Contrast(+)" },
	{ 4,	"Contrast(-)" },
	{ -1,	"Unknown" },
};


/* CCD sensitivity. */

static struct descrip nikon_ccd[] = {
	{ 0,	"ISO 80" },
	{ 2,	"ISO 160" },
	{ 4,	"ISO 320" },
	{ 5,	"ISO 100" },
	{ -1,	"Unknown" },
};


/* White balance. */

static struct descrip nikon_white[] = {
	{ 0,	"Auto" },
	{ 1,	"Preset" },
	{ 2,	"Daylight" },
	{ 3,	"Incandescent" },
	{ 4,	"Fluorescent" },
	{ 5,	"Cloudy" },
	{ 6,	"Speedlight" },
	{ -1,	"Unknown" },
};


/* Converter. */

static struct descrip nikon_convert[] = {
	{ 0,	"None" },
	{ 2,	"Fisheye" },
	{ -1,	"Unknown" },
};


/* Old school Nikon "lookup" maker note IFD tags. */

static struct exiftag nikon_tags0[] = {
	{ 0x0003, TIFF_SHORT, 1, ED_IMG, "NikonQuality",
	  "Image Quality", nikon_quality },
	{ 0x0004, TIFF_SHORT, 1, ED_IMG, "NikonColor",
	  "Color Mode", nikon_color },
	{ 0x0005, TIFF_SHORT, 1, ED_IMG, "NikonImgAdjust",
	  "Image Adjustment", nikon_adjust },
	{ 0x0006, TIFF_SHORT, 1, ED_IMG, "NikonCCDSensitive",
	  "CCD Sensitivity", nikon_ccd },
	{ 0x0007, TIFF_SHORT, 1, ED_IMG, "NikonWhiteBal",
	  "White Balance", nikon_white },
	{ 0x0008, TIFF_RTNL, 1, ED_UNK, "NikonFocus",
	  "Focus", NULL },
	{ 0x000a, TIFF_RTNL, 1, ED_IMG, "NikonDigiZoom",
	  "Digital Zoom", NULL },
	{ 0x000b, TIFF_SHORT, 1, ED_IMG, "NikonAdapter",
	  "Lens Adapter", nikon_convert },
	{ 0xffff, TIFF_UNKN, 0, ED_UNK, "Unknown",
	  "Nikon Unknown", NULL },
};


/* Newer Nikon ASCII maker note IFD tags. */

static struct exiftag nikon_tags1[] = {
	{ 0x0001, TIFF_UNDEF, 4, ED_VRB, "NikonVersion",
	  "Nikon Note Version", NULL },
	{ 0x0002, TIFF_SHORT, 2, ED_UNK, "NikonISOUsed",
	  "ISO Speed Used", NULL },
	{ 0x0003, TIFF_ASCII, 0, ED_IMG, "NikonColor",
	  "Color Mode", NULL },
	{ 0x0004, TIFF_ASCII, 0, ED_IMG, "NikonQuality",
	  "Image Quality", NULL },
	{ 0x0005, TIFF_ASCII, 0, ED_IMG, "NikonWhiteBal",
	  "White Balance", NULL },
	{ 0x0006, TIFF_ASCII, 0, ED_IMG, "NikonImgSharp",
	  "Image Sharpening", NULL },
	{ 0x0007, TIFF_ASCII, 0, ED_IMG, "NikonFocusMode",
	  "Focus Mode", NULL },
	{ 0x0008, TIFF_ASCII, 0, ED_IMG, "NikonFlashSet",
	  "Flash Setting", NULL },
	{ 0x0009, TIFF_ASCII, 0, ED_IMG, "NikonAutoFlash",
	  "Auto Flash Mode", NULL },
	{ 0x000b, TIFF_UNKN, 0, ED_UNK, "NikonWhiteBalBias",
	  "White Balance Bias", NULL },
	{ 0x000f, TIFF_ASCII, 0, ED_IMG, "NikonISOSelect",
	  "ISO Selection", NULL },
	{ 0x0012, TIFF_SHORT, 1, ED_IMG, "NikonFlashComp",
	  "Flash Compensation", NULL },
	{ 0x0013, TIFF_SHORT, 2, ED_IMG, "NikonISOReq",
	  "ISO Speed Requested", NULL },
	{ 0x0018, TIFF_SHORT, 1, ED_IMG, "NikonFlashBrackComp",
	  "Flash Bracket Compensation", NULL },
	{ 0x0019, TIFF_SHORT, 1, ED_IMG, "NikonAEBrackComp",
	  "AE Bracket Compensation", NULL },
	{ 0x0080, TIFF_ASCII, 0, ED_IMG, "NikonImgAdjust",
	  "Image Adjustment", NULL },
	{ 0x0081, TIFF_ASCII, 0, ED_IMG, "NikonToneComp",
	  "Tone Compensation", NULL },
	{ 0x0082, TIFF_ASCII, 0, ED_IMG, "NikonLensAdapter",
	  "Lens Adapter", NULL },
	{ 0x0083, TIFF_ASCII, 0, ED_IMG, "NikonLensType",
	  "Lens Type", NULL },
	{ 0x0084, TIFF_ASCII, 0, ED_IMG, "NikonLensRange",
	  "Lens Range", NULL },
	{ 0x0085, TIFF_ASCII, 0, ED_IMG, "NikonFocusDist",
	  "Focus Distance", NULL },
	{ 0x0086, TIFF_ASCII, 0, ED_IMG, "NikonDigiZoom",
	  "Digital Zoom", NULL },
	{ 0x0087, TIFF_SHORT, 1, ED_IMG, "NikonFlashUsed",
	  "Flash Used", NULL },
	{ 0x0088, TIFF_SHORT, 1, ED_IMG, "NikonAutoFocus",
	  "Auto Focus Position", NULL },
	{ 0x0089, TIFF_SHORT, 1, ED_IMG, "NikonBrackShoot",
	  "Bracketing/Shooting Mode", NULL },
	{ 0x008d, TIFF_ASCII, 0, ED_IMG, "NikonColorMode",
	  "Color Mode 2", NULL },
	{ 0x008f, TIFF_ASCII, 0, ED_IMG, "NikonLighting",
	  "Lighting Type", NULL },
	{ 0x0092, TIFF_UNDEF, 0, ED_UNK, "NikonHueAdjust",
	  "Hue Adjustment", NULL },
	{ 0x0094, TIFF_SHORT, 1, ED_IMG, "NikonSaturate",
	  "Saturation", NULL },
	{ 0x0095, TIFF_ASCII, 0, ED_IMG, "NikonNoiseReduce",
	  "Noise Reduction", NULL },
	{ 0x00a7, TIFF_SHORT, 1, ED_IMG, "NikonAcuations",
	  "Camera Actuations", NULL },
	{ 0x00a9, TIFF_ASCII, 0, ED_IMG, "NikonImageOpt",
	  "Image Optimization", NULL },
	{ 0x00aa, TIFF_ASCII, 0, ED_IMG, "NikonSaturate2",
	  "Saturation 2", NULL },
	{ 0x00ab, TIFF_ASCII, 0, ED_IMG, "NikonDigiProg",
	  "Digital Vari-Program", NULL },
	{ 0xffff, TIFF_UNKN, 0, ED_UNK, "Unknown",
	  "Nikon Unknown", NULL },
};


/*
 * Process older Nikon maker note tags.
 */
static void
nikon_prop0(struct exifprop *prop, struct exiftags *t)
{
	u_int32_t a, b;

	switch (prop->tag) {

	/* Digital zoom. */

	case 0x000a:
		a = exif4byte(t->md.btiff + prop->value, t->md.order);
		b = exif4byte(t->md.btiff + prop->value + 4, t->md.order);

		if (!a) {
			snprintf(prop->str, 31, "None");
			prop->lvl = ED_VRB;
		} else
			snprintf(prop->str, 31, "x%.1f", (float)a / (float)b);
		break;
	}
}


/*
 * Process newer Nikon maker note tags.
 */
static void
nikon_prop1(struct exifprop *prop, struct exiftags *t)
{
	u_int32_t a, b;

	switch (prop->tag) {

	/* Manual focus distance. */

	case 0x0085:
		a = exif4byte(t->md.btiff + prop->value, t->md.order);
		b = exif4byte(t->md.btiff + prop->value + 4, t->md.order);

		if (a == b) {
			snprintf(prop->str, 31, "N/A");
			prop->lvl = ED_VRB;
		} else
			snprintf(prop->str, 31, "x%.1f m", (float)a / (float)b);
		break;

	/* Digital zoom. */

	case 0x0086:
		a = exif4byte(t->md.btiff + prop->value, t->md.order);
		b = exif4byte(t->md.btiff + prop->value + 4, t->md.order);

		if (a == b) {
			snprintf(prop->str, 31, "None");
			prop->lvl = ED_VRB;
		} else
			snprintf(prop->str, 31, "x%.1f", (float)a / (float)b);
		break;
	}
}


/*
 * Process Nikon maker note tags.
 */
void
nikon_prop(struct exifprop *prop, struct exiftags *t)
{

	if (prop->tagset == nikon_tags0) {
		nikon_prop0(prop, t);
		return;
	}

	if (prop->tagset == nikon_tags1) {
		nikon_prop1(prop, t);
		return;
	}
}


/*
 * Try to read a Nikon maker note IFD.
 */
struct ifd *
nikon_ifd(u_int32_t offset, struct tiffmeta *md)
{
	struct ifd *myifd;
	unsigned char *b;
	struct tiffmeta mkrmd;

	b = md->btiff + offset;
	mkrmd = *md;

	/*
	 * Seems that some Nikon maker notes start with an ID string and
	 * a version of some sort.
	 */

	if (!strcmp((const char *)b, "Nikon")) {
		b += 6;

		switch (exif2byte(b, md->order)) {
		case 0x0001:
			readifd(offset + 8, &myifd, nikon_tags0, &mkrmd);
			return (myifd);

		case 0x0200:
			b += 4;

			/*
			 * So, this is interesting: they've put a full-fledged
			 * TIFF header here.
			 */

			/* Determine endianness of the TIFF data. */

			if (*((u_int16_t *)b) == 0x4d4d)
				mkrmd.order = BIG;
			else if (*((u_int16_t *)b) == 0x4949)
				mkrmd.order = LITTLE;
			else {
				exifwarn("invalid Nikon TIFF header");
				return (NULL);
			}
			mkrmd.btiff = b;	/* Beginning of maker. */
			b += 2;

			/* Verify the TIFF header. */

			if (exif2byte(b, mkrmd.order) != 42) {
				exifwarn("invalid Nikon TIFF header");
				return (NULL);
			}
			b += 2;

			readifd(exif4byte(b, mkrmd.order), &myifd,
			    nikon_tags1, &mkrmd);
			return (myifd);

		default:
			exifwarn("Nikon maker note version not supported");
			return (NULL);
		}
	}

	/*
	 * Others are just normal IFDs.
	 */

	readifd(offset, &myifd, nikon_tags1, &mkrmd);
	return (myifd);
}
