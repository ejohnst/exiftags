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
 * $Id: exiftags.c,v 1.2 2002/01/21 03:21:17 ejohnst Exp $
 */

/*
 * exifdump: dump Exif information embedded in JPEG images.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "exifdump.h"
#include "jpeg.h"
#include "exif.h"


int debug, quiet;
static char *version = "0.8";
static const char *progname;
static int fnum;


void
exifdie(const char *msg)
{
	fprintf(stderr, "%s: %s\n", progname, msg);
	exit(1);
}


void
exifwarn(const char *msg)
{
	fprintf(stderr, "%s: %s\n", progname, msg);
}


void
exifwarn2(const char *msg1, const char *msg2)
{
	fprintf(stderr, "%s: %s (%s)\n", progname, msg1, msg2);
}


static void
printprops(struct exifprop *list, unsigned short lvl)
{
	static int prevf = -1;
	const char *n;

	if (!quiet) {
		if (prevf == fnum)
			printf("\n");
		else
			prevf = fnum;

		switch (lvl) {
		case ED_UNK:
			printf("Unsupported Properties:\n\n");
			break;
		case ED_CAM:
			printf("Camera-Specific Properties:\n\n");
			break;
		case ED_IMG:
			printf("Image-Specific Properties:\n\n");
			break;
		case ED_VRB:
			printf("Other Properties:\n\n");
			break;
		}
	}

	while (list) {
		if (list->lvl == lvl) {
			n = list->descr ? list->descr : list->name;
			if (list->str)
				printf("%s: %s\n", n, list->str);
			else
				printf("%s: %d\n", n, list->value);
		}

		list = list->next;
	}
}


static void
doit(FILE *fp, int dumplvl)
{
	int mark, gotapp1;
	unsigned int len, rlen;
	unsigned char *exifbuf;
	struct exifprop *proplist;

	gotapp1 = FALSE;

	while (jpegscan(fp, &mark, &len)) {
		exifbuf = (unsigned char *)malloc(len);
		if (!exifbuf)
			exifdie((const char *)strerror(errno));

		rlen = fread(exifbuf, 1, len, fp);

		if (rlen != len)
			exifdie("error reading JPEG (length mismatch)");

		if (mark == JPEG_M_APP1) {
			gotapp1 = TRUE;
			proplist = exifscan(exifbuf, len);

			if (proplist) {
				if (dumplvl & ED_CAM)
					printprops(proplist, ED_CAM);
				if (dumplvl & ED_IMG)
					printprops(proplist, ED_IMG);
				if (dumplvl & ED_VRB)
					printprops(proplist, ED_VRB);
				if (dumplvl & ED_UNK)
					printprops(proplist, ED_UNK);
			}
			exiffree(proplist);

			free(exifbuf);
		}
	}

	if (!gotapp1)
		exifdie("couldn't find Exif data");
	/*if (jpeginfo());*/
}


static
void usage()
{
	fprintf(stderr, "Usage: %s [options] [files]\nDisplays Exif data "
	    "from the specified files or standard input.\n", progname);
	fprintf(stderr, "Version: %s\n\n", version);
	fprintf(stderr, "Available options:\n");
	fprintf(stderr, "  -a\tDisplay camera-specific, image-specific, "
	    "and verbose properties.\n");
	fprintf(stderr, "  -c\tDisplay camera-specific properties.\n");
	fprintf(stderr, "  -i\tDisplay image-specific properties.\n");
	fprintf(stderr, "  -v\tDisplay verbose properties.\n");
	fprintf(stderr, "  -u\tDisplay unknown/unsupported properties.\n");
	fprintf(stderr, "  -d\tDisplay parse debug information.\n");
	fprintf(stderr, "  -q\tSuppress section headers.\n");

	exit(1);
}


int
main(int argc, char **argv)
{
	register int ch;
	int dumplvl, eval;
	int fnum;
	FILE *fp;

	progname = argv[0];
	dumplvl = eval = 0;
	debug = quiet = FALSE;

	while ((ch = getopt(argc, argv, "acivudq")) != -1)
		switch (ch) {
		case 'a':
			dumplvl |= (ED_CAM | ED_IMG | ED_VRB);
			break;
		case 'c':
			dumplvl |= ED_CAM;
			break;
		case 'i':
			dumplvl |= ED_IMG;
			break;
		case 'v':
			dumplvl |= ED_VRB;
			break;
		case 'u':
			dumplvl |= ED_UNK;
			break;
		case 'd':
			dumplvl |= ED_UNK;
			debug = TRUE;
			break;
		case 'q':
			quiet = TRUE;
			break;
		case '?':
		default:
			usage();
		}
	argc -= optind;
	argv += optind;

	if (!dumplvl)
		dumplvl |= (ED_CAM | ED_IMG);

	if (*argv) {
		for (fnum = 0; *argv; ++argv) {
			if ((fp = fopen(*argv, "r")) == NULL) {
				exifwarn2(strerror(errno), *argv);
				eval = 1;
				continue;
			}

			fnum++;

			/* Print filenames if more than one. */

			if (argc > 1)
				printf("%s%s:\n", fnum == 1 ? "" : "\n", *argv);

			doit(fp, dumplvl);
			fclose(fp);
		}
        } else
		doit(stdin, dumplvl);

	exit(eval);
}
