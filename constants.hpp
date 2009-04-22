/******************************************************************************************
 *
 * HighNoon - Duel in Space
 * Copyright (c) 2005 Patrick Gerdsmeier <patrick@gerdsmeier.net>
 *
 * "constants.cpp"
 * 
 * This file contains important parameters of the game.
 *
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ******************************************************************************************/

#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__

#include <SDL/SDL.h>

//#define __DEBUG__

const double PI=3.1415;

// SCREEN
const int TICK_INTERVAL_SCREEN=30; 
const int TICK_INTERVAL_GAME=30; 
const int SCREENWIDTH=1024; 
const int SCREENHEIGHT=768; 
const int BORDERWIDTH=70; 
const int SCROLLBORDERWIDTH=20;		// Screen scrolls if shoot is about that line  
const int SCROLLBACKSPEED=10;  
const int ANIMFRAME=3;
const int BLINKTIME=45;
const int SCROLLERSPEED=5;
const int _TITLETEXT=9;			// Hmm, actually I don't know how to get the size of an array :)

// KEYBOARD, I think this Keyboard functions are Ok:
const int KEY_DECSHOOT=SDLK_LEFT;
const int KEY_INCSHOOT=SDLK_RIGHT;
const int KEY_MOVEUP=SDLK_UP;
const int KEY_MOVEDOWN=SDLK_DOWN;
const int KEY_FIRE=SDLK_SPACE;
const int KEY_QUIT=SDLK_ESCAPE;
const int KEY_WARPGALAXY=SDLK_TAB;
const int KEY_TOGGLEFULLSCREEN=SDLK_f;
const int KEY_TOGGLESOUND=SDLK_s;
const int KEY_TOGGLEHINT=SDLK_h;	// undocumented feature, built in for testing only... =)
const int KEY_TOGGLESCROLLER=SDLK_F1;
const int KEY_TOGGLELANGUAGE=SDLK_F2;
const int KEY_SCREENSHOT=SDLK_F12;
const int KEY_ONEPLAYER=SDLK_1;
const int KEY_TWOPLAYER=SDLK_2;
const int KEY_DEMO=SDLK_3;

// GAME+GALAXY
const int MAXPLAYER=2;			// Don't change!
const int MAXENERGY=100;
const int SUBENERGY=25;
const int WINNINGWAIT=500;

const int MAXHOLE=80;
const int MAXWORM=40;
const int MAXGOLDRAIN=150;
const int MAXSTARS=100;
const int MAXPLANETS=12;
const int MINPLANETS=5;
const int MAXHOLES=3;			// Max. black-/wormholes (of MAXPLANETS)
const int PLANETTYPES=6;
enum Planettype {
	JUPITER=0,
	EARTH=1,
	MARS=2,
	MOON=3,
	BLACKHOLE=4,
	WORMHOLE=5
};

const int WEIGHT_JUPITER=350;		// Weight is important for the gravity!
const int WEIGHT_EARTH=300;
const int WEIGHT_MARS=200;
const int WEIGHT_MOON=100;
const int WEIGHT_BLACKHOLE=350;
const int WEIGHT_WORMHOLE=25;

// SHOOT
const int MAXSHOOTPOWER=100;
const int SHOOTPOWERFACTOR=3;
const int MAXSHOOTRUN=800;
const int SHOOT_INTERVAL=30; 

// COMPUTER
const int MAXCOMPUTERSEARCH=300; 	// Amount of paths the computer explores for shooting.
const double COMPUTERSTRENGTH=3; 	// 1 - VERY HARD (about 33% Hits!) ... 2 3 4 5 VERY BAD
const int MAXPRECALCPERFRAME=50;
const int MAXPRECALC=MAXSHOOTRUN;	// How deep should the computer think.

// SOUND
const int NUMBEROFCHANNELS=4;
static const int _SOUNDSETNAMES=6;
enum SoundId {
	EXPLOSION=0,
	SHOOT=1,
	WINNINGGAME=2,
	WARPGALAXY=3,
	HITUFO=4,
	NEWGAME=5
};

#endif

