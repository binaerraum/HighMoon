/******************************************************************************************
 *
 * HighNoon - Duell im All
 * Copyright (c) 2005 Patrick Gerdsmeier <patrick@gerdsmeier.net>
 *
 * "galaxy.cpp"
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

#include <cmath>

#include "galaxy.hpp"
#include "sound.hpp"

extern SDL_Surface* MYSDLSCREEN;
extern Soundset *sound;

#ifdef __DEBUG__
extern int __SHOOTS;
extern int __HITS;
#endif

/************************************************************************
 *									*
 * Spaceobject								*	
 *									*
 ************************************************************************/
Spaceobject::Spaceobject( double x, double y )
:
	x(x),
	y(y),
	width(0),
	weight(0),
	speed(0),
	direction(0),
	spacing(0),
	in_background(false)
{}

Spaceobject::~Spaceobject() {};

void Spaceobject::set_Pos( double x, double y ) 
{
	this->x=x;
	this->y=y;
}

bool Spaceobject::is_in_Background() const
{
	return in_background;
}

double Spaceobject::get_X() const 
{ 
	return x; 
}

double Spaceobject::get_Y() const 
{ 
	return y; 
}

double Spaceobject::get_Width() const 
{
	return width; 
}

double Spaceobject::get_Weight() const
{
	return weight;
}

double Spaceobject::get_Speed() const
{
	return speed;
}

double Spaceobject::get_Direction() const
{
	return direction;
}

double Spaceobject::get_Spacing() const
{
	return spacing;
}

bool Spaceobject::has_collision( Spaceobject *object )
{
	if ( check_collision( object->get_X(), object->get_Y(), object->get_Width() ) ) {
		hit( object );
		
		return true;
	}
	
	return false;
}

bool Spaceobject::check_collision( double x, double y, double width, bool spacing )
{
	double my_x=get_X();
	double my_y=get_Y();
	double my_width=get_Width();

	if ( spacing ) 
		my_width+=this->spacing;

	double dist_Center=sqrt( (x-my_x)*(x-my_x) + (y-my_y)*(y-my_y) );
	double dist_Radius=( width+ my_width )/2;

	return dist_Center<=dist_Radius;	
}

void Spaceobject::draw() {}

void Spaceobject::hit( Spaceobject *object ) {}

/************************************************************************
 *									*
 * Planet								*
 *									*
 ************************************************************************/
Planet::Planet( double x, double y ) 
:
	Spaceobject ( x, y ),
	hit_vector( Vector_2( 0, 0, K ) )
{	
	char *planet_filename="";
	planet_type=(Planettype)RANDOM(4,0);

	switch ( planet_type ) {

		case P_JUPITER:
			planet_filename="gfx/jupiter.gif";
			weight=WEIGHT_JUPITER;
			spacing=50;
			break;
	
		case P_EARTH:
			planet_filename="gfx/earth.gif";
			weight=WEIGHT_EARTH;
			spacing=40;
			break;
			
		case P_MARS:
			planet_filename="gfx/mars.gif";
			weight=WEIGHT_MARS;
			spacing=30;
			break;

		case P_MOON:
			planet_filename="gfx/moon.gif";
			weight=WEIGHT_MOON;
			spacing=10;
	}
	
	planet_sprite=new Sprite( planet_filename );
	width=(double)planet_sprite->getWidth()-2;	// Sub Anti-Alias Borders!
}

Planet::~Planet()
{
	delete planet_sprite;
}

void Planet::draw()
{
	if ( hit_vector.getLength() > 1 ) {
		Vector_2 v=Vector_2( get_X(), get_Y(), K );
		v+=hit_vector;
		hit_vector=hit_vector.newLength( hit_vector.getLength()/2 );
		set_Pos( v.getX(), v.getY() );
	}

	planet_sprite->setPos( (int)x, (int)y );
	planet_sprite->draw();

}

