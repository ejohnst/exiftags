/*
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
 * $Id: exifgps.c,v 1.2 2003/08/02 17:45:44 ejohnst Exp $
 */

/*
 * Exif GPS information tags.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "exif.h"
#include "exifint.h"


/* Speed. */

static struct descrip gps_speed[] = {
	{ 'K',	"km/hr" },
	{ 'M',	"mph" },
	{ 'N',	"knots" },
	{ -1,	"" },
};


/* Status. */

static struct descrip gps_status[] = {
	{ 'A',	"Measurement In Progress" },
	{ 'V',	"Measurement Interoperability" },
	{ -1,	"Unknown" },
};


/* Distance. */

static struct descrip gps_dist[] = {
	{ 'K',	"km" },
	{ 'M',	"mi" },
	{ 'N',	"knots" },
	{ -1,	"" },
};


/* Differential correction. */

static struct descrip gps_diff[] = {
	{ 0,	"No Correction" },
	{ 1,	"Correction Applied" },
	{ -1,	"Unknown" },
};


/* Bearing reference. */

static struct descrip gps_bear[] = {
	{ 'M',	"Magnetic North" },
	{ 'T',	"True North" },
	{ -1,	"Unknown" },
};


/* GPS info version 2.2 tags. */

struct exiftag gpstags[] = {
	{ 0x0000, TIFF_BYTE,  4,  ED_VRB,
	    "GPSVersionID", "GPS Info Version", NULL },
	{ 0x0001, TIFF_ASCII, 2,  ED_VRB,
	    "GPSLatitudeRef", "Latitude Reference", NULL },
	{ 0x0002, TIFF_RTNL,  3,  ED_IMG,
	    "GPSLatitude", "Latitude", NULL },
	{ 0x0003, TIFF_ASCII, 2,  ED_VRB,
	    "GPSLongitudeRef", "Longitude Reference", NULL },
	{ 0x0004, TIFF_RTNL,  3,  ED_IMG,
	    "GPSLongitude", "Longitude", NULL },
	{ 0x0005, TIFF_BYTE,  1,  ED_VRB,
	    "GPSAltitudeRef", "Altitude Reference", NULL },
	{ 0x0006, TIFF_RTNL,  1,  ED_IMG,		/* meters */
	    "GPSAltitude", "Altitude", NULL },
	{ 0x0007, TIFF_RTNL,  3,  ED_IMG,
	    "GPSTimeStamp", "Time (UTC)", NULL },
	{ 0x0008, TIFF_ASCII, 0,  ED_IMG,
	    "GPSSatellites", "GPS Satellites", NULL },
	{ 0x0009, TIFF_ASCII, 2,  ED_IMG,
	    "GPSStatus", "GPS Status", gps_status },
	{ 0x000a, TIFF_ASCII, 2,  ED_IMG,
	    "GPSMeasureMode", "GPS Measurement Mode", NULL },
	{ 0x000b, TIFF_RTNL,  1,  ED_IMG,
	    "GPSDOP", "GPS Degree of Precision", NULL },
	{ 0x000c, TIFF_ASCII, 2,  ED_VRB,
	    "GPSSpeedRef", "GPS Speed Reference", gps_speed },
	{ 0x000d, TIFF_RTNL,  1,  ED_IMG,
	    "GPSSpeed", "Movement Speed", NULL },
	{ 0x000e, TIFF_ASCII, 2,  ED_VRB,
	    "GPSTrackRef", "GPS Direction Reference", gps_bear },
	{ 0x000f, TIFF_RTNL,  1,  ED_IMG,		/* degrees */
	    "GPSTrack", "Movement Direction", NULL },
	{ 0x0010, TIFF_ASCII, 2,  ED_VRB,
	    "GPSImgDirectionRef", "GPS Image Direction Ref", gps_bear },
	{ 0x0011, TIFF_RTNL,  1,  ED_IMG,		/* degrees */
	    "GPSImgDirection",  "Image Direction", NULL },
	{ 0x0012, TIFF_ASCII, 0,  ED_IMG,
	    "GPSMapDatum", "Geodetic Survey Data", NULL },
	{ 0x0013, TIFF_ASCII, 2,  ED_VRB,
	    "GPSDestLatitudeRef", "GPS Dest Latitude Ref", NULL },
	{ 0x0014, TIFF_RTNL,  3,  ED_IMG,
	    "GPSDestLatitude", "Destination Latitude", NULL },
	{ 0x0015, TIFF_ASCII, 2,  ED_VRB,
	    "GPSDestLongitudeRef", "GPS Dest Longitude Ref", NULL },
	{ 0x0016, TIFF_RTNL,  3,  ED_IMG,
	    "GPSDestLongitude", "Destination Longitude", NULL },
	{ 0x0017, TIFF_ASCII, 2,  ED_VRB,
	    "GPSDestBearingRef", "GPS Dest Bearing Ref", gps_bear },
	{ 0x0018, TIFF_RTNL,  1,  ED_IMG,		/* degrees */
	    "GPSDestBearing", "Destination Direction", NULL },
	{ 0x0019, TIFF_ASCII, 2,  ED_VRB,
	    "GPSDestDistanceRef", "GPS Dest Distance Ref", gps_dist },
	{ 0x001a, TIFF_RTNL,  1,  ED_IMG,
	    "GPSDestDistance", "Destination Distance", NULL },
	{ 0x001b, TIFF_UNDEF, 0,  ED_IMG,
	    "GPSProcessingMethod", "GPS Processing Method", NULL },
	{ 0x001c, TIFF_UNDEF, 0,  ED_IMG,
	    "GPSAreaInformation", "GPS Area", NULL },
	{ 0x001d, TIFF_ASCII, 11, ED_IMG,
	    "GPSDateStamp", "Date (UTC)", NULL },
	{ 0x001e, TIFF_SHORT, 1,  ED_IMG,
	    "GPSDifferental", "GPS Differential Correction", gps_diff },
	{ 0xffff, TIFF_UNKN,  0,  ED_UNK,
	    "Unknown", NULL, NULL },
};


