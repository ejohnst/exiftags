/*
 * Copyright (c) 2002, Eric M. Johnston <emj@postal.net>
 * Copyright (c) 2002, Javier Crespo <jcrespoc@dsland.org>
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
 * $Id: minolta.c,v 1.3 2003/01/11 18:52:30 ejohnst Exp $
 *
 */ 

/*
 * Exif tag definitions for Minolta DiMAGE maker notes.
 *
 * Compatibility:
 *
 *   DiMAGE 5		OK, except MMN_IMAGESIZE
 *   DiMAGE 7		OK
 *   DiMAGE 7i v1.0e	OK
 *   DiMAGE 7Hi 1.00u	OK
 *
 *   DiMAGE S404	not OK; short TAG0x01; interpreted with errors
 *   DiMAGE S304	not OK; short TAG0x01; interpreted with errors
 *   DiMAGE F100	not OK; no TAG0x01
 *   DiMAGE X		not OK; no TAG0x01
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "makers.h"


/*
 * Uncomment this to overwrite standard tags with more appropriate values 
 *
 * i.e.  Exposure Program: Not Defined
 *       Scene Capture Type: Protrait 
 *       with
 *       Exposure Program: Program AE (Portrait)
 *
 * or    Flash: Flash, Compulsory
 *       with
 *       Flash: Flash, Compulsory (Fill Flash) 
 *
 *  In any case, you'll get the Minolta custom fields as verbose properties
 * 
 */

#define REDEFINE_STANDARD TRUE


/* Bracketing mode. */

static struct descrip minolta_brackmode[] = {
	{ 0,	"Exposure" },
	{ 1,	"Contrast" },
	{ 2,	"Saturation" },
	{ 3,	"Filter" },
	{ -1,	"Unknown" },
};


/* Bracketing step. */

static struct descrip minolta_brackstep[] = {
	{ 0,	"1/3" },
	{ 1,	"1/2" },
	{ 2,	"1" },
	{ -1,	"Unknown" },
};


/* Sharpness. */

static struct descrip minolta_sharp[] = {
	{ 0,	"Normal" },
	{ 1,	"Hard" },
	{ 2,	"Soft" },
	{ -1,	"Unknown" },
};


/* Color mode. */

static struct descrip minolta_color[] = {
	{ 0,	"Natural Color" },
	{ 1,	"Black & White" },
	{ 2,	"Vivid Color" },
	{ 3,	"Solarization" },
	{ 4,	"Adobe RGB" },
	{ -1,	"Unknown" },
};


/* ISO. */

static struct descrip minolta_iso[] = {
	{ 0,	"100" },
	{ 1,	"200" },
	{ 2,	"400" },
	{ 3,	"800" },
	{ 4,	"Auto" },
	{ -1,	"Unknown" },
};


/* Generic boolean. */

static struct descrip minolta_bool[] = {
	{ 0,	"No" },
	{ 1,	"Yes" },
	{ -1,	"Unknown" },
};


/* Focus mode. */

static struct descrip minolta_focusmode[] = {
	{ 0,	"Auto" },
	{ 1,	"Manual" },
	{ -1,	"Unknown" },
};


/* Focus area. */

static struct descrip minolta_focusarea[] = {
	{ 0,	"Wide Area" },
	{ 1,	"Spot Focus Point" },
	{ -1,	"Unknown" },
};


/* Wide area focus zone. */

static struct descrip minolta_widefocus[] = {
	{ 0,	"No Zone" },
	{ 1,	"Center" },
	{ 3,	"Left" },
	{ 4,	"Right" },
	{ -1,	"Unknown" },
};


/* Drive mode. */

static struct descrip minolta_drive[] = {
	{ 0,	"Single Frame" },
	{ 1,	"Continous Advance" },
	{ 2,	"Self Timer" },
	{ 4,	"Bracketing" },
	{ 5,	"Interval" },
	{ 6,	"UHS" },
	{ 7,	"HS" },
	{ -1,	"Unknown" },
};


/* White balance. */