void Planet::hit( Spaceobject *object )
{
	hit_vector=Vector_2( get_X(), get_Y(), K ) - Vector_2( object->get_X(), object->get_Y(), K );
	hit_vector+=Vector_2( object->get_Speed(), object->get_Direction(), P );
	hit_vector=hit_vector.newLength( 10/get_Weight()+2 );
	
	object->hit( this );
}

/************************************************************************
 *									*
 * Blackhole								*	
 *									*
 ************************************************************************/
Blackhole::Blackhole( double x, double y ) 
:
	Spaceobject( x, y )
{
	hole_sprite=new Sprite( "gfx/hole.gif" );
	width=-1;
	weight=350;
	spacing=150;
	in_background=true;

	for ( int i=0; i<MAXHOLE; i++ ) 
		particles[i]=new Vector_2(
			RANDOM(65,5), 
			RANDOM(2*PI,0),
			P );
}

Blackhole::~Blackhole() 
{
	delete hole_sprite;

	for ( int i=0; i<MAXHOLE; i++ ) 
		delete particles[i];
}

void Blackhole::draw() 
{
	static double hole_animPos=0;

	double x_anim=RANDOM(1,-1);
	double y_anim=RANDOM(1,-1);
	
	hole_sprite->setAlpha(30);
	hole_sprite->setPos( (int)(x+x_anim), (int)(y+y_anim) );
	hole_sprite->draw();

	hole_animPos+=( 5*PI/180 );

	if ( hole_animPos>2*PI )
		hole_animPos-=2*PI;
	else if ( hole_animPos<0 )
		hole_animPos+=2*PI;

	double t_speed=1;

	SDL_LockSurface(MYSDLSCREEN);
	
	for (int i=0; i<MAXHOLE; i++) {
		double t_len=particles[i]->getLength();
		double t_ang=particles[i]->getAngle();

		if ( ( t_ang-=PI/180*3 )<0 ) 
			t_ang+=2*PI;
		
		if ( ( t_len-=t_speed/8+3 )<5 ) 
			t_len+=60+RANDOM(5,-5);

		delete particles[i];
		particles[i]=new Vector_2( t_len, t_ang, P );

		double f=cos( hole_animPos*PI/180 )/6+.75;
		int xx=(int)( x+( particles[i]->getX())*f ) + Sprite::x_offset;
		int yy=(int)( y+( particles[i]->getY())*f ) + Sprite::y_offset;

		if (t_speed++>3 ) 
			t_speed-=3;

		int r=(int)( t_len*2 + 40 );
		int g=r;
		int b=20+(int)( t_len*2 + 20 );
		
		Sprite::putpixel( xx, yy, SDL_MapRGB( MYSDLSCREEN->format, r, g, b ) );

		r/=2;
		g/=2;
		b/=2;
		
		if (t_len>50)
			Sprite::putpixel( xx, yy+1, SDL_MapRGB( MYSDLSCREEN->format, r, g, b ) );
		
		if (t_len>70) {
			Sprite::putpixel( xx+1, yy, SDL_MapRGB( MYSDLSCREEN->format, r, g, b ) );
			Sprite::putpixel( xx+1, yy+1, SDL_MapRGB( MYSDLSCREEN->format, r, g, b ) );
		}

	}
	
	SDL_UnlockSurface(MYSDLSCREEN);
}

void Blackhole::hit( Spaceobject *object ) {}

/************************************************************************
 *									*
 * Wormhole								*	
 *									*
 ************************************************************************/
Wormhole::Wormhole( double x, double y ) 
:
	Spaceobject( x, y )
{
	width=25;
	weight=50;
	spacing=60;
	in_background=true;
	exit_x=RANDOM(350, 150);
	exit_y=RANDOM(350, 150);
	
	if ( (int)RANDOM(2,0)==1 ) exit_x=-exit_x;
	
	if ( (int)RANDOM(2,0)==1 ) exit_y=-exit_y;

	for ( int i=0; i<MAXWORM; i++ ) 
		particles[i]=RANDOM(800,-30);

	for ( int i=0; i<MAXWORM/15; i++ ) 
		start_particles[i]=new Vector_2(
			RANDOM( get_Width()*1.5, 5), 
			RANDOM(2*PI,0),
			P );
}

