# HighMoon - Duel in Space
# Copyright (c) 2005, Patrick Gerdsmeier <patrick@gerdsmeier.net>

# __Something like /usr/local/share/highmoon. All Files (Data and Binary) will be copied there:__ 
INSTALLPATH=/home/pat/HighMoon
# __Set this to a bin-Path. The Installer will create a small Execution-Script in that Path:__
INSTALLBIN=/home/pat/prg/bin

CFLAGS = -g -O3 -Wall `sdl-config --cflags`
CXXFLAGS = $(CFLAGS)
LIBS = -L. `sdl-config --libs` -lSDL_image
MAINNAME = ufo
CC=g++

OBJS = main.o vector_2.o language.o sound.o graphics.o galaxy.o

all:	ufo

ufo:	$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(MAINNAME) $(OBJS) $(LIBS)

clean:        
	rm -f *.o
	rm -f *~

install:
	mkdir -p $(INSTALLBIN)
	mkdir -p $(INSTALLPATH)
	mkdir -p $(INSTALLPATH)/gfx
	mkdir -p $(INSTALLPATH)/snd
	chmod 755 $(INSTALLPATH)/gfx
	chmod 755 $(INSTALLPATH)/snd
	install --strip --mode=755 ufo $(INSTALLPATH)
	install --mode=644 gfx/* $(INSTALLPATH)/gfx
	install --mode=644 snd/* $(INSTALLPATH)/snd
	echo >$(INSTALLBIN)/highmoon '#!/bin/sh'
	echo >>$(INSTALLBIN)/highmoon 'cd $(INSTALLPATH)'
	echo >>$(INSTALLBIN)/highmoon './ufo'
	chmod 755 $(INSTALLBIN)/highmoon

uninstall:
	rm -f $(INSTALLBIN)/highmoon
	rm -f $(INSTALLPATH)/ufo
	rm -f $(INSTALLPATH)/gfx/*
	rm -f $(INSTALLPATH)/snd/*
	rmdir $(INSTALLPATH)/gfx
	rmdir $(INSTALLPATH)/snd
	rmdir $(INSTALLPATH)
	
