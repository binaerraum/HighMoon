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

#include <iostream>

#include <SDL/SDL.h>

//#define __DEBUG__
//#define __THREADS__	
#define __LINUX__	// Actually, this has no Effect.
//#define __WIN32__
//#define __MACOSX__

const std::string VERSION="1.1";
const std::string COPYRIGHT="Copyright (c) 2005, Patrick Gerdsmeier <patrick@gerdsmeier.net>";
const std::string WEBSITE="http://highmoon.gerdsmeier.net";

const double PI=3.1415;

// SCREEN
const int SCREENWIDTH=1024; 
const int SCREENHEIGHT=768; 
const int BORDERWIDTH=70;
const int LINEHEIGHT=30;
const int SCROLLBORDERWIDTH=20;		// Screen scrolls if shoot is about that line  
const int SCROLLBACKSPEED=10;  		// Steps to scroll back Screen
const int TICK_INTERVAL_SCREEN=30; 
const int TICK_INTERVAL_GAME=30; 
const int ANIMFRAME=3;			// Normal nth Frame is next Anim
const int BLINKTIME=45;			// For flashing Text
const int SCROLLERSPEED=4;
const int _TITLETEXT=10;

// KEYBOARD. I think this Keyboard functions are Ok:
const int KEY_DECSHOOT=SDLK_LEFT;
const int KEY_INCSHOOT=SDLK_RIGHT;
const int KEY_MOVEUP=SDLK_UP;
const int KEY_MOVEDOWN=SDLK_DOWN;
const int KEY_FIRE=SDLK_SPACE;
const int KEY_QUIT=SDLK_ESCAPE;
const int KEY_WARPGALAXY=SDLK_TAB;
const int KEY_TOGGLEFULLSCREEN=SDLK_f;
const int KEY_TOGGLESCROLLER=SDLK_F1;
const int KEY_TOGGLELANGUAGE=SDLK_F2;
const int KEY_SCREENSHOT=SDLK_F12;
const int KEY_ONEPLAYER=SDLK_1;
const int KEY_TWOPLAYER=SDLK_2;
const int KEY_DEMO=SDLK_3;
const int KEY_STRENGTH=SDLK_c;
const int KEY_TOGGLESOUND=SDLK_s;	// undocumented screenshot feature... =)
const int KEY_TOGGLEHINT=SDLK_h;	// undocumented feature, built in for testing only... =)

// GAME+GALAXY
const int MAXPLAYER=2;			// Don't change!
const int MAXENERGY=100;
const int WINNINGWAIT=500;		// Frames to wait if Game has a Winner

const int MAXHOLE=60;			// Pixels in Blackholes
const int MAXWORM=150;			// Pixels in Wormholes
const int MAXGOLDRAIN=150;		// Pixels in Goldrain
const int MAXSTARS=100;			// Stars in Background

// PLANETS
const int MAXPLANETS=12;		// max. Planets in a Galaxy
const int MINPLANETS=5;			// min. -"-
const int WEIGHT_JUPITER=350;		// Weight is important for the gravity!
const int WEIGHT_EARTH=300;
const int WEIGHT_MARS=200;
const int WEIGHT_MOON=100;
const int WEIGHT_BLACKHOLE=350;
const int WEIGHT_WORMHOLE=50;

// SHOOT
const int MAXSHOOTS=1;			// Shoots per Player (Don't change!)
const int MAXSHOOTPOWER=100;		// 1..100
const int SHOOTPOWERFACTOR=3;
const int MAXSHOOTRUN=800;		// max. Frames a Shoot should run
const int SHOOT_INTERVAL=30; 

// COMPUTER
const int MAXCOMPUTERSEARCH=200; 	// Amount of paths the computer explores for shooting.
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

#define RANDOM(max,min) ((max-min)*(rand()/(RAND_MAX+1.0))+min)

#endif

