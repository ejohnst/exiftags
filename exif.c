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
 * $Id: exif.c,v 1.5 2002/06/30 08:58:48 ejohnst Exp $
 */

/*
 * Exchangeable image file format (Exif) parser.
 *
 * Developed using the TIFF 6.0 specification:
 * (http://partners.adobe.com/asn/developer/pdfs/tn/TIFF6.pdf)
 * and the EXIF 2.1 standard:
 * (http://www.pima.net/standards/it10/PIMA15740/Exif_2-1.PDF)
 *
 * Portions of this code were developed while referencing the public domain
 * 'Jhead' program (version 1.2) by Matthias Wandel <mwandel@rim.net>.
 */


#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <ctype.h>

#include "exiftags.h"
#include "exif.h"


static struct exifprop *head;	/* Start of our property list. */
static void (*mkrprop)();	/* Function for maker note properties. */
static struct ifd *(*mkrifd)();	/* Function for maker note IFDs. */


/* A handful of global variables (used by maker note modules). */

enum order tifforder;		/* Endianness of TIFF. */
unsigned char *btiff;		/* Beginning of TIFF. */
unsigned char *etiff;		/* End of TIFF. */


/* Function prototypes. */

static int parsetag(struct exifprop *prop, struct ifd *dir);


/*
 * Lookup a property entry.
 */
static struct exifprop *
findprop(u_int16_t tag)
{
	struct exifprop *curprop;

	for (curprop = head; curprop && curprop->tag != tag;
	    curprop = curprop->next);
	return (curprop);
}


/*
 * Create an Exif property.
 */
static void
readtag(struct field *afield, int ifdseq, struct ifd *dir)
{
	int i, j;
	struct exifprop *prop, *tmpprop;

	prop = newprop();

	/* Field info. */

	prop->tag = exif2byte(afield->tag);
	prop->type = exif2byte(afield->type);
	prop->count = exif4byte(afield->count);
	prop->value = exif4byte(afield->value);

	/* IFD identifying info. */

	prop->ifdseq = ifdseq;
	prop->ifdtag = dir->tag;

	/* Lookup the field name. */

	for (i = 0; tags[i].tag < EXIF_T_UNKNOWN &&
	    tags[i].tag != prop->tag; i++);
	prop->name = tags[i].name;
	prop->descr = tags[i].descr;
	prop->lvl = tags[i].lvl;

	/* Lookup and check the field type. */

	for (j = 0; types[j].type && types[j].type != prop->type; j++);
	if (!types[j].type)
		exifdie("unknown TIFF field type");

	/*
	 * Ignore sanity checking on maker note tags -- we'll get to them
	 * later.
	 */

	if (prop->ifdtag != EXIF_T_MAKERNOTE) {
		/*
		 * XXX Ignore UserComment -- a hack to get around an apparent
		 * WinXP Picture Viewer bug (err, liberty).  When you rotate
		 * a picture in the viewer, it modifies the IFD1 (thumbnail)
		 * tags to UserComment without changing the type appropriately.
		 */

		if (tags[i].type && tags[i].type != prop->type &&
		   prop->tag != EXIF_T_USERCOMMENT)
			exifwarn2("field type mismatch", prop->name);

		/* Check the field count. */

		if (tags[i].count && tags[i].count != prop->count)
			exifwarn2("field count mismatch", prop->name);
	}

	if (debug)
		printf("   %s (0x%04X): %s, %d, %d\n", prop->name, prop->tag,
		    types[j].name, prop->count, prop->value);

	/*
	 * Do as much as we can with the tag at this point and add it
	 * to our list if it's not an IFD pointer.
	 */

	if (parsetag(prop, dir)) {
		if ((tmpprop = head)) {
			while (tmpprop->next)
				tmpprop = tmpprop->next;
			tmpprop->next = prop;
		} else
			head = prop;
	} else
		free(prop);
}


/*
 * Process the Exif tags for each field of an IFD.
 *
 * Note that this function is only called once per IFD.  Therefore, in order
 * to associate an IFD sequence number with the property, we keep track of
 * the count here.  Root IFDs (0 and 1) are processed first (along with any
 * other "root" IFDs we find), then any nested IFDs in the order they're
 * encountered.
 */