static struct descrip minolta_whitebal[] = {
	{ 0,	"Auto" },
	{ 1,	"Daylight" },
	{ 2,	"Cloudy" },
	{ 3,	"Tungsten" },
	{ 5,	"Custom" },
	{ 7,	"Flourescent" },
	{ 8,	"Flourescent 2" },
	{ 11,	"Custom 2" },
	{ 12,	"Custom 3" },
	{ -1,	"Unknown" },
};


/* Flash mode. */

static struct descrip minolta_flash[] = {
	{ 0,	"Fill Flash" },
	{ 1,	"Red-Eye Reduction" },
	{ 2,	"Rear Sync" },
	{ 3,	"Remote" },
	{ -1,	"Unknown" },
};


/* Capture scene. */

static struct descrip minolta_scene[] = {
	{ 0,	"Normal" },
	{ 1,	"Portrait" },
	{ 2,	"Text" },
	{ 3,	"Night Portrait" },
	{ 4,	"Sunset" },
	{ 5,	"Sports Action" },
	{ -1,	"Unknown" },
};


/* Image quality. */

static struct descrip minolta_quality[] = {
	{ 0,	"Raw" },
	{ 1,	"Super Fine" },
	{ 2,	"Fine" },
	{ 3,	"Standard" },
	{ 4,	"Economy" },
	{ 5,	"Extra Fine" },
	{ -1,	"Unknown" },
};


/* Exposure program. */

static struct descrip minolta_prog[] = {
	{ 0,	"Program AE" },
	{ 1,	"Aperture Priority" },
	{ 2,	"Shutter Priority" },
	{ 3,	"Manual" },
	{ -1,	"Unknown" },
};


/*
 * Image size.
 * This valid only for 5 megapixel cameras (D7's) but is wrong for D5.
 */

static struct descrip minolta_size[] = {
	{ 0,	"2560x1920" },
	{ 1,	"1600x1200" },
	{ 2,	"1280x960" },
	{ 3,	"640x480" },
	{ -1,	"Unknown" },
};


/* Folder name. */

static struct descrip minolta_folder[] = {
	{ 0,	"Standard" },
	{ 1,	"Date" },
	{ -1,	"Unknown" },
};


/* Data imprinting. */

static struct descrip minolta_imprint[] = {
	{ 0,	"None" },
	{ 1,	"DataTime Y/M/D" },
	{ 2,	"DataTime M/D H:M" },
	{ 3,	"Text" },
	{ 4,	"Text + ID" },
	{ -1,	"Unknown" },
};


/* Camera model. */

static struct descrip minolta_model[] = {
	{ 0,	"DiMAGE 7" },
	{ 1,	"DiMAGE 5" },
	{ 4,	"DiMAGE 7i" },
	{ 5,	"DiMAGE 7Hi" },
	{ -1,	"Unknown" },
};


/* Maker note IFD tags. */

static struct exiftag minolta_tags[] = {
	{ 0x0000, TIFF_UNDEF, 4, ED_UNK, "MinoltaMakerType",
	  "Minolta Maker Note Type", NULL },
	{ 0x0001, TIFF_UNDEF, 0, ED_UNK, "Minolta1Tag",
	  "Minolta Tag1 Offset", NULL },
	{ 0x0003, TIFF_UNDEF, 0, ED_UNK, "Minolta3Tag",
	  "Minolta Tag3 Offset", NULL },
	{ 0x0040, TIFF_LONG, 1, ED_UNK, "MinoltaCompImgSz",
	  "Minolta Compressed Image Size", NULL },
	{ 0x0088, TIFF_LONG, 1, ED_UNK, "MinoltaThumbOff",
	  "Minolta Thumbnail Offset", NULL },
	{ 0x0089, TIFF_LONG, 1, ED_UNK, "MinoltaThumbLen",
	  "Minolta Thumbnail Length", NULL },
	{ 0x0e00, TIFF_UNDEF, 0, ED_UNK, "MinoltaPIM",
	  "Minolta Print Image Management", NULL },
	{ 0xffff, TIFF_UNKN, 0, ED_UNK, "Unknown",
	  "Minolta Unknown", NULL },
};


/* Fields under tags 0x0001 and 0x0003. */

