/*
 * Copyright (c) 2004, Eric M. Johnston <emj@postal.net>
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
 * $Id: exiftime.c,v 1.1 2004/04/08 04:13:29 ejohnst Exp $
 */

/*
 * exiftime: display or change date & time Exif tags.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* For getopt(). */

#ifndef WIN32
#include <unistd.h>
#else
extern char *optarg;
extern int optind, opterr, optopt;
int getopt(int, char * const [], const char *);
#endif

#include "jpeg.h"
#include "exif.h"
#include "timevary.h"


struct linfo {
	char *fn;
	time_t ts;
};

static const char *version = "0.98";
static int fnum, iflag, lflag, nflag, wflag, ttags;
static const char *delim = ": ";
static struct linfo *lorder;

#define EXIFTIMEFMT	"%Y:%m:%d %H:%M:%S"
#define ET_CREATE	0x01
#define ET_GEN		0x02
#define ET_DIGI		0x04


/*
 * Compare two linfo members for our sort.
 */
int
lcomp(const void *a, const void *b)
{

	if (((struct linfo *)a)->ts < ((struct linfo *)b)->ts)
		return (-1);
	if (((struct linfo *)a)->ts == ((struct linfo *)b)->ts)
		return (0);
	return (1);
}


/*
 * Stuff a standard Exif timestamp into a struct *tm.
 * I've got to say that it's pretty annoying that Win32 doesn't have
 * strptime()...
 */
static int
etstrptm(const char *buf, struct tm *tp)
{
	int n;

	memset(tp, 0, sizeof(struct tm));
	n = sscanf(buf, "%d:%d:%d %d:%d:%d", &tp->tm_year, &tp->tm_mon,
	    &tp->tm_mday, &tp->tm_hour, &tp->tm_min, &tp->tm_sec);
	tp->tm_year -= 1900;

	return (n != 6);
}


/*
 * Grab the timestamps for listing.  Doesn't touch file.
 */
static int
listts(struct exiftags *t, struct linfo *li)
{
	struct exifprop *p;
	struct tm tv;

	/*
	 * Try for DateTime, DateTimeOriginal, then DateTimeDigitized.
	 * If none found, print error and list first.
	 */

	p = findprop(t->props, tags, EXIF_T_DATETIME);

	if (!p || !p->str || etstrptm(p->str, &tv)) {
		p = findprop(t->props, tags, EXIF_T_DATETIMEORIG);

		if (!p || !p->str || etstrptm(p->str, &tv)) {
			p = findprop(t->props, tags, EXIF_T_DATETIMEDIGI);

			if (!p || !p->str || etstrptm(p->str, &tv)) {
				exifwarn("no timestamp available");
				li->ts = 0;
				return (1);
			}
		}
	}

	li->ts = mktime(&tv);
	return (0);
}


/*
 * Display the timestamps.  This function just uses what's returned by
 * exifscan() -- it doesn't touch the file.
 */
static int
printts(struct exiftags *t)
{
	int found, rc;
	struct exifprop *p;

	found = rc = 0;

	/*
	 * If ttags = 0, print them all or an error if there are none.
	 * If it's != 0, then the user explicitly requested the timestamp(s),
	 * so print an error for each one that doesn't exist.
	 */

	if (ttags & ET_CREATE || !ttags) {
		p = findprop(t->props, tags, EXIF_T_DATETIME);

		if (!p || !p->str) {
			if (ttags) {
				exifwarn("image created time not available");
				rc = 1;
			}
		} else {
			found++;
			printf("%s%s%s\n", p->descr, delim, p->str);
		}
	}

	if (ttags & ET_GEN || !ttags) {
		p = findprop(t->props, tags, EXIF_T_DATETIMEORIG);

		if (!p || !p->str) {
			if (ttags) {
				exifwarn("image generated time not available");
				rc = 1;
			}
		} else {
			found++;
			printf("%s%s%s\n", p->descr, delim, p->str);
		}
	}

	if (ttags & ET_DIGI || !ttags) {
		p = findprop(t->props, tags, EXIF_T_DATETIMEDIGI);

		if (!p || !p->str) {
			if (ttags) {
				exifwarn("image digitized time not available");
				rc = 1;
			}
		} else {
			found++;
			printf("%s%s%s\n", p->descr, delim, p->str);
		}
	}

	/* No timestamp tags. */

	if (!ttags && !found) {
		exifwarn("no timestamps available");
		return (1);
	}

	return (rc);
}


