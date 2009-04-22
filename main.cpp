/******************************************************************************************
 *
 * HighNoon - Duell im All
 * Copyright (c) 2005 Patrick Gerdsmeier <patrick@gerdsmeier.net>
 *
 * "main.cpp"
 * 
 * Gametimer:
 * For Game-Timing 
 *
 * Playfield: merges all objects+text+logic together. play()
 * to play Game. Needs global Font, Soundset+Language!
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
#include <unistd.h>
#include <time.h>

#include "stdio.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#ifdef __THREADS__
#include <SDL/SDL_thread.h>
#endif

#include "constants.hpp"
#include "vector_2.hpp"
#include "language.hpp"
#include "sound.hpp"
#include "graphics.hpp"
#include "galaxy.hpp"

#ifdef __DEBUG__
int __SHOOTS=0, __HITS=0;
#endif

class Playfield;

SDL_Surface *MYSDLSCREEN;
Language language;
Soundset *sound;
Font *font;

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
	Playfield() 
	:
		is_done(false),
		blink(BLINKTIME),
		winner_is_player(-1),
		scroller_active(false),
		hintmode(false),
		computerstrength(2),
		show_winner_time(WINNINGWAIT)
	{
		stars=new Star[MAXSTARS];
		goldrain=new Goldrain[MAXGOLDRAIN];

		for (int i=0; i<MAXPLAYER; i++) players[i]=new Ufo( SCREENWIDTH/2, SCREENHEIGHT/2 );

		galaxy=new Galaxy( planets=(int)RANDOM(MAXPLANETS,MINPLANETS), seed=time(NULL) );
		galaxy->set_Ufos( players, MAXPLAYER );
	}
	
	~Playfield()
	{
		delete[] stars;
		delete[] goldrain;
		for (int i=0; i<MAXPLAYER; i++) delete players[i];
		delete galaxy;
	}

	bool is_Done() 
	{ 
		return is_done;
	}
	
	#ifdef __THREADS__
	static int gameLogicThread( void* )  
	{
		Gametimer timer=Gametimer(TICK_INTERVAL_SCREEN);

		while ( !pf->is_Done() ) {
		 	pf->draw();
			timer.delay();
		}
		
		return 0;
	}
	#endif
	
	void draw()
	{
		draw_Background();
		
		galaxy->draw();

		if ( hintmode )	
			players[active_player]->draw_hint( galaxy );

		// Gewinner-Zeremonie
		if ( winner_is_player!=-1 ) {
			int xx=(int)( players[winner_is_player]->get_X() );
			int yy=(int)( players[winner_is_player]->get_Y() )-100;
			
			SDL_LockSurface( MYSDLSCREEN );
			
			for ( int i=0; i<MAXGOLDRAIN; i++ ) {		
				goldrain[i].setOffset(xx, yy);
				goldrain[i].draw();
			}
			
			SDL_UnlockSurface( MYSDLSCREEN );
		}

		draw_Text();	

		if (scroller_active)
			draw_Scroller();
		
		SDL_Flip(MYSDLSCREEN);	
	}
	
	void play() 
	{
		Gametimer timer=Gametimer(TICK_INTERVAL_GAME);

		bool p_mode[]={false,false};
		init_Game( p_mode );

		bool targeting=false,
			targetlocked=false,
			keypressed=false;
		SDL_Event event;

		#ifdef __THREADS__
		SDL_Thread *t=SDL_CreateThread( Playfield::gameLogicThread, NULL );
		#endif

		do {			
	
			while ( SDL_PollEvent(&event) ) {
				switch (event.type) {
				
					case SDL_QUIT:
						is_done=true;
						break;
					
					case SDL_KEYUP:
						keypressed=false;		// ???
						
						if ( targeting ) {
							galaxy->set_Shoot( players[active_player]->shoot() );
							targeting=false;
							targetlocked=true;
						}
					
						break;
				}
			}
		
			Uint8 *keys=SDL_GetKeyState(NULL);

			// Ingame-Tasten
			if (!keypressed) {

				if( keys[KEY_QUIT] ) {
					is_done=true;
				}

				if( keys[KEY_STRENGTH] ) {
					computerstrength+=1;
					computerstrength%=5;
					keypressed=true;
				}

				if( keys[KEY_TOGGLEFULLSCREEN] ) {
					keypressed=true;
					SDL_WM_ToggleFullScreen(MYSDLSCREEN);
				}

				if( keys[KEY_TOGGLEHINT] ) {
					keypressed=true;
					hintmode=!hintmode;
				}

				if( keys[KEY_TOGGLESCROLLER] ) {
					keypressed=true;
					scroller_active=!scroller_active;
				
					if ( scroller_active )
						draw_Scroller(true);					
				}

				if( keys[KEY_TOGGLELANGUAGE] ) {
					keypressed=true;
					language.toggle();
					SDL_WM_SetCaption( language.getWindowtext(), NULL );
				}

				if( keys[KEY_WARPGALAXY] ) {
					
					keypressed=true;

					init_Galaxy();
						
					int p, s;
					
					if ( galaxy->create(
						p=(int)RANDOM(MAXPLANETS, MINPLANETS),
						s=time(NULL) ) ) {
						planets=p;
						seed=s;
					}					
				}

				if( keys[KEY_TOGGLESOUND] ) {
					sound->toggle();
					keypressed=true;
				}
				
				if( keys[KEY_ONEPLAYER] ) {
					bool p_mode[]={true,false};
					init_Game( p_mode );
					targetlocked=false;
					keypressed=true;
					sound->play(NEWGAME);
				}

				if( keys[KEY_TWOPLAYER] ) {
					bool p_mode[]={true,true};
					init_Game( p_mode );
					targetlocked=false;
					keypressed=true;
					sound->play(NEWGAME);
				}
				if( keys[KEY_DEMO] ) {
					bool p_mode[]={false,false};
					init_Game( p_mode );
					targetlocked=false;
					keypressed=true;
				}
			}
			
			// Diese Tasten sind gesperrt sofern der Schuss abgefeuert wurde
			// oder es einen Gewinner gibt!
			if ( winner_is_player==-1 ) {

				if ( players[active_player]->is_Computer() ) {
					static int computer_strength[]={9, 7, 5, 3, 1};
						players[active_player]->calculate_Computer_Move(galaxy,computer_strength[computerstrength]);			
				} else {
					if ( !targetlocked ) {
			
						if( keys[KEY_DECSHOOT] ) {
							players[active_player]->dec_ShootAngle();
						}
			
						if( keys[KEY_INCSHOOT] ) {
							players[active_player]->inc_ShootAngle();
						}
			
						if( keys[KEY_MOVEUP] ) {
							players[active_player]->move_Up();
						}
						
						if( keys[KEY_MOVEDOWN] ) {
							players[active_player]->move_Down();
						}
						
						if( keys[KEY_FIRE] ) {
							players[active_player]->inc_ShootPower();
							targeting=true;
						}
					}
				}
			} else {

				if ( --show_winner_time==0 ) {
					init_Game();
				}
			} 


			// Animate the Shoots in Galaxy
			if ( galaxy->animate() ) {
				next_Player();
				targetlocked=false;
			}

			set_Scrolling();

			winner_is_player=check_for_Winner();			

			#ifndef __THREADS__
			draw();
			#endif
			
			if( keys[KEY_SCREENSHOT] && !keypressed ) {
				static int screenshotid=0;

				if ( screenshotid++>25 )
					screenshotid=25;

				char filename[]="screenshot__.bmp";
				filename[11]=(char)(screenshotid+96);

				SDL_SaveBMP(MYSDLSCREEN, filename );

				keypressed=true;
			}
			
			timer.delay();	

		} while ( !is_done );
		
		#ifdef __THREADS__
		SDL_WaitThread(t,NULL);
		SDL_KillThread(t);
		#endif
	}
	
private:
	bool is_done;
	int blink;
	int active_player, winner_is_player;
	Star *stars;
	Goldrain *goldrain;
	Ufo *players[MAXPLAYER];
	Galaxy *galaxy;
	bool scroller_active, hintmode;
	int seed;
	int planets;
	int computerstrength;
	int show_winner_time;
	
	void set_Scrolling()
	{
		// Scrollwerte setzen!
		if ( galaxy->is_ShootActive() ) {
	
			// Set Screen to Shootposition
			int xs=(int)galaxy->get_ShootX();
			int ys=(int)galaxy->get_ShootY();
			int xo=0, yo=0;
	
			if (xs<SCROLLBORDERWIDTH) 
				xo=SCROLLBORDERWIDTH-xs;
	
			if (xs>SCREENWIDTH-SCROLLBORDERWIDTH)
				xo=SCREENWIDTH-SCROLLBORDERWIDTH-xs;
	
			if (ys<SCROLLBORDERWIDTH) 
				yo=SCROLLBORDERWIDTH-ys;
	
			if (ys>SCREENHEIGHT-SCROLLBORDERWIDTH) 
				yo=SCREENHEIGHT-SCROLLBORDERWIDTH-ys;
	
			Sprite::setOffset( xo, yo );
		} else {
			
			// Scroll back Screen...
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
	}

	void draw_Background()
	{
		SDL_FillRect( MYSDLSCREEN, NULL, SDL_MapRGB( MYSDLSCREEN->format, 0, 0, 30 ));
		
		SDL_LockSurface( MYSDLSCREEN );

		for (int i=0; i<MAXSTARS; i++)
			stars[i].draw();

		SDL_UnlockSurface( MYSDLSCREEN );		
	}	
	
	void draw_Text() 
	{
		// Game-Display
		bool blinkon=( blink<BLINKTIME*2/3 ) ? true : false;

		//
		static int old_scores[MAXPLAYER]={ 0, 0 };

		for ( int i=0; i<MAXPLAYER; i++ ) {
			int a=255, x_pos, y_pos;
			int pl=(int)players[i]->get_Y()+Sprite::y_offset;

			if (pl<150 && pl>150-90)
				a=(int)pl*2-65;

			if (pl<=150-90 && pl>=50-90)
				a=(int)110-pl*2+65;

			
			switch (i) {
			
				case 1:
					x_pos=SCREENWIDTH-255;
					y_pos=30;
					break;
				default:
					x_pos=35;
					y_pos=30;					
			}
			
			if ( blinkon || active_player!=i || winner_is_player!=-1 ) { 
		
				if (players[i]->is_Computer()) 
					font->print( x_pos, y_pos, language.getComputertext(computerstrength), a );
				else 
					font->print( x_pos, y_pos, language.getPlayertext(i+1), a );
			}

			font->print( x_pos, y_pos+LINEHEIGHT, language.getShieldtext( old_scores[i] ), a );

			if ( old_scores[i]<players[i]->get_Energy() )
				old_scores[i]++;

			if ( old_scores[i]>players[i]->get_Energy() ) 
				old_scores[i]--;

		}
		
		// if Computer vs. Computer->Titletext
		if ( players[0]->is_Computer() && players[1]->is_Computer() ) {
			int yy=SCREENHEIGHT/2-LINEHEIGHT*_TITLETEXT/2;	

			for (int i=0; i<_TITLETEXT; i++) {	
				std::string t=language.getTitletext(i);
				if (i!=_TITLETEXT-2 || blinkon) font->print( (SCREENWIDTH-font->getWidth(t))/2, yy,t );
				yy+=LINEHEIGHT;
			}	
				
		} else {

			if (winner_is_player!=-1 && blinkon ) {	
				std::string win=language.getWinnertext( ( players[winner_is_player]->is_Computer() ) ? -1 : winner_is_player+1 );
				font->print( (SCREENWIDTH-font->getWidth(win))/2, SCREENHEIGHT/2, win );
			}

		}
		
		// Galaxy-Warp-Text
		if ( galaxy->is_Imploding() ) {
			std::string ss=language.getWarptext( seed, planets );
			std::string s1=language.getWarptext();
			font->print( (SCREENWIDTH-font->getWidth(s1))/2, LINEHEIGHT, s1);
			font->print( (SCREENWIDTH-font->getWidth(ss))/2, LINEHEIGHT+LINEHEIGHT, ss);
		}		

		if (--blink<0) blink=BLINKTIME;		
	}
	
	void draw_Scroller( bool reset=false )
	{
		static int x=SCREENWIDTH;
		
		if (reset) x=SCREENWIDTH;
		
		std::string scrollstr=language.getScrollertext();
		int w=font->getWidth(scrollstr);
		
		if ((x-=SCROLLERSPEED)<-w) x=SCREENWIDTH;
		
		font->print( x, SCREENHEIGHT-45, scrollstr);
	}

	int check_for_Winner() 
	{
		int last_man_standing=0;
		int dead_men_laying=0;
		int winner=-1;
		
		for ( int i=0; i<MAXPLAYER; i++ ) {

			if ( players[i]->is_dead() )
				dead_men_laying++;
			else 
				last_man_standing=i;
		}
		
		if ( dead_men_laying==MAXPLAYER-1 ) {
			winner=last_man_standing;
			sound->play(WINNINGGAME);
		}
		
		return winner;
	}

	void init_Game( bool *player_mode=NULL )
	{
		galaxy->kill_all_Shoots();
		
		for ( int i=0; i<MAXPLAYER; i++ ) {
		
			if ( player_mode!=NULL ) {
				if ( !player_mode[i] )
					players[i]->set_Computer();
				else 
					players[i]->set_Human();
			}
			
			players[i]->reset();
			
			if ( i==0 ) 
				players[i]->activate();
			else 
				players[i]->deactivate();
		}

		show_winner_time=WINNINGWAIT;			
		winner_is_player=-1;					
		active_player=0;
	}
 	
	void init_Galaxy()
	{
		int s=time(NULL);
		int p;
		
		if ( galaxy->create( p=(int)RANDOM( MAXPLANETS, MINPLANETS), s ) ) {
			seed=s;
			planets=p;
		}
	}

	void next_Player()
	{
		if ( ++active_player>=MAXPLAYER )
			active_player=0;

		for ( int i=0; i<MAXPLAYER; i++ ) {
			players[i]->deactivate();
		}
		
		players[active_player]->activate();		
	}
};

//-----------------------------------------------------------------------------------------
int main(int argc, char *argv[])
//-----------------------------------------------------------------------------------------
{
	const std::string __BLUE="\x1b[34m"; 
	const std::string __RED="\x1b[31m"; 
	const std::string __NORMAL="\x1b[0m"; 

	Uint32 videoflags=0;
	
	bool get_video_info=false, param_ok;

	for (int i=1; i<argc; i++) {
		param_ok=false;
		char *arg=argv[i];

		if ( arg==(std::string)"--version" || arg==(std::string)"-v" ) {
			std::cout << "HighMoon - Duel in Space v" << VERSION << std::endl
				<< COPYRIGHT << std::endl
				<< __BLUE << WEBSITE << __NORMAL
				<< std::endl;
			exit(0);
		}		

		if ( arg==(std::string)"--help" || arg==(std::string)"-h" ) {
			std::cout << "HighMoon - Duel in Space v" << VERSION << std::endl
				<< COPYRIGHT << std::endl
				<< __BLUE << WEBSITE << __NORMAL
				<< std::endl
				<< "Usage: "
				<< argv[0] 
				<< " [--help|--version|--fullscreen|--videoinfo]" << std::endl
				<< __RED
				<< "[1]     Player vs Computer" << std::endl 
				<< "[2]     Player vs Player" << std::endl 
				<< "[3]     Computer vs Computer (Demo on Titlescreen)" << std::endl 
				<< "[UP]    Move Flying Saucer Up" << std::endl 
				<< "[DOWN]  Move Flying Saucer Down" << std::endl 
				<< "[LEFT]  Decrease Shootangle" << std::endl 
				<< "[RIGHT] Increase Shootangle" << std::endl 
				<< "[SPACE] Keep pressed to increase Power. Release for shooting." << std::endl 
				<< "[F1]    Toggle Help-Scroller on/off" << std::endl 
				<< "[F2]    Toggle Language English/German" << std::endl 
				<< "[C]     Toggles Computerstrength (Trainee...Insane)" << std::endl 
				<< "[F]     Toggle Window/Fullscreen" << std::endl 
				<< "[S]     Toggle Sound on/off" << std::endl
				<< "\x1b[0m";
			exit(0);
		}
		
		if ( arg==(std::string)"--videoinfo" || arg==(std::string)"-vi" ) {
			get_video_info=true;	
			param_ok=true;
		}

		if ( arg==(std::string)"--fullscreen" || arg==(std::string)"-f" ) {
			videoflags|=SDL_FULLSCREEN;
			param_ok=true;
		}
		
		if ( !param_ok ) {
			std::cout << "Unknown parameter: " << arg << std::endl;
			exit(1);
		}
	}
	
    	if( SDL_Init( SDL_INIT_VIDEO 
	#ifdef __THREADS__
	| SDL_INIT_EVENTTHREAD 
	#endif
	| SDL_INIT_AUDIO ) == -1 ) {
		std::cout << "Can't init SDL: " << SDL_GetError() << std::endl;
	       	exit(1);
	}
	
 	atexit(SDL_Quit); 

	const SDL_VideoInfo *video_Info=SDL_GetVideoInfo();

	if (get_video_info) {
		std::cout << "Video Memory size is: " << ( video_Info->video_mem ) << " KB"
			<< std::endl
			<< "Hardware Surfaces are: " << ( ( video_Info->hw_available ) ? "enabled" : __RED+"disabled"+__NORMAL )
			<< std::endl
			<< "Hardware to Hardware blits are: " << ( ( video_Info->blit_hw ) ? "accelerated" : __RED+"not accelerated"+__NORMAL )
			<< std::endl
			<< "Hardware to Hardware colorkeyblits are: " << ( ( video_Info->blit_hw_CC ) ? "accelerated" : __RED+"not accelerated"+__NORMAL )
			<< std::endl
			<< "Hardware to Hardware alphablits are: " << ( ( video_Info->blit_hw_A ) ? "accelerated" : __RED+"not accelerated"+__NORMAL )
			<< std::endl
			<< "Software to Hardware blits are: " << ( ( video_Info->blit_sw ) ? "accelerated" : __RED+"not accelerated"+__NORMAL )
			<< std::endl
			<< "Software to Hardware colorkeyblits are: " << ( ( video_Info->blit_sw_CC ) ? "accelerated" : __RED+"not accelerated"+__NORMAL )
			<< std::endl
			<< "Software to Hardware alphablits are: " << ( ( video_Info->blit_sw_A ) ? "accelerated" : __RED+"not accelerated"+__NORMAL )
			<< " " << std::endl;
		exit(0);
	}
	
	if ( video_Info->hw_available )
		videoflags|= SDL_HWSURFACE | SDL_DOUBLEBUF;
	else videoflags|= SDL_SWSURFACE;

	SDL_WM_SetIcon(IMG_Load("gfx/highmoon.png"), NULL );

    	if ( ( MYSDLSCREEN = SDL_SetVideoMode( SCREENWIDTH, SCREENHEIGHT, 0, videoflags ) ) == NULL ) {
		std::cout << "Can't set video mode: " << SDL_GetError() << std::endl;
        	exit(1);
    	}
	
    	SDL_WM_SetCaption( language.getWindowtext(), NULL );
	SDL_ShowCursor(SDL_DISABLE);
	sound=new Soundset();
	font=new Font();

	srand(time(NULL));
	
	Playfield *pf=new Playfield();

	pf->play();
	
	delete pf;
	delete font;

    	return 0;
}