static struct exiftag minolta_0TLM[] = {
	{ 1,  TIFF_LONG, 1, ED_UNK, "MinoltaExpProg",
	  NULL, minolta_prog },
	{ 2,  TIFF_LONG, 1, ED_VRB, "MinoltaFlashMode",
	  "Flash Mode", minolta_flash },
	{ 3,  TIFF_LONG, 1, ED_IMG, "MinoltaWhiteB",
	  "White Balance", minolta_whitebal },
	{ 4,  TIFF_LONG, 1, ED_UNK, "MinoltaSize",
	  NULL, minolta_size },
	{ 5,  TIFF_LONG, 1, ED_IMG, "MinoltaQuality",
	  "Image Quality", minolta_quality },
	{ 6,  TIFF_LONG, 1, ED_IMG, "MinoltaDriveMode",
	  "Drive Mode", minolta_drive },
	{ 7,  TIFF_LONG, 1, ED_UNK, "MinoltaMeterMode",
	  NULL, NULL },
	{ 8,  TIFF_LONG, 1, ED_UNK, "MinoltaISO",
	  NULL, NULL },
	{ 9,  TIFF_LONG, 1, ED_UNK, "MinoltaExpTime",
	  NULL, NULL },
	{ 10, TIFF_LONG, 1, ED_UNK, "MinoltaAperture",
	  NULL, NULL },
	{ 11, TIFF_LONG, 1, ED_IMG, "MinoltaMacro",
	  "Macro", minolta_bool },
	{ 12, TIFF_LONG, 1, ED_IMG, "MinoltaDigiZoom",
	  "Digital Zoom", minolta_bool },
	{ 13, TIFF_LONG, 1, ED_UNK, "MinoltaExpComp",
	  NULL, NULL },
	{ 14, TIFF_LONG, 1, ED_IMG, "MinoltaBracketStep",
	  "Bracketing Step", minolta_brackstep },
	{ 16, TIFF_LONG, 1, ED_VRB, "MinoltaIntrvlTime",
	  "Interval Time", NULL },
	{ 17, TIFF_LONG, 1, ED_VRB, "MinoltaIntrvlPics",
	  "Interval Pics", NULL },
	{ 18, TIFF_LONG, 1, ED_UNK, "MinoltaFocalLen",
	  NULL, NULL },
	{ 19, TIFF_LONG, 1, ED_IMG, "MinoltaFocusDist",
	  "Focus Distance", NULL },
	{ 20, TIFF_LONG, 1, ED_VRB, "MinoltaFlash",
	  "Flash Fired", minolta_bool },
	{ 21, TIFF_LONG, 1, ED_VRB, "MinoltaDate",
	  "Date", NULL },
	{ 22, TIFF_LONG, 1, ED_VRB, "MinoltaTime",
	  "Time", NULL },
	{ 23, TIFF_LONG, 1, ED_UNK, "MinoltaMaxAperture",
	  NULL, NULL },
	{ 26, TIFF_LONG, 1, ED_VRB, "MinoltaRmbrFileNum",
	  "File Number Memory", minolta_bool },
	{ 27, TIFF_LONG, 1, ED_VRB, "MinoltaSequence",
	  "Sequence Number", NULL },
	{ 31, TIFF_LONG, 1, ED_IMG, "MinoltaSaturate",
	  "Saturation", NULL },
	{ 32, TIFF_LONG, 1, ED_IMG, "MinoltaContrast",
	  "Contrast", NULL },
	{ 33, TIFF_LONG, 1, ED_IMG, "MinoltaSharpness",
	  "Sharp", minolta_sharp },
	{ 34, TIFF_LONG, 1, ED_IMG, "MinoltaScene",
	  "Scene Capture Type", minolta_scene },
	{ 35, TIFF_LONG, 1, ED_IMG, "MinoltaFlashComp",
	  "Flash Compensation", NULL },
	{ 36, TIFF_LONG, 1, ED_VRB, "MinoltaISO",
	  "ISO Speed Rating", minolta_iso },
	{ 37, TIFF_LONG, 1, ED_UNK, "MinoltaModel",
	  NULL, minolta_model },
	{ 38, TIFF_LONG, 1, ED_VRB, "MinoltaIntervalMode",
	  "Interval Mode", minolta_bool },
	{ 39, TIFF_LONG, 1, ED_VRB, "MinoltaFolder",
	  "Folder Name", minolta_folder },
	{ 40, TIFF_LONG, 1, ED_IMG, "MinoltaColorMode",
	  "Color Mode", minolta_color },
	{ 41, TIFF_LONG, 1, ED_IMG, "MinoltaColorFilt",
	  "Color Warming", NULL },
	{ 42, TIFF_LONG, 1, ED_IMG, "MinoltaBWFilt",
	  "Black & White Tone", NULL },
	{ 43, TIFF_LONG, 1, ED_VRB, "MinoltaIntFlash",
	  "Internal Flash", minolta_bool },
	{ 45, TIFF_LONG, 1, ED_VRB, "MinoltaFocusX",
	  "AF Point X", NULL },
	{ 46, TIFF_LONG, 1, ED_VRB, "MinoltaFocusY",
	  "AF Point Y", NULL },
	{ 47, TIFF_LONG, 1, ED_VRB, "MinoltaWFocusZone",
	  "AF Zone", minolta_widefocus },
	{ 48, TIFF_LONG, 1, ED_IMG, "MinoltaFocusMode",
	  "Focus Mode", minolta_focusmode },
	{ 49, TIFF_LONG, 1, ED_IMG, "MinoltaAFArea",
	  "Wide Focus Area", minolta_focusarea },
	{ 50, TIFF_LONG, 1, ED_IMG, "MinoltaBracketMode",
	  "Bracketing Mode", minolta_brackmode },
	{ 51, TIFF_LONG, 1, ED_VRB, "MinoltaDataImprint",
	  "Data Imprint", minolta_imprint },
	{ 0xffff, TIFF_UNKN, 0, ED_UNK, "MinoltaUnknown",
	  "Minolta Field Unknown", NULL },
};