static void
readtags(struct ifd *dir, int seq)
{
	int i;

	if (debug) {
		if (dir->tag != EXIF_T_UNKNOWN) {
			for (i = 0; tags[i].tag < EXIF_T_UNKNOWN &&
			    tags[i].tag != dir->tag; i++);
			printf("Processing %s directory, %d entries\n",
			    tags[i].name, dir->num);
		} else
			printf("Processing directory %d, %d entries\n",
			    seq, dir->num);
	}

	for (i = 0; i < dir->num; i++)
		readtag(&(dir->fields[i]), seq, dir);
	if (debug)
		printf("\n");
}


/*
 * Post-process property values.
 */
static void
postprop(struct exifprop *prop)
{
	struct exifprop *tmpprop;
	u_int32_t val;
	float fval;
	char slop[16];

	/* Process maker note tags specially... */

	if (prop->ifdtag == EXIF_T_MAKERNOTE && mkrprop) {
		mkrprop(prop);
		return;
	}

	switch (prop->tag) {

	case EXIF_T_XRES:
	case EXIF_T_YRES:
	case EXIF_T_FPXRES:
	case EXIF_T_FPYRES:
		if (prop->tag == EXIF_T_XRES || prop->tag == EXIF_T_YRES) {
			if (!(tmpprop = findprop(EXIF_T_RESUNITS))) break;
		} else {
			if (!(tmpprop = findprop(EXIF_T_FPRESUNITS))) break;
		}
		val = exif4byte(btiff + prop->value) /
		    exif4byte(btiff + prop->value + 4);
		snprintf(prop->str, 31, "%d dp%s", val, tmpprop->str);
		prop->str[31] = '\0';
		break;

	case EXIF_T_SHUTTER:
		fval = (float)exif4sbyte(btiff + prop->value) /
		    (float)exif4sbyte(btiff + prop->value + 4);
		/* 1 / (2^speed) */
		snprintf(prop->str, 31, "1/%d", (int)rintf(powf(2, fval)));
		prop->str[31] = '\0';
		/* FALLTHROUGH */

	case EXIF_T_EXPOSURE:
		if (strlen(prop->str) > 27) break;
		strcat(prop->str, " sec");
		break;

	case EXIF_T_FNUMBER:
		fval = (float)exif4byte(btiff + prop->value) /
		    (float)exif4byte(btiff + prop->value + 4);
		snprintf(prop->str, 31, "f/%.1f", fval);
		prop->str[31] = '\0';
		break;

	case EXIF_T_LAPERTURE:
	case EXIF_T_MAXAPERTURE:
		fval = (float)exif4byte(btiff + prop->value) /
		    (float)exif4byte(btiff + prop->value + 4);
		/* sqrt(2)^aperture */
		snprintf(prop->str, 31, "f/%.1f", powf(1.4142, fval));
		prop->str[31] = '\0';
		break;

	/* XXX How do you calculate ExposureBiasValue? */

	case EXIF_T_DISTANCE:
		fval = (float)exif4sbyte(btiff + prop->value) /
		    (float)exif4sbyte(btiff + prop->value + 4);
		snprintf(prop->str, 31, "%.2f m", fval);
		prop->str[31] = '\0';
		break;

	case EXIF_T_FOCALLEN:
		fval = (float)exif4byte(btiff + prop->value) /
		    (float)exif4byte(btiff + prop->value + 4);
		snprintf(prop->str, 31, "%.2f mm", fval);
		prop->str[31] = '\0';
		break;

	case EXIF_T_VERSION:
		/* These contortions are to make 0210 = 2.10. */
		if (!(prop->str = (char *)malloc(8)))
			exifdie((const char *)strerror(errno));
		strncpy(slop, (const char *)(&prop->value), 2);
		strncpy(slop + 3, (const char *)(&prop->value) + 2, 2);
		slop[2] = slop[5] = prop->str[7] = '\0';
		snprintf(prop->str, 7, "%d.%d", atoi(slop), atoi(slop + 3));
		break;
		
	}
}


/*
 * This gives us an opportunity to change the dump level based on
 * property values after all properties are established.
 */
