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
 * $Id: nikon.c,v 1.15 2003/08/06 02:26:42 ejohnst Exp $
 */

/*
 * Exif tag definitions for Nikon maker notes.
 *
 * Some information for Nikon D1X support obtained from JoJoThumb, version
 * 2.7.2 (http://www.jojosoftware.de/jojothumb/).
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
	{ 2,	"Preset" },
	{ 4,	"Daylight" },
	{ 5,	"Incandescent" },
	{ 5,	"Fluorescent" },
	{ 5,	"Cloudy" },
	{ 5,	"Speedlight" },
	{ -1,	"Unknown" },
};


/* Converter. */

static struct descrip nikon_convert[] = {
	{ 0,	"None" },
	{ 2,	"Fisheye" },
	{ -1,	"Unknown" },
};


/* Maker note IFD tags. */

static struct exiftag nikon_tags0[] = {
	{ 0x0002, TIFF_SHORT, 2, ED_UNK, "NikonISOSet",
	  "ISO Setting", NULL },
	{ 0x0003, TIFF_ASCII, 0, ED_IMG, "NikonColor",
	  "Color Mode", NULL },
	{ 0x0004, TIFF_ASCII, 0, ED_IMG, "NikonQuality",
	  "Image Quality", NULL },
	{ 0x0005, TIFF_ASCII, 0, ED_IMG, "NikonWhiteBal",
	  "White Balance", NULL },
	{ 0x0006, TIFF_ASCII, 0, ED_IMG, "NikonSharp",
	  "Image Sharpening", NULL },
	{ 0x0007, TIFF_ASCII, 0, ED_IMG, "NikonFocusMode",
	  "Focus Mode", NULL },
	{ 0x0008, TIFF_ASCII, 0, ED_IMG, "NikonFlashSet",
	  "Flash Setting", NULL },
	{ 0x000f, TIFF_ASCII, 0, ED_IMG, "NikonISOSelect",
	  "ISO Selection", NULL },
	{ 0x0080, TIFF_ASCII, 0, ED_IMG, "NikonImgAdjust",
	  "Image Adjustment", NULL },
	{ 0x0082, TIFF_ASCII, 0, ED_IMG, "NikonAdapter",
	  "Lens Adapter", NULL },
	{ 0x0085, TIFF_ASCII, 0, ED_IMG, "NikonFocusDist",
	  "Focus Distance", NULL },
	{ 0x0086, TIFF_ASCII, 0, ED_IMG, "NikonDigiZoom",
	  "Digital Zoom", NULL },
	{ 0xffff, TIFF_UNKN, 0, ED_UNK, "Unknown",
	  "Nikon Unknown", NULL },
};


static struct exiftag nikon_tags1[] = {
	{ 0x0003, TIFF_SHORT, 1, ED_UNK, "NikonQuality",
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


static struct exiftag nikon_tags2[] = {
	{ 0x0001, TIFF_UNDEF, 4, ED_VRB, "NikonVersion",
	  "Nikon Note Version", NULL },
	{ 0x0002, TIFF_SHORT, 2, ED_IMG, "NikonISOSetting",
	  "ISO Setting", NULL },
	{ 0x0003, TIFF_ASCII, 0, ED_IMG, "NikonColor",
	  "Color Mode", NULL },
	{ 0x0004, TIFF_ASCII, 0, ED_IMG, "NikonQuality",
	  "Image Quality", NULL },
	{ 0x0005, TIFF_ASCII, 0, ED_IMG, "NikonWhiteBal",
	  "White Balance", NULL },
	{ 0x0006, TIFF_ASCII, 0, ED_IMG, "NikonImgSharp",
	  "Image Sharpening", NULL },
	{ 0x0007, TIFF_ASCII, 0, ED_IMG, "NikonFocus",
	  "Focus Mode", NULL },
	{ 0x0008, TIFF_ASCII, 0, ED_IMG, "NikonFlash",
	  "Flash Setting", NULL },
	{ 0x000b, TIFF_UNKN, 0, ED_UNK, "NikonWhiteBalBias",
	  "White Balance Bias", NULL },
	{ 0x0081, TIFF_ASCII, 0, ED_IMG, "NikonImgAdjust",
	  "Image Adjustment", NULL },
	{ 0x0088, TIFF_UNDEF, 0, ED_UNK, "NikonAutoFocus",
	  "Auto Focus Position", NULL },
	{ 0x0092, TIFF_UNDEF, 0, ED_UNK, "NikonHueAdjust",
	  "Hue Adjustment", NULL },
	{ 0xffff, TIFF_UNKN, 0, ED_UNK, "NikonUnknown",
	  "Nikon Unknown", NULL },
};


/*
 * Process normal Nikon maker note tags.
 */
static void
nikon_prop0(struct exifprop *prop, struct exiftags *t)
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
 * Process Nikon maker note tags that start from an offset.
 */
static void
nikon_prop1(struct exifprop *prop, struct exiftags *t)
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
 * Process Nikon maker note tags.
 */
void
nikon_prop(struct exifprop *prop, struct exiftags *t)
{

	if (prop->tagset == nikon_tags1)
		nikon_prop1(prop, t);
	else
		nikon_prop0(prop, t);
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
	 * Seems that some Nikon maker notes start with an ID string.
	 */

	if (!strcmp((const char *)b, "Nikon")) {
		b += 6;

		switch (*((u_int16_t *)b)) {
		case 0x0001:
			readifd(offset + 8, &myifd, nikon_tags1, &mkrmd);
			return (myifd);

		case 0x0002:
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
			    nikon_tags2, &mkrmd);
			return (myifd);
		}
	}

	readifd(offset, &myifd, nikon_tags0, &mkrmd);
	return (myifd);
}
