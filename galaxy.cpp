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
extern Soundset sound;

#ifdef __DEBUG__
extern int __SHOOTS;
#endif

Planet::Planet( int x, int y, Planettype type ) :
	x(x),
	y(y),
	weight(getWeightByType(type)),
	hit(0),
	type(type),
	planet(getImageByType(type)),
	holeanim((int)(360*(rand()/(RAND_MAX+1.0))))
{ 
	for (int i=0; i<MAXHOLE; i++) {
		double l;
		
		if (type==BLACKHOLE) l=60*(rand()/(RAND_MAX+1.0))+5;
		else l=25*(rand()/(RAND_MAX+1.0))+5;
		
		double a=2*PI*(rand()/(RAND_MAX+1.0));
		holes[i]=new Vector2d( l, a, P );
	}
	
	if (type==WORMHOLE) {
		worm_x=((SCREENWIDTH-350)*(rand()/(RAND_MAX+1.0))+175);
		worm_y=(SCREENHEIGHT*(rand()/(RAND_MAX+1.0)));
	}
}
	
Planet::~Planet()
{
	for (int i=0; i<MAXHOLE; i++) delete holes[i];
}

int Planet::getX() { return (int)x; }

int Planet::getY() { return (int)y; }

int Planet::getWeight() { return weight; }

int Planet::getWidth()
{ 
	if (type==BLACKHOLE) return -1;
	if (type==WORMHOLE) return 25;

	return planet.getWidth()-2;
}

bool Planet::isWormhole() { return (type==WORMHOLE) ? true : false; }

int Planet::getWormX() { return (int)worm_x; }

int Planet::getWormY() { return (int)worm_y; }

void Planet::setX( int x ) { this->x=(double)x; }

void Planet::setY( int y ) { this->y=(double)y; }