Wormhole::~Wormhole()
{
	for ( int i=0; i<MAXWORM/15; i++ ) 
		delete start_particles[i];
}

void Wormhole::draw() 
{
	Vector_2 path_to_exit=Vector_2( exit_x, exit_y, K );
	double path_len=path_to_exit.getLength();

	static int mooover=0, mooover2=0;
	static double xm=0, ym=0;
	
	if ( (mooover+=2)>360 ) mooover-=360;
	
	if ( (mooover2-=3)<0 ) mooover+=360;

	SDL_LockSurface(MYSDLSCREEN);
	
	for (int i=0; i<MAXWORM; i++) {

		double p=particles[i];

		if ( ( p+= i%3+1 )>path_len )
			p-=path_len;
		
		particles[i]=p;
		
		double rel_pos=p/path_len;
		Vector_2 pos=path_to_exit.newLength(p);

		// Magic Wormhole Formula =)))
		pos+=Vector_2( 
			( ( cos( (mooover+p*10+i*20)*PI/180 ) * 10 )
			+
			( cos( (mooover2+p*2)*PI/180 ) * 15 ) )
			*
			( ( cos( (rel_pos*360)*PI/180 ) ) ),
			
			pos.getAngle()-(PI/2), 
			P );

		if (p>0 && p<1) {
			xm=pos.getX();	
			ym=pos.getY();	
		}
			
		// Color Factor (Fade out in the middle)
		double cf=(cos( (p/path_len)*2*PI )+1) /3+0.25;
		double xx, yy, rr, gg, bb;

		xx=( get_X()+pos.getX() ) + Sprite::x_offset;
		yy=( get_Y()+pos.getY() ) + Sprite::y_offset;
		rr=( 255-( (255*p/path_len)/4 ) )*cf;
		bb=gg=( 120+( (255*p/path_len)/4 ) )*cf;

		Sprite::putpixel( (int)xx, (int)yy, SDL_MapRGB( MYSDLSCREEN->format, (int)rr, (int)gg, (int)bb ) );
	}

	Vector_2 p_to_moving_center=Vector_2( xm, ym, K );

	for (int i=0; i<MAXWORM/15; i++ ) {
		Vector_2 particle=Vector_2( start_particles[i]->getLength(), start_particles[i]->getAngle(), P );
		Vector_2 part_to_mcenter=p_to_moving_center-particle;
		particle+=part_to_mcenter.newLength( RANDOM(3,1) );	
				
		if (particle.getLength()<=8) {
			particle=Vector_2(
				RANDOM( get_Width()*1.5, get_Width()-10), 
				RANDOM(2*PI,0),
				P );
		}

		delete start_particles[i];
		start_particles[i]=new Vector_2( particle.getX(), particle.getY(), K );

		int rr=(int)(205*8/particle.getLength()+50);
		int gg=(int)(100*8/particle.getLength()+0);
		int bb=gg;

		int xx=(int)( get_X()+particle.getX() ) + Sprite::x_offset;
		int yy=(int)( get_Y()+particle.getY() ) + Sprite::y_offset;

		Sprite::putpixel (xx, yy, SDL_MapRGB( MYSDLSCREEN->format, rr, gg, bb ) );
	}
	
	SDL_UnlockSurface(MYSDLSCREEN);
}

void Wormhole::hit( Spaceobject *object )
{
	object->set_Pos( get_X()+exit_x, get_Y()+exit_y );
}

/************************************************************************
 *									*
 * Flying Saucer							*	
 *									*
 ************************************************************************/
