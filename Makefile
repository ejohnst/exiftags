# $Id: Makefile,v 1.7 2002/07/11 02:07:21 ejohnst Exp $

#
# Add any new maker note modules here.
#
MKRS=canon.o olympus.o fuji.o nikon.o

#
# A few parameters...
#
CC=cc
DEBUG=-g
CFLAGS=-Wall -O2 $(DEBUG)
PREFIX=/usr/local

OBJS=exiftags.o exif.o tagdefs.o exifutil.o jpeg.o $(MKRS)
HDRS=exif.h exifint.h exiftags.h jpeg.h makers.h


.SUFFIXES: .o .c
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<


all: exiftags

exiftags: $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lm

clean:
	@rm -f $(OBJS) exiftags

install: exiftags
	cp exiftags $(PREFIX)/bin
	chmod a+x $(PREFIX)/bin
	cp exiftags.1 $(PREFIX)/man/man1
	chmod a+r $(PREFIX)/man/man1/exiftags.1