/* Display debug information about the propery. */

void
minolta_pdump(struct exifprop *prop, int custom)
{
	int i, n;
	static int once = 0;	/* XXX Breaks on multiple files. */

	if (!once) {
		printf("Processing Minolta Maker Note\n");
		once = 1;
	}

	for (i = 0; ftypes[i].type &&
	    ftypes[i].type != prop->type; i++);
	printf("   %s (0x%04X): %s, %d, %d\n", prop->name,
	    custom ? prop->tag & (MMN_CUSTOMTAG ^ 0xffff) : prop->tag,
	    ftypes[i].name, prop->count, prop->value);

#if 0
	if (!(prop->str=(char *)calloc((5*(prop->count))+1,sizeof(char))))
		exifdie((const char *)strerror(errno));

	for (n=0;n<prop->count;n++)
		snprintf (prop->str+(n*5),6,"0x%02x ",(unsigned char) off[n]);
#endif
}


/*
 * Process maker note tag 0x0001 and 0x0003 values.
 */

void
minolta_cprop(struct exifprop *prop, char *off, struct exiftags *t)
{
	int i, j;
	struct exifprop *aprop;

	for (i = 0; i * 4 < prop->count; n++) {
		aprop = childprop(prop);

		/* Note: these are big-endian regardless. */
		aprop->value = exif4byte(off + (4 * i), BIG);

		/* Lookup property name and description. */

		for (j = 0; minolta_0TLM[j].tag < EXIF_T_UNKNOWN &&
			minolta_0TLM[j].tag != i; j++);
		aprop->name = minolta_0TLM[j].name;
		aprop->descr = minolta_0TLM[j].descr;
		aprop->lvl = minolta_0TLM[j].lvl;
		if (minolta_0TLM[j].table)
			aprop->str = finddescr(minolta_0TLM[j].table,
			    aprop->value);

		if (debug)
			printf("     %s (%d): %d\n", aprop->name, i,
			    aprop->value);

		/* Further process known properties. */

		switch (i) {

		/* Interval time and sequence number. */

		case 16:
		case 27:
			prop->value += 1;
			break;

		/* Exposure and flash compensation. */

		case 13:
		case 35:
			prop->str = strdup("-0.7");
			if (prop->value != 6)
				snprintf(prop->str, 5, "%0.1f",
				    ((double)prop->value - 6) / 3);
			else
				strcpy(prop->str, "0");
			break;

		/* Focal length. */

		case 18:
			prop->str = strdup("11.11");
			snprintf(prop->str, 6, "%02f", (double)prop->value / 256);
			break;
#if 0
		case MMN_UNKFLD_28:
		case MMN_UNKFLD_29:
		case MMN_UNKFLD_30:
			prop->str=(char *)calloc(12,sizeof(char));
			snprintf(prop->str,12,"%0.6f",(double)prop->value/256);
			break;
#endif
		/* ISO setting. */

		case 36:
			unsigned int a=pow(2,((double)prop->value/8)-1)*(double)3.125;
			prop->str=(char *)calloc(4,sizeof(char));
			snprintf(prop->str,4,"%d",a);
			break;

		/* Aperture and max aperture. */

		case 10:
		case 23:
			double a=pow(2,((double)prop->value/16)-0.5);
			prop->str=(char *)calloc(20,sizeof(char));
			snprintf(prop->str,20,"%0.1f",a);
			break;

		/* Exposure time. */

		case 9:
			double a=(double)pow(2,((double)abs(48-prop->value))/8);
			prop->str=(char *)calloc(20,sizeof(char));

			//1 sec limit
			if (prop->value<56)
				snprintf(prop->str,20,"%0.1f",a);
			else
				snprintf(prop->str,20,"1/%d",(unsigned int)a);

			/* Bulb mode D7i bug, always recorded as 30secs in standard EXIF
			 * Replace EXIF_T_EXPOSURE data with correct value
			 */
			if (prop->value<32) {
				struct exifprop *tmpprop;

				tmpprop=findprop((struct exifprop *)t->props,EXIF_T_EXPOSURE);
				if (!tmpprop)
					break;

				free(tmpprop->str);
				tmpprop->str=strdup(prop->str);
			}
			break;

		/* Focus distance. */

		case 19:
			if (!(prop->str=(char *)calloc(20,sizeof(char))))
				exifdie((const char *)strerror(errno));
			if (prop->value==0)
				strcpy(prop->str,"Infinite");
			else
				snprintf(prop->str,20,"%.1f",(float)(prop->value/(float)1000));
			prop->value/=100;
			break;

		/*
		 * Flash mode.
		 * Add Flash mode info to standard EXIF_T_FLASH
		 * i.e. Flash. Compulsory (Fill Flash)
		 */

		case 2:
			if (REDEFINE_STANDARD) {
				struct exifprop *tmpprop;

				tmpprop=findprop( (struct exifprop *) t->props, EXIF_T_FLASH );
				if (!tmpprop)
					break;

				/* Flash fired */
				if (tmpprop->value & 0x01) {
					if ( !(tmpprop->str=realloc(tmpprop->str,
						(strlen(tmpprop->str) + strlen(prop->str) + 4) * sizeof(char) )) )
					exifdie((const char *)strerror(errno));
					strcat(tmpprop->str," (");
					strcat(tmpprop->str,prop->str);
					strcat(tmpprop->str,")");
				}
			}
			break;

		/*
		 * Exposure program.
		 * Redefine EXIF_T_EXPPROG in benefict of custom Exposure Program field
		 * with scene info.
		 */

		case 1:
			if (prop->value==0 && REDEFINE_STANDARD) {
				struct exifprop *tmpprop;
				char *ptr;

				/* Find MinoltaScene. */
				if (!(tmpprop=findprop((struct exifprop *)t->props, 34)))
					break;

				if ( !(ptr=finddescr(minolta_scene, tmpprop->value)))
					break;

				if (!(tmpprop=findprop((struct exifprop *)t->props,EXIF_T_EXPPROG)))
					break;

				free(tmpprop->str);
				if ( !(tmpprop->str=(char *)calloc(strlen(prop->str) + strlen(ptr) + 4, sizeof(char))))
					exifdie((const char *)strerror(errno));

				sprintf(tmpprop->str,"%s (%s)",prop->str,ptr);
			}
			break;

		/* Date. */

		case 21:
			unsigned char *ptr = (unsigned char *)&prop->value;

			if (!(prop->str = (char *)calloc(11, sizeof(char))))
				exifdie((const char *)strerror(errno));
			snprintf(prop->str, 11, "%02d/%02d/%04d", ptr[0], ptr[1], ptr[3] << 8 | ptr[2]);
			break;

		/* Time. */

		case 22:
			unsigned char *ptr = (unsigned char *)&prop->value;

			if (!(prop->str = (char *)calloc(9, sizeof(char))))
				exifdie((const char *)strerror(errno));
			snprintf(prop->str, 9, "%02d:%02d:%02d", ptr[2], ptr[1], ptr[0]);
			break;

		/* White balance. */

		case 3:
			prop->override = EXIF_T_WHITEBAL;
			break;

		/* Color filter. */

		case 41:
			prop->str=strdup(" 0");
			prop->value-=3;
			if (prop->value!=0)
				snprintf(prop->str,3,"%+d",prop->value);
			break;

		/* Scene. */

		case 34:
			prop->override = EXIF_T_SCENECAPTYPE;
			break;

		/* Contrast. */

		case 32:
			prop->str = strdup(" 0");
			prop->value -= 3;
			if (prop->value != 0)
				snprintf(prop->str, 3, "%+d", prop->value);
			prop->override = EXIF_T_CONTRAST;
			break;

		/* Saturation. */

		case 31:
			prop->str=strdup(" 0");
			prop->value -= 3;
			if (prop->value != 0)
				snprintf(prop->str, 3, "%+d", prop->value);
			prop->override = EXIF_T_SATURATION;
			break;
		}
		if (debug)
			minolta_pdump(prop, 1);
	}
}