Ufo::Ufo( double x, double y )
:
	Spaceobject(),
	player_id(Ufo::current_playerid),
	is_human(false),
	is_active(false),
	is_locked(false),
	shield_strength(MAXENERGY)
{
	reset();
	
	char *u_filename="", *c_filename="";
	double angle=0;
	
	switch (Ufo::current_playerid++) {
	
		case 0:
			x=BORDERWIDTH;
			angle=0;
			u_filename="gfx/ufored.gif";
			c_filename="gfx/cpktred.gif";
			break;	
		case 1:
			x=SCREENWIDTH-BORDERWIDTH;
			angle=PI;
			u_filename="gfx/ufoblue.gif";
			c_filename="gfx/cpktblue.gif";
			break;
	}

	set_Pos( x, y );
	width=48;
	
	shoot_angle=angle;
	shoot_power=0;
	
	ufo_shoot=new Shoot();
	
	ufo_sprite=new Sprite( u_filename, 25 );
	circle_sprite=new Sprite( c_filename );

	thinking_sprite=new Sprite( "gfx/c_thinking.gif", 2 );
	thinking_sprite->setFramerate(25);
	thinking_sprite->setAlpha(150);

	shooting_sprite=new Sprite( "gfx/c_shooting.gif", 2 );
	shooting_sprite->setFramerate(25);
	shooting_sprite->setAlpha(150);	
}

Ufo::~Ufo() 
{
	delete ufo_sprite;
	delete ufo_shoot;
	delete circle_sprite;
	delete thinking_sprite;
	delete shooting_sprite;
}

bool Ufo::is_dead() const 
{
	return shield_strength<=0;
}

bool Ufo::is_Computer() const
{
	return !is_human;
}

int Ufo::get_Energy() const
{
	return shield_strength;
}

void Ufo::reset()
{
	shield_strength=MAXENERGY;
	is_active=false;
	is_locked=false;
}

void Ufo::set_Human()
{
	is_human=true;
}

void Ufo::set_Computer()
{
	is_human=false;
}

void Ufo::activate()
{	
	is_active=true;
	is_locked=false;
	shoot_power=0;
}
	
void Ufo::deactivate()
{
	is_active=false;
}

void Ufo::move_Up()
{
	if (y>BORDERWIDTH) y=y-2;
}

void Ufo::move_Down()
{
	if ( is_active )
		if (y<SCREENHEIGHT-BORDERWIDTH) y=y+2;
}

void Ufo::inc_ShootAngle()
{
	if ( is_active )
		shoot_angle+=PI/180; 
}

void Ufo::dec_ShootAngle()
{
	if ( is_active )
		shoot_angle-=PI/180; 
}

void Ufo::inc_ShootPower()
{
	if ( is_active )
		if ( ++shoot_power>MAXSHOOTPOWER ) shoot_power=MAXSHOOTPOWER;
}

Shoot *Ufo::shoot()
{
	static Shoot *ufo_shoot=new Shoot();

	if ( is_active ) {
		Vector_2 v_start=Vector_2( get_X(), get_Y(), K )+Vector_2( 60, shoot_angle, P );
		Vector_2 v_direction=Vector_2( shoot_power*SHOOTPOWERFACTOR, shoot_angle, P );

		ufo_shoot->activate( v_start, v_direction );
		
		#ifdef __DEBUG__ 
		__SHOOTS++;
		//std::cout << __HITS << "/" << __SHOOTS << "=" << (int)(__HITS*100/__SHOOTS) << "%" << std::endl;
		#endif
	
		return ufo_shoot;
	}
	
	return NULL;
}