static void
tweaklvl(struct exifprop *prop)
{
	char *c;

	/* Change any ASCII properties to verbose if they're empty. */

	if (prop->type == TIFF_ASCII &&
	    (prop->lvl == ED_CAM || prop->lvl == ED_IMG)) {
		c = prop->str;
		while (c && *c && isspace(*c)) c++;
		if (!c || !*c)
			prop->lvl = ED_VRB;
	}

	/* IFD1 refers to the thumbnail image; we don't really care. */

	if (prop->ifdseq == 1 && prop->lvl != ED_UNK)
		prop->lvl = ED_VRB;
}


/*
 * Fetch the data for an Exif tag.
 */
static int
parsetag(struct exifprop *prop, struct ifd *dir)
{
	int i;
	u_int16_t v = (u_int16_t)prop->value;

	switch (prop->tag) {

	/* Process an Exif IFD. */

	case EXIF_T_EXIFIFD:
	case EXIF_T_GPSIFD:
	case EXIF_T_INTEROP:
		while (dir->next)
			dir = dir->next;

		/*
		 * XXX Olympus cameras don't seem to include a proper offset
		 * at the end of the ExifOffset IFD, so just read one IFD.
		 */

		if (prop->tag == EXIF_T_EXIFIFD)
			readifd(btiff + prop->value, &dir->next);
		else
			dir->next = readifds(prop->value);

		if (!dir->next) {

			/*
			 * XXX Ignore the case where interoperability offset
			 * is invalid.  This appears to be the case with some
			 * Olympus cameras, and we don't want to abort things
			 * things on an IFD we don't really care about anyway.
			 */

			if (prop->tag == EXIF_T_INTEROP)
				break;

			exifdie("invalid Exif format (IFD length mismatch)");
		}

		dir->next->tag = prop->tag;
		return (FALSE);		/* No need to add to property list. */

	/* Process a maker note. */

	case EXIF_T_MAKERNOTE:
		while (dir->next)
			dir = dir->next;

		/*
		 * Try to process maker note IFDs.  If we have a special
		 * function for reading them, use it.  Otherwise, read it
		 * normally.  If we don't get anything, just process the
		 * tag like any other.
		 *
		 * XXX Note that for this to work right, we have to see
		 * the manufacturer tag first to figure out makerifd().
		 */

		if (mkrifd)
			dir->next = mkrifd(prop->value);
		else
			dir->next = readifds(prop->value);

		if (!dir->next)
			break;

		dir->next->tag = prop->tag;
		return (FALSE);		/* No need to add to property list. */

	/* Lookup functions for maker note. */

	case EXIF_T_EQUIPMAKE:
		for (i = 0; makers[i].val != -1; i++)
			if (!strncasecmp(btiff + prop->value, makers[i].name,
			    strlen(makers[i].name)))
				break;
		mkrprop = makers[i].propfun;
		mkrifd = makers[i].ifdfun;
		if (!mkrprop)
			exifwarn2("no maker note support for ",
			    btiff + prop->value);

		/* Keep processing (ASCII value). */
		break;

	/* Lookup strings for various values. */

	case EXIF_T_COMPRESS:
		prop->str = finddescr(compresss, v);
		return (TRUE);
	case EXIF_T_PHOTOINTERP:
		prop->str = finddescr(pixelcomps, v);
		return (TRUE);
	case EXIF_T_ORIENT:
		prop->str = finddescr(orients, v);
		return (TRUE);
	case EXIF_T_PLANARCONFIG:
		prop->str = finddescr(planarconfigs, v);
		return (TRUE);
	case EXIF_T_RESUNITS:
	case EXIF_T_FPRESUNITS:
		prop->str = finddescr(resunits, v);
		return (TRUE);
	case EXIF_T_CHROMRATIO:
		prop->str = finddescr(chromratios, v);
		return (TRUE);
	case EXIF_T_CHROMPOS:
		prop->str = finddescr(chrompos, v);
		return (TRUE);
	case EXIF_T_EXPPROG:
		prop->str = finddescr(expprogs, v);
		return (TRUE);
	case EXIF_T_COMPCONFIG:
		prop->str = finddescr(compconfig, v);
		return (TRUE);
	case EXIF_T_METERMODE:
		prop->str = finddescr(metermodes, v);
		return (TRUE);
	case EXIF_T_LIGHTSRC:
		prop->str = finddescr(lightsrcs, v);
		return (TRUE);
	case EXIF_T_FLASH:
		prop->str = finddescr(flashes, v);
		return (TRUE);
	case EXIF_T_COLORSPC:
		prop->str = finddescr(colorspcs, v);
		return (TRUE);
	case EXIF_T_IMGSENSOR:
		prop->str = finddescr(imgsensors, v);
		return (TRUE);
	case EXIF_T_FILESRC:
		prop->str = finddescr(filesrcs, v);
		return (TRUE);
	case EXIF_T_SCENETYPE:
		prop->str = finddescr(scenetypes, v);
		return (TRUE);
	}

	/*
	 * ASCII types: sanity check the offset.
	 * (InteroperabilityOffset has an odd ASCII value.)
	 */

	if (prop->type == TIFF_ASCII &&
	    (prop->value + prop->count < (u_int32_t)(etiff - btiff))) {
		if (!(prop->str = (char *)malloc(prop->count + 1)))
			exifdie((const char *)strerror(errno));
		strncpy(prop->str, btiff + prop->value, prop->count);
		prop->str[prop->count] = '\0';
		return (TRUE);
	}

	/* Rational types.  (Note that we'll redo some in our later pass.) */

	if ((prop->type == TIFF_RTNL || prop->type == TIFF_SRTNL) &&
	    (prop->value + prop->count * 8 <= (u_int32_t)(etiff - btiff))) {
		if (!(prop->str = (char *)malloc(32)))
			exifdie((const char *)strerror(errno));

		if (prop->type == TIFF_RTNL)
			snprintf(prop->str, 31, "%d/%d",
			    exif4byte(btiff + prop->value),
			    exif4byte(btiff + prop->value + 4));

		else
			snprintf(prop->str, 31, "%d/%d",
			    exif4sbyte(btiff + prop->value),
			    exif4sbyte(btiff + prop->value + 4));
		prop->str[31] = '\0';
		return (TRUE);
	}

	return (TRUE);
}


