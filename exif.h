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
 * $Id: exif.h,v 1.7 2002/07/12 21:35:11 ejohnst Exp $
 */

/*
 * Exchangeable image file format (Exif) parser.
 *
 * Developed using the TIFF 6.0 specification
 * (http://partners.adobe.com/asn/developer/pdfs/tn/TIFF6.pdf)
 * and the EXIF 2.1 standard
 * (http://www.pima.net/standards/it10/PIMA15740/Exif_2-1.PDF)
 */


#ifndef _EXIF_H
#define _EXIF_H

#include <sys/types.h>

/*
 * XXX Only checking for Solaris now.  Other platforms will probably need
 * this if they don't have u_int16_t or u_int32_t.
 */

#if (defined(sun) && (defined(__svr4__) || defined(__SVR4)))
typedef unsigned short u_int16_t;
typedef unsigned int u_int32_t;
#endif


/* TIFF types. */

#define TIFF_UNKN	0
#define TIFF_BYTE	1
#define TIFF_ASCII	2
#define TIFF_SHORT	3
#define TIFF_LONG	4
#define TIFF_RTNL	5
#define TIFF_SBYTE	6
#define TIFF_UNDEF	7
#define TIFF_SSHORT	8
#define TIFF_SLONG	9
#define TIFF_SRTNL	10
#define TIFF_FLOAT	11
#define TIFF_DBL	12


/* Dump level. */

#define ED_UNK	0x01	/* Unknown or unimplemented info. */
#define ED_CAM	0x02	/* Camera-specific info. */
#define ED_IMG	0x04	/* Image-specific info. */
#define ED_VRB	0x08	/* Verbose info. */


/* Byte order. */

enum order { LITTLE, BIG };


/* Final Exif property info.  (Note: descr can be NULL.) */

struct exifprop {
	u_int16_t tag;		/* The Exif tag. */
	u_int16_t type;
	u_int32_t count;
	u_int32_t value;
	const char *name;
	const char *descr;
	char *str;		/* String representation of value (dynamic). */
	unsigned short lvl;	/* Verbosity level. */
	int ifdseq;		/* Sequence number of parent IFD. */
	u_int16_t ifdtag;	/* Parent IFD tag association. */
	struct exifprop *next;
};


/* Image info and exifprop pointer returned by exifscan(). */

struct exiftags {
	struct exifprop *props;	/* The good stuff. */

	enum order tifforder;	/* Endianness of TIFF. */
	unsigned char *btiff;	/* Beginning of TIFF. */
	unsigned char *etiff;	/* End of TIFF. */
	int mkrval;		/* Maker index (XXX uhh, somewhat opaque). */
	int mkrinfo;		/* Maker info (XXX uhh, a hack for Nikon). */
};


/* Eternal interfaces. */

extern void exiffree(struct exiftags *t);
extern struct exiftags *exifscan(unsigned char *buf, int len);

#endif