bool Ufo::calculate_Computer_Move( Galaxy *galaxy, int factor ) 
{
	static Shoot s=Shoot();
	static bool found=false;
	static int searches=MAXCOMPUTERSEARCH;
	static double new_ShootPower, new_ShootAngle, new_y;
	bool canshoot=false;
	computer_mode=NONE;
	
	// Zug suchen
	if (!found && !is_locked) {
		canshoot=false;

		// Randomize Shoot-configuration
		new_y=(int)RANDOM( SCREENHEIGHT-200, 100 );
		new_ShootPower=(int)RANDOM( MAXSHOOTPOWER , 10 );
		new_ShootAngle=RANDOM( 2*PI, 0 );
		
		Vector_2 start=Vector_2( get_X(), new_y, K ) + Vector_2( 60, new_ShootAngle, P );
		Vector_2 direction=Vector_2( new_ShootPower*SHOOTPOWERFACTOR, new_ShootAngle, P);

		found=s.will_be_a_Hit( player_id, factor, start, direction, galaxy );
		
		#ifdef __DEBUG__ 
		//if (found) std::cout << "*** FEIND ENTDECKT BEI MIT " <<  start << ", " << direction << std::endl;
		#endif
		
		if (--searches<0) found=true;
		
		if (!found) computer_mode=THINKING;
	}

	// Schuss und Position einstellen
	if (found && !canshoot) {
		canshoot=true;

		if ((int)new_y<y) {
			canshoot=false;
			move_Up(); 
			if ((int)new_y>y) y=(int)new_y;
		}

		if ((int)new_y>y) { 
			canshoot=false;
			move_Down(); 
			if ((int)new_y<y) y=(int)new_y;
		}

		if (new_ShootAngle>shoot_angle) { 
			canshoot=false; 
			inc_ShootAngle(); 

			if (new_ShootAngle<shoot_angle) shoot_angle=new_ShootAngle; 
		}

		if (new_ShootAngle<shoot_angle) {
			canshoot=false;
			dec_ShootAngle(); 

			if (new_ShootAngle>shoot_angle) shoot_angle=new_ShootAngle; 
		}

		if (canshoot && new_ShootPower>shoot_power) {
			canshoot=false;
			inc_ShootPower();
		}

		if (!canshoot) computer_mode=SHOOTING;
	}

	// Schuss
	if (canshoot) {
		computer_mode=NONE;
		searches=MAXCOMPUTERSEARCH;
		shoot_power=new_ShootPower;
		canshoot=false;
		galaxy->set_Shoot( shoot() );
		found=false;
		is_locked=true;
		
		return true;
	}
	
	return false;

}

void Ufo::draw() 
{
	ufo_sprite->setPos( (int)(this->get_X()-2), (int)(get_Y()+10) );
	ufo_sprite->draw();
	
	draw_Targetmode();

	draw_Computermode();
}

void Ufo::hit( Spaceobject *object )
{
	#ifdef __DEBUG__ 
	__HITS++;
	std::cout << __HITS << "/" << __SHOOTS << "=" << (int)(__HITS*100/__SHOOTS) << "%" << std::endl;
	#endif
	
	shield_strength-=(int)object->get_Speed()/10;
	
	if (shield_strength<0) 
		shield_strength=0;

	object->hit( this );
}

void Ufo::draw_hint( Galaxy *galaxy )
{
	Vector_2 start=Vector_2( get_X(), get_Y(), K ) + Vector_2( 60, shoot_angle, P );
	Vector_2 direction=Vector_2( 100*SHOOTPOWERFACTOR, shoot_angle, P );
	
	ufo_shoot->draw_hint( start, direction, galaxy ); 
}

void Ufo::draw_Targetmode()
{	
	if (is_active) {

		Vector_2 m=Vector_2(x, y, K);
		double distance=30;
		int alpha=150;
		
		for (int i=0; i<5; i++) {

			if ( (int)( shoot_power/20 )>i ) circle_sprite->setAlpha( alpha+105 );
			else circle_sprite->setAlpha(alpha);

			Vector_2 pos=m+Vector_2( distance, shoot_angle, P );
			circle_sprite->setPos( (int)pos.getX(), (int)pos.getY() );
			circle_sprite->draw();

			alpha-=20;
			distance+=10;
		}
	
	}
}

void Ufo::draw_Computermode()
{
	if ( is_Computer() ) { 
		
		switch (computer_mode) {
		
			case THINKING: 
				thinking_sprite->setPos( (int)get_X(), (int)get_Y() );
				thinking_sprite->draw();
				break;
			
			case SHOOTING:
				shooting_sprite->setPos( (int)get_X(), (int)get_Y() );
				shooting_sprite->draw();
				break;
			default:
				break;
		}
	}
}	

int Ufo::current_playerid=0;


/************************************************************************
 *									*
 * Explosion								*	
 *									*
 ************************************************************************/
