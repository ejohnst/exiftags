/*
 * Copyright (c) 2001, Eric M. Johnston <emj@postal.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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
 * $Id: exifutil.c,v 1.2 2002/01/20 23:59:09 ejohnst Exp $
 */

/*
 * Some utilities for dealing with Exif data.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "exifdump.h"
#include "exif.h"


/*
 * Read an unsigned 2-byte int from the buffer.
 */
u_int16_t
exif2byte(unsigned char *b)
{
	if (tifforder == BIG)
		return ((b[0] << 8) | b[1]);
	else
		return ((b[1] << 8) | b[0]);
}


/*
 * Read an unsigned 4-byte int from the buffer.  (XXX Endian test...)
 */
u_int32_t
exif4byte(unsigned char *b)
{
	if (tifforder == BIG)
		return ((b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3]);
	else
		return ((b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0]);
}


/*
 * Read a signed 4-byte int from the buffer.  (XXX Endian test...)
 */
int32_t
exif4sbyte(unsigned char *b)
{
	if (tifforder == BIG)
		return ((b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3]);
	else
		return ((b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0]);
}


/*
 * Lookup description for a value.
 */
char *
finddescr(struct descrip *table, u_int16_t val)
{
	int i;
	char *c;

	for (i = 0; table[i].val != -1 && table[i].val != val; i++);
	if (!(c = (char *)malloc(strlen(table[i].descr) + 1)))
		exifdie((const char *)strerror(errno));
	strcpy(c, table[i].descr);
	return (c);
}


/*
 * Create a new Exif property.
 */
struct exifprop *
newprop(void)
{
	struct exifprop *prop;

	prop = (struct exifprop *)malloc(sizeof(struct exifprop));
	if (!prop)
		exifdie((const char *)strerror(errno));
	memset(prop, 0, sizeof(struct exifprop));

	return (prop);
}


/*
 * Given a parent, create a new child Exif property.
 */
struct exifprop *
childprop(struct exifprop *parent)
{
	struct exifprop *prop;

	prop = newprop();

	/*
	 * Set tag and type to unknown.  We'll key off of the unknown
	 * tag to avoid processing this property again.
	 */

	prop->tag = EXIF_T_UNKNOWN;
	prop->type = TIFF_UNKN;

	/* New property inherits everything else from its parent. */

	prop->name = parent->name;
	prop->descr = parent->descr;
	prop->lvl = parent->lvl;
	prop->ifdseq = parent->ifdseq;
	prop->ifdtag = parent->ifdtag;
	prop->next = parent->next;

	/* Now insert the new property into our list. */

	parent->next = prop;

	return (prop);
}
