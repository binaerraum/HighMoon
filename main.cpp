/******************************************************************************************
 *
 * HighNoon - Duell im All
 * Copyright (c) 2005 Patrick Gerdsmeier <patrick@gerdsmeier.net>
 *
 * "main.cpp"
 * 
 * Beinhaltet momentan:
 *   
 * Gametimer
 * kleine Klasse zum Timing des Spielflusses. 
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
#include <SDL/SDL_thread.h>
#include "constants.hpp"
#include "vector2d.hpp"
#include "language.hpp"
#include "sound.hpp"
#include "graphics.hpp"
#include "galaxy.hpp"

//#define __THREADS__

#ifdef __DEBUG__
int __SHOOTS=0, __HITS=0;
#endif

class Playfield;

SDL_Surface *MYSDLSCREEN;
Language language=Language();
Soundset sound=Soundset();
Font *font;
Playfield *pf;

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

		if ( delay>0 ) SDL_Delay( (Uint32)delay );

        	next_time += ticks;
	}
			      
private:
	Uint32 next_time, ticks;
};

//-----------------------------------------------------------------------------------------
class Playfield
//-----------------------------------------------------------------------------------------
{
public:
	Playfield() :
	done(false),
	x(0),
	blink(BLINKTIME),
	active_player(0),
	winner(-1),
	shoot(Shoot()),
	explosion(Explosion()),
	scroller_active(false),
	hintmode(false)
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

	bool isDone() { return done; }
	
	#ifdef __THREADS__
	static int gameLogicThread( void* )  
	{
		Gametimer timer=Gametimer(TICK_INTERVAL_SCREEN);

		while (!pf->isDone()) {
			pf->draw();
			timer.delay();
		}
		
		return 0;
	}
	#endif
	
	void draw()
	{
		drawBack();

		// Spieler zeichnen
		for (int i=0; i<MAXPLAYER; i++) {
			players[i]->draw();
		}	
		
		galaxy->draw();

		shoot.draw();
		
		if (hintmode && !shoot.isActive()) shoot.drawPrePath( players[active_player]->getShootStart(),
			Vector2d( 100*SHOOTPOWERFACTOR, players[active_player]->getShootAngle(), P ),
			galaxy);

		explosion.draw();

		// Gewinner-Zeremonie
		if (winner!=-1) {
			int xx=players[winner-1]->getX();
			int yy=players[winner-1]->getY()-100;
			
			SDL_LockSurface( MYSDLSCREEN );
			for (int i=0; i<MAXGOLDRAIN; i++) {		
				goldrain[i]->setOffset(xx, yy);
				goldrain[i]->draw();
			}
			SDL_UnlockSurface( MYSDLSCREEN );
		}

		drawText();	

		if (scroller_active) drawScroller();
		
		//SDL_UpdateRect(MYSDLSCREEN, 0, 0, SCREENWIDTH, SCREENHEIGHT );
		SDL_Flip(MYSDLSCREEN);	
	}
	
	void play() 
	{
		Gametimer timer=Gametimer(TICK_INTERVAL_GAME);

		initPlayers();
		active_player=0;
		players[active_player]->activate();	
		setPlayer(active_player);
		
		bool targeting=false, targetlocked=false;
		bool keypressed=false;
		SDL_Event event;
		Uint8 *keys;
		int winning_time=WINNINGWAIT;

		#ifdef __THREADS__
		SDL_Thread *t=SDL_CreateThread( Playfield::gameLogicThread, NULL );
		#endif

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
				if( keys[SDLK_F2] ) {
					language.toggle();
					SDL_WM_SetCaption( language.getWindowtext(), NULL );
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
					shoot.kill();	
					targetlocked=false;
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
					shoot.kill();	
					targetlocked=false;
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
					shoot.kill();	
					targetlocked=false;
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

			// Schussberechnungen
			if (shoot.isActive()) {
				if (!shoot.animate( galaxy->getPlanetList(), galaxy->getPlanets() )) {
					nextPlayer();
					targetlocked=false;
				}
				
				int p;
				
				if ( ( p=galaxy->collision( shoot.getX(), shoot.getY(), shoot.getWidth() ) ) != -1 ) {
					if ( galaxy->getPlanetList()[p]->isWormhole() ) {
						shoot.setPos( galaxy->getPlanetList()[p]->getWormX(),
							galaxy->getPlanetList()[p]->getWormY() );
					} else {
						explosion.activate( shoot.getX(), shoot.getY() );
						shoot.kill();	
						nextPlayer();	
						targetlocked=false;

						#ifdef __DEBUG__ 
						std::cout << __HITS << "/" << __SHOOTS << "=" << (int)(__HITS*100/__SHOOTS) << "%" << std::endl;
						#endif
					}
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
				int sbx=(int)abs(xo)/15+1; 
				int sby=(int)abs(yo)/15+1; 
				if (xo>0) { xo-=sbx; if (xo<0) xo=0; }
				if (xo<0) { xo+=sbx; if (xo>0) xo=0; }
				if (yo>0) { yo-=sby; if (yo<0) xo=0; }
				if (yo<0) { yo+=sby; if (yo>0) xo=0; }
			
				Sprite::setOffset( xo, yo );
			}
			
			setScores( players[0]->getEnergy(), players[1]->getEnergy() );
			setPlayer(active_player);

			#ifndef __THREADS__
			draw();
			#endif
			
			if( keys[SDLK_F12] ) {
				static int screenshotid=0;
				screenshotid++; if (screenshotid>25) screenshotid=25;
				char filename[]="ufoscreenshotA.bmp";
				int q=(char)(screenshotid+64);
				filename[13]=(char)q;
				SDL_SaveBMP(MYSDLSCREEN, filename );
			}
			
			timer.delay();	

		} while ( !done );
		
		#ifdef __THREADS__
		SDL_WaitThread(t,NULL);
		SDL_KillThread(t);
		#endif
	}
	
private:
	bool done;
	int x;
	int blink;
	int active_player, winner;
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
		
		SDL_LockSurface( MYSDLSCREEN );
		for (int i=0; i<MAXSTARS; i++) {
			stars[i]->draw();
		}
		SDL_UnlockSurface( MYSDLSCREEN );		
	}	
	
	void drawText() 
	{
		// Spieleranzeigen
		bool blinkon=(blink<BLINKTIME*2/3) ? true : false;
		
		if ( blinkon || active_player==1 || winner!=-1 ) { 
			if (players[0]->isComputer()) font->print( 35, 30, language.getComputertext() );
			else font->print( 35, 30, language.getPlayertext(1) );
		}
		font->print( 35, 60, language.getShieldtext( scores[0] ) );
		
		if ( blinkon || active_player==0 || winner!=-1 ) {
			if (players[1]->isComputer()) font->print( SCREENWIDTH-255, 30, language.getComputertext() );
			else font->print( SCREENWIDTH-255, 30, language.getPlayertext(2) );
		}
		font->print( SCREENWIDTH-255, 60, language.getShieldtext( scores[1] ) );
		
		// Falls Computer vs. Computer: Titelbildschirm
		if (players[0]->isComputer() && players[1]->isComputer()) {
			int yy=SCREENHEIGHT/2-140;	
			for (int i=0; i<_TITLETEXT; i++) {	
				std::string t=language.getTitletext(i);
				if (i!=_TITLETEXT-2 || blinkon) font->print( (SCREENWIDTH-font->getWidth(t))/2, yy,t );
				yy+=30;
			}	
				
		} else {
			if (winner!=-1 && blinkon ) {	
				std::string win=language.getWinnertext( ( players[winner-1]->isComputer() ) ? -1 : winner );
				font->print( (SCREENWIDTH-font->getWidth(win))/2, SCREENHEIGHT/2, win );
			}
		}
		
		// Galaxie-Warp
		if (galaxy->isImploding()) {
			std::string ss=language.getWarptext( seed,galaxy->getPlanets() );
			std::string s1=language.getWarptext();
			font->print( (SCREENWIDTH-font->getWidth(s1))/2, 30, s1);
			font->print( (SCREENWIDTH-font->getWidth(ss))/2, 60, ss);
		}		

		if (--blink<0) blink=BLINKTIME;		
	}
	
	void drawScroller( bool reset=false )
	{
		static int x=SCREENWIDTH;
		
		if (reset) x=SCREENWIDTH;
		
		std::string scrollstr=language.getScrollertext();
		int w=font->getWidth(scrollstr);
		
		if ((x-=SCROLLERSPEED)<-w) x=SCREENWIDTH;
		
		font->print( x, SCREENHEIGHT-45, scrollstr);
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
			bool col, hum=0;
			int x=0, y;
			double a=0;
			
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
			
				col=( galaxy->collision( x, y, 100 )!=-1 );
				
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
    	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTTHREAD | SDL_INIT_AUDIO ) == -1 ) {
		std::cout << "Can't init SDL: " << SDL_GetError() << std::endl;
	       	exit(1);
	}
	
 	atexit(SDL_Quit); 

	/*
	const SDL_VideoInfo *vi=SDL_GetVideoInfo();
	std::cout << "hw=" << vi->hw_available 
		<< " wm=" << vi->wm_available
		<< " vmem=" << vi->video_mem
		<< " blit_hw=" << vi->blit_hw
		<< std::endl;
	*/
	
    	if ( ( MYSDLSCREEN = SDL_SetVideoMode( SCREENWIDTH, SCREENHEIGHT, 0, SDL_HWSURFACE | SDL_DOUBLEBUF ) ) == NULL ) {
		std::cout << "Can't set video mode: " << SDL_GetError() << std::endl;
        	exit(1);
    	}
	
    	SDL_WM_SetCaption( language.getWindowtext(), NULL );
	SDL_ShowCursor(SDL_DISABLE);

	language=Language();
	font=new Font();

	srand(time(NULL));
	
	pf=new Playfield();
	pf->play();
	
	delete pf;
	delete font;

    	return 0;
}