Explosion::Explosion( double x, double y )
:
	Spaceobject( x, y ),
	exploding(false)
{
	explosion_sprite=new Sprite( "gfx/explosionanim.gif", 6 );
	explosion_sprite->setAlpha(230);
	explosion_sprite->setRepeatmode(false);
	explosion_sprite->setFramerate(1);
}

Explosion::~Explosion()
{
	delete explosion_sprite;
}

bool Explosion::is_active() const
{
	return exploding;
}

void Explosion::activate( double x, double y )
{
	set_Pos( x, y );
	explosion_sprite->setPos( (int)x, (int)y );
	explosion_sprite->resetFrames();
	exploding=true;
	
	sound->play(EXPLOSION);
}

void Explosion::draw()
{
	if ( is_active() ) {
		explosion_sprite->draw();
		exploding=!explosion_sprite->is_onLastFrame();
	}	
}

void Explosion::hit( Spaceobject *object ) {}

/************************************************************************
 *									*
 * Shoot								*	
 *									*
 ************************************************************************/
Shoot::Shoot( double x, double y )
:
	Spaceobject( x, y ),
	is_exploding(false),
	moving_time(0),
	last_shootPos( Vector_2(0,0,K) ),
	pre_calculated_Steps(0)
{
	width=8;
	
	laser_sprite=new Sprite( "gfx/shoot.gif" );
	laserback_sprite=new Sprite( "gfx/shootback.gif" );
	explosion=new Explosion();
}

Shoot::~Shoot()
{
	delete laser_sprite;
	delete laserback_sprite;
	delete explosion;
}
	
bool Shoot::is_active()
{
	return moving_time>0;
}

bool Shoot::will_be_a_Hit( int player_id, double factor, Vector_2 start, Vector_2 direction, Galaxy *galaxy )
{
	calculate_ShootPath( start, direction, galaxy );
	
	for (int i=0; i<pre_calculated_Steps; i++) {

		if ( galaxy->is_Ufo_In_Area( player_id, pre_calculated_Pos[i].x, pre_calculated_Pos[i].y, factor ) )
			
			return true;		
	}
		
	return false;
}

void Shoot::activate( Vector_2 start, Vector_2 vector )
{
	set_Pos( start.getX(), start.getY() );
	direction=vector.getAngle();
	speed=vector.getLength();
	last_shootPos=start;
	is_exploding=false;
	moving_time=MAXSHOOTRUN;

	sound->play(SHOOT);
}

void Shoot::destroy()
{
	if ( is_active() )
		hit( this );
}

bool Shoot::move( Galaxy *galaxy )
{
	if ( is_active() ) {
		if ( --moving_time==0 ) return true;
	
		Vector_2 my_shootPos=Vector_2( get_X(), get_Y(), K );
		Vector_2 my_shootVector=Vector_2( speed, direction, P );
		last_shootPos=my_shootPos;
		
		galaxy->calculate_nextPos( my_shootPos, my_shootVector );

		x=my_shootPos.getX();
		y=my_shootPos.getY();
		speed=my_shootVector.getLength();
		direction=my_shootVector.getAngle();
		
		if ( galaxy->has_collision( this ) &&
			!is_active() )
			
			return true;
	}
	
	return false;
}

void Shoot::draw_hint( Vector_2 start, Vector_2 direction, Galaxy *galaxy )
{
	static int colorpos=50;
	
	if ( (colorpos-=20)<50 ) colorpos=255;
	
	calculate_ShootPath( start, direction, galaxy );
	
	int c=colorpos;
	
	SDL_LockSurface( MYSDLSCREEN );

	for (int i=0; i<pre_calculated_Steps; i++) {
		Sprite::putpixel( 
			pre_calculated_Pos[i].x+Sprite::x_offset,
			pre_calculated_Pos[i].y+Sprite::y_offset,
			SDL_MapRGB( MYSDLSCREEN->format, c, c, c ) );

		if ((c+=30)>255) c=50;
	}

	SDL_UnlockSurface( MYSDLSCREEN );
}

