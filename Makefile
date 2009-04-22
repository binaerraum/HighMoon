# Makefile for ufo

CFLAGS = -g -O3 -Wall `sdl-config --cflags`
CXXFLAGS = $(CFLAGS)
LIBS = `sdl-config --libs` -lSDL_gfx
MAINNAME = ufo
CC=g++

OBJS = main.o vector2d.o language.o sound.o graphics.o galaxy.o

all:	ufo

ufo:	$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(MAINNAME) $(OBJS) $(LIBS)

clean:        
	rm -f *.o
	rm -f *~
