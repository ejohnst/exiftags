# $Id: Makefile,v 1.2 2002/01/20 23:58:18 ejohnst Exp $

OBJ=.
SRC=.

all: exifdump

clean:
	rm *.o
	rm exifdump

objs = $(OBJ)/exif.o $(OBJ)/jpeg.o $(OBJ)/exifdump.o $(OBJ)/exiftags.o \
	$(OBJ)/exifutil.o $(OBJ)/canon.o

./exif.o: ./exif.c
	cc -O3 -Wall -c -g $< -o $@

./exiftags.o: ./exiftags.c
	cc -O3 -Wall -c -g $< -o $@

./exifutil.o: ./exifutil.c
	cc -O3 -Wall -c -g $< -o $@

./jpeg.o: ./jpeg.c
	cc -O3 -Wall -c -g $< -o $@

./canon.o: ./canon.c
	cc -O3 -Wall -c -g $< -o $@

exifdump: $(objs) exifdump.h jpeg.h exif.h
	cc -g -o exifdump $(objs) -lm

