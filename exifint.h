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
 * $Id: exifint.h,v 1.25 2003/08/05 00:40:30 ejohnst Exp $
 */

/*
 * Exchangeable image file format (Exif) parser.
 *
 * Developed using the TIFF 6.0 specification
 * (http://partners.adobe.com/asn/developer/pdfs/tn/TIFF6.pdf)
 * and the EXIF 2.2 standard: (http://tsc.jeita.or.jp/avs/data/cp3451.pdf)
 *
 * Definitions internal to the Exif parsing library.
 *
 */

#ifndef _EXIFINT_H
#define _EXIFINT_H

#include "exif.h"


/* Exif IFD tags. */

#define EXIF_T_EXIFIFD		0x8769
#define EXIF_T_GPSIFD		0x8825
#define EXIF_T_MAKERNOTE	0x927c
#define EXIF_T_INTEROP		0xa005


/* IFD field types. */

struct fieldtype {
	u_int16_t type;
	const char *name;
	size_t size;
};


/* A raw Image File Directory (IFD) entry (12 bytes). */
 
struct field {
	unsigned char tag[2];
	unsigned char type[2];
	unsigned char count[4];
	unsigned char value[4];
};


/* IFD entry. */

struct ifd {
	u_int16_t num;		/* Number of fields. */
	struct field *fields;	/* Array of fields. */
	struct exiftag *tagset;	/* Tag definitions. */

	enum order ifdorder;	/* Endianness of IFD. */
	unsigned char *btiff;	/* Beginning of TIFF. */
	unsigned char *etiff;	/* End of TIFF. */
	struct exifprop *par;	/* Parent property association. */
	struct ifd *next;
};


/* Macro for making sense of a fraction. */

#define fixfract(str, n, d, t)	{ \
	if ((t)) { (n) /= (t); (d) /= (t); } \
	if (!(n)) sprintf((str), "0"); \
	else if (!(d)) sprintf((str), "Infinite"); \
	else if (abs((n)) == abs((d))) sprintf((str), "%d", (n) / (d)); \
	else if (abs((d)) == 1) snprintf((str), 31, "%d", (n) / (d)); \
	else if (abs((n)) > abs((d))) snprintf((str), 31, "%.1f", \
	    (double)(n) / (double)(d)); \
	else if (abs((d)) > 2 && abs((n)) > 1 && \
	    ((double)(n) / (double)(d)) >= 0.1) \
		snprintf((str), 31, "%.1f", (double)(n) / (double)(d)); \
	else snprintf((str), 31, "%d/%d", (n), (d)); \
}


/* The tables from tagdefs.c. */

extern struct fieldtype ftypes[];
extern struct descrip ucomment[];
extern struct descrip flashes[];
extern struct descrip filesrcs[];


/* Utility functions from exifutil.c. */

extern u_int16_t exif2byte(unsigned char *b, enum order o);
extern int16_t exif2sbyte(unsigned char *b, enum order o);
extern u_int32_t exif4byte(unsigned char *b, enum order o);
extern void byte4exif(u_int32_t n, unsigned char *b, enum order o);
extern int32_t exif4sbyte(unsigned char *b, enum order o);
extern char *finddescr(struct descrip *table, u_int16_t val);
extern struct exifprop *newprop(void);
extern struct exifprop *childprop(struct exifprop *parent);
extern void exifstralloc(char **str, int len);
extern void hexprint(unsigned char *b, int len);
extern void dumpprop(struct exifprop *prop, struct field *afield);
extern struct ifd *readifds(unsigned char *beg, unsigned char *end,
    u_int32_t off, struct exiftag *set, enum order o);
extern u_int32_t readifd(unsigned char *b, unsigned char *e, u_int32_t off,
    struct ifd **dir, struct exiftag *set, enum order o);
extern u_int32_t gcd(u_int32_t a, u_int32_t b);

/* Interface to exifgps.c. */

extern struct exiftag gpstags[];
extern void gpsprop(struct exifprop *prop, struct exiftags *t);

#endif
