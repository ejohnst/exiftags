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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS `AS IS'' AND
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
 *
 */ 



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * minolta.c
 * provides Minolta DiMAGE (5,7,7i and 7Hi) MakerNote support
 * to exiftags utility from Eric M. Johnston
 *
 * History
 * minolta.c v.0.1 08/01/03: TAG 0x03 decoded almost fully.
 *
 *
 * Exif tag definitions for Minolta maker notes.
 * tested aganist  	DiMAGE 7i v1.0e     OK
 *					DiMAGE 7Hi 1.00u	OK
 *					DiMAGE 5			OK except MMN_IMAGESIZE
 *					DiMAGE 7			OK

 *					DiMAGE S404			not supported, has short TAG0x01. Interpreted with errors
 *					DiMAGE S304			not supported, has short TAG0x01. Interpreted with errors
 *					DiMAGE F100			not supported, has no TAG0x01
 *					DIMAGE X			not supported, has no TAG0x01
 *
  * contact jcrespoc@dsland.org
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "makers.h"


/* Minolta Dimage7 MakerNotes Tags
 *
 * TAG 0x00		char	[4]		'MLT0'									OK
 * TAG 0x03		long	[56]	Camera Settings for D7u, D7i, D7Hi		OK almost fully
 * TAG 0x01		long	[56]	Camera Settings for D5, D7				Same format as 0x03
 * TAG 0x10		byte	[11492]	Unknown									
 * TAG 0x20		byte	[394]	Unknown									
 * TAG 0x40		long	[1]		Compressed Image Size					unprocesed
 * TAG 0x88		long	[1]		Thumbnail Offset						unprocesed
 * TAG 0x89		long	[1]		Thumbnail Lenght						unprocesed
 * TAG 0x0e00	byte	[40]	PIM (Print Image Management)			unprocesed
 *
 * This module parses only Tags 0x01 and 0x03.
 */



/* Uncomment this to overwrite standard tags with more apropiated values 
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


/* Standard Tags not defined in exif.c, defined here for overriding */
#define EXIF_T_CONTRAST 0xa408
#define EXIF_T_SATURATION 0xa409
#define EXIF_T_SCENECAPTURETYPE 0xa406

unsigned int CAMERA_MODEL=-1;


enum MINOLTA_TAG_0x03 {
	MMN_CUSTOMTAG=0x0300,
 	MMN_UNKFLD_00=MMN_CUSTOMTAG,
	MMN_EXPPROGRAM,		// Output level set to Unknown, no sense
	MMN_FLASHMODE,
	MMN_WB,
	MMN_IMAGESIZE,		// Output level set to Unknown, no sense
	MMN_IMAGEQUALITY,
	MMN_DRIVEMODE,
	MMN_METERMODE,		// not implemented yet, no sense
	MMN_ISO,			// Output level set to Unknown, no sense
	MMN_EXPOSURETIME,	// Output level set to Unknown, no sense
	MMN_APERTURE,		// Output level set to Unknown, no sense
	MMN_MACRO,
	MMN_DIGZOOM,
	MMN_EXPCOMP,		// Output level set to Unknown, no sense
	MMN_BRACKETSTEP,
	MMN_UNKFLD_15,
	MMN_INTERVALTIME,
	MMN_INTERVALPICS,
	MMN_FOCALLENGHT,	// Output level set to Unknown, no sense
	MMN_FOCUSDISTANCE,
	MMN_FLASH,
	MMN_DATE,
	MMN_TIME,
	MMN_MAXAPERTURE,	// Output level set to Unknown, no sense
	MMN_UNKFLD_24,
	MMN_UNKFLD_25,
	MMN_REMEMBERFILENUMBERING,
	MMN_SEQNUMBER,
	MMN_UNKFLD_28,
	MMN_UNKFLD_29,
	MMN_UNKFLD_30,
	MMN_COL,
	MMN_CON,
	MMN_SHARP,
	MMN_SCENE,
	MMN_FLACHCOMP,
	MMN_ISOSET,
	MMN_CAMERAMODEL,	// Output level set to Unknown, no sense
	MMN_INTERVALMODE,	//<------------------------------------- Last Field for D5, D7
	MMN_FOLDERNAME,
	MMN_COLORMODE,
	MMN_FIL,
	MMN_BWFILTER,
	MMN_INTFLASH,
	MMN_UNKFLD_44,
	MMN_FOCUSX,
	MMN_FOCUSY,
	MMN_WFOCUSZONE,
	MMN_FOCUSMODE,
	MMN_AUTOFOCUSAREA,
	MMN_BRACKETMODE,
	MMN_DATAIMPRINT,
	MMN_UNKFLD_52,
	MMN_UNKFLD_53,
	MMN_UNKFLD_54,
	MMN_UNKFLD_55,		//<------------------------------------- Last Field for D7i
	MMN_UNKFLD_56		//<------------------------------------- Last Field for D7Hi
};

/* Some fields are marked as no sense because it doesn't aports extra information to the standard ones
 * so they are set to ED_UNK in minolta_makernote_tags
 *
 *
 */


/* Bracketing Mode */
static struct descrip minolta_brackmode[] = {
	{ 0, "Exposure" }, 
	{ 1, "Contrast" }, 
	{ 2, "Saturation" }, 
	{ 3, "Filter" }, 
	{ -1, "Unknown" },
};


/* Bracketing Step. */
static struct descrip minolta_bracketstep[] = {
	{ 0, "1/3" }, 
	{ 1, "1/2" }, 
	{ 2, "1" }, 
	{ -1, "Unknown" },
};


/* Sharpnes. */
static struct descrip minolta_sharpnes[] = {
	{ 0, "Normal" }, 
	{ 1, "Hard" }, 
	{ 2, "Soft" }, 
	{ -1, "Unknown" },
};


/* Color Mode. */
static struct descrip minolta_colormode[] = {
	{ 0, "Natural Color" }, 
	{ 1, "B&W" }, 
	{ 2, "Vivid Color" }, 
	{ 3, "Solarization" }, 
	{ 4, "AdobeRGB" },
	{ -1, "Unknown" },
};


/* ISO. */
static struct descrip minolta_isoset[] = {
	{ 0, "100" }, 
	{ 1, "200" }, 
	{ 2, "400" }, 
	{ 3, "800" }, 
	{ 4, "AUTO" }, 
	{ -1, "Unknown" },
};


/* Boolean. */
static struct descrip minolta_bool[] = {
	{ 0, "No" }, 
	{ 1, "Yes" }, 
	{ -1, "Unknown" },
};


/* Focus Mode. */
static struct descrip minolta_focusmode[] = {
	{ 0, "Auto" }, 
	{ 1, "Manual" }, 
	{ -1, "Unknown" },
};


/* Focus Area. */
static struct descrip minolta_focusarea[] = {
	{ 0, "Wide Area" }, 
	{ 1, "Spot Focus Point" },
	{ -1, "Unknown" },
};


/* Wide Area Focus Zone. */
static struct descrip minolta_wfocuszone[] = {
	{ 0, "No Zone" },
	{ 1, "Center" },
	{ 3, "Left" },
	{ 4, "Right" },
	{ -1, "Unknown" },
};


/* Drive mode. */
static struct descrip minolta_drive[] = {
	{ 0, "Single Frame" }, 
	{ 1, "Continous Advance" },
	{ 2, "Self Timer" },
	{ 4, "Bracketing" },
	{ 5, "Interval" },
	{ 6, "UHS" },
	{ 7, "HS" },
	{ -1, "Unknown" },
};


/* White Balance. */
static struct descrip minolta_wb[] = {
	{ 0, "Auto" }, 
	{ 1, "Daylight" },
	{ 2, "Cloudy" },
	{ 3, "Tungsten" },
	{ 5, "Custom" },
	{ 7, "Flourescent" },
	{ 8, "Flourescent2" },
	{ 11, "Custom2" },
	{ 12, "Custom3" },
	{ -1, "Unknown" },
};


/* Flash Modes. */
static struct descrip minolta_flashmode[] = {
	{ 0, "Fill Flash" }, 
	{ 1, "Red Eye" },
	{ 2, "Rear Sync" },
	{ 3, "Remote" },
	{ -1, "Unknown" },
};


/* Capture Scene. */
static struct descrip minolta_scene[] = {
	{ 0, "Normal" },	
	{ 1, "Portrait" },
	{ 2, "Text" }, 
	{ 3, "Night Portrait" },
	{ 4, "Sunset" },
	{ 5, "Sports Action" },
	{ -1, "Unknown" },
};


/* Image Quality. */
static struct descrip minolta_quality[] = {
	{ 0, "RAW" }, 
	{ 1, "Superfine" },
	{ 2, "Fine" },
	{ 3, "Standard" },
	{ 4, "Economy" },
	{ 5, "Extra Fine" },
	{ -1, "Unknown" },
};


/* Exposure Mode. */
static struct descrip minolta_expprogram[] = {
	{ 0, "Program AE" },
	{ 1, "Aperture Priority" },
	{ 2, "Shutter Priority" },
	{ 3, "Manual" },
	{ -1, "Unknown" },
};


/* Image Size. This valid only for 5Mpix cameras (D7's) but is wrong for D5 */
static struct descrip minolta_imgsize[] = {
	{ 0, "2560x1920" }, 
	{ 1, "1600x1200" }, 
	{ 2, "1280x960" }, 
	{ 3, "640x480" }, 
	{ -1, "Unknown" },
};


/* Folder Name. */
static struct descrip minolta_foldername[] = {
	{ 0, "Standard" }, 
	{ 1, "Date" }, 
	{ -1, "Unknown" },
};


/* Data Imprinting. */
static struct descrip minolta_imprint[] = {
	{ 0, "None" },
	{ 1, "DataTime Y/M/D" },
	{ 2, "DataTime M/D H:M" },
	{ 3, "Text" },
	{ 4, "Text + ID" },
	{ -1, "Unknown" },
};


/* Camera Model */
//TODO: Add more entries
static struct descrip minolta_cameramodel[] = {
	{ 0, "DiMAGE 7" },
	{ 1, "DiMAGE 5" },
	{ 4, "DiMAGE 7i" },
	{ 5, "DiMAGE 7Hi" }, 
	{ -1, "Unknown" },
};



static struct exiftag minolta_makernote_tags[] = {
	{ 0x00, TIFF_SHORT, 1, ED_UNK, "MMN_TAG0" , NULL , NULL },	
 	{ 0x01, TIFF_SHORT, 1, ED_UNK, "MMN_TAG1" , NULL , NULL },
	{ 0x03, TIFF_SHORT, 1, ED_UNK, "MMN_TAG3" , NULL , NULL },
	{ 0x10, TIFF_SHORT, 1, ED_UNK, "MMN_TAG10" , NULL , NULL },
	{ 0x20, TIFF_SHORT, 1, ED_UNK, "MMN_TAG20" , NULL , NULL },
	{ 0x40, TIFF_SHORT, 1, ED_UNK, "MMN_TAG40" , NULL , NULL },
	{ 0x88, TIFF_SHORT, 1, ED_UNK, "MMN_TAG88" , NULL , NULL },
	{ 0x89, TIFF_SHORT, 1, ED_UNK, "MMN_TAG89" , NULL , NULL },
	{ 0xe00, TIFF_SHORT, 1, ED_UNK, "MMN_TAGE00" , NULL , NULL },
	{ MMN_UNKFLD_00, TIFF_SHORT, 1, ED_UNK, "MMN_UNKFLD_00" , NULL , NULL },
	{ MMN_EXPPROGRAM, TIFF_SHORT, 1, ED_UNK, "MMN_EXPPROGRAM" , NULL , minolta_expprogram },
	{ MMN_FLASHMODE, TIFF_SHORT, 1, ED_VRB, "MMN_FLASHMODE" , "Flash Mode" , minolta_flashmode },
	{ MMN_WB, TIFF_SHORT, 1, ED_IMG, "MMN_WB" , "White Balance" , minolta_wb },
	{ MMN_IMAGESIZE, TIFF_SHORT, 1, ED_UNK, "MMN_IMAGESIZE", NULL , minolta_imgsize },
	{ MMN_IMAGEQUALITY, TIFF_SHORT, 1, ED_IMG, "MMN_IMAGEQUALITY" , "Image Quality" , minolta_quality },
	{ MMN_DRIVEMODE, TIFF_SHORT, 1, ED_IMG, "MMN_DRIVEMODE" , "Drive Mode" , minolta_drive },
	{ MMN_METERMODE, TIFF_SHORT, 1, ED_UNK, "MMN_METERMODE" , NULL , NULL },
	{ MMN_ISO, TIFF_SHORT, 1, ED_UNK, "MMN_ISO" , NULL , NULL },
	{ MMN_EXPOSURETIME, TIFF_SHORT, 1, ED_UNK, "MMN_EXPOSURETIME" , NULL , NULL },
	{ MMN_APERTURE, TIFF_SHORT, 1, ED_UNK, "MMN_APERTURE" , NULL , NULL },
	{ MMN_MACRO, TIFF_SHORT, 1, ED_IMG, "MMN_MACRO" , "Macro" , minolta_bool },
	{ MMN_DIGZOOM, TIFF_SHORT, 1, ED_IMG, "MMN_DIGZOOM" , "Digital Zoom" , minolta_bool },
	{ MMN_EXPCOMP, TIFF_SHORT, 1, ED_UNK, "MMN_EXPCOMP" , NULL , NULL },
	{ MMN_BRACKETSTEP, TIFF_SHORT, 1, ED_IMG, "MMN_BRACKETSTEP" , "Bracketing Step" , minolta_bracketstep },
	{ MMN_UNKFLD_15, TIFF_SHORT, 1, ED_UNK, "MMN_UNKFLD_15" , NULL , NULL },
	{ MMN_INTERVALTIME, TIFF_SHORT, 1, ED_VRB, "MMN_INTERVALTIME" , "Interval Time" , NULL },
	{ MMN_INTERVALPICS, TIFF_SHORT, 1, ED_VRB, "MMN_INTERVALPICS" , "Interval Pics" , NULL },
	{ MMN_FOCALLENGHT, TIFF_SHORT, 1, ED_UNK, "MMN_FOCALLENGHT" , NULL , NULL },
	{ MMN_FOCUSDISTANCE, TIFF_SHORT, 1, ED_IMG, "MMN_FOCUSDISTANCE" , "Focus Distance" , NULL },
	{ MMN_FLASH, TIFF_SHORT, 1, ED_VRB, "MMN_FLASH" , "Flash Fired" , minolta_bool },
	{ MMN_DATE, TIFF_SHORT, 1, ED_VRB, "MMN_DATE" , "Date" , NULL },
	{ MMN_TIME, TIFF_SHORT, 1, ED_VRB, "MMN_TIME" , "Time" , NULL },
	{ MMN_MAXAPERTURE, TIFF_SHORT, 1, ED_UNK, "MMN_MAXAPERTURE" , NULL , NULL },
	{ MMN_UNKFLD_24, TIFF_SHORT, 1, ED_UNK, "MMN_UNKFLD_24" , NULL , NULL },
	{ MMN_UNKFLD_25, TIFF_SHORT, 1, ED_UNK, "MMN_UNKFLD_25" , NULL , NULL },
	{ MMN_REMEMBERFILENUMBERING, TIFF_SHORT, 1, ED_VRB, "MMN_REMEMBERFILENUMBERING" , "File Number Memory" , minolta_bool },
	{ MMN_SEQNUMBER, TIFF_SHORT, 1, ED_VRB, "MMN_SEQNUMBER" , "Sequence Number" , NULL },
	{ MMN_UNKFLD_28, TIFF_SHORT, 1, ED_UNK, "MMN_UNKFLD_28" , NULL , NULL },
	{ MMN_UNKFLD_29, TIFF_SHORT, 1, ED_UNK, "MMN_UNKFLD_29" , NULL , NULL },
	{ MMN_UNKFLD_30, TIFF_SHORT, 1, ED_UNK, "MMN_UNKFLD_30" , NULL , NULL },
	{ MMN_COL, TIFF_SHORT, 1, ED_IMG, "MMN_COL" , "Saturation" , NULL },
	{ MMN_CON, TIFF_SHORT, 1, ED_IMG, "MMN_CON" , "Contrast" , NULL },
	{ MMN_SHARP, TIFF_SHORT, 1, ED_IMG, "MMN_SHARP" , "Sharp" , minolta_sharpnes },
	{ MMN_SCENE, TIFF_SHORT, 1, ED_IMG, "MMN_SCENE" , "Scene Capture Type" , minolta_scene },
	{ MMN_FLACHCOMP, TIFF_SHORT, 1, ED_IMG, "MMN_FLACHCOMP" , "Flash Compensation" , NULL },
	{ MMN_ISOSET, TIFF_SHORT, 1, ED_VRB, "MMN_ISOSET" , "ISO Camera Setting" , minolta_isoset },
	{ MMN_CAMERAMODEL, TIFF_SHORT, 1, ED_UNK, "MMN_CAMERAMODEL" , NULL , minolta_cameramodel },
	{ MMN_INTERVALMODE, TIFF_SHORT, 1, ED_VRB, "MMN_INTERVALMODE" , "Interval Mode Engaged" , minolta_bool },
	{ MMN_FOLDERNAME, TIFF_SHORT, 1, ED_VRB, "MMN_FOLDERNAME" , "Folder Naming" , minolta_foldername },
	{ MMN_COLORMODE, TIFF_SHORT, 1, ED_IMG, "MMN_COLORMODE" , "Color Mode" , minolta_colormode },
	{ MMN_FIL, TIFF_SHORT, 1, ED_IMG, "MMN_FIL" , "Color Warming" , NULL },
	{ MMN_BWFILTER, TIFF_SHORT, 1, ED_IMG, "MMN_BWFILTER" , "B&W Tone" , NULL },
	{ MMN_INTFLASH, TIFF_SHORT, 1, ED_VRB, "MMN_INTFLASH" , "Internal Flash" , minolta_bool },
	{ MMN_UNKFLD_44, TIFF_SHORT, 1, ED_UNK, "MMN_UNKFLD_44" , NULL , NULL },
	{ MMN_FOCUSX, TIFF_SHORT, 1, ED_VRB, "MMN_FOCUSX" , "AF Point X" , NULL },
	{ MMN_FOCUSY, TIFF_SHORT, 1, ED_VRB, "MMN_FOCUSY" , "AF Point Y" , NULL },
	{ MMN_WFOCUSZONE, TIFF_SHORT, 1, ED_VRB, "MMN_WFOCUSZONE" , "AF Zone" , minolta_wfocuszone },
	{ MMN_FOCUSMODE, TIFF_SHORT, 1, ED_IMG, "MMN_FOCUSMODE" , "Focus Mode" , minolta_focusmode },
	{ MMN_AUTOFOCUSAREA, TIFF_SHORT, 1, ED_IMG, "MMN_AUTOFOCUSAREA" , "Wide Focus Area" , minolta_focusarea },
	{ MMN_BRACKETMODE, TIFF_SHORT, 1, ED_IMG, "MMN_BRACKETMODE" , "Bracketing Mode" , minolta_brackmode },
	{ MMN_DATAIMPRINT, TIFF_SHORT, 1, ED_VRB, "MMN_DATAIMPRINT" , "Data Imprint" , minolta_imprint },
	{ MMN_UNKFLD_52, TIFF_SHORT, 1, ED_UNK, "MMN_UNKFLD_52" , NULL , NULL },
	{ MMN_UNKFLD_53, TIFF_SHORT, 1, ED_UNK, "MMN_UNKFLD_53" , NULL , NULL },
	{ MMN_UNKFLD_54, TIFF_SHORT, 1, ED_UNK, "MMN_UNKFLD_54" , NULL , NULL },
	{ MMN_UNKFLD_55, TIFF_SHORT, 1, ED_UNK, "MMN_UNKFLD_55" , NULL , NULL },
	{ MMN_UNKFLD_56, TIFF_SHORT, 1, ED_UNK, "MMN_UNKFLD_56" , NULL , NULL },
	{ 0xffff, TIFF_SHORT, 0, ED_UNK, "MinoltaUnknown" , "Minolta Tag Unknown" , NULL },
};


/* Just dump bulk data in hexadecimal */
void
minolta_propdump(struct exifprop *prop, char *off, struct exiftags *t)
{
	int n;

	if (!(prop->str=(char *)calloc((5*(prop->count))+1,sizeof(char))))
		exifdie((const char *)strerror(errno));

	for (n=0;n<prop->count;n++)
		snprintf (prop->str+(n*5),6,"0x%02x ",(unsigned char) off[n]);
}


/* Process MakerNote TAGs 0x01 and 0x03 fields
 *
 */
void
minolta_prop3(struct exifprop *prop, char *off, struct exiftags *t)
{
	int n;
	struct exifprop *aprop;

	for (n=0;(4*n)<prop->count;n++)
	{
		int j;

		aprop=childprop(prop);
		aprop->tag=(MMN_CUSTOMTAG | n) ;
		aprop->value=exif4byte(off + (4*n), BIG);

		for (j = 0; minolta_makernote_tags[j].tag < EXIF_T_UNKNOWN && 
			minolta_makernote_tags[j].tag != aprop->tag; j++);

		aprop->name=minolta_makernote_tags[j].name;
		aprop->descr=minolta_makernote_tags[j].descr;
		aprop->lvl=minolta_makernote_tags[j].lvl;
		aprop->type=minolta_makernote_tags[j].type;
		aprop->count=minolta_makernote_tags[j].count;

		if (minolta_makernote_tags[j].table)
			aprop->str=finddescr(minolta_makernote_tags[j].table,aprop->value);

   		switch (aprop->tag)
     	{
      		case MMN_CAMERAMODEL:
				CAMERA_MODEL=aprop->value;
        		break;
        }
	}
}


/* Process Maker Note TAGS and new childs created */
void
minolta_prop(struct exifprop *prop, struct exiftags *t)
{
	if (!(prop->tag & MMN_CUSTOMTAG))
	{
   		int j;

		for (j = 0; minolta_makernote_tags[j].tag < EXIF_T_UNKNOWN &&
			minolta_makernote_tags[j].tag != prop->tag; j++);
        		
		prop->name=minolta_makernote_tags[j].name;
		prop->lvl=minolta_makernote_tags[j].lvl;
	}

	switch (prop->tag)
	{
		/* CHAR.
		 * 4 bytes long, just M L T 0
		 */
		case 0x00:
		{
			if (!(prop->str=(char *)calloc(5,sizeof(char))))
				exifdie((const char *)strerror(errno));

			strncpy(prop->str,(char *)&prop->value,4);
			if (strncmp(prop->str,"0TLM",4) &&	//D7i, D7Hi, S304
   				strncmp(prop->str,"mlt0",4))	//D5, D7, S404
//     			strncmp(prop->str,"MLT0",4))	//F100, X
			{
				char str[255];
				sprintf (str,"Unsupported Minolta Makernote version %s",prop->str);
				exifwarn(str);
				return;
			}
			break;
		}


		/* Camera Settings for D5, D7
  		 * Variable Length:		D5		long[39]
     	 *						D7		long[39]
       	 *						S304	long[43]
       	 *						S404	long[43]
  		 */
		case 0x01:
		{
   			if (prop->count!=39*4)
      		{
      			exifwarn ("Unsupported Minolta MakerNote format");
         		break;
           	}
			prop->str=strdup("Camera Settings");
         	minolta_prop3(prop,t->btiff + prop->value, t);
			break;	
		}


		/* CHAR. Camera Settings in D7u, D7i, D7Hi
  		 * Variable Length:		D7i		long[56]
		 * 						D7Hi    long[57]
		 */
		case 0x03:
		{
   			if (prop->count!=56*4 &&
      			prop->count!=57*4)
         	{
      			exifwarn ("Unsupported Minolta MakerNote format");
         		break;
           	}
			prop->str=strdup("Camera Settings");
			minolta_prop3(prop,t->btiff + prop->value, t);
			break;
		}


		/* CHAR. Unknown, probably image
		 * 11492 bytes long
		 */
		case 0x10:
		{
			/* Too long for dumping */
			if (debug)
				prop->str=strdup("Too long for dumping");
				//minolta_propdump(prop,t->btiff + prop->value, t);
			break;
		} 


		/* CHAR. Unknown 
		 * 394 bytes long
		 */
		case 0x20:
		{
			if (debug)
				minolta_propdump(prop,t->btiff + prop->value, t);
			break;
		}


		case 0x40: /* LONG. CompressedImageSize */
		case 0x88: /* LONG. ThumbOffset */
		case 0x89: /* LONG. ThumbLenght */
			break;


		/* CHAR. Epson PIM 
		 * 40 bytes long
		 */
		case 0x0E00:
		{
			if (debug)
				minolta_propdump(prop,t->btiff + prop->value, t);
			break;
		}
	}



	/* All childs generated. Go with them.
	 * Now we do the adjustment to some fields.
	 *
	 */
	if (prop->tag & MMN_CUSTOMTAG)
	{
		/* Dumps unknown tags values in hex */
		if (prop->lvl==ED_UNK && prop->str==NULL)
		{
			char ptr[1024];
			snprintf (ptr,sizeof(ptr),"%d (0x%02x)",prop->value,prop->value);
			prop->str=strdup(ptr);
		}

		switch (prop->tag)
		{
			case MMN_INTERVALTIME:			
			case MMN_SEQNUMBER:
			{
				prop->value+=1;
				break;
			}

			case MMN_FLACHCOMP:
			case MMN_EXPCOMP:
			{
				prop->str=strdup("-0.7");

				if (prop->value!=6)
					snprintf(prop->str,5,"%0.1f",((double)prop->value-6)/3);
				else
					strcpy(prop->str,"0");
				break;
			}

			case MMN_FOCALLENGHT:
			{
				prop->str=strdup("11.11");
				snprintf(prop->str,6,"%02f",(double)prop->value/256);
				break;
			}

			case MMN_UNKFLD_28:
			case MMN_UNKFLD_29:
			case MMN_UNKFLD_30:
			{
				prop->str=(char *)calloc(12,sizeof(char));
				snprintf(prop->str,12,"%0.6f",(double)prop->value/256);
				break;
			}

			case MMN_ISO:
			{
				unsigned int a=pow(2,((double)prop->value/8)-1)*(double)3.125;
				prop->str=(char *)calloc(4,sizeof(char));
				snprintf(prop->str,4,"%d",a);
				break;				
			}


			case MMN_APERTURE:
			case MMN_MAXAPERTURE:
			{
				double a=pow(2,((double)prop->value/16)-0.5);
				prop->str=(char *)calloc(20,sizeof(char));
				snprintf(prop->str,20,"%0.1f",a);
				break;
			}
		 		
			case MMN_EXPOSURETIME:
			{
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
				if (prop->value<32)
				{
					struct exifprop *tmpprop;

					tmpprop=findprop((struct exifprop *)t->props,EXIF_T_EXPOSURE);
					if (!tmpprop)
				break;
			
					free(tmpprop->str);
					tmpprop->str=strdup(prop->str);
				}
				break;
			}

			case MMN_FOCUSDISTANCE: //Focus distance
			{
				if (!(prop->str=(char *)calloc(20,sizeof(char))))
					exifdie((const char *)strerror(errno));
				if (prop->value==0)
					strcpy(prop->str,"Infinite");
				else	
					snprintf(prop->str,20,"%.1f",(float)(prop->value/(float)1000));
				prop->value/=100;
				break;
			}


			/* Add Flash mode info to standard EXIF_T_FLASH
			 * 			i.e. Flash. Compulsory (Fill Flash)
			 */
			case MMN_FLASHMODE:
			{
				if (REDEFINE_STANDARD)
				{
					struct exifprop *tmpprop;
	
					tmpprop=findprop( (struct exifprop *) t->props, EXIF_T_FLASH );
					if (!tmpprop)
				break;

					/* Flash fired */
					if (tmpprop->value & 0x01)
					{
				if ( !(tmpprop->str=realloc(tmpprop->str,
						(strlen(tmpprop->str) + strlen(prop->str) + 4) * sizeof(char) )) )
				exifdie((const char *)strerror(errno));
				strcat(tmpprop->str," (");
				strcat(tmpprop->str,prop->str);
				strcat(tmpprop->str,")");			
					}
				}
				break;
			}


			/* Redefine EXIF_T_EXPPROG in benefict of custom Exposure Program field
			 * with scene info
			 */
			case MMN_EXPPROGRAM:
			{
				if (prop->value==0 && REDEFINE_STANDARD)
				{
					struct exifprop *tmpprop;
					char *ptr;
	
					if (!(tmpprop=findprop((struct exifprop *)t->props,MMN_SCENE)))
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
			}

			case MMN_DATE:
			{
				unsigned char *ptr=(unsigned char *)&prop->value;
	
				if (!(prop->str=(char *)calloc(11,sizeof(char))))
					exifdie((const char *)strerror(errno));
				snprintf (prop->str,11,"%02d/%02d/%04d",ptr[0],ptr[1],ptr[3]<<8 | ptr[2]);
				break;
			}

			case MMN_TIME:
			{
				unsigned char *ptr=(unsigned char *)&prop->value;

				if (!(prop->str=(char *)calloc(9,sizeof(char))))
					exifdie((const char *)strerror(errno));
				snprintf (prop->str,9,"%02d:%02d:%02d",ptr[2],ptr[1],ptr[0]);
				break;
			}
			
			case MMN_WB:
			{
				prop->override=EXIF_T_WHITEBAL;
				break;
			}

			case MMN_FIL:
			{
				prop->str=strdup(" 0");
				prop->value-=3;
				if (prop->value!=0)
					snprintf(prop->str,3,"%+d",prop->value);
				break;
			}

			/* Replace standard field. More types than supported in standard */
			case MMN_SCENE:
			{
				prop->override=EXIF_T_SCENECAPTURETYPE;
				break;
			}

			/* Replace standard field. More types than supported in standard */
			case MMN_CON:
			{
				prop->str=strdup(" 0");
				prop->value-=3;
				if (prop->value!=0)
					snprintf(prop->str,3,"%+d",prop->value);
				prop->override=EXIF_T_CONTRAST;
				break;
			}

			/* Replace standard field. More types than supported in standard */
			case MMN_COL:
			{
				prop->str=strdup(" 0");
				prop->value-=3;
				if (prop->value!=0)
					snprintf(prop->str,3,"%+d",prop->value);
				prop->override=EXIF_T_SATURATION;
				break;
			}
		}
		if (debug)
			printf(" %s (%d): %d\n", prop->name, prop->tag & (MMN_CUSTOMTAG^0xffff), prop->value);
	}
}