/* Process Minolta maker note tags. */

void
minolta_prop(struct exifprop *prop, struct exiftags *t)
{
	int i;

	/*
	 * Don't process properties we've created while looking at other
	 * maker note tags.
	 */

	if (prop->tag == EXIF_T_UNKNOWN)
		return;

	/* Lookup the field name (if known). */

	for (i = 0; minolta_tags[i].tag < EXIF_T_UNKNOWN &&
	    minolta_tags[i].tag != prop->tag; i++);
	prop->name = minolta_tags[i].name;
	prop->descr = minolta_tags[i].descr;
	prop->lvl = minolta_tags[i].lvl;

	if (debug)
		minolta_pdump(prop, 0);

	switch (prop->tag) {

	/* Maker note type. */

	case 0x0000:
		if (!(prop->str = (char *)malloc(prop->count + 1)))
			exifdie((const char *)strerror(errno));
		strncpy(prop->str, (const char *)&prop->value, prop->count);
		prop->str[prop->count] = '\0';

		/* We recognize two types: 0TLM and mlt0. */

		if (strcmp(prop->str, "0TLM") && strcmp(prop->str, "mlt0")) {
			exifwarn2("Minolta maker note version not supported",
			    prop->str);
			t->mkrinfo = 0;
		} else
			t->mkrinfo = 1;
		break;

	/*
	 * Various image data.
	 * For now, we only trust specifically-sized tags.
	 */

	case 0x0001:
		if (prop->count != 39 * 4) {
			exifwarn("Minolta maker note version not supported");
			break;
		}
		minolta_cprop(prop, t->btiff + prop->value, t);
		break;

	case 0x0003:
		if (prop->count != 56 * 4 && prop->count != 57 * 4) {
			exifwarn("Minolta maker note version not supported");
			break;
		}
		minolta_cprop(prop, t->btiff + prop->value, t);
		break;
	}
}


/*
 * Try to read a Minolta maker note IFD, which differ by model.
 */

struct ifd *
minolta_ifd(u_int32_t offset, struct exiftags *t)
{

	/* DiMAGE E201. */

	if (!strcmp(t->btiff + offset, "+M")) {
		exifwarn("Minolta maker note version not supported");
		return (NULL);
	}

	/*
	 * Assume that if IFD num > 255, this isn't a real IFD.
	 * Takes care of the unfortunate DiMAGE 2300.
	 */

	if (exif2byte(t->btiff + offset, t->tifforder) > 0xff) {
		exifwarn("Minolta maker note version not supported");
		return (NULL);
	}

	return (readifds(offset, t));
}