bool Planet::collision( int x, int y, int w, bool active, bool init )
{
	if (!init && type==BLACKHOLE) return false;
	
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

void Planet::draw() 
{
	if (type<BLACKHOLE) {
		animateHit();
		planet.setPos((int)x, (int)y);
		planet.draw();
	} else {
		drawBlackHole();
	}
	
}	
	
int Planet::getWeightByType( Planettype type )
{
	int w;

	switch (type) {
		case JUPITER:
			w=WEIGHT_JUPITER;
			break;
		case EARTH:
			w=WEIGHT_EARTH;
			break;
		case MARS:
			w=WEIGHT_MARS;
			break;
		case MOON:
			w=WEIGHT_MOON;
			break;
		case BLACKHOLE: w=WEIGHT_BLACKHOLE;
			break;
		default:
			w=WEIGHT_WORMHOLE;
	}

	return w;
}

Sprite Planet::getImageByType( Planettype type )
{
	char* filename;
	
	switch (type) {
		case JUPITER:
			filename="gfx/jupiter.bmp";
			break;
		case EARTH:
			filename="gfx/earth.bmp";
			break;
		case MARS:
			filename="gfx/mars.bmp";
			break;	
		case MOON:
			filename="gfx/moon.bmp";
			break;
		default:
			filename="gfx/hole.bmp";
	}
	
	return Sprite(filename);
}

void Planet::drawBlackHole()
{
	int xp=(int)(2*(rand()/(RAND_MAX+1.0))-1);
	int yp=(int)(2*(rand()/(RAND_MAX+1.0))-1);
	planet.setAlpha(20);
	planet.setPos((int)x+xp, (int)y+yp);
	planet.draw();

	holeanim+=2; if (holeanim>360) holeanim-=360;
	double m=1;
	SDL_LockSurface(MYSDLSCREEN);
	
	for (int i=0; i<( (type==WORMHOLE)?MAXWORM:MAXHOLE ); i++) {
		double l=holes[i]->getLength();
		double a=holes[i]->getAngle();
		l-=m/5*3; 
		
		if (type==BLACKHOLE) { 
			a-=PI/180*4; 
			if (a<0) a+=2*PI;
			if (l<5) l+=65;
		} else {
			a+=PI/180*4; 
			if (a>2*PI) a-=2*PI;		
			if (l<5) l+=35;
		}

		delete holes[i];
		holes[i]=new Vector2d( l, a, P );

		double f=cos(holeanim*PI/180)/6+.75;
		int xx=(int)( x+(holes[i]->getX())*f)+Sprite::x_offset;
		int yy=(int)( y+(holes[i]->getY())*f)+Sprite::y_offset;

		if (m++>3) m-=3;

		int r=(type==WORMHOLE) ? (int)(l*3+30) : (int)(l*2+50);
		int g=(type==WORMHOLE) ? (int)(l*2+50) : (int)(m*l/2+30);
		int b=(type==WORMHOLE) ? (int)(m*l/2) : (int)(l*2+50);
		Sprite::putpixel( xx, yy, SDL_MapRGB( MYSDLSCREEN->format, r, g, b ) );
	}
	
	SDL_UnlockSurface(MYSDLSCREEN);
}

void Planet::animateHit()
{
	if (hit>0) {
		double hh=(cos(45+(90-(double)hit*9)/180*PI)+1)*100/(double)weight;
		Vector2d v=Vector2d(hit_x, hit_y, K).newLength(hh);
		x+=v.getX();
		y+=v.getY();
		--hit;
	}
}

Galaxy::Galaxy( int planets ) : imploding(false)
{
	initGalaxy( planets );
}	

Galaxy::~Galaxy() 
{
	for (int i=0; i<planets; i++) delete galaxy[i];
}

Planet** Galaxy::getPlanetList() { return galaxy; }

int Galaxy::getPlanets() { return planets; }

bool Galaxy::isImploding() { return imploding; }

void Galaxy::initGalaxy( int planets )
{
	if (!imploding) {
		
		srand(time(NULL));

		// Clear current planets:
		for (int i=0; i<this->planets; i++) delete galaxy[i];

		if (planets>MAXPLANETS) planets=MAXPLANETS;
		
		int myholes=0;
		this->planets=planets;

		for (int i=0; i<planets; i++) {
			bool pos_notfound;
			int x, y, p, w;

			do {
				x=(int)((SCREENWIDTH-350)*(rand()/(RAND_MAX+1.0))+175);
				y=(int)(SCREENHEIGHT*(rand()/(RAND_MAX+1.0)));
				p=(int)((PLANETTYPES)*(rand()/(RAND_MAX+1.0)));
				w=Planet::getWeightByType((Planettype)p);
									
				if (p>=4) myholes++;
				pos_notfound=false;
				
				if (myholes<=MAXHOLES || p<4) {
					for (int j=0; j<i; j++) {
						if (galaxy[j]->collision(x, y, 250, false, true)) {
							pos_notfound=true;
							break;
						}	
					}
				} else pos_notfound=true;
				
			} while (pos_notfound);
			
			galaxy[i]=new Planet(x, y, (Planettype)p);
			wanted_pos[i].y=y;
		}
		
		for (int i=0; i<planets; i++) galaxy[i]->setY(-600);

		sound.play(WARPGALAXY);
		imploding=true;
	}
}

int Galaxy::collision( int x, int y, int w, bool active )
{
	for (int i=0; i<planets; i++) {
		if (galaxy[i]->collision( x, y, w, active )) {
			return i;
		}	
	}

	return -1;
}
	
void Galaxy::draw() 
{
	if (imploding) imploding=bigBang();
		
	for (int i=0; i<planets; i++) {
		galaxy[i]->draw();
	}
}
	
bool Galaxy::bigBang() 
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

Shoot::Shoot() :
	x(200), 
	y(200),
	active(false),
	w(8),
	preCalculated(0),
	shoot(Sprite("gfx/shoot.bmp")), 
	shootback(Sprite("gfx/shootback.bmp")),
	vdir(Vector2d(0,0,K)), 
	last_shoot(Vector2d(0,0,K)), 
	last_angle(Vector2d(0,0,K)) 
{}

int Shoot::getX() { return (int)x; }

int Shoot::getY() { return (int)y; }

int Shoot::getWidth() { return w; }

bool Shoot::isActive() { return active; }

void Shoot::setPos( double x, double y )
{
	this->x=x;
	this->y=y;
}

void Shoot::kill() { active=false; }

void Shoot::activate( Vector2d p, Vector2d v )
{
	vdir=v;
	this->x=p.getX();
	this->y=p.getY();
	lastx=x;
	lasty=y;
	active=true;
	shoot_time=MAXSHOOTRUN;
	sound.play(SHOOT);
	
	#ifdef __DEBUG__
	__SHOOTS++;
	#endif
}

void Shoot::draw()
{
	if (isActive()) {
		int blink=(int)(100*(rand()/(RAND_MAX+1.0)));

		if (shoot_time>100 || blink<(shoot_time/2)+25) {
			
			int xp=(int)(3*(rand()/(RAND_MAX+1.0))-2);
			int yp=(int)(3*(rand()/(RAND_MAX+1.0))-2);
			int ap=(int)(20*(rand()/(RAND_MAX+1.0))-15);
			
			shootback.setAlpha(120+ap);
			shootback.setPos((int)(x+lastx)/2-xp, (int)(y+lasty)/2-yp);
			shootback.draw();
			shootback.setAlpha(160+ap);
			shootback.setPos((int)x+xp, (int)y+yp);
			shootback.draw();
			shoot.setPos((int)x, (int)y);
			shoot.draw();
		}
	}
}

void Shoot::drawPrePath( Vector2d vecShootPos, Vector2d vecShoot, Galaxy *galaxy )
{
	static int colorpos=50;
	if ((colorpos-=20)<50) colorpos=255;
	
	preCalculatePath(vecShootPos, vecShoot, galaxy );
	
	int c=colorpos;
	
	SDL_LockSurface( MYSDLSCREEN );
	for (int i=0; i<preCalculated; i++) {
		Sprite::putpixel( preCalculatedPos[i].x+Sprite::x_offset, preCalculatedPos[i].y+Sprite::x_offset, SDL_MapRGB( MYSDLSCREEN->format, c, c, c ) );
		if ((c+=30)>255) c=50;
	}
	SDL_UnlockSurface( MYSDLSCREEN );
}

bool Shoot::hitEnemy( Vector2d vecShootPos, Vector2d vecShoot, Galaxy *galaxy, int target_x, int target_y, int target_width )
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

bool Shoot::animate( Planet* galaxy[], int planets ) 
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
			elapsed_time=SHOOT_INTERVAL;
			
			getNextShootStatus(vecShootPos, vdir, galaxy, planets, elapsed_time);

			x=vecShootPos.getX();
			y=vecShootPos.getY();
		}
		
	}

	return true;
}	

