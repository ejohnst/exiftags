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
 * $Id: canon.c,v 1.7 2002/10/05 17:11:58 ejohnst Exp $
 */

/*
 * Exif tag definitions for Canon maker notes.
 * Developed from http://www.burren.cx/david/canon.html.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "exiftags.h"			/* XXX For exifdie(). */
#include "exifint.h"
#include "makers.h"


/* Custom function field description lookup table. */

struct ccstm {
	int32_t val;
	const char *descr;
	struct descrip *table;
};


/* Maker note IFD tags. */

static struct exiftag canon_tags[] = {
	{ 0x0001, TIFF_SHORT, 0,  ED_VRB, "Canon1Tag", "Canon Tag1 Offset" },
	{ 0x0004, TIFF_SHORT, 0,  ED_VRB, "Canon4Tag", "Canon Tag4 Offset" },
	{ 0x0006, TIFF_ASCII, 32, ED_VRB, "ImageType", "Image Type" },
	{ 0x0007, TIFF_ASCII, 24, ED_CAM, "FirmwareVer", "Firmware Version" },
	{ 0x0008, TIFF_LONG,  1,  ED_IMG, "ImgNum", "Image Number" },
	{ 0x0009, TIFF_ASCII, 32, ED_CAM, "OwnerName", "Owner Name" },
	{ 0x000c, TIFF_LONG,  1,  ED_CAM, "Serial", "Serial Number" },
	{ 0x000f, TIFF_SHORT, 0,  ED_IMG, "CustomFunc", "Custom Function" },
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
	{ 13, TIFF_SHORT, 0, ED_IMG, "CanonContrast", "Contrast" },
	{ 14, TIFF_SHORT, 0, ED_IMG, "CanonSaturate", "Saturation" },
	{ 15, TIFF_SHORT, 0, ED_IMG, "CanonSharpness", "Sharpness" },
	{ 16, TIFF_SHORT, 0, ED_UNK, "CanonISO", "ISO Speed Rating" },
	{ 17, TIFF_SHORT, 0, ED_IMG, "CanonMeterMode", "Metering Mode" },
	{ 18, TIFF_SHORT, 0, ED_IMG, "CanonFocusType", "Focus Type" },
	{ 19, TIFF_SHORT, 0, ED_UNK, "CanonAFPoint", "Autofocus Point" },
	{ 20, TIFF_SHORT, 0, ED_IMG, "CanonExpMode", "Exposure Mode" },
	{ 23, TIFF_SHORT, 0, ED_UNK, "CanonMaxFocal", "Max Focal Length" },
	{ 24, TIFF_SHORT, 0, ED_UNK, "CanonMinFocal", "Min Focal Length" },
	{ 25, TIFF_SHORT, 0, ED_UNK, "CanonFocalUnits", "Focal Units/mm" },
	{ 28, TIFF_SHORT, 0, ED_UNK, "CanonFlashAct", "Flash Activity" },
	{ 29, TIFF_SHORT, 0, ED_UNK, "CanonFlashDet", "Flash Details" },
	{ 36, TIFF_SHORT, 0, ED_VRB, "CanonDZoomRes", "Zoomed Resolution" },
	{ 37, TIFF_SHORT, 0, ED_VRB, "CanonBZoomRes", "Base Zoom Resolution" },
	{ 0xffff, TIFF_SHORT, 0, ED_UNK, "CanonUnknown", "Canon Tag1 Unknown" },
};


/* Fields under tag 0x0004. */

