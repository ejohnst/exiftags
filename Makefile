# $Id: Makefile,v 1.10 2002/11/02 22:18:02 ejohnst Exp $

#
# Add any new maker note modules here.
#
MKRS=canon.o olympus.o fuji.o nikon.o casio.o

#
# A few parameters...
#
CC=cc
DEBUG=
CFLAGS=$(DEBUG)
PREFIX=/usr/local

OBJS=exif.o tagdefs.o exifutil.o jpeg.o
HDRS=exif.h exifint.h jpeg.h makers.h


.SUFFIXES: .o .c
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<


all: exiftags exifcom

exiftags: exiftags.o $(OBJS) $(MKRS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ exiftags.o $(OBJS) $(MKRS) -lm

exifcom: exifcom.o $(OBJS) $(MKRS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ exifcom.o $(OBJS) $(MKRS) -lm

clean:
	@rm -f $(OBJS) exiftags.o exifcom.o exiftags exifcom

install: all
	cp exiftags exifcom $(PREFIX)/bin
	chmod a+x $(PREFIX)/bin/exiftags $(PREFIX)/bin/exifcom
	cp exiftags.1 exifcom.1 $(PREFIX)/man/man1
	chmod a+r $(PREFIX)/man/man1/exiftags.1 $(PREFIX)/man/man1/exifcom.1