/*
 * Delete dynamic Exif property memory.
 */
void
exiffree(struct exifprop *list)
{
	struct exifprop *tmpprop;

	while ((tmpprop = list)) {
		if (list->str) free(list->str);
		list = list->next;
		free(tmpprop);
	}
}


/*
 * Scan and parse the Exif section.
 */
struct exifprop *
exifscan(unsigned char *b, int len)
{
	int seq;
	u_int32_t ifdoff;
	struct ifd *ifd0, *curifd, *tmpifd;
	struct exifprop *curprop;

	seq = 0;
	head = NULL;
	etiff = b + len;	/* End of TIFF. */

	/* Make sure we've got the proper Exif header. */

	if (memcmp(b, "Exif\0\0", 6))
		exifdie("invalid Exif header");
	b += 6;

	/* Determine endianness of the TIFF data. */

	if (*((u_int16_t *)b) == 0x4d4d)
		tifforder = BIG;
	else if (*((u_int16_t *)b) == 0x4949)
		tifforder = LITTLE;
	else
		exifdie("invalid TIFF header");

	btiff = b;		/* Beginning of TIFF. */
	b += 2;

	/* Verify the TIFF header. */

	if (exif2byte(b) != 42)
		exifdie("invalid TIFF header");
	b += 2;

	/* Get the 0th IFD, where all of the good stuff should start. */

	ifdoff = exif4byte(b);
	ifd0 = readifds(ifdoff);
	if (!ifd0)
		exifdie("invalid Exif format (IFD0 length mismatch)");

	/* Now, let's parse the fields... */

	curifd = ifd0;
	while ((tmpifd = curifd)) {
		readtags(curifd, seq++);
		curifd = curifd->next;
		free(tmpifd);		/* No need to keep it around... */
	}

	/* Finally, make field values pretty. */

	curprop = head;
	while (curprop) {
		postprop(curprop);
		tweaklvl(curprop);
		curprop = curprop->next;
	}

	return (head);
}
