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
 * $Id: exiftime.c,v 1.3 2004/04/08 07:29:08 ejohnst Exp $
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
static struct vary *v;
static struct linfo *lorder;

#define EXIFTIMEFMT	"%Y:%m:%d %H:%M:%S"
#define EXIFTIMELEN	20

#define ET_CREATE	0x01
#define ET_GEN		0x02
#define ET_DIGI		0x04


/*
 * Some helpful info...
 */
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
	fprintf(stderr, "  -l\tList files ordered by image created timestamp; "
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

	vary_destroy(v);
	exit(1);
}


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
	tp->tm_mon -= 1;

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
 * Grab the specified timestamp and vary it, if necessary.
 * The provided buffer must be at least EXIFTIMELEN bytes.
 */
static int
ettime(char *b, struct exifprop *p)
{
	struct tm tv;
	const struct vary *badv;

	/* Slurp the timestamp into tv. */

	if (!p || !p->str || etstrptm(p->str, &tv))
		return (1);

	/* Apply any adjustments.  (Bad adjustment = fatal.) */

	badv = vary_apply(v, &tv);
	if (badv) {
		exifwarn2("cannot apply adjustment", badv->arg);
		usage();
	}

	if (strftime(b, EXIFTIMELEN, EXIFTIMEFMT, &tv) != EXIFTIMELEN - 1)
		return (1);

	return (0);
}


/*
 * Overwrite a timestamp with an adjusted value (in nt).  Note that we
 * rely on buf being good and EXIFTIMELEN bytes long (including ending zero).
 * XXX Whew -- too many parameters!
 */
static int
writets(FILE *fp, const char *fname, long pos, struct exiftags *t,
    struct exifprop *p, const unsigned char *buf, const char *ttype,
    const char *nt)
{
	int ch, checkch;
	long psave;

	/* Some sanity checking. */

	if (strlen(nt) != EXIFTIMELEN - 1) {
		fprintf(stderr, "%s: invalid %s timestamp (%s)\n",
		    fname, ttype, nt);
		return (1);
	}

	if (!strcmp(nt, p->str)) {
		fprintf(stderr, "%s: new %s timestamp (%s) identical to old\n",
		    nt, ttype, fname);
		return (0);
	}

	/* Prompt user, if desired. */

	if (iflag) {
		if (nflag)
			return (1);
		fprintf(stderr, "change time %s in %s from %s to %s? "
		    "(y/n [n]) ", ttype, fname, p->str, nt);
		checkch = ch = getchar();
		while (ch != '\n' && ch != EOF)
			ch = getchar();
		if (checkch != 'y' && checkch != 'Y') {
			fprintf(stderr, "not adjusted\n");
			return (1);
		}
	}

	/* Remember where we are and move to the comment in our file. */

	psave = ftell(fp);
	if (fseek(fp, pos + (t->md.btiff - buf) + p->value, SEEK_SET))
		exifdie((const char *)strerror(errno));

	/* Write the new timestamp. */

	if (fwrite(nt, EXIFTIMELEN, 1, fp) != 1)
		exifdie((const char *)strerror(errno));

	/* Restore the file pointer. */

	if (fseek(fp, psave, SEEK_SET))
		exifdie((const char *)strerror(errno));

	return (0);
}


/*
 * Process the timestamps.
 */
static int
procts(FILE *fp, const char *fname, long pos, struct exiftags *t,
    const unsigned char *buf)
{
	int found, r, rc;
	char nt[EXIFTIMELEN];
	struct exifprop *p;

	found = rc = 0;

	/*
	 * If ttags = 0, print them all or an error if there are none.
	 * If it's != 0, then the user explicitly requested the timestamp(s),
	 * so print an error for each one that doesn't exist.
	 */

	if (ttags & ET_CREATE || !ttags) {
		p = findprop(t->props, tags, EXIF_T_DATETIME);
		r = ettime(nt, p);

		if (r && ttags) {
			fprintf(stderr, "%s: image created time not "
			    "available\n", fname);
			rc = 1;
		}
		if (!r) {
			found++;
			if (wflag)
				rc = (writets(fp, fname, pos, t, p, buf,
				    "created", nt) || rc);
			printf("%s%s%s\n", p->descr, delim, nt);
		}
	}

	if (ttags & ET_GEN || !ttags) {
		p = findprop(t->props, tags, EXIF_T_DATETIMEORIG);
		r = ettime(nt, p);

		if (r && ttags) {
			fprintf(stderr, "%s: image generated time not "
			    "available\n", fname);
			rc = 1;
		}
		if (!r) {
			found++;
			if (wflag)
				rc = (writets(fp, fname, pos, t, p, buf,
				    "generated", nt) || rc);
			printf("%s%s%s\n", p->descr, delim, nt);
		}
	}

	if (ttags & ET_DIGI || !ttags) {
		p = findprop(t->props, tags, EXIF_T_DATETIMEDIGI);
		r = ettime(nt, p);

		if (r && ttags) {
			fprintf(stderr, "%s: image digitized time not "
			    "available\n", fname);
			rc = 1;
		}
		if (!r) {
			found++;
			if (wflag)
				rc = (writets(fp, fname, pos, t, p, buf,
				    "digitized", nt) || rc);
			printf("%s%s%s\n", p->descr, delim, nt);
		}
	}

	/* No timestamp tags. */

	if (!ttags && !found) {
		fprintf(stderr, "%s: no timestamps available\n", fname);
		return (1);
	}

	return (rc);
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
			fprintf(stderr, "%s: error reading JPEG (length "
			    "mismatch)\n", fname);
			free(exifbuf);
			return (1);
		}

		t = exifscan(exifbuf, len, FALSE);

		if (t && t->props) {
			gotapp1 = TRUE;
			if (lflag)
				rc = listts(t, &lorder[n]);
			else
				rc = procts(fp, fname, app1, t, exifbuf);
		}
		exiffree(t);
		free(exifbuf);
	}

	if (!gotapp1) {
		fprintf(stderr, "%s: couldn't find Exif data\n", fname);
		return (1);
	}

	return (rc);
}


int
main(int argc, char **argv)
{
	register int ch;
	int eval;
	char *rmode, *wmode;
	FILE *fp;

	progname = argv[0];
	debug = FALSE;
	ttags = eval = 0;
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

	for (fnum = 0; *argv; ++argv, fnum++) {

		/* Only open for read+write if we need to. */

		if ((fp = fopen(*argv, wflag ? wmode : rmode)) == NULL) {
			exifwarn2(strerror(errno), *argv);
			eval = 1;
			continue;
		}

		/* Print filenames if more than one. */

		if (argc > 1 && !wflag && !lflag)
			printf("%s%s:\n", fnum == 0 ? "" : "\n", *argv);

		if (lflag)
			lorder[fnum].fn = *argv;

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
		free(lorder);	/* XXX Over in usage()? */
	}

	vary_destroy(v);
	return (eval);
}