void Shoot::draw()
{
	if ( is_active() ) {

		if ( moving_time>100 ||
			(int)RANDOM(100, 0)<( moving_time/2 )+25 ) {
			
			double x_anim=RANDOM(2, -2);
			double y_anim=RANDOM(2, -2);
			int alpha_anim=(int)RANDOM(10, -10);
			
			laserback_sprite->setAlpha( 160+alpha_anim );
			laserback_sprite->setPos(
				(int)( ( get_X()+last_shootPos.getX() )/2 + x_anim ),
				(int)( ( get_Y()+last_shootPos.getY() )/2 + y_anim ) );				
			laserback_sprite->draw();
			
			laser_sprite->setAlpha( 200+alpha_anim );
			laser_sprite->setPos( (int)get_X(), (int)get_Y() );
			laser_sprite->draw();
		}

	}
	
	if ( is_exploding ) {
		explosion->draw();
		is_exploding=explosion->is_active();
	}
}

void Shoot::hit( Spaceobject *object )
{
	explosion->activate( x, y );
	is_exploding=true;
	moving_time=0;
}

void Shoot::calculate_ShootPath( Vector_2 start, Vector_2 direction, Galaxy *galaxy )
{
	static Vector_2 last_angle=Vector_2( 0, 0, K );
	static Vector_2 last_shoot=Vector_2( 0, 0, K );

	if ( last_angle!=direction || last_shoot!=start ) {

		pre_calculated_Steps=0;
		last_angle=direction;
		last_shoot=start;
		
		for (int i=0; i<MAXPRECALC; i++) {
			galaxy->calculate_nextPos( start, direction );
			double x=start.getX();
			double y=start.getY();
			
			if ( !galaxy->check_collision( x, y, get_Width() ) ) {	
				pre_calculated_Pos[i].x=(int)x;
				pre_calculated_Pos[i].y=(int)y; 
				pre_calculated_Steps++;
			} else break; 
		}
	}
}

/************************************************************************
 *									*
 * Galaxy								*
 *									*
 ************************************************************************/
Galaxy::Galaxy( int max, int id )
:
	is_imploding(false),
	objects_in_galaxy(0),
	ufos_in_galaxy(0)
{
	create( max, id );

	shoot=NULL;
}
	
Galaxy::~Galaxy()
{
	for (int i=0; i<objects_in_galaxy; i++) delete objects[i];	
}

double Galaxy::get_ShootX() const
{
	return ( shoot==NULL ) ? 0 : shoot->get_X();
}
	
double Galaxy::get_ShootY() const
{
	return ( shoot==NULL ) ? 0 : shoot->get_Y();
}

bool Galaxy::is_ShootActive() const
{
	return ( shoot==NULL ) ? false : shoot->is_active();
}
	
bool Galaxy::is_Imploding() const
{
	return is_imploding;
}

bool Galaxy::is_Ufo_In_Area( int player_id, double x, double y, double factor )
{
	for (int i=0; i<MAXPLAYER; i++) {

		if ( i!=player_id ) {
			if ( ufos[i]->check_collision( x, y, 8*factor ) ) return true;
		}
	}
	
	return false;
}

void Galaxy::set_Ufos( Ufo **ufos, int max ) 
{
	this->ufos=ufos;
	ufos_in_galaxy=max;	
}

void Galaxy::set_Shoot( Shoot *shoot )
{
	if ( shoot!=NULL ) {
		this->shoot=shoot;
	}
}

void Galaxy::kill_all_Shoots() 
{
	if ( shoot!=NULL ) shoot->destroy();
}

bool Galaxy::has_collision( Spaceobject *object )
{
	for (int i=0; i<objects_in_galaxy; i++ )

		if ( objects[i]->has_collision(object) ) return true;

	for (int i=0; i<ufos_in_galaxy; i++ )

		if ( ufos[i]->has_collision(object) ) return true;

	return false;
}
	
bool Galaxy::check_collision( double x, double y, double width, bool spacing )
{
	for (int i=0; i<objects_in_galaxy; i++ ) 

		if ( objects[i]->check_collision( x, y, width, spacing ) ) 

			return true;

	return false;
}

