# $Id: Makefile,v 1.12 2002/11/04 00:16:55 ejohnst Exp $

#
# Add any new maker note modules here.
#
MKRS=makers.o canon.o olympus.o fuji.o nikon.o casio.o

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
PREFIX=/usr/local

OBJS=exif.o tagdefs.o exifutil.o jpeg.o
HDRS=exif.h exifint.h jpeg.h makers.h


.SUFFIXES: .o .c
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<


all: exiftags exifcom

exiftags: exiftags.o $(OBJS) $(MKRS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ exiftags.o $(OBJS) $(MKRS) -lm

exifcom: exifcom.o $(OBJS) $(NOMKRS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ exifcom.o $(OBJS) $(NOMKRS) -lm

clean:
	@rm -f $(OBJS) $(MKRS) $(NOMKRS) exiftags.o exifcom.o exiftags exifcom

install: all
	cp exiftags exifcom $(PREFIX)/bin
	chmod a+x $(PREFIX)/bin/exiftags $(PREFIX)/bin/exifcom
	cp exiftags.1 exifcom.1 $(PREFIX)/man/man1
	chmod a+r $(PREFIX)/man/man1/exiftags.1 $(PREFIX)/man/man1/exifcom.1
