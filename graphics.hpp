/******************************************************************************************
 *
 * HighNoon - Duell im All
 * Copyright (c) 2005 Patrick Gerdsmeier <patrick@gerdsmeier.net>
 *
 * "graphics.hpp"
 * 
 * This File contains some graphical classes:
 * Font, Sprite, Star and Goldrain
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

#ifndef __GRAPHICS_HPP__
#define __GRAPHICS_HPP__

#include <SDL/SDL.h>

#include "constants.hpp"

/******************************************************************************************
 *
 * Font
 *
 * Font is a class that manages a BMP proportional font.
 *
 ******************************************************************************************/
class Font
{
public:
	Font();
	
	~Font();
	
	void print( int x, int y, std::string txt ); 

	int getWidth( std::string txt );
	
private:
	SDL_Surface *font_image;
	SDL_Rect src, dst;

	int font_pos[26+10];
	int font_width[26+10];
};

/******************************************************************************************
 *
 * Sprite
 *
 * Sprite manages a BMP as a screen sprite
 *
 ******************************************************************************************/
class Sprite
{
public:	
	static int x_offset, y_offset;

	Sprite( char* filename, int frames=1 );

	~Sprite();

	static void setOffset( int x, int y );

	int getWidth();
		
	void setFramerate( int a );

	void setRepeatmode( bool r );

	void resetFrames();
	
 	void setPos( int x, int y );
 	
	void setAlpha( int a );
		
	void draw();
	
	static void putpixel( int x, int y, Uint32 pixel );

private:
	SDL_Surface *sprite_image;
	SDL_Rect sprite_rect, rect;
	int x, y, w, h;
	bool repeat;
	int frames, act_frame, frame_del, frame_rate;
	int alpha;
};

/******************************************************************************************
 *
 * Star
 *
 * Star contains a Star-Structure (for the background)
 *
 ******************************************************************************************/
class Star
{
public:
	Star();

	void draw();
	
private:
	int x, y, b, c, sp;

	int rx();

	int ry();
	
	int color();
	
	int blink();

};

/******************************************************************************************
 *
 * Goldrain
 *
 * Goldrain is a winner goldrain pixel
 *
 ******************************************************************************************/
class Goldrain
{
public:
	Goldrain();
	
	void setOffset(int x, int y);

	void draw();
	
private:
	double x, y, sp, xoffset, yoffset;
	int b, cr, cg, cb ;
	
	int rx();

	int ry();
	
	double speed();

	int color();
	
	int blink();

};

#endif

