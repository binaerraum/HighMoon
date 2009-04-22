/******************************************************************************************
 *
 * HighNoon - Duell im All
 * Copyright (c) 2005 Patrick Gerdsmeier <patrick@gerdsmeier.net>
 *
 * "galaxy.hpp"
 * 
 * This file contains the galaxy and it's objects:
 * Planet, Galaxy, Shoot, Ufo, Explosion
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

#ifndef __GALAXY_HPP__
#define __GALAXY_HPP__

#include "constants.hpp"
#include "vector2d.hpp"
#include "graphics.hpp"


/******************************************************************************************
 *
 * Planet
 *
 * Contains the data of a planet.
 *
 ******************************************************************************************/
class Planet
{
public:
	Planet( int x, int y, Planettype type );
	
	~Planet();
	
	int getX();

	int getY();
	
	int getWeight();

	int getWidth();
	
	bool isWormhole();
	
	int getWormX();
	
	int getWormY();

	void setX( int x );

	void setY( int y );

	bool collision( int x, int y, int w, bool active=true, bool init=false );
	
	void draw();
		
	void drawBlackHole();

	static int getWeightByType( Planettype type );

	static Sprite getImageByType( Planettype type );

private:
	double x, y;
	int weight;
	int hit;
	int type;
	Sprite planet;
	double holeanim;
	double hit_x, hit_y;
	Vector2d *holes[MAXHOLE];
	double worm_x, worm_y;
	
	void animateHit();

};

/******************************************************************************************
 *
 * Galaxy
 *
 * Contains the information about the galaxy (planets).
 *
 ******************************************************************************************/
class Galaxy
{
public:
	Galaxy( int planets );
	
	~Galaxy();

	Planet** getPlanetList();
	
	int getPlanets();
	
	bool isImploding();
	
	void initGalaxy( int planets );

	int collision( int x, int y, int w, bool active=true );		

	void draw();
		
private:
	struct { int x, y; } wanted_pos[MAXPLANETS];
	Planet* galaxy[MAXPLANETS];
	int planets;
	bool imploding;	

	bool bigBang();
	
};

/******************************************************************************************
 *
 * Shoot
 *
 * Contains the lasershoot. Functions for the gravity animation and the
 * calculation of a complete path.
 *
 ******************************************************************************************/
class Shoot
{
public:
	Shoot();
	
	int getX();

	int getY();

	int getWidth();

	bool isActive();

	void setPos( double x, double y );
	
	void kill();
	
	void activate( Vector2d p, Vector2d v );
	
	void draw();
	
	void drawPrePath( Vector2d vecShootPos, Vector2d vecShoot, Galaxy *galaxy );
		
	bool hitEnemy( Vector2d vecShootPos, Vector2d vecShoot, Galaxy *galaxy, int target_x, int target_y, int target_width );
	
	bool animate( Planet* galaxy[], int planets );
	
private:
	struct { int x, y; } preCalculatedPos[MAXPRECALC];
	double x, y, lastx, lasty;
	bool active;
	int w;
	int preCalculated;
	Sprite shoot, shootback;
	Vector2d vdir, last_shoot, last_angle;	
	int shoot_time;
	
	void preCalculatePath( Vector2d &vecShootPos, Vector2d &vecShoot, Galaxy *galaxy );

	void getNextShootStatus( Vector2d &vecShootPos, Vector2d &vecShoot, Planet *galaxy[], int planets, double timeElapsed );
};

/******************************************************************************************
 *
 * Ufo
 *
 * Contains the information of the spaceship. Includes the ai-computer move
 *
 ******************************************************************************************/
class Ufo
{
public:
	Ufo( int x, int y, double sa, bool human );
	
	bool isDead();

	bool isActive();

	bool isComputer();

	int getX();

	int getY();

	int getWidth();

	int getEnergy();

	double getShootAngle();

	Vector2d getShootVector();

	Vector2d getShootStart();

	void resetEnergy();

	void subEnergy( int e );

	void incShootAngle();

	void decShootAngle();

	void incShootPower();
	
	void activate();

	void inactivate();
	
	void moveN();

	void moveS();

	void setComputer( int computer );

	void setPos( int x, int y );

	bool collision( int x, int y, int w );
	
	bool calculateComputerMove( Galaxy *galaxy, int target_x, int target_y, int target_width );
	
	void draw();
	
private:
	int x, y;
	int energy;
      	double shoot_angle, shoot_power;
	bool active, human;
	enum { NONE, THINKING, SHOOTING } computer_mode;
	Sprite ufo_sprite, cpkt_sprite, c_think, c_shoot;
	
	static Sprite getUfoSprite();

	static Sprite getCirclePktSprite();
	
	void drawCircle();
};

/******************************************************************************************
 *
 * Explosion
 *
 * Just to show an explosion on the screen.
 *
 ******************************************************************************************/
class Explosion
//-----------------------------------------------------------------------------------------
{
public:
	Explosion();

	void activate( int x, int y );
		
	void draw();
	
private:
	int x, y;
	bool active;
	int counter;
	Sprite expl_sprite;
};

#endif