/*
 * Blank or write a comment.
 */
static int
writets(FILE *fp, const char *fname, long pos, struct exiftags *t,
    const unsigned char *buf)
{
	u_int32_t l;
	int ch, checkch;
	long psave;
	struct exifprop *p;

	/* No comment tag or it's zero length. */

	if (!p) {
		exifwarn("comment not available");
		return (1);
	}

	if (p->count < 9) {
		exifwarn("comment size zero");
		return (1);
	}

	/* Be careful with existing or unsupported comments. */

#if 0
	if (iflag && *(t->md.btiff + p->value)) {
		if (memcmp(ASCCOM, t->md.btiff + p->value, 8)) {
			if (nflag)
				return (1);
			fprintf(stderr, "overwrite %.8s comment in %s? "
			    "(y/n [n]) ", t->md.btiff + p->value, fname);

			checkch = ch = getchar();
			while (ch != '\n' && ch != EOF)
				ch = getchar();
			if (checkch != 'y' && checkch != 'Y') {
				fprintf(stderr, "not overwritten\n");
				return (1);
			}

		} else if (p->str && *(p->str)) {
			if (nflag)
				return (1);
			fprintf(stderr, "overwrite comment in %s? (y/n [n]) ",
			    fname);

			checkch = ch = getchar();
			while (ch != '\n' && ch != EOF)
				ch = getchar();
			if (checkch != 'y' && checkch != 'Y') {
				fprintf(stderr, "not overwritten\n");
				return (1);
			}
		}
	}
#endif
	/* Remember where we are and move to the comment in our file. */

	psave = ftell(fp);
	if (fseek(fp, pos + (t->md.btiff - buf) + p->value, SEEK_SET))
		exifdie((const char *)strerror(errno));

	/* Write the character code and comment. */
#if 0
	if (com) {
		l = strlen(com);
		if (l > p->count - 8) {
			exifwarn("truncating comment to fit");
			l = p->count - 8;
		}

		/* Character code. */

		if (fwrite(ASCCOM, 8, 1, fp) != 1)
			exifdie((const char *)strerror(errno));

		/* Comment. */

		if (fwrite(com, l, 1, fp) != 1)
			exifdie((const char *)strerror(errno));

		/*
		 * Pad with spaces (this seems to be standard practice).
		 * XXX For now we're not NUL terminating the string.
		 * This doesn't appear to be required by the spec, but it's
		 * always possible that something out there will break.
		 * I've seen some utilities pad with spaces and set the
		 * last byte to NUL.
		 */

		for (l = p->count - 8 - l; l; l--)
			if (fwrite(" ", 1, 1, fp) != 1)
				exifdie((const char *)strerror(errno));
	}
#endif
	/* Restore the file pointer. */

	if (fseek(fp, psave, SEEK_SET))
		exifdie((const char *)strerror(errno));

	return (0);
}


/*
 * Scan the JPEG file for Exif data and parse it.
 */
static int
doit(FILE *fp, const char *fname, int n)
{
	int mark, gotapp1, first, rc;
	unsigned int len, rlen;
	unsigned char *exifbuf;
	struct exiftags *t;
	struct exifprop *p;
	long app1;

	gotapp1 = FALSE;
	first = 0;
	exifbuf = NULL;
	rc = 0;

	while (jpegscan(fp, &mark, &len, !(first++))) {

		if (mark != JPEG_M_APP1) {
			if (fseek(fp, len, SEEK_CUR))
				exifdie((const char *)strerror(errno));
			continue;
		}

		exifbuf = (unsigned char *)malloc(len);
		if (!exifbuf)
			exifdie((const char *)strerror(errno));

		app1 = ftell(fp);
		rlen = fread(exifbuf, 1, len, fp);
		if (rlen != len) {
			exifwarn("error reading JPEG (length mismatch)");
			free(exifbuf);
			return (1);
		}

		t = exifscan(exifbuf, len, FALSE);

		if (t && t->props) {
			gotapp1 = TRUE;
			if (wflag)
				rc = writets(fp, fname, app1, t, exifbuf);
			else if (lflag)
				rc = listts(t, &lorder[n - 1]);
			else
				rc = printts(t);
		}
		exiffree(t);
		free(exifbuf);
	}

	if (!gotapp1) {
		exifwarn("couldn't find Exif data");
		return (1);
	}

	return (rc);
}


