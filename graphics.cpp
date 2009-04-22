/******************************************************************************************
 *
 * HighNoon - Duell im All
 * Copyright (c) 2005 Patrick Gerdsmeier <patrick@gerdsmeier.net>
 *
 * "graphics.cpp"
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

#include <iostream>

#include "graphics.hpp"

extern SDL_Surface* MYSDLSCREEN;

Font::Font()
{
	if ( (font_image=SDL_LoadBMP( "gfx/font.bmp" ) ) == NULL ) {
		std::cout << "Error in Font: " << SDL_GetError() << std::endl;
		exit(1);
	}
	
	SDL_SetColorKey(font_image, SDL_SRCCOLORKEY, SDL_MapRGB(font_image->format, 255, 0, 255));
	font_image=SDL_DisplayFormat(font_image);
	
	// 		  A									     Z   0                          9
	int font_size[]={ 6, 6, 6, 6, 6, 5, 6, 6, 2, 5, 6, 5, 8, 6, 6, 6, 6, 5, 6, 6, 6, 6, 8, 6, 6, 6,  6, 2, 6, 5, 6, 6, 6, 6, 6, 6 };
	int p=0;
	
	// Breiten- und Positionlisten des Fonts erstellen
	for (int i=0; i<26+10; i++) {
		font_pos[i]=p;
		font_width[i]=font_size[i]*4;
		p+=font_size[i]*4;
	}
}

Font::~Font() 
{
	SDL_FreeSurface(font_image);
}

void Font::print( int x, int y, std::string txt ) 
{
	
	src.y=0;
	src.h=dst.h=28;
	dst.y=y;
	dst.x=x;
	
	for (int i=0; i<(int)txt.length(); i++) {
		int t=(int)txt[i];
	
		if (t==32) {
			x+=3*4;
		} else {
			int srcp=t-65;
			if (t>=48 && t<48+10) srcp+=65-48+26;
		
			src.x=font_pos[srcp];
			src.w=dst.w=font_width[srcp];
			if (dst.x>SCREENWIDTH || dst.y>SCREENHEIGHT) break; 
			if (dst.x+dst.w>=0 && dst.y+dst.h>=0)
				SDL_BlitSurface( font_image, &src, MYSDLSCREEN, &dst );
			x+=font_width[srcp];
		}	
		dst.x=x;
	}	
}

int Font::getWidth( std::string txt )
{
	int s=0;
	
	for (int i=0; i<(int)txt.length(); i++) {
		int t=(int)txt[i];
		
		if (t==32) s+=3*4;
		else {
			int srcp=t-65;
			if (t>=48 && t<48+10) srcp+=65-48+26;
			
			s+=font_width[srcp];
		}	
	}	

	return s;
}

Sprite::Sprite( char* filename, int frames ) :
	x(0), 
	y(0), 
	repeat(true),
	frames(frames), 
	act_frame(0),
	frame_del(ANIMFRAME),
	frame_rate(ANIMFRAME),
	alpha(255)
{

	if ( (sprite_image=SDL_LoadBMP( filename )) == NULL ) {
		std::cout << "Error in Sprite: " << SDL_GetError() << std::endl;
		exit(1);
	}	

	SDL_SetColorKey(sprite_image, SDL_SRCCOLORKEY, SDL_MapRGB(sprite_image->format, 255, 0, 255));
	sprite_image=SDL_DisplayFormat(sprite_image);

	w=sprite_image->w/frames;
	h=sprite_image->h;
}

Sprite::~Sprite()
{
	SDL_FreeSurface( sprite_image );
}	

void Sprite::setOffset( int x, int y ) { x_offset=x; y_offset=y; }

int Sprite::getWidth() { return w; }
	
void Sprite::setFramerate( int a ) { frame_rate=a; frame_del=a; }

void Sprite::setRepeatmode( bool r ) { repeat=r; }

void Sprite::resetFrames() { act_frame=0; }

void Sprite::setPos( int x, int y )
{
	this->x=x;
	this->y=y;
} 

void Sprite::setAlpha( int a )
{
	this->alpha=a;
} 
	
void Sprite::draw()
{
	rect.x = x+x_offset-w/2;
	rect.y = y+y_offset-w/2;
	rect.w = w;
	rect.h = h;
	sprite_rect.x = act_frame*w;
	sprite_rect.y = 0;
	sprite_rect.w = w;
	sprite_rect.h = h;
	SDL_SetAlpha( sprite_image, SDL_SRCALPHA, alpha);
	SDL_BlitSurface( sprite_image, &sprite_rect, MYSDLSCREEN, &rect );
		
	if (frame_del--<1) {
		frame_del=frame_rate;
		if (++act_frame>=frames) {
			if (repeat) act_frame=0;
			else act_frame=frames-1;
		}	
	}
}

void Sprite::putpixel( int x, int y, Uint32 pixel )
{
	// NOTE: The surface must be locked before calling this!
	if ( x>=0 && x<SCREENWIDTH && y>=0 && y<SCREENHEIGHT) {
		
		int bpp = MYSDLSCREEN->format->BytesPerPixel;
		/* Here p is the address to the pixel we want to set */
		Uint8 *p = (Uint8 *)MYSDLSCREEN->pixels + y * MYSDLSCREEN->pitch + x * bpp;
	
		switch(bpp) {
			case 1:
				*p = pixel;
				break;
	
			case 2:
				*(Uint16 *)p = pixel;
				break;
	
			case 3:
				if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
					p[0] = (pixel >> 16) & 0xff;
					p[1] = (pixel >> 8) & 0xff;
					p[2] = pixel & 0xff;
				} else {
					p[0] = pixel & 0xff;
					p[1] = (pixel >> 8) & 0xff;
					p[2] = (pixel >> 16) & 0xff;
				}
				break;
	
			case 4:
				*(Uint32 *)p = pixel;
				break;
		}
	}
}

