/******************************************************************************************
 *
 * HighNoon - Duell im All Version 1.0
 * Copyright (c) 2005 Patrick Gerdsmeier <patrick@gerdsmeier.net>
 *
 * "main.cpp"
 * 
 * Beinhaltet momentan:
 *   
 * Soundset
 * steuert die Sounds (via SDL). Sounds koennen ueber
 * deren Indexnummern abgespielt werden.
 *  
 * Font
 * verwaltet den Bitmap-Proportional-Font, mit Funktionen
 * zur Bildschirmausgabe von Strings.
 *   
 * Star
 * Darstellung eines blinkendes Sterns fuer den Hintergrund. 
 *   
 * Goldrain
 * Darstellung eines Konfetti-Regen-Partikels. Goldrain
 * ist der Effekt fuer den Gewinner einer Partie. 
 *   
 * Sprite
 * zur Ausgabe von BMP-Dateien. Animationen und 
 * Alpha-Einstellungen sind einfach moeglich.
 *   
 * Gametimer
 * kleine Klasse zum Timing des Spielflusses. 
 *
 * Planet
 * beinhaltet die Darstellung und einen Kollisions-Check
 * fuer einen Planeten.
 *
 * Galaxy
 * Initialisiert und verwaltet eine Anzahl von Planeten. 
 * Steuert weiterhin deren Bildschirmdarstellung. 
 *
 * Shoot
 * stellt den Schuss dar. Beinhaltet die Funktion der 
 * Gravitationsanimation. Enthaelt weiterhin eine praktische
 * Funktion zum vorberechnen eines kompletten Pfades, sowie
 * gleichzeitigem Test ob sich darauf ein Gegner befindet =)
 *
 * Ufo
 * beinhaltet die Spieler-Steuerung. Ist der Spieler der
 * Computer steuert eine Funktion die Computerzuege. 
 * Die Computerintelligenz besteht z.Z darin, einen
 * Pfad zu erraten der den Gegner unscharf trifft.
 * Bei einer Konstante COMPUTERSTRENGTH von 1.0 bei einer
 * MAXCOMPUTERSEARCH von 300 trifft in etwa jeder dritte Schuss.
 * Betraegt COMPUTERSTRENGTH 3.0 liegt die Trefferrate in etwa
 * bei 20% d.h jedem fuenften Schuss.
 *
 * Explosion 
 * kleine Klasse fuer Explosions-Animationen und Sound.
 *
 * Playfield
 * steuert den Spielfluss samt Tastaturabfragen,
 * Bildschirmdarstellung und Spiellogik. Diese Klasse 
 * instantiiert alle Objekte: Eine Galaxy mit ihren Planeten,
 * eine Explosion, zwei Ufos, einen Schuss, einen Gametimer,
 * eine handvoll Sterne und ein bisschen Goldregen. 
 * Sprites werden i.d.R. von den sie abstrahierenden Objekten
 * instantiiert.
 * Font und Soundset muessen global vorhanden sein.	
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
#include <sstream>
#include <cmath>
#include <stdlib.h>
#include <time.h>

#include <SDL/SDL.h>
#include "SDL/SDL_audio.h"
#include "SDL/SDL_mixer.h"

#include "vector2d.hpp"

//#define __DEBUG__

const double PI=3.1415;

// SCREEN
const int TICK_INTERVAL=25; 
const int SCREENWIDTH=1024; 
const int SCREENHEIGHT=768; 
const int BORDERWIDTH=70;  
const int SCROLLBORDERWIDTH=20;  
const int SCROLLBACKSPEED=10;  
const int ANIMFRAME=2;
const int BLINKTIME=50;
const int SCROLLERSPEED=4;
const std::string TITLETEXT[]={
	"HIGHMOON",
	"",
	"DUELL IM ALL",
	"VON PATRICK GERDSMEIER",
	"JANUAR 2005",
	"LIZENSIERT UNTER DER GPL",
	"",
	"F1 FUER HILFE"	
};		
const int _TITLETEXT=8;
const std::string SCROLLERTEXT="DIE TASTEN  1  2  3  FUER SPIELMODUS   "
	"TAB WECHLSELT DIE GALAXIE   "
	"CURSOR LINKS UND RECHTS STELLT DEN SCHUSSWINKEL EIN   "
	"CURSOR OBEN UND UNTEN BEWEGEN DAS UFO   "
	"LEERTASTE GEDRUECKT LASSEN ZUM ERHOEHEN DER SCHUSSSTAERKE   LEERTASTE LOSLASSEN ZUM FEUERN   "
	"F WECHSELT ZWISCHEN FENSTER UND FULLSCREEN MODUS   "
	"HIGHMOON DAS DUELL IM ALL WURDE GESCHRIEBEN IM JANUAR 2005 VON PATRICK GERDSMEIER UND IST LIZENSIERT UNTER DER GPL   VIEL SPASS   ";

// GAME+GALAXY
const int MAXPLAYER=2;
const int MAXENERGY=100;
const int MAXGOLDRAIN=150;
const int MAXSTARS=150;
const int MAXPLANETS=12;
const int MINPLANETS=5;
const int PLANETTYPES=4;
const int WINNINGWAIT=500;

// COMPUTER
const int MAXCOMPUTERSEARCH=300;
const double COMPUTERSTRENGTH=3; // 1 - SEHR SCHWER (ca 33% Treffer) ... 2 3 4 5 MIESERABEL

// SHOOT
const int MAXSHOOTPOWER=100;
const int SHOOTPOWERFACTOR=3;
const int MAXSHOOTRUN=800;

// PRECALCULATION
const int MAXPRECALCPERFRAME=50;
const int MAXPRECALC=MAXSHOOTRUN;

// SOUND
const int NUMBEROFCHANNELS=4;
char* SOUNDSETNAMES[]={ 
	"snd/explosion.wav",	// 0
	"snd/laser.wav",    	// 1
	"snd/applause.wav",	// 2
	"snd/beam.wav", 	// 3
	"snd/kling.wav", 	// 4
	"snd/pluck.wav" };	// 5
const int _SOUNDSETNAMES=6;

#ifdef __DEBUG__
int __SHOOTS=0, __HITS=0;
#endif

SDL_Surface *MYSDLSCREEN;

//-----------------------------------------------------------------------------------------
class Soundset
//-----------------------------------------------------------------------------------------
{
public:
	struct Sample {
	    Uint8 *data;                                      
	    Uint32 dpos;
	    Uint32 dlen;
	};

	Soundset()
	{
		format.freq = 22050;
		format.format = AUDIO_S16;
		format.channels = 2;
		format.samples = 512;
		format.callback = &(Soundset::mixAudio);
		format.userdata = NULL;
		
		for (int i=0; i<_SOUNDSETNAMES; i++) {
			loadAudio( SOUNDSETNAMES[i], i );
		}

		if ( SDL_OpenAudio(&format, NULL) < 0 ) {
			std::cout << "Error in Sound: " << SDL_GetError() << std::endl;
			exit(1);
		}
		
		SDL_PauseAudio(0);
	}
	
	~Soundset()
	{
		SDL_CloseAudio();
		
		for (int i=0; i<_SOUNDSETNAMES; i++) free( cvt[i].buf ); 
	}
	
	void play(int id)
	{
		if (id>=0 && id<=_SOUNDSETNAMES) {
			int index;
	
			// einen leeren Audio-Slot suchen
			for ( index=0; index<NUMBEROFCHANNELS; ++index ) {
				if ( sounds[index].dpos == sounds[index].dlen ) {
					break;
				}
			}
			
			if ( index < NUMBEROFCHANNELS ) {
				
				SDL_LockAudio();
				sounds[index].data = cvt[id].buf;
				sounds[index].dlen = cvt[id].len_cvt;
				sounds[index].dpos = 0;
				SDL_UnlockAudio();
			}
		}
	}
		
private:
	int amount;
	SDL_AudioSpec format;
	SDL_AudioCVT cvt[_SOUNDSETNAMES];
	static Sample sounds[];
		
	void loadAudio( char *filename, int id )
	{
		SDL_AudioSpec wave;
		Uint8 *data;
		Uint32 dlen;
	
		// Audio-Datei laden und nach 16 Bit und 22KHz wandeln
		if ( SDL_LoadWAV(filename, &wave, &data, &dlen) == NULL ) {
			std::cout << "Error in Sound: " << SDL_GetError() << std::endl;
			exit(1);
		}

		SDL_BuildAudioCVT( &cvt[id], wave.format, wave.channels, wave.freq, AUDIO_S16, 2, 22050 );
		cvt[id].buf = (Uint8*)malloc( dlen*cvt[id].len_mult );
		memcpy( cvt[id].buf, data, dlen );
		
		cvt[id].len = dlen;
		SDL_ConvertAudio(&cvt[id]);
		SDL_FreeWAV(data);
	}

	static void mixAudio( void *data, Uint8 *stream, int length )
	{
		for ( int i=0; i<NUMBEROFCHANNELS; ++i ) {
			Uint32 size = (sounds[i].dlen-sounds[i].dpos);
			if ( size > length ) size = length;
			SDL_MixAudio( stream, &sounds[i].data[ sounds[i].dpos ], size, SDL_MIX_MAXVOLUME );
			sounds[i].dpos += size;
	    	}
	}
} sound;

	Soundset::Sample Soundset::sounds[ NUMBEROFCHANNELS ];

//-----------------------------------------------------------------------------------------
class Font
//-----------------------------------------------------------------------------------------
{
public:
	Font()
	{	
		if ( ( font_image = SDL_LoadBMP( "gfx/font.bmp" ) ) == NULL ) {
        		std::cout << "Error in Font: " << SDL_GetError() << std::endl;
        		exit(1);
    		}
		
		SDL_SetColorKey(font_image, SDL_SRCCOLORKEY, SDL_MapRGB(font_image->format, 255, 0, 255));

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

	void print( int x, int y, std::string txt ) 
	{
		
		src.y=0;
		src.h=dst.h=28;
		dst.y=y;
		dst.x=x;
		
		for (int i=0; i<txt.length(); i++) {
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

	int getWidth( std::string txt )
	{
		int s=0;
		
		for (int i=0; i<txt.length(); i++) {
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
	
private:
	SDL_Surface *font_image;
	SDL_Rect src, dst;

	int font_pos[26+10];
	int font_width[26+10];
} font;

//-----------------------------------------------------------------------------------------
class Sprite
//-----------------------------------------------------------------------------------------
{
public:	
	static int x_offset, y_offset;

	Sprite( char* filename, int frames=1 ) : x(0), y(0), repeat(true), frames(frames), act_frame(0), frame_del(ANIMFRAME), frame_rate(ANIMFRAME), alpha(255)
       	{
		if ( ( sprite_image = SDL_LoadBMP( filename ) ) == NULL ) {
        		std::cout << "Error in Sprite: " << SDL_GetError() << std::endl;
        		exit(1);
    		}	

		SDL_SetColorKey(sprite_image, SDL_SRCCOLORKEY, SDL_MapRGB(sprite_image->format, 255, 0, 255));

		w=sprite_image->w/frames;
		h=sprite_image->h;
	}

	~Sprite()
	{
    		SDL_FreeSurface( sprite_image );
	}	

	static void setOffset( int x, int y ) { x_offset=x; y_offset=y; }

	int getWidth() { return w; }
		
	void setFramerate( int a ) { frame_rate=a; frame_del=a; }

	void setRepeatmode( bool r ) { repeat=r; }

	void resetFrames() { act_frame=0; }
	
 	void setPos( int x, int y )
       	{
		this->x=x;
		this->y=y;
	} 
 	
	void setAlpha( int a )
       	{
		this->alpha=a;
	} 
		
	void draw()
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
	
	static void putpixel( int x, int y, Uint32 pixel )
	{
		// NOTE: The surface must be locked before calling this!
		if ( x>=0 && x<SCREENWIDTH && y>=0 && y<=SCREENHEIGHT) {
			
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


private:
	SDL_Surface *sprite_image;
	SDL_Rect sprite_rect, rect;
	bool repeat;
	int x, y, w, h;
	int alpha;
	int frames, act_frame, frame_del, frame_rate;
};

	int Sprite::x_offset=0;
	int Sprite::y_offset=0;

//-----------------------------------------------------------------------------------------
class Star
//-----------------------------------------------------------------------------------------
{
public:
	Star() : x(rx()), y(ry()), b(blink()), c(color()) {}

	void draw()
	{
		if ( b--==0 ) b=blink();
		else if (b>5) {
			Sprite::putpixel( x, y, SDL_MapRGB( MYSDLSCREEN->format, c, c, c ));
		}	
	}
	
private:
	int x, y, b, c, sp;

	int rx() { return (int)((SCREENWIDTH+100)*(rand()/(RAND_MAX+1.0))+50); }

	int ry() { return (int)((SCREENHEIGHT+100)*(rand()/(RAND_MAX+1.0))+50); }

	int color() {return (int)(220*(rand()/(RAND_MAX+1.0))+25); }
	
	int blink() { return (int)(2000*(rand()/(RAND_MAX+1.0))+1); }

};

//-----------------------------------------------------------------------------------------
class Goldrain
//-----------------------------------------------------------------------------------------
{
public:
	Goldrain() : x((double)rx()), y((double)ry()), xoffset(0), yoffset(0), b(blink()), cr(color()), cg(color()), cb(color()), sp(speed()) {}
	
	void setOffset(int x, int y) { xoffset=(double)x; yoffset=(double)y; }

	void draw()
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
	
private:
	double x, y, sp, xoffset, yoffset;
	int b, cr, cg, cb ;
	
	int rx() { return (int)(80*(rand()/(RAND_MAX+1.0))-40); }

	int ry() { return (int)(100*(rand()/(RAND_MAX+1.0))); }

	double speed() { return (1*(rand()/(RAND_MAX+1.0))+1); }

	int color() {return (int)(255*(rand()/(RAND_MAX+1.0))); }
	
	int blink() { return (int)(10*(rand()/(RAND_MAX+1.0))); }

};

//-----------------------------------------------------------------------------------------
class Gametimer 
//-----------------------------------------------------------------------------------------
{
public:
	Gametimer( Uint32 ticks ) : next_time( SDL_GetTicks() + ticks ), ticks(ticks) {}

	void init()
       	{
	  	next_time = SDL_GetTicks() + ticks;
	}
		
	void delay()
	{
		int delay = next_time-SDL_GetTicks();

		if ( delay<0 ) delay=0;

		SDL_Delay( (Uint32)delay );
        	next_time += ticks;
	}
			      
private:
	Uint32 next_time, ticks;
};

//-----------------------------------------------------------------------------------------
class Planet
//-----------------------------------------------------------------------------------------
{
public:
	Planet( int x, int y, int weight, char* filename ) : x(x), y(y), weight(weight), hit(0), planet(Sprite(filename)) {}
	
	int getX() { return (int)x; }

	int getY() { return (int)y; }

	int getWeight() { return weight; }

	int getWidth() { return planet.getWidth()-2; }

	void setX( int x ) { this->x=(double)x; }

	void setY( int y ) { this->y=(double)y; }

	bool collision( int x, int y, int w, bool active=true )
	{
		bool r=false;
		
		int x2=getX();
		int y2=getY();
		int w2=getWidth();
 		int distCenter=(int)(sqrt((x-x2)*(x-x2)+(y-y2)*(y-y2)));
  		int distRadius=(w+w2)/2;

 		if (distCenter <= distRadius) {
			r=true;	
			
			if (active) {
				hit_x=x2-x;
				hit_y=y2-y;
				hit=10;
			}
		} else hit=0;

		return r;
	}
	
	void draw() 
	{
		animateHit();
		
		planet.setPos((int)x, (int)y);
		planet.draw();
	}	
		
private:
	double x, y;
	int weight;
	Sprite planet;
	int hit;
	double hit_x, hit_y;

	void animateHit()
	{
		if (hit>0) {
			double hh=(cos(45+(90-(double)hit*9)/180*PI)+1)*100/(double)weight;
			Vector2d v=Vector2d(hit_x, hit_y, K).newLength(hh);
			x+=v.getX();
			y+=v.getY();
			--hit;
		}
	}
};

//-----------------------------------------------------------------------------------------
class Galaxy
//-----------------------------------------------------------------------------------------
{
public:
	Galaxy( int planets ) : imploding(false)
	{
		initGalaxy( planets );
	}	
	
	~Galaxy() 
	{
		for (int i=0; i<planets; i++) delete galaxy[i];
	}

	Planet** getPlanetList() { return galaxy; }

	int getPlanets() { return planets; }

	bool isImploding() { return imploding; }
	
	void initGalaxy( int planets )
	{
		if (!imploding) {
			
			for (int i=0; i<planets; i++) delete galaxy[i];

			if (planets>MAXPLANETS) planets=MAXPLANETS;
			
			this->planets=planets;
	
			for (int i=0; i<planets; i++) {
				bool pos_notfound;
				int x, y, p, w;
				char* filename;
	
				do {
					x=(int)((SCREENWIDTH-350)*(rand()/(RAND_MAX+1.0))+175);
					y=(int)(SCREENHEIGHT*(rand()/(RAND_MAX+1.0)));
					p=(int)((PLANETTYPES)*(rand()/(RAND_MAX+1.0)));
					
					switch (p) {
						case 0:
							w=350;
							filename="gfx/jupiter.bmp";
							break;
						case 1:
							w=300;
							filename="gfx/earth.bmp";
							break;
						case 2:
							w=225;
							filename="gfx/mars.bmp";
							break;
						case 3:
							w=100;
							filename="gfx/moon.bmp";
							break;
					}
						
					pos_notfound=false;
					
					for (int j=0; j<i; j++) {
						if (galaxy[j]->collision(x, y, w, false)) {
							pos_notfound=true;
							break;
						}	
					}
				} while (pos_notfound);
				
				galaxy[i]=new Planet(x, y, w, filename);
				wanted_pos[i].y=y;
			}
			
			for (int i=0; i<planets; i++) {
				galaxy[i]->setY(-1000);
			}
			imploding=true;
			
			sound.play(3);
		}
	}

	bool collision( int x, int y, int w, bool active=true )
	{
  		for (int i=0; i<planets; i++) {
			if (galaxy[i]->collision( x, y, w, active )) {
				return true;
			}	
		}
	
		return false;
	}
		
	void draw() 
	{
		if (imploding) imploding=bigBang();
		
		for (int i=0; i<planets; i++) {
			galaxy[i]->draw();
		}
	}
		
private:
	struct { int x, y; } wanted_pos[MAXPLANETS];
	Planet* galaxy[MAXPLANETS];
	int planets;
	bool imploding;	

	bool bigBang() 
	{
		bool moving=false;
		
		if (imploding) {
			for (int i=0; i<planets; i++) {
				int wp=wanted_pos[i].y;
				int pp=galaxy[i]->getY();
				int s;
				
				if (wp>pp) {
					moving=true;
					s=(wp-pp)/10+1;
					pp=pp+s;
					if (wp<pp) {
						pp=wp;
					}
				}
				galaxy[i]->setY(pp);
			}
		}
		
		return moving;
	}
	
};

//-----------------------------------------------------------------------------------------
class Shoot
//-----------------------------------------------------------------------------------------
{
public:
	Shoot() : x(200), y(200), active(false), w(8), preCalculated(0), shoot(Sprite("gfx/shoot.bmp")), shootback(Sprite("gfx/shootback.bmp")), vdir(Vector2d(0,0,K)) ,last_shoot(Vector2d(0,0,K)) , last_angle(Vector2d(0,0,K)) {}
	
	int getX() { return (int)x; }

	int getY() { return (int)y; }

	int getWidth() { return w; }

	bool isActive() { return active; }

	void kill()
	{
		active=false;
	}
	
	void activate( Vector2d p, Vector2d v )
	{
		vdir=v;
		this->x=p.getX();
		this->y=p.getY();
		lastx=x;
		lasty=y;
		active=true;
		shoot_time=MAXSHOOTRUN;
		sound.play(1);
		
		#ifdef __DEBUG
		__SHOOTS++;
		#endif
	}
	
	void draw()
	{
		if (isActive()) {
			int blink=(int)(100*(rand()/(RAND_MAX+1.0)));
	
			if (shoot_time>100 || blink<(shoot_time/2)+25) {
				
				int xp=(int)(3*(rand()/(RAND_MAX+1.0))-2);
				int yp=(int)(3*(rand()/(RAND_MAX+1.0))-2);
				int ap=(int)(20*(rand()/(RAND_MAX+1.0))-15);
				
				shootback.setAlpha(130+ap);
				shootback.setPos((int)lastx-xp, (int)lasty-yp);
				shootback.draw();
				shootback.setAlpha(160+ap);
				shootback.setPos((int)x+xp, (int)y+yp);
				shootback.draw();
				shoot.setPos((int)x, (int)y);
				shoot.draw();
			}
		}
	}
	
	void drawPrePath( Vector2d vecShootPos, Vector2d vecShoot, Galaxy *galaxy )
	{
		static int colorpos=50;
		if ((colorpos-=20)<50) colorpos=255;
		
		preCalculatePath(vecShootPos, vecShoot, galaxy );
		
		int c=colorpos;
		
		for (int i=0; i<preCalculated; i++) {
			Sprite::putpixel( preCalculatedPos[i].x, preCalculatedPos[i].y, SDL_MapRGB( MYSDLSCREEN->format, c, c, c ) );
			if ((c+=30)>255) c=50;
		}
	}
	
	bool hitEnemy( Vector2d vecShootPos, Vector2d vecShoot, Galaxy *galaxy, int target_x, int target_y, int target_width )
	{
		preCalculatePath( vecShootPos, vecShoot, galaxy );

		int w2=getWidth();

		for (int i=0; i<preCalculated; i++) {
			int x2=preCalculatedPos[i].x;
			int y2=preCalculatedPos[i].y;		
			int distCenter=(int)(sqrt((target_x-x2)*(target_x-x2)+(target_y-y2)*(target_y-y2)));
			int distRadius=(w2+target_width)/2;

			if (distCenter <= distRadius) {
				return true;
			}
		}
		
		return false;		
	}
	
	bool animate( Planet* galaxy[], int planets ) 
	{
 		if ( isActive() ) {
		
			static double last_time;
			
			if (shoot_time==MAXSHOOTRUN) last_time=SDL_GetTicks();
			
			if (shoot_time--<0) {
				kill();
				return false;
			} else {		
			
				Vector2d vecShootPos=Vector2d(x, y, K);
				Vector2d vecShoot=vdir;
				
				lastx=x;
				lasty=y;
	
				double now_time=SDL_GetTicks();
				double elapsed_time=now_time-last_time;
				last_time=now_time;
				elapsed_time=TICK_INTERVAL;
				
				getNextShootStatus(vecShootPos, vdir, galaxy, planets, elapsed_time);

				x=vecShootPos.getX();
				y=vecShootPos.getY();
			}
			
		}

		return true;
	}	
	
private:
	struct { int x, y; } preCalculatedPos[MAXPRECALC];
	int preCalculated;
	Vector2d vdir, last_shoot, last_angle;	
	bool active;
	double x, y, lastx, lasty;
	int w;
	int shoot_time;
	Sprite shoot, shootback;
	
	bool preCalculatePath( Vector2d &vecShootPos, Vector2d &vecShoot, Galaxy *galaxy ) 
	{
		//if ((preCalculated+=MAXPRECALCPERFRAME)>MAXPRECALC)
		preCalculated=0;		

		if (last_angle!=vecShoot || last_shoot!=vecShootPos ) {
			for (int i=0; i<MAXPRECALC; i++) {
				getNextShootStatus(vecShootPos, vecShoot, galaxy->getPlanetList(), galaxy->getPlanets(), TICK_INTERVAL);
				int x=(int)vecShootPos.getX();
				int y=(int)vecShootPos.getY();
				if (!galaxy->collision( x, y, w, false )) {
					preCalculatedPos[i].x=x;
					preCalculatedPos[i].y=y;
					preCalculated++;
				} else break;	
			}
		}
	}

	void getNextShootStatus( Vector2d &vecShootPos, Vector2d &vecShoot, Planet *galaxy[], int planets, double timeElapsed )
	{
		Vector2d vecToPlanets=Vector2d(0, 0, K);

		for (int i=0; i<planets; i++) {
			double posx=galaxy[i]->getX();
			double posy=galaxy[i]->getY();
			double weight=galaxy[i]->getWeight();
	
			// Vektor zum aktuellen Planeten
			Vector2d vecPlanetPos=Vector2d(posx, posy, K);
			// Abstand zu unserem Schuss
			double distance=vecPlanetPos.distance(vecShootPos);
			// Vektor vom Schuss zum Planeten:
			Vector2d vecToPlanet=vecPlanetPos-vecShootPos;
			// Neue Laenge fuer den Vector
			// (Anziehungskraft ergibt sich physikalisch ;)
			vecToPlanet=vecToPlanet.newLength(weight/distance);
			// gemeinsamen Anziehungsvektor aller Planeten erstellen:
			vecToPlanets=vecToPlanets+vecToPlanet;
		}
		// Alle Anziehungsvektoren werden zum Schussvektor addiert
		vecShoot=vecShoot+vecToPlanets;

		// Neuen Schussvektor zur Schussposition addieren:
		vecShootPos+=vecShoot.newLength( vecShoot.getLength()*timeElapsed/1000);
	}
};

//-----------------------------------------------------------------------------------------
class Ufo
//-----------------------------------------------------------------------------------------
{
public:
	Ufo( int x, int y, double sa, bool human ) : x(x), y(y), human(human), shoot_angle(sa), computer_mode(NONE), energy(MAXENERGY),  shoot_power(0), c_think(Sprite("gfx/c_thinking.bmp", 2)), c_shoot(Sprite("gfx/c_shooting.bmp", 2)), cpkt_sprite(getCirclePktSprite()), ufo_sprite(getUfoSprite()) 
       	{
		c_think.setFramerate(25);
		c_think.setAlpha(150);
		c_shoot.setFramerate(25);
		c_shoot.setAlpha(150);
	}

	bool isDead() { return (energy<=0) ? true : false ; }

	bool isActive() { return active; }

	bool isComputer() { return !human; }

	int getX() { return x; }	

	int getY() { return y; }

	int getWidth() { return 50; }

	int getEnergy() { return energy; }

	double getShootAngle() { return shoot_angle; }

	Vector2d getShootVector() { return Vector2d( shoot_power*SHOOTPOWERFACTOR, shoot_angle, P); }	

	Vector2d getShootStart()
       	{ 
		Vector2d me=Vector2d(x,y,K);
		Vector2d p=Vector2d(60, shoot_angle, P);
		Vector2d pos=me+p;
		
		return pos;
       	}	

	void resetEnergy() { energy=MAXENERGY; }

	void subEnergy( int e ) { energy-=e; }

	void incShootAngle() { shoot_angle+=PI/180; }

	void decShootAngle() { shoot_angle-=PI/180; }

	void incShootPower()
       	{
	       	shoot_power+=1;
		if (shoot_power>MAXSHOOTPOWER) shoot_power=MAXSHOOTPOWER;
	}
	
	void activate() { active=true; shoot_power=0; }

	void inactivate() { active=false; }
	
	void moveN() { if (y>BORDERWIDTH) y=y-2; }	

	void moveS() { if (y<SCREENHEIGHT-BORDERWIDTH) y=y+2; }	

	void setComputer( int computer ) { human=!computer; computer_mode=NONE; }

	void setPos( int x, int y )
       	{
		this->x=x;
		this->y=y;
	}
	
	bool collision( int x, int y, int w )
	{
		bool r=false;
		
		int x2=getX();
		int y2=getY();
		int w2=getWidth();
 		int distCenter=(int)(sqrt((x-x2)*(x-x2)+(y-y2)*(y-y2)));
  		int distRadius=(w+w2)/2;

 		if (distCenter <= distRadius) r=true;

		return r;	
	}
	
	bool calculateComputerMove( Galaxy *galaxy, int target_x, int target_y, int target_width )
	{
		static Shoot s=Shoot();
		static bool found=false;
		static int searches=MAXCOMPUTERSEARCH;
		static double rpow, rang, ypos;
		bool canshoot=false;
		computer_mode=NONE;
		
		// Zug suchen
		if (!found) {
			canshoot=false;
			srand(time(NULL));
			rpow=(int)((MAXSHOOTPOWER-1)*(rand()/(RAND_MAX+1.0))+1);
			rang=2*PI*(rand()/(RAND_MAX+1.0));
			ypos=(int)((SCREENHEIGHT-200)*(rand()/(RAND_MAX+1.0))+100);
			
			Vector2d vsta=Vector2d( (int)rpow*SHOOTPOWERFACTOR, rang, P);
			Vector2d vpos=Vector2d( x, ypos, K )+Vector2d(60, rang, P);
			found=s.hitEnemy( vpos, vsta, galaxy, target_x, target_y, (int)(target_width*COMPUTERSTRENGTH));
	
			Vector2d vvv=Vector2d( target_x, target_y, K );
			
			#ifdef __DEBUG__ 
			if (found) std::cout << "*** FEIND ENTDECKT BEI " <<  vvv << std::endl;
			#endif
			
			if (--searches<0) found=true;
			
			if (!found) { 
				computer_mode=THINKING;
			}
		}

		// Schuss und Position einstellen
		if (found && !canshoot) {
			canshoot=true;
			if ((int)ypos<y) {
				canshoot=false;
				moveN(); 
				if ((int)ypos>y) y=(int)ypos;
			}
			if ((int)ypos>y) { 
				canshoot=false;
				moveS(); 
				if ((int)ypos<y) y=(int)ypos;
			}

			if (rang>shoot_angle) { 
				canshoot=false; 
				incShootAngle(); 
				if (rang<shoot_angle) shoot_angle=rang; 
			}
			if (rang<shoot_angle) {
				canshoot=false;
				decShootAngle(); 
				if (rang>shoot_angle) shoot_angle=rang; 
			}
			if (canshoot && rpow>shoot_power) {
				canshoot=false;
				incShootPower();
			}
			if (!canshoot) {
				computer_mode=SHOOTING;
			}
		}
		
		// Schuss
		if (canshoot) {
			computer_mode=NONE;
			searches=MAXCOMPUTERSEARCH;
			shoot_power=rpow;
			found=false;
			canshoot=false;
			return true;
		}
		
		return false;
	}
	
	void draw() 
	{
		ufo_sprite.setPos( x-2, y+9 );
		ufo_sprite.draw();
		
		if (isActive()) drawCircle();

		if (isComputer()) { 
			switch (computer_mode) {
				case THINKING: 
					c_think.setPos(getX(), getY());
					c_think.draw();
					break;
				case SHOOTING:
					c_shoot.setPos(getX(), getY());
					c_shoot.draw();
					break;
			}
		}
	}
	
private:
	int x, y;
	int energy;
      	double shoot_angle, shoot_power;
	bool active, human;
	enum { NONE, THINKING, SHOOTING } computer_mode;
	Sprite ufo_sprite, cpkt_sprite, c_think, c_shoot;
	
	static Sprite getUfoSprite()
	{
		char* filename;
		static int member_of_game=0;
		
		switch (member_of_game) {
			case 0:
				filename="gfx/ufoanimred.bmp";
				break;	
			case 1:
				filename="gfx/ufoanimblue.bmp";
				break;	
		}

		member_of_game++;

		return Sprite(filename,25);
	}

	static Sprite getCirclePktSprite()
	{
		char* filename;
		static int member_of_game=0;
		
		switch (member_of_game) {
			case 0:
				filename="gfx/cpktred.bmp";
				break;	
			case 1:
				filename="gfx/cpktblue.bmp";
				break;	
		}

		member_of_game++;

		return Sprite(filename);
	}
	
	void drawCircle() 
	{
		Vector2d m=Vector2d(x, y, K);
		double ss=30;

		int a=150;
		
		for (int i=0; i<5; i++) {
			int b;
			if ((int)(shoot_power/20)>i) cpkt_sprite.setAlpha(a+105);
			else cpkt_sprite.setAlpha(a);
			a-=20;
			Vector2d pos=m+Vector2d(ss, shoot_angle, P);
			cpkt_sprite.setPos( (int)pos.getX(), (int)pos.getY() );
			cpkt_sprite.draw();
			ss+=10;
		}
	}
};

//-----------------------------------------------------------------------------------------
class Explosion
//-----------------------------------------------------------------------------------------
{
public:
	Explosion() : active(false), expl_sprite(Sprite("gfx/explosionanim.bmp",6)) {}

	void activate( int x, int y ) 
	{
		this->x=x;
		this->y=y;
		counter=50;
		active=true;
		expl_sprite.setFramerate(1);
		expl_sprite.setAlpha(230);
		expl_sprite.setRepeatmode(false);
		expl_sprite.resetFrames();
		sound.play(0);	
	}
	
	void draw() 
	{
		if (active) {
			if (counter-->0) {
				expl_sprite.setPos( x, y );
				expl_sprite.draw();
			} else {
				active=false;
			}	
		}	
	}
	
private:
	int x, y;
	bool active;
	int counter;
	Sprite expl_sprite;
};

//-----------------------------------------------------------------------------------------
class Playfield
//-----------------------------------------------------------------------------------------
{
public:
	Playfield() : x(0), active_player(0), blink(BLINKTIME), hintmode(false), scroller_active(false), start_time(500), winner(-1), explosion(Explosion()), shoot(Shoot())
       	{
		// Sterne und...
		for (int i=0; i<MAXSTARS; i++) {
			stars[i]=new Star();
		}

		// Goldregen initialisieren
		for (int i=0; i<MAXGOLDRAIN; i++) {		
			goldrain[i]=new Goldrain();
		}

		
		galaxy=new Galaxy((int)((MAXPLANETS-MINPLANETS)*(rand()/(RAND_MAX+1.0)))+MINPLANETS );
		seed=time(NULL);
		srand(seed);
	}
	
	~Playfield()
	{
		for (int i=0; i<MAXSTARS; i++) delete stars[i];
		for (int i=0; i<MAXGOLDRAIN; i++) delete goldrain[i];
		for (int i=0; i<MAXPLAYER; i++) delete players[i];
		delete galaxy;
	}

	void play() 
	{
		Gametimer timer=Gametimer(TICK_INTERVAL);

		initPlayers();
		active_player=0;
		players[active_player]->activate();	
		setPlayer(active_player);
		
		bool done=false, targeting=false, targetlocked=false;
		bool keypressed=false;
		SDL_Event event;
		Uint8 *keys;
		int winning_time=WINNINGWAIT;
		
		do {
			
			while ( SDL_PollEvent(&event) ) {
				switch (event.type) {
					case SDL_QUIT:
						done=true;
						break;
					case SDL_KEYUP:
						keypressed=false;		// ???
						if ( targeting && !shoot.isActive()) {
							targeting=false;
							targetlocked=true;
							shoot.activate( players[active_player]->getShootStart(),
								players[active_player]->getShootVector() );
						}
						break;
				}
			}
		
			keys = SDL_GetKeyState(NULL);
			
			// Ingame-Tasten
			if (!keypressed) {

				if( keys[SDLK_ESCAPE] ) {
					done=true;
				}
				if( keys[SDLK_f] ) {
					SDL_WM_ToggleFullScreen(MYSDLSCREEN);
					keypressed=true;
				}
				if( keys[SDLK_h] ) {
					hintmode=!hintmode;
					keypressed=true;
				}
				if( keys[SDLK_F1] ) {
					scroller_active=!scroller_active;
					if (scroller_active) drawScroller(true);
					keypressed=true;
				}
				if( keys[SDLK_TAB] ) {
					initGalaxy();
					keypressed=true;
				}
				
				if( keys[SDLK_1] ) {
					players[0]->setComputer(false);
					players[1]->setComputer(true);
					players[0]->resetEnergy();
					players[1]->resetEnergy();			
					players[0]->activate();
					players[1]->inactivate();			
					active_player=0;
					setPlayer(0);
					sound.play(5);
					keypressed=true;
				}
				if( keys[SDLK_2] ) {
					players[0]->setComputer(false);
					players[1]->setComputer(false);
					players[0]->resetEnergy();
					players[1]->resetEnergy();			
					players[0]->activate();
					players[1]->inactivate();			
					active_player=0;
					setPlayer(0);
					sound.play(5);
					keypressed=true;
				}
				if( keys[SDLK_3] ) {
					players[0]->setComputer(true);
					players[1]->setComputer(true);
					players[0]->resetEnergy();
					players[1]->resetEnergy();			
					players[0]->activate();
					players[1]->inactivate();			
					active_player=0;
					setPlayer(0);
					keypressed=true;
				}
			}
			
			// Diese Tasten sind gesperrt sofern der Schuss abgefeuert wurde
			// oder es einen Gewinner gibt!
			if (!targetlocked && winner==-1 && !players[active_player]->isComputer()) {
				if( keys[SDLK_LEFT] ) {
					players[active_player]->decShootAngle();
				}
				if( keys[SDLK_RIGHT] ) {
					players[active_player]->incShootAngle();
				}
				 if( keys[SDLK_UP] ) {
					players[active_player]->moveN();
				}
				if( keys[SDLK_DOWN] ) {
					players[active_player]->moveS();
				}
				if( keys[SDLK_SPACE] ) {
					players[active_player]->incShootPower();
					targeting=true;
				}	
			}
			
			drawBack();

			// Computermove???
			if (!targetlocked && winner==-1 && players[active_player]->isComputer()) {
				if ( players[active_player]->calculateComputerMove( galaxy, 
					players[1-active_player]->getX(),
					players[1-active_player]->getY(),
					players[1-active_player]->getWidth() ) ) {
					shoot.activate( players[active_player]->getShootStart(),
						players[active_player]->getShootVector() );
					targetlocked=true;
				}
			}
		
			if (winner!=-1) {
				if ( --winning_time==0 ) {
					winning_time=WINNINGWAIT;
					active_player=0;
					winner=-1;
					for (int i=0; i<MAXPLAYER; i++) {
						players[i]->resetEnergy();
						players[i]->inactivate();
					}
					players[active_player]->activate();
					setPlayer(active_player);
				}
			} 
			
			shoot.draw();
	
			// Schussberechnungen
			if (shoot.isActive()) {
				if (!shoot.animate( galaxy->getPlanetList(), galaxy->getPlanets() )) {
					nextPlayer();
					targetlocked=false;
				}
				
				if ( galaxy->collision( shoot.getX(), shoot.getY(), shoot.getWidth() ) ) {
					explosion.activate( shoot.getX(), shoot.getY() );
					shoot.kill();	
					nextPlayer();	
					targetlocked=false;

					#ifdef __DEBUG__ 
					std::cout << __HITS << "/" << __SHOOTS << "=" << (int)(__HITS*100/__SHOOTS) << "%" << std::endl;
					#endif
					
			} else {
				
					bool hitufo=false;
					
					for (int i=0; i<MAXPLAYER; i++) {
						if (players[i]->collision( shoot.getX(), shoot.getY(), shoot.getWidth())) {
							hitufo=true;
							players[i]->subEnergy(25);
							sound.play(4);
							
							#ifdef __DEBUG__ 
							__HITS++;
							std::cout << __HITS << "/" << __SHOOTS << "=" << (int)(__HITS*100/__SHOOTS) << "%" << std::endl;
							#endif
							
							if (players[i]->isDead()) {
								winner=2-i; // oh oh...
								sound.play(2);
							}
							break;
						}
					}
	
					if (hitufo) {
						explosion.activate( shoot.getX(), shoot.getY() );
						shoot.kill();
						if (winner==-1)	nextPlayer();
						targetlocked=false;
					}
				}
				
			} else {
				if (hintmode) shoot.drawPrePath( players[active_player]->getShootStart(),
						  Vector2d( 100*SHOOTPOWERFACTOR, players[active_player]->getShootAngle(), P ),
						  galaxy);
			}
			
			// Scrollwerte setzen!
			if ( shoot.isActive()) {
				// Bildschirm der Schussposition angleichen
				int xs=shoot.getX();
				int ys=shoot.getY();
				int xo=0, yo=0;
				if (xs<SCROLLBORDERWIDTH) xo=SCROLLBORDERWIDTH-xs;
				if (xs>SCREENWIDTH-SCROLLBORDERWIDTH) xo=SCREENWIDTH-SCROLLBORDERWIDTH-xs;
				if (ys<SCROLLBORDERWIDTH) yo=SCROLLBORDERWIDTH-ys;
				if (ys>SCREENHEIGHT-SCROLLBORDERWIDTH) yo=SCREENHEIGHT-SCROLLBORDERWIDTH-ys;
				Sprite::setOffset( xo, yo );
			} else {
				// Bildschirm zurueckscrollen
				int xo=Sprite::x_offset;
				int yo=Sprite::y_offset;
				int sbx=(int)abs(xo)/15+1; //(int)sqrt((xo*xo)+(yo*yo))/10+1;
				int sby=(int)abs(yo)/15+1; //(int)sqrt((xo*xo)+(yo*yo))/10+1;
				if (xo>0) { xo-=sbx; if (xo<0) xo=0; }
				if (xo<0) { xo+=sbx; if (xo>0) xo=0; }
				if (yo>0) { yo-=sby; if (yo<0) xo=0; }
				if (yo<0) { yo+=sby; if (yo>0) xo=0; }
			
				Sprite::setOffset( xo, yo );
			}
			
			// Spieler zeichnen
			for (int i=0; i<MAXPLAYER; i++) {
				players[i]->draw();
			}	
			
			galaxy->draw();
			explosion.draw();

			setScores( players[0]->getEnergy(), players[1]->getEnergy() );
			setPlayer(active_player);
			draw();
			
			// Gewinner-Zeremonie
			if (winner!=-1) {
				int xx=players[winner-1]->getX();
				int yy=players[winner-1]->getY()-100;
				
				for (int i=0; i<MAXGOLDRAIN; i++) {		
					goldrain[i]->setOffset(xx, yy);
					goldrain[i]->draw();
				}
				
			}

			SDL_Flip(MYSDLSCREEN);
			
			if( keys[SDLK_F12] ) SDL_SaveBMP(MYSDLSCREEN, "ufoMYSDLSCREEN.bmp" );

			timer.delay();	

		} while ( !done );
		
	}
	
private:
	int x;
	int blink;
	int active_player, winner, start_time;
	int scores[MAXPLAYER];
	Star *stars[MAXSTARS];
	Goldrain *goldrain[MAXGOLDRAIN];
	Ufo *players[MAXPLAYER];
	Galaxy *galaxy;
	Shoot shoot;
	Explosion explosion;
	bool scroller_active, hintmode;
	int seed;

	void drawBack()
	{
		SDL_FillRect( MYSDLSCREEN, NULL, SDL_MapRGB( MYSDLSCREEN->format, 0, 0, 30 ));
		
		for (int i=0; i<MAXSTARS; i++) {
			stars[i]->draw();
		}
		
	}	
	
	void draw() 
	{
		// Spieleranzeigen
		bool blinkon=(blink<BLINKTIME*2/3) ? true : false;
		std::ostringstream os1, os2;
		
		if ( blinkon || active_player==1 || winner!=-1 ) { 
			if (players[0]->isComputer()) font.print( 35, 30, "COMPUTER" );
			else font.print( 35, 30, "SPIELER 1" );
		}
		os1 << "SCHILD " << scores[0];
		font.print( 35, 60, os1.str() );
		
		if ( blinkon || active_player==0 || winner!=-1 ) {
			if (players[1]->isComputer()) font.print( SCREENWIDTH-255, 30, "COMPUTER" );
			else font.print( SCREENWIDTH-255, 30, "SPIELER 2" );
		}
		os2 << "SCHILD " << scores[1];
		font.print( SCREENWIDTH-255, 60, os2.str() );
		
		// Falls Computer vs. Computer: Titelbildschirm
		if (players[0]->isComputer() && players[1]->isComputer()) {
			//start_time--;	
			int yy=SCREENHEIGHT/2-120;	
			for (int i=0; i<_TITLETEXT; i++) {	
				if (i!=_TITLETEXT-1 || blinkon) font.print( (SCREENWIDTH-font.getWidth(TITLETEXT[i]))/2, yy, TITLETEXT[i] );
				yy+=30;
			}	
				
		} else {
			if (winner!=-1 && blinkon ) {	
				std::ostringstream os3;
				os3 << "SPIELER " << winner << " HAT GEWONNEN";
				std::string winstr=os3.str();
				font.print( (SCREENWIDTH-font.getWidth(winstr))/2, SCREENHEIGHT/2, winstr );
			}
		}
		
		if (scroller_active) drawScroller();
		
		// Galaxie-Warp
		if (galaxy->isImploding()) {
			std::ostringstream os;
			os << "NUMMER " << seed << " " << galaxy->getPlanets();
			std::string ss=os.str();
			std::string s1="WARPE ZUR GALAXIE";
			font.print( (SCREENWIDTH-font.getWidth(s1))/2, 30, s1);
			font.print( (SCREENWIDTH-font.getWidth(ss))/2, 60, ss);
		}		

		if (--blink<0) blink=BLINKTIME;		
	}
	
	void drawScroller( bool reset=false )
	{
		static int x=SCREENWIDTH;
		
		if (reset) x=SCREENWIDTH;
		
		std::string scrollstr=SCROLLERTEXT;
		int w=font.getWidth(scrollstr);
		
		if ((x-=SCROLLERSPEED)<-w) x=SCREENWIDTH;
		
		font.print( x, SCREENHEIGHT-45, scrollstr);
	}
	
	void setScores( int s1, int s2 ) 
	{
		scores[0]=s1;
		scores[1]=s2;
	}
	
	void setPlayer( int player )
	{
		active_player=player;
	}
	
	void initGalaxy()
	{
		if (!galaxy->isImploding()) {
			seed=time(NULL);
			srand(seed);
			galaxy->initGalaxy((int)((MAXPLANETS-MINPLANETS)*(rand()/(RAND_MAX+1.0)))+MINPLANETS );
		}
	}
	
	void initPlayers()
	{
		for (int i=0; i<MAXPLAYER; i++) {
			bool col, hum;
			int x, y;
			double a;
			
			do {
				switch (i) {
					case 0:
						x=BORDERWIDTH;
						a=0;
						hum=!true;
						break;
					case 1:
						x=SCREENWIDTH-BORDERWIDTH;
						a=PI;
						hum=false;
						break;
					
				}
				y=(int)((SCREENHEIGHT-200)*(rand()/(RAND_MAX+1.0))+100);
			
				col=galaxy->collision( x, y, 100 );
				
				for (int j=0; j<i; j++) {
					if (players[j]->collision(x, y, 500)) {
						col=true;
						break;
					}
				}
			} while(col);
			
			players[i]=new Ufo( x, y, a, hum );
		}	
		
	}

	void nextPlayer()
	{
		players[active_player]->inactivate();
		if (++active_player>=MAXPLAYER) active_player=0;
		players[active_player]->activate();		
	}
};

//-----------------------------------------------------------------------------------------
int main(int argc, char *argv[])
//-----------------------------------------------------------------------------------------
{
    	if( SDL_Init(SDL_INIT_VIDEO) == -1 ) {
		std::cout << "Can't init SDL: " << SDL_GetError() << std::endl;
	       	exit(1);
	}
	
 	atexit(SDL_Quit); 

    	if ( ( MYSDLSCREEN = SDL_SetVideoMode( SCREENWIDTH, SCREENHEIGHT, 16, SDL_HWSURFACE | SDL_DOUBLEBUF ) ) == NULL ) {
		std::cout << "Can't set video mode: " << SDL_GetError() << std::endl;
        	exit(1);
    	}
	
    	SDL_WM_SetCaption( "HighMoon - Duell im All von Patrick Gerdsmeier, patrick@gerdsmeier.net", NULL );
	SDL_ShowCursor(SDL_DISABLE);

	srand(time(NULL));

	Playfield pf=Playfield();
	pf.play();
		
    	return 0;
}