/*
 * Process GPS tags.
 */
void
gpsprop(struct exifprop *prop, struct exiftags *t)
{
	int i, x;
	u_int32_t n, d;
	u_int16_t v = (u_int16_t)prop->value;
	double deg, min, sec;
	char fmt[32], buf[8];
	unsigned char *c;
	struct exifprop *tmpprop;

	/* Lookup field description values. */

	for (x = 0; gpstags[x].tag < EXIF_T_UNKNOWN &&
	    gpstags[x].tag != prop->tag; x++);
	if (gpstags[x].table)
		if (gpstags[x].type != TIFF_ASCII)
			prop->str = finddescr(gpstags[x].table, v);

	switch (prop->tag) {

	/* Version. */

	case 0x0000:
		if (!(prop->str = (char *)malloc(8)))
			exifdie((const char *)strerror(errno));

		/* Platform byte order affects this... */

		i = 1;
		if (*(char *)&i == 1)
			for (i = 0; i < 4; i++) {
				prop->str[i * 2] = '0' +
				    ((const char *)&prop->value)[3 - i];
				prop->str[i * 2 + 1] = '.';
			}
		else
			for (i = 0; i < 4; i++) {
				prop->str[i * 2] = '0' +
				    ((const char *)&prop->value)[i];
				prop->str[i * 2 + 1] = '.';
			}
		prop->str[7] = '\0';
		break;

	/*
	 * Reference values.  The value is 2-count nul-terminated ASCII,
	 * not an offset to the ASCII string.  Being a little lazy here
	 * with how I'm extracting the byte: both the byte orders of the
	 * platform and the file matter.  So, I just look to see where the
	 * non-zero byte is in the value and assume it's the one I want.
	 * (All this is due primarily to the fact that the value is normally
	 * an offset to the string, not the string itself.)
	 */

	case 0x0001:
	case 0x0003:
	case 0x0009:
	case 0x000a:
	case 0x000c:
	case 0x000e:
	case 0x0010:
	case 0x0013:
	case 0x0015:
	case 0x0017:
	case 0x0019:
		for (i = 0; i < 3 && !((unsigned char *)&prop->value)[i]; i++);
		if (gpstags[x].table)
			prop->str = finddescr(gpstags[x].table,
			    ((unsigned char *)&prop->value)[i]);
		else {
			if (!(prop->str = (char *)malloc(2)))
				exifdie((const char *)strerror(errno));
			prop->str[0] = ((unsigned char *)&prop->value)[i];
			prop->str[1] = '\0';
		}
		break;

	/*
	 * Coordinate values.
	 *
	 * This is really kind of a mess.  The display behavior here is
	 * based on image samples from a Nikon D1X and a Fuji FinePix S1 Pro.
	 * The specification allows for fractional minutes (and no seconds).
	 * Not sure if there are any other combinations...
	 */

	case 0x0002:
	case 0x0004:
	case 0x0014:
	case 0x0016:
	 	if (prop->count != 3 || prop->value + prop->count * 8 >
		    (u_int32_t)(t->etiff - t->btiff)) {
			exifwarn("unexpected GPS coordinate values");
			break;
		}

		if (!(prop->str = (char *)malloc(32)))
			exifdie((const char *)strerror(errno));
		prop->str[0] = prop->str[31] = '\0';

		/* Figure out the reference prefix. */

		switch (prop->tag) {
		case 0x0002:
			tmpprop = findtprop(t->props, gpstags, 0x0001);
			break;
		case 0x0004:
			tmpprop = findtprop(t->props, gpstags, 0x0003);
			break;
		case 0x0014:
			tmpprop = findtprop(t->props, gpstags, 0x0013);
			break;
		case 0x0016:
			tmpprop = findtprop(t->props, gpstags, 0x0015);
			break;
		default:
			tmpprop = NULL;
		}

		/* Degrees. */

		i = 0;
		n = exif4byte(t->btiff + prop->value + i * 8, t->tifforder);
		d = exif4byte(t->btiff + prop->value + 4 + i * 8, t->tifforder);

		strcpy(fmt, "%s %.f° ");
		if (!n)				/* Punt. */
			deg = 0.0;
		else {
			deg = (double)n / (double)d;
			if (d != 1)
				sprintf(fmt, "%%s %%.%df° ",
				    (int)log10((double)d));
		}

		/* Minutes. */

		i++;
		n = exif4byte(t->btiff + prop->value + i * 8, t->tifforder);
		d = exif4byte(t->btiff + prop->value + 4 + i * 8, t->tifforder);

		if (!n) {			/* Punt. */
			min = 0.0;
			strcat(fmt, "%.f'");
		} else {
			min = (double)n / (double)d;
			if (d != 1) {
				sprintf(buf, "%%.%df'", (int)log10((double)d));
				strcat(fmt, buf);
			} else
				strcat(fmt, "%.f'");
		}

		/*
		 * Seconds.  We'll assume if minutes are fractional, we
		 * should just ignore seconds.
		 */

		i++;
		n = exif4byte(t->btiff + prop->value + i * 8, t->tifforder);
		d = exif4byte(t->btiff + prop->value + 4 + i * 8, t->tifforder);

		if (!n) {			/* Assume no seconds. */
			snprintf(prop->str, 31, fmt, tmpprop && tmpprop->str ?
			    tmpprop->str : "", deg, min);
			break;
		} else {
			sec = (double)n / (double)d;
			if (d != 1) {
				sprintf(buf, " %%.%df", (int)log10((double)d));
				strcat(fmt, buf);
			} else
				strcat(fmt, " %.f");
		}
		snprintf(prop->str, 31, fmt, tmpprop && tmpprop->str ?
		    tmpprop->str : "", deg, min, sec);
		break;
	}
}