static struct exiftag canon_tags4[] = {
	{ 0,  TIFF_SHORT, 0, ED_VRB, "Canon4Len", "Canon Tag4 Length" },
	{ 7,  TIFF_SHORT, 0, ED_IMG, "CanonWhiteB", "White Balance" },
	{ 9,  TIFF_SHORT, 0, ED_IMG, "CanonSequence", "Sequence Number" },
	{ 14, TIFF_SHORT, 0, ED_UNK, "CanonAFPoint2", "Autofocus Point" },
	{ 15, TIFF_SHORT, 0, ED_VRB, "CanonFlashBias", "Flash Bias" },
	{ 19, TIFF_SHORT, 0, ED_UNK, "CanonSubjDst", "Subject Distance" },
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


/* Flash bias.  Is this ever something other than 0? */

static struct descrip canon_fbias[] = {
	{ 0x0000,	"0 EV" },
	{ 0x000c,	"0.33 EV" },
	{ 0x0010,	"0.50 EV" },
	{ 0x0014,	"0.67 EV" },
	{ 0x0020,	"1 EV" },
	{ 0x002c,	"1.33 EV" },
	{ 0x0030,	"1.50 EV" },
	{ 0x0034,	"1.67 EV" },
	{ 0x0040,	"2 EV" },
	{ 0xffc0,	"-2 EV" },
	{ 0xffcc,	"-1.67 EV" },
	{ 0xffd0,	"-1.50 EV" },
	{ 0xffd4,	"-1.33 EV" },
	{ 0xffe0,	"-1 EV" },
	{ 0xffec,	"-0.67 EV" },
	{ 0xfff0,	"-0.50 EV" },
	{ 0xfff4,	"-0.33 EV" },
	{ -1,		"Unknown" },
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
	{ 3,	"Manual" },
	{ 4,	"Single" },
	{ 5,	"Continuous" },
	{ 6,	"Manual" },
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


/* Contrast, saturation, & sharpness. */

static struct descrip canon_range[] = {
	{ 0,	"Normal" },
	{ 1,	"High" },
	{ 0xffff, "Low" },
	{ -1,	"Unknown" },
};


/* Metering mode. */

static struct descrip canon_meter[] = {
	{ 3,	"Evaluative" },
	{ 4,	"Partial" },
	{ 5,	"Center-Weighted" },
	{ -1,	"Unknown" },
};


/* Exposure mode. */

static struct descrip canon_expmode[] = {
	{ 0,	"Easy Shooting" },
	{ 1,	"Program" },
	{ 2,	"Tv-Priority" },
	{ 3,	"Av-Priority" },
	{ 4,	"Manual" },
	{ 5,	"A-DEP" },
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


/* Value descriptions for D30 custom functions. */

static struct descrip ccstm_offon[] = {
	{ 0,	"Off" },
	{ 1,	"On" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_shutter[] = {
	{ 0,	"AF/AE Lock" },
	{ 1,	"AE Lock/AF" },
	{ 2,	"AF/AF Lock" },
	{ 3,	"AE+Release/AE+AF" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_disen[] = {
	{ 0,	"Disabled" },
	{ 1,	"Enabled" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_explvl[] = {
	{ 0,	"1/2 Stop" },
	{ 1,	"1/3 Stop" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_autooff[] = {
	{ 0,	"Auto" },
	{ 1,	"Off" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_shutspd[] = {
	{ 0,	"Auto" },
	{ 1,	"1/200 (Fixed)" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_aebseq[] = {
	{ 0,	"0,-,+/Enabled" },
	{ 1,	"0,-,+/Disabled" },
	{ 2,	"-,0,+/Enabled" },
	{ 3,	"-,0,+/Disabled" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_shutsync[] = {
	{ 0,	"1st-Curtain Sync" },
	{ 1,	"2nd-Curtain Sync" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_lensaf[] = {
	{ 0,	"AF Stop" },
	{ 1,	"Operate AF" },
	{ 2,	"Lock AE & Start Timer" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_endis[] = {
	{ 0,	"Enabled" },
	{ 1,	"Disabled" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_menubut[] = {
	{ 0,	"Top" },
	{ 1,	"Previous (Volatile)" },
	{ 2,	"Previous" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_setbut[] = {
	{ 0,	"Not Assigned" },
	{ 1,	"Change Quality" },
	{ 2,	"Change ISO Speed" },
	{ 3,	"Select Parameters" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_yesno[] = {
	{ 0,	"Yes" },
	{ 1,	"No" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_onoff[] = {
	{ 0,	"On" },
	{ 1,	"Off" },
	{ -1,	"Unknown" },
};


/* D30 custom functions. */

static struct ccstm canon_d30custom[] = {
	{ 1,	"Long exposure noise reduction", ccstm_offon },
	{ 2,	"Shutter/AE lock buttons", ccstm_shutter },
	{ 3,	"Mirror lockup", ccstm_disen },
	{ 4,	"Tv/Av and exposure level", ccstm_explvl },
	{ 5,	"AF-assist light", ccstm_autooff },
	{ 6,	"Av mode shutter speed", ccstm_shutspd },
	{ 7,	"AEB sequence/auto cancellation", ccstm_aebseq },
	{ 8,	"Shutter curtain sync", ccstm_shutsync },
	{ 9,	"Lens AF stop button", ccstm_lensaf },
	{ 10,	"Fill flash auto reduction", ccstm_endis },
	{ 11,	"Menu button return position", ccstm_menubut },
	{ 12,	"Shooting Set button function", ccstm_setbut },
	{ 13,	"Sensor cleaning", ccstm_disen },
	{ 14,	"Superimposed display", ccstm_onoff },
	{ 15,	"Shutter release w/o CF card", ccstm_yesno },
	{ -1,	"Unknown function", NULL },
};


/*
 * Process maker note tag 0x0001 values.
 */

static void
canon_prop1(struct exifprop *prop, char *off, struct exiftags *t)
{
	int i, j;
	u_int16_t v, flmin, flmax, flunit;
	struct exifprop *aprop, *tmpprop;
	enum order o = t->tifforder;

	for (i = 0; i < (int)prop->count; i++) {
		v = exif2byte(off + i * 2, o);

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

			if (!v && exif2byte(off + 2 * 2, o))
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
				    (float)exif2byte(off + 37 * 2, o) /
				    (float)exif2byte(off + 36 * 2, o));
				aprop->str[31] = '\0';
			} else
				aprop->str = finddescr(canon_dzoom, v);
			break;
		case 13:
		case 14:
		case 15:
			aprop->str = finddescr(canon_range, v);
			break;
		case 17:
			aprop->str = finddescr(canon_meter, v);
			/* Maker meter mode overrides standard one. */
			if ((tmpprop = findprop(t->props, EXIF_T_METERMODE)))
				tmpprop->lvl = ED_VRB;
			break;
		case 18:
			aprop->str = finddescr(canon_focustype, v);
			break;
		case 20:
			aprop->str = finddescr(canon_expmode, v);
			break;
		case 23:
			flmax = v;
			break;
		case 24:
			flmin = v;
			break;
		case 25:
			flunit = v;
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

	/*
	 * Create a new value for the lens' focal length range.  If it's
	 * not a zoom lens, we'll make it verbose (it should match the
	 * existing focal length Exif tag).
	 */

	aprop = childprop(prop);
	aprop->name = "CanonLensSz";
	aprop->descr = "Lens Size";
	if (!(aprop->str = (char *)malloc(32)))
		exifdie((const char *)strerror(errno));

	if (flmin == flmax) {
		snprintf(aprop->str, 31, "%.2f mm",
		    (float)flmax / (float)flunit);
		aprop->lvl = ED_VRB;
	} else {
		snprintf(aprop->str, 31, "%.2f - %.2f mm",
		    (float)flmin / (float)flunit, (float)flmax / (float)flunit);
		aprop->lvl = ED_CAM;
	}
}


/*
 * Process maker note tag 0x0004 values.
 */

static void
canon_prop4(struct exifprop *prop, char *off, enum order o)
{
	int i, j;
	u_int16_t v;
	struct exifprop *aprop;

	for (i = 0; i < (int)prop->count; i++) {
		v = exif2byte(off + i * 2, o);

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
		case 15:
			aprop->str = finddescr(canon_fbias, v);
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
 * Process custom function tag values.
 */

static void
canon_custom(struct exifprop *prop, char *off, struct exiftags *t)
{
	int i, j;
	const char *cn;
	char *cv = NULL;
	u_int16_t v;
	struct exifprop *aprop, *tmpprop;
	struct ccstm *table = NULL;
	enum order o = t->tifforder;

	/*
	 * Determine what kind of camera we've got.  Only support D30
	 * right now...
	 */

	tmpprop = findprop(t->props, EXIF_T_MODEL);
	if (tmpprop && tmpprop->str && !strcmp(tmpprop->str, "Canon EOS D60"))
		table = canon_d30custom;

	for (i = 0; i < (int)prop->count; i++) {
		v = exif2byte(off + i * 2, o);

		aprop = childprop(prop);
		aprop->value = (u_int32_t)v;
		aprop->name = prop->name;
		aprop->descr = prop->descr;
		aprop->lvl = prop->lvl;

		/* If we have a table, lookup function name and value. */

		if (table) {
			for (j = 0; table[j].val != -1 &&
			    table[j].val != (v >> 8 & 0xff); j++);
			if (table[j].table)
				cv = finddescr(table[j].table, v & 0xff);
			cn = table[j].descr;
		} else
			cn = "Unknown";

		if (!(aprop->str = (char *)malloc(4 + strlen(cn) +
		    (cv ? strlen(cv) : 10))))
			exifdie((const char *)strerror(errno));

		if (cv) {
			snprintf(aprop->str, 4 + strlen(cn) + strlen(cv),
			    "%s - %s", cn, cv);
			free(cv);
			cv = NULL;
		} else {
			snprintf(aprop->str, 4 + strlen(cn) + 10, "%s %d - %d",
			    cn, v >> 8 & 0xff, v & 0xff);
			aprop->str[3 + strlen(cn) + 10] = '\0';
			aprop->lvl = ED_UNK;
		}
	}
}


/* Process Canon maker note tags. */

void
canon_prop(struct exifprop *prop, struct exiftags *t)
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

	        for (i = 0; ftypes[i].type &&
		    ftypes[i].type != prop->type; i++);
		printf("   %s (0x%04X): %s, %d, %d\n", prop->name, prop->tag,
		    ftypes[i].name, prop->count, prop->value);
	}

	switch (prop->tag) {

	/* Various image data. */

	case 0x0001:
		offset = t->btiff + prop->value;
		if (exif2byte(offset, t->tifforder) != 2 * prop->count) {
			exifwarn("Canon maker note appears corrupt (0x0001)");
			break;
		}
		canon_prop1(prop, offset, t);
		break;

	case 0x0004:
		offset = t->btiff + prop->value;
		if (exif2byte(offset, t->tifforder) != 2 * prop->count) {
			exifwarn("Canon maker note appears corrupt (0x0004)");
			break;
		}
		canon_prop4(prop, offset, t->tifforder);
		break;

	/* Image number. */

	case 0x0008:
		if (!(prop->str = (char *)malloc(32)))
			exifdie((const char *)strerror(errno));
		snprintf(prop->str, 31, "%03d-%04d", prop->value / 10000,
		    prop->value % 10000);
		prop->str[31] = '\0';
		break;

	/* Serial number. */

	case 0x000c:
		if (!(prop->str = (char *)malloc(32)))
			exifdie((const char *)strerror(errno));
		snprintf(prop->str, 31, "%04X-%05d", prop->value >> 16,
		    prop->value & 0xffff);
		prop->str[31] = '\0';
		break;

	/* Custom function. */

	case 0x000f:
		canon_custom(prop, t->btiff + prop->value, t);
		/* Set parent tag to verbose after children finished. */
		prop->lvl = ED_VRB;
		break;
	}
}