void Shoot::preCalculatePath( Vector2d &vecShootPos, Vector2d &vecShoot, Galaxy *galaxy ) 
{
	//if ((preCalculated+=MAXPRECALCPERFRAME)>MAXPRECALC)
	preCalculated=0;		

	if (last_angle!=vecShoot || last_shoot!=vecShootPos ) {
		for (int i=0; i<MAXPRECALC; i++) {
			getNextShootStatus(vecShootPos, vecShoot, galaxy->getPlanetList(), galaxy->getPlanets(), SHOOT_INTERVAL);
			int x=(int)vecShootPos.getX();
			int y=(int)vecShootPos.getY();
			int p=galaxy->collision( x, y, w, false );
			
			if ( p!=-1) {
				if ( galaxy->getPlanetList()[p]->isWormhole() ) {
					double nx=galaxy->getPlanetList()[p]->getWormX();
					double ny=galaxy->getPlanetList()[p]->getWormY();
				
					vecShootPos=Vector2d( nx, ny, K );
					x=(int)nx;
					y=(int)ny;
				} else break;
			}
			preCalculatedPos[i].x=x;
			preCalculatedPos[i].y=y;
			preCalculated++;
		}
	}
}

void Shoot::getNextShootStatus( Vector2d &vecShootPos, Vector2d &vecShoot, Planet *galaxy[], int planets, double timeElapsed )
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

