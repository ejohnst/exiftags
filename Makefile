# $Id: Makefile,v 1.4 2002/06/30 09:01:20 ejohnst Exp $

OBJ=.
SRC=.

all: exiftags

clean:
	rm *.o
	rm exiftags

objs = $(OBJ)/exif.o $(OBJ)/jpeg.o $(OBJ)/exiftags.o $(OBJ)/tagdefs.o \
	$(OBJ)/exifutil.o $(OBJ)/canon.o $(OBJ)/olympus.o

./exiftags.o: ./exiftags.c
	cc -O3 -Wall -c -g $< -o $@

./exif.o: ./exif.c
	cc -O3 -Wall -c -g $< -o $@

./tagdefs.o: ./tagdefs.c
	cc -O3 -Wall -c -g $< -o $@

./exifutil.o: ./exifutil.c
	cc -O3 -Wall -c -g $< -o $@

./jpeg.o: ./jpeg.c
	cc -O3 -Wall -c -g $< -o $@

./canon.o: ./canon.c
	cc -O3 -Wall -c -g $< -o $@

./olympus.o: ./olympus.c
	cc -O3 -Wall -c -g $< -o $@

exiftags: $(objs) exiftags.h jpeg.h exif.h
	cc -g -o exiftags $(objs) -lm

