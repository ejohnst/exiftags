# $Id: Makefile,v 1.22 2005/01/05 00:30:32 ejohnst Exp $

#
# Add any new maker note modules here.
#
MKRS=makers.o canon.o olympus.o fuji.o nikon.o casio.o minolta.o \
	sanyo.o asahi.o leica.o panasonic.o sigma.o

#
# Or, if you don't want maker note modules...
#
NOMKRS=makers_stub.o

#
# A few parameters...
#
CC=cc
DEBUG=
CFLAGS=$(DEBUG)
DESTDIR=

prefix=/usr/local
datadir=$(DESTDIR)$(prefix)
bindir=$(DESTDIR)$(prefix)/bin
mandir=$(datadir)/man

OBJS=exif.o tagdefs.o exifutil.o exifgps.o jpeg.o
HDRS=exif.h exifint.h jpeg.h makers.h


.SUFFIXES: .o .c
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<


all: exiftags exifcom exiftime

exiftags: exiftags.o $(OBJS) $(MKRS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ exiftags.o $(OBJS) $(MKRS) -lm

exifcom: exifcom.o $(OBJS) $(NOMKRS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ exifcom.o $(OBJS) $(NOMKRS) -lm

exiftime: exiftime.o timevary.o $(OBJS) $(NOMKRS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ exiftime.o timevary.o $(OBJS) $(NOMKRS) -lm

clean:
	@rm -f $(OBJS) $(MKRS) $(NOMKRS) exiftags.o exifcom.o exiftime.o \
	timevary.o exiftags exifcom exiftime

install: all
	cp exiftags exifcom exiftime $(bindir)
	chmod 0755 $(bindir)/exiftags $(bindir)/exifcom $(bindir)/exiftime
	cp exiftags.1 exifcom.1 exiftime.1 $(mandir)/man1
	chmod 0644 $(mandir)/man1/exiftags.1 $(mandir)/man1/exifcom.1 \
	$(mandir)/man1/exiftime.1