bool Galaxy::create( int max, int id )
{
	if ( !is_imploding ) {
		srand(id);
		
		for ( int i=0; i<objects_in_galaxy; i++ )
			delete objects[i];

		objects_in_galaxy=0;
		max=( max<MAXPLANETS ) ? max : MAXPLANETS;
		
		for (int i=0; i<max; i++ ) {
			double x_test, y_test, width_test;
			Spaceobject *tmp_planet=NULL;
			int type_of_planet=(int)RANDOM(8,0);

			//std::cout << "PLANETTYPE=" << type_of_planet << std::endl;

			switch ( type_of_planet ) {
				
				case 5: 
					tmp_planet=new Blackhole();
					break;
				case 6: 
					tmp_planet=new Wormhole();
					break;
				default:
					tmp_planet=new Planet();
			}
			
			do {	
				x_test=RANDOM( SCREENWIDTH-175, 175 );
				y_test=RANDOM( SCREENHEIGHT-20, +10 );
				width_test=tmp_planet->get_Width() + tmp_planet->get_Spacing();
			} while ( check_collision( x_test, y_test, width_test, true ) );

			tmp_planet->set_Pos( x_test, y_test );
			animate_position[i].y=y_test;
			objects[objects_in_galaxy++]=tmp_planet;
		}

		for (int i=0; i<objects_in_galaxy; i++)
			objects[i]->set_Pos( objects[i]->get_X(), -600 );

		is_imploding=true;
		
		sound->play(WARPGALAXY);

		return true;
	}
	
	return false;
}
	
void Galaxy::calculate_nextPos( Vector_2 &position, Vector_2 &direction )
{
	Vector_2 vec_toPlanets=Vector_2( 0, 0, K );

	for (int i=0; i<objects_in_galaxy; i++) {
		// Vektor zum aktuellen Planeten
		Vector_2 vec_PlanetPos=Vector_2( objects[i]->get_X(), objects[i]->get_Y(), K);
		// Abstand zu unserem Schuss
		double distance=vec_PlanetPos.distance(position);
		// Vektor vom Schuss zum Planeten:
		Vector_2 vec_toPlanet=vec_PlanetPos-position;
		// Neue Laenge fuer den Vector
		// (Anziehungskraft ergibt sich physikalisch ;)
		vec_toPlanet=vec_toPlanet.newLength( objects[i]->get_Weight()/distance );
		// gemeinsamen Anziehungsvektor aller Planeten erstellen:
		vec_toPlanets=vec_toPlanets+vec_toPlanet;
	}
	// Alle Anziehungsvektoren werden zum Schussvektor addiert
	direction+=vec_toPlanets;

	// Neuen Schussvektor zur Schussposition addieren:
	position+=direction.newLength( direction.getLength()*SHOOT_INTERVAL/1000 );
}
		
bool Galaxy::animate()
{
	if ( shoot!=NULL )
		
		if ( shoot->move(this) )
			return true;

	return false;
}

void Galaxy::draw()
{
	is_imploding=animate_BigBang();
	
	for (int i=0; i<objects_in_galaxy; i++)

		if (objects[i]->is_in_Background())
			objects[i]->draw();

	for (int i=0; i<objects_in_galaxy; i++)

		if (!objects[i]->is_in_Background())
			objects[i]->draw();

	for (int i=0; i<ufos_in_galaxy; i++)
		ufos[i]->draw();

	if ( shoot!=NULL )
		shoot->draw();
}

bool Galaxy::animate_BigBang() 
{
	bool still_moving=false;

	if (is_imploding) {
	
		for (int i=0; i<objects_in_galaxy; i++) {
			double object_y=objects[i]->get_Y();
			double object_final_y=animate_position[i].y;
			
			if (object_final_y>object_y) {
				still_moving=true;
				object_y=object_y+( (object_final_y-object_y)/10+1 );
				
				if ( object_final_y<object_y ) object_y=object_final_y;
			}
			
			objects[i]->set_Pos( objects[i]->get_X(), object_y);
		}
	}
	
	return still_moving;
	
}

