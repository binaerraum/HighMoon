/******************************************************************************************
 *
 * HighNoon - Duell im All
 * Copyright (c) 2005 Patrick Gerdsmeier <patrick@gerdsmeier.net>
 *
 * "constants.cpp"
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

//#define __DEBUG__

const double PI=3.1415;

// SCREEN
const int TICK_INTERVAL_SCREEN=30; 
const int TICK_INTERVAL_GAME=30; 
const int SCREENWIDTH=1024; 
const int SCREENHEIGHT=768; 
const int BORDERWIDTH=70;  
const int SCROLLBORDERWIDTH=20;  
const int SCROLLBACKSPEED=10;  
const int ANIMFRAME=3;
const int BLINKTIME=50;
const int SCROLLERSPEED=4;
const int _TITLETEXT=9;

// GAME+GALAXY
const int MAXPLAYER=2;
const int MAXENERGY=100;
const int MAXHOLE=80;
const int MAXWORM=40;
const int MAXGOLDRAIN=150;
const int MAXSTARS=100;
const int MAXPLANETS=12;
const int MAXHOLES=3;
const int MINPLANETS=5;
const int PLANETTYPES=6;
const int WINNINGWAIT=500;

// COMPUTER
const int MAXCOMPUTERSEARCH=300;
const double COMPUTERSTRENGTH=3; // 1 - SEHR SCHWER (ca 33% Treffer) ... 2 3 4 5 MIESERABEL

// SHOOT
const int MAXSHOOTPOWER=100;
const int SHOOTPOWERFACTOR=3;
const int MAXSHOOTRUN=800;
const int SHOOT_INTERVAL=30; 

// PRECALCULATION
const int MAXPRECALCPERFRAME=50;
const int MAXPRECALC=MAXSHOOTRUN;

// SOUND
const int NUMBEROFCHANNELS=4;
static const int _SOUNDSETNAMES=6;

#endif