int Sprite::x_offset=0;
int Sprite::y_offset=0;

Star::Star() : 
	x(rx()),
	y(ry()),
	b(blink()),
	c(color())
{}

void Star::draw()
{
	if ( b--==0 ) b=blink();
	else if (b>5) {
		Sprite::putpixel( x, y, SDL_MapRGB( MYSDLSCREEN->format, c, c, c ));
	}	
}

int Star::rx() { return (int)((SCREENWIDTH+100)*(rand()/(RAND_MAX+1.0))+50); }

int Star::ry() { return (int)((SCREENHEIGHT+100)*(rand()/(RAND_MAX+1.0))+50); }

int Star::color() {return (int)(220*(rand()/(RAND_MAX+1.0))+25); }

int Star::blink() { return (int)(2000*(rand()/(RAND_MAX+1.0))+1); }

Goldrain::Goldrain() :
	x((double)rx()), 
	y((double)ry()), 
	sp(speed()), 
	xoffset(0),
	yoffset(0),
	b(blink()), 
	cr(color()),
	cg(color()), 
	cb(color())
{}
	
void Goldrain::setOffset(int x, int y) { xoffset=(double)x; yoffset=(double)y; }

void Goldrain::draw()
{
	if ((y+=sp)>100) {
		y-=100;
		x=(double)rx();
	}
	x+=(int)(4*(rand()/(RAND_MAX+1.0))-2);
	
	if ( b--==0 ) b=blink();
	else if (b>1 && y>=0) {
		Sprite::putpixel( (int)(x+xoffset), (int)(y+yoffset),
			SDL_MapRGB( MYSDLSCREEN->format, (int)(cr*y/150)+40, (int)(cg*y/150)+40, (int)(cb*y/150)+40 ));
	}	
}

int Goldrain::rx() { return (int)(80*(rand()/(RAND_MAX+1.0))-40); }

int Goldrain::ry() { return (int)(100*(rand()/(RAND_MAX+1.0))); }

double Goldrain::speed() { return (1*(rand()/(RAND_MAX+1.0))+1); }

int Goldrain::color() {return (int)(255*(rand()/(RAND_MAX+1.0))); }

int Goldrain::blink() { return (int)(10*(rand()/(RAND_MAX+1.0))); }