static
void usage()
{
	fprintf(stderr, "Usage: %s [options] file ...\n", progname);
	fprintf(stderr, "Displays or adjusts Exif timestamps in the specified "
	    "files.\n");
	fprintf(stderr, "Version: %s\n\n", version);
	fprintf(stderr, "Available options:\n");
	fprintf(stderr, "  -f\tAnswer 'yes' to any confirmation prompts.\n");
	fprintf(stderr, "  -i\tConfirm overwrites (default).\n");
	fprintf(stderr, "  -l\tList files ordered by image created timestamp, "
	    "overrides -t, -v, -w.\n");
	fprintf(stderr, "  -n\tAnswer 'no' to any confirmation prompts.\n");
	fprintf(stderr, "  -t[acdg]\n\tDisplay/adjust specified timestamp(s), "
	    "if they exist:\n");
	fprintf(stderr, "\t  a: All timestamps (default);\n\t  c: Image "
	    "created;\n\t  g: Image generated; or\n\t  d: Image digitized.\n");
	fprintf(stderr, "  -v[+|-]val[ymwdHMS]\n\tAdjust the timestamp(s) by "
	    "the given amount.\n");
	fprintf(stderr, "  -w\tWrite adjusted timestamp(s).\n");
	fprintf(stderr, "  -s\tSet delimiter to provided string "
	    "(default: \": \").\n");

	exit(1);
}


int
main(int argc, char **argv)
{
	register int ch;
	int eval;
	char *rmode, *wmode;
	FILE *fp;
	struct vary *v;

	progname = argv[0];
	ttags = eval = 0;
	debug = FALSE;
	lflag = nflag = wflag = FALSE;
	iflag = TRUE;
	v = NULL;
#ifdef WIN32
	rmode = "rb";
	wmode = "r+b";
#else
	rmode = "r";
	wmode = "r+";
#endif

	while ((ch = getopt(argc, argv, "filnt:v:ws:")) != -1)
		switch (ch) {
		case 'f':
			iflag = FALSE;
			break;
		case 'i':
			iflag = TRUE;
			break;
		case 'l':
			lflag = TRUE;
			break;
		case 'n':
			iflag = nflag = TRUE;
			break;
		case 't':
			if (strchr(optarg, 'c'))
				ttags |= ET_CREATE;
			if (strchr(optarg, 'd'))
				ttags |= ET_DIGI;
			if (strchr(optarg, 'g'))
				ttags |= ET_GEN;
			/* XXX Do we care if an invalid option given? */
			break;
		case 'v':
			v = vary_append(v, optarg);
			break;
		case 'w':
			wflag = TRUE;
			break;
		case 's':
			delim = optarg;
			break;
		case '?':
		default:
			usage();
		}
	argc -= optind;
	argv += optind;

	if (!*argv)
		usage();

	/* Prepare an array for sorting... */

	if (lflag) {
		wflag = 0;
		lorder = (struct linfo *)calloc(argc, sizeof(struct linfo));
		if (!lorder)
			exifdie((const char *)strerror(errno));
	}

	for (fnum = 0; *argv; ++argv) {

		/* Only open for read/write if we need to. */

		if ((fp = fopen(*argv, wflag ? wmode : rmode)) == NULL) {
			exifwarn2(strerror(errno), *argv);
			eval = 1;
			continue;
		}

		fnum++;

		/* Print filenames if more than one. */

		if (argc > 1 && !wflag && !lflag)
			printf("%s%s:\n", fnum == 1 ? "" : "\n", *argv);

		if (lflag)
			lorder[fnum - 1].fn = *argv;

		if (doit(fp, *argv, fnum))
			eval = 1;
		fclose(fp);
	}

	/*
	 * We use mergesort() here (instead of qsort()) because qsort() isn't
	 * stable w/members that compare equal and we exepect the list of
	 * files to frequently already be in order.
	 */
	if (lflag) {
		eval = mergesort(lorder, argc, sizeof(struct linfo), lcomp);
		for (fnum = 0; fnum < argc; fnum++)
			printf("%s\n", lorder[fnum].fn);
	}

	return (eval);
}
