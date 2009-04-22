# Makefile for ufo

CFLAGS = -g -O3 -Wall
LIBS = -lSDL_gfx
MAINNAME = ufo
CC=g++

OBJS = main.o vector2d.o

all:	ufo

ufo:	$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(MAINNAME) $(OBJS) $(LIBS)

clean:	
	rm -f *.o
	rm -f *~