Ufo::Ufo( int x, int y, double sa, bool human ) :
	x(x),
	y(y),
	energy(MAXENERGY), 
	shoot_angle(sa),
	shoot_power(0),
	human(human),
	computer_mode(NONE),
	ufo_sprite(getUfoSprite()),
	cpkt_sprite(getCirclePktSprite()), 
	c_think(Sprite("gfx/c_thinking.bmp", 2)), 
	c_shoot(Sprite("gfx/c_shooting.bmp", 2))
{
	c_think.setFramerate(25);
	c_think.setAlpha(150);
	c_shoot.setFramerate(25);
	c_shoot.setAlpha(150);
}

bool Ufo::isDead() { return (energy<=0) ? true : false ; }

bool Ufo::isActive() { return active; }

bool Ufo::isComputer() { return !human; }

int Ufo::getX() { return x; }	

int Ufo::getY() { return y; }

int Ufo::getWidth() { return 49; }

int Ufo::getEnergy() { return energy; }

double Ufo::getShootAngle() { return shoot_angle; }

Vector2d Ufo::getShootVector() { return Vector2d( shoot_power*SHOOTPOWERFACTOR, shoot_angle, P); }	

Vector2d Ufo::getShootStart()
{ 
	Vector2d me=Vector2d(x,y,K);
	Vector2d p=Vector2d(60, shoot_angle, P);
	Vector2d pos=me+p;
	
	return pos;
}	

void Ufo::resetEnergy() { energy=MAXENERGY; }

void Ufo::subEnergy( int e ) { energy-=e; }

void Ufo::incShootAngle() { shoot_angle+=PI/180; }

void Ufo::decShootAngle() { shoot_angle-=PI/180; }

void Ufo::incShootPower()
{
	shoot_power+=1;
	if (shoot_power>MAXSHOOTPOWER) shoot_power=MAXSHOOTPOWER;
}

void Ufo::activate() { active=true; shoot_power=0; }

void Ufo::inactivate() { active=false; }

void Ufo::moveN() { if (y>BORDERWIDTH) y=y-2; }	

void Ufo::moveS() { if (y<SCREENHEIGHT-BORDERWIDTH) y=y+2; }	

void Ufo::setComputer( int computer ) { human=!computer; computer_mode=NONE; }

void Ufo::setPos( int x, int y )
{
	this->x=x;
	this->y=y;
}

bool Ufo::collision( int x, int y, int w )
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

bool Ufo::calculateComputerMove( Galaxy *galaxy, int target_x, int target_y, int target_width )
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

void Ufo::draw() 
{
	ufo_sprite.setPos( x-2, y+10 );
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
			case NONE: 
				break;
		}
	}
}

Sprite Ufo::getUfoSprite()
{
	char* filename="";
	static int member_of_game=0;
	
	switch (member_of_game) {
		case 0:
			filename="gfx/ufored.bmp";
			break;	
		case 1:
			filename="gfx/ufoblue.bmp";
			break;	
	}

	member_of_game++;

	return Sprite(filename,25);
}

Sprite Ufo::getCirclePktSprite()
{
	char* filename="";
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

void Ufo::drawCircle() 
{
	Vector2d m=Vector2d(x, y, K);
	double ss=30;

	int a=150;
	
	for (int i=0; i<5; i++) {
		if ((int)(shoot_power/20)>i) cpkt_sprite.setAlpha(a+105);
		else cpkt_sprite.setAlpha(a);
		a-=20;
		Vector2d pos=m+Vector2d(ss, shoot_angle, P);
		cpkt_sprite.setPos( (int)pos.getX(), (int)pos.getY() );
		cpkt_sprite.draw();
		ss+=10;
	}
}

Explosion::Explosion() :
	active(false),
	expl_sprite(Sprite("gfx/explosionanim.bmp",6))
{}

void Explosion::activate( int x, int y ) 
{
	this->x=x;
	this->y=y;
	counter=50;
	active=true;
	expl_sprite.setFramerate(1);
	expl_sprite.setAlpha(230);
	expl_sprite.setRepeatmode(false);
	expl_sprite.resetFrames();
	sound.play(EXPLOSION);	
}

void Explosion::draw() 
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

