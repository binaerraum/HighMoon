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

#include <SDL/SDL_image.h>

#include "graphics.hpp"

extern SDL_Surface* MYSDLSCREEN;

/******************************************************************************************
 *
 * Font
 *
 ******************************************************************************************/
Font::Font()
{
	verbose( "Initializing Font" );
	
	if ( (font_image=IMG_Load( "gfx/font.gif" ) ) == NULL ) {
		std::cout << "Error in Font: " << SDL_GetError() << std::endl;
		exit(1);
	}
	
	SDL_SetColorKey(font_image, SDL_SRCCOLORKEY, SDL_MapRGB(font_image->format, 255, 0, 255));
	font_image=SDL_DisplayFormat(font_image);
	
	int font_sizes[]={
		0, 2, 4, 0, 6, 0, 0, 2, 3, 3, 0, 6, 2, 5, 2, 0,
		6, 3, 6, 5, 6, 6, 6, 6, 6, 6, 2, 2, 0, 0, 0, 5,
		6, 6, 6, 6, 6, 6, 5, 6, 6, 2, 5, 6, 5, 8, 6, 6,
		6, 6, 5, 6, 6, 6, 6, 8, 6, 6, 6, 0, 0, 0, 0, 0,
		0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int p=0;
	
	// Breiten- und Positionlisten des Fonts erstellen
	for (int i=0; i<96; i++) {
		font_pos[i]=p;
		font_width[i]=font_sizes[i]*4;
		p+=font_sizes[i]*4;
	}

}

Font::~Font() 
{
	SDL_FreeSurface(font_image);
}

void Font::print( int x, int y, std::string txt, int alpha ) 
{
	src.y=0;
	src.h=dst.h=28;
	dst.y=y;
	dst.x=x;
	
	SDL_SetAlpha( font_image, SDL_SRCALPHA, alpha );

	for (int i=0; i<(int)txt.length(); i++) {
		int t=(int)txt[i];
	
		if (t==32) 
			x+=3*4;
		else {
			int srcp=t-32;
			src.x=font_pos[srcp];
			src.w=dst.w=font_width[srcp];

			if ( dst.x>SCREENWIDTH || dst.y>SCREENHEIGHT ) 
				break; 

			if ( dst.x+dst.w>=0 && dst.y+dst.h>=0 && src.w>0 )
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
		
		if (t==32)
			s+=3*4;
		else 
			s+=font_width[t-32];
	}	

	return s;
}

/******************************************************************************************
 *
 * Sprite
 *
 ******************************************************************************************/
Sprite::Sprite( char* filename, int frames ) :
	x(0), 
	y(0),
	alpha(255),
	repeat_mode(true),
	frames(frames), 
	actual_frame(0),
	frame_delay(ANIMFRAME),
	frame_rate(ANIMFRAME)
{
	verbose( "Initializing Sprite: " + std::string(filename) );

	if ( (sprite_image=IMG_Load( filename )) == NULL ) {
		std::cout << "Error in Sprite: " << SDL_GetError() << std::endl;
		exit(1);
	}	

	SDL_SetColorKey(sprite_image, SDL_SRCCOLORKEY, SDL_MapRGB(sprite_image->format, 255, 0, 255));
	sprite_image=SDL_DisplayFormat(sprite_image);

	width=sprite_image->w/frames;
	height=sprite_image->h;
}

Sprite::~Sprite()
{
	SDL_FreeSurface( sprite_image );
}	

void Sprite::setOffset( int x, int y )
{
	x_offset=x; y_offset=y; 
}

bool Sprite::is_onLastFrame()
{
	return ( actual_frame==frames-1 && frame_delay==0 ) ? true : false;
}

int Sprite::getWidth()
{
	return width; 
}
	
void Sprite::setFramerate( int rate )
{	
	frame_rate=rate; 
	frame_delay=rate;
}

void Sprite::setRepeatmode( bool mode )
{
	repeat_mode=mode; 
}

void Sprite::resetFrames() 
{
	actual_frame=0;
}

void Sprite::setPos( int x, int y )
{
	this->x=x;
	this->y=y;
} 

void Sprite::setAlpha( int alpha )
{
	this->alpha=alpha;
} 
	
void Sprite::draw()
{
	rect.x = x+x_offset- width/2;
	rect.y = y+y_offset- width/2;
	rect.w = width;
	rect.h = height;
	sprite_rect.x = actual_frame*width;
	sprite_rect.y = 0;
	sprite_rect.w = width;
	sprite_rect.h = height;

	SDL_SetAlpha( sprite_image, SDL_SRCALPHA, alpha);
	SDL_BlitSurface( sprite_image, &sprite_rect, MYSDLSCREEN, &rect );
		
	if (frame_delay--<1) {
		frame_delay=frame_rate;

		if (++actual_frame>=frames) {

			if (repeat_mode) actual_frame=0;
			else actual_frame=frames-1;
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

/******************************************************************************************
 *
 * Star
 *
 ******************************************************************************************/
Star::Star() 
:
	x(rx()),
	y(ry()),
	b(blink()),
	c(color())
{}

void Star::draw()
{
	if ( b--==0 ) b=blink();

	if (b>5) {
		Sprite::putpixel( x, y, SDL_MapRGB( MYSDLSCREEN->format, c, c, c ));
		
		if (c>200) {
			Sprite::putpixel( x+1, y, SDL_MapRGB( MYSDLSCREEN->format, c/2, c/2, c/2 ));
			Sprite::putpixel( x+1, y+1, SDL_MapRGB( MYSDLSCREEN->format, c/2, c/2, c/2 ));
			Sprite::putpixel( x, y+1, SDL_MapRGB( MYSDLSCREEN->format, c/2, c/2, c/2 ));
		}
	}

}

int Star::rx() 
{
	return (int)RANDOM( SCREENWIDTH-20, 20);
}

int Star::ry()
{
	return (int)RANDOM( SCREENHEIGHT-20, 20);
}

int Star::color() 
{
	return (int)RANDOM( 245,25 );
}

int Star::blink() 
{ 
	return (int)RANDOM( 2000,1 );
}

/******************************************************************************************
 *
 * Shooting-Star
 *
 ******************************************************************************************/
Shootingstar::Shootingstar()
:
	x(rx()),
	y(ry()),
	s(speed()),
	w(wait())
{}

void Shootingstar::draw()
{
	if ( w--==0 ) {
		w=wait();
		x=rx();
		y=ry();
		s=speed();
	}

	double as=(s<0)? -s : s;
	
	if (w<40) {

		for (int i=0; i<10; i++) {
			int c=(int)(((double)w)/40*(i*25));
			if (i==9) c+=10;
			int x=(int)(this->x+s*i);
			int y=(int)(this->y+as*i);
			Sprite::putpixel( x, y, SDL_MapRGB( MYSDLSCREEN->format, c, c, c+20 ));
		}
		x+=s*4;
		y+=as*4;
	}
}

double Shootingstar::rx() 
{
	return (int)RANDOM( SCREENWIDTH-200, 200);
}

double Shootingstar::ry()
{
	return (int)RANDOM( SCREENHEIGHT-200, 100);
}

double Shootingstar::speed()
{
	return ( (int)RANDOM(2,0)==1 ) ? 1 : -1;
}

int Shootingstar::wait() 
{ 
	return (int)RANDOM(2000,1000);
}

/******************************************************************************************
 *
 * Goldrain
 *
 ******************************************************************************************/
Goldrain::Goldrain() 
:
	x((double)rx()), 
	y((double)ry()), 
	sp(speed()), 
	xoffset(0),
	yoffset(0),
	b(blink()),
	cr(0),
	cg(0),
	cb(0)
{
	switch ((int)RANDOM(4,0)) {

		case 0:
			cr=color();
			break;

		case 1:
			cg=color();
			break;

		case 2:
			cg=cr=color();
			break;

		default:
			cb=color();
	}


}
	
void Goldrain::setOffset(int x, int y) 
{
	xoffset=(double)x; 
	yoffset=(double)y; 
}

void Goldrain::draw()
{
	if ((y+=sp)>100) {
		y-=100;
		x=(double)rx();
	}
	
	x+=(int)(4*(rand()/(RAND_MAX+1.0))-2);
	
	if ( b--==0 )
		b=blink();
	else if (b>1 && y>=0) {
		int r=(int)(cr*y/110)+40;
		int g=(int)(cg*y/110)+40;
		int b=(int)(cb*y/110)+40;
		
		Sprite::putpixel( (int)(x+xoffset), (int)(y+yoffset), SDL_MapRGB( MYSDLSCREEN->format, r, g, b ) );
		
		if (r>150 || g>150 || b>130)
			Sprite::putpixel( (int)(x+xoffset+1), (int)(y+yoffset), SDL_MapRGB( MYSDLSCREEN->format, r/2, g/2, b/2 ) );

		if (r>170 || g>170 || b>150)
			Sprite::putpixel( (int)(x+xoffset), (int)(y+yoffset+1), SDL_MapRGB( MYSDLSCREEN->format, r/2, g/2, b/2 ) );

		if (r>200 || g>200 || b>180)
			Sprite::putpixel( (int)(x+xoffset+1), (int)(y+yoffset+1), SDL_MapRGB( MYSDLSCREEN->format, r/2, g/2, b/2 ) );		
	}	
}

double Goldrain::rx() 
{
	return RANDOM( 40,-40 );
}

double Goldrain::ry() 
{
	return RANDOM( 100,0 );
}

double Goldrain::speed()
{
	return RANDOM( 2,1 );
}

int Goldrain::color() 
{
	return (int)RANDOM( 255, 100 );
}

int Goldrain::blink()
{
	return (int)RANDOM( 10,0 );
}

