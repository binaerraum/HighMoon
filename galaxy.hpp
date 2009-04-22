/******************************************************************************************
 *
 * HighNoon - Duell im All
 * Copyright (c) 2005 Patrick Gerdsmeier <patrick@gerdsmeier.net>
 *
 * "galaxy.hpp"
 * 
 * Spaceobject - virtual Object that contains all Information
 * that is needed for an Object in Galaxy. Extended by all
 * other Objects.
 *
 * Planet, Wormhole, Blackhole - Shown on Screen
 *
 * Ufo - Flying Saucer. An Ufo-Object contains Shoot-Objects
 *
 * Shoot - A Lasershoot. Contains an Explosion-Object that
 * is drawn when Shoot hits/is been hit.
 *
 * Explosion - Animate an Explosion.
 *
 * Galaxy - This Object contains Planets, Wormholes, Blackholes,
 * Ufos and Shoots.
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
#include "vector_2.hpp"
#include "graphics.hpp"

class Shoot;
class Galaxy;

/************************************************************************
 *									*
 * Spaceobject								*	
 *									*
 ************************************************************************/
class Spaceobject
{
public:
	Spaceobject( double x=0, double y=0 );

	virtual ~Spaceobject();
	
	void set_Pos( double x, double y );
	
	bool is_in_Background() const;

	double get_X() const;

	double get_Y() const;

	double get_Width() const;
	
	double get_Weight() const;
	
	double get_Speed() const;

	double get_Direction() const;
	
	double get_Spacing() const;
	
	// This is a "real" collision Function. That means
	// that within this Function the Method hit() is
	// called.
	bool has_collision( Spaceobject *object );

	// This checks if the Object has a collision with an
	// Area presented by x, y and width values.
	// Returns TRUE if there was a collision.
	bool check_collision( double x, double y, double width, bool spacing=false );
	
	// Graphical Output on the Screen
	virtual void draw();
	
	// hit() should be implemented from any Spaceobject and should
	// contain the reaction of the Object if it has a collision
	// with another Spaceobject.
	// One can change the Shoot-Status, get the Direction and the
	// Speed of that Spaceobject etc.
	virtual void hit( Spaceobject *object );

protected:
	double x, y;
	double width, weight;
	double speed, direction;
	double spacing;
	bool in_background;
	
};

/************************************************************************
 *									*
 * Planet								*	
 *									*
 ************************************************************************/
class Planet : public Spaceobject
{
public:
	Planet( double x=0, double y=0 );

	~Planet();
		
	void draw();

	void hit( Spaceobject *object );
	
private:
	Vector_2 hit_vector;

	enum Planettype { 
		P_JUPITER=0,
		P_EARTH=1,
		P_MARS=2,
		P_MOON=3 
	} planet_type;

	Sprite *planet_sprite;

};

/************************************************************************
 *									*
 * Blackhole								*	
 *									*
 ************************************************************************/
class Blackhole : public Spaceobject
{
public:
	Blackhole( double x=0, double y=0 );

	~Blackhole();
		
	void draw();

	void hit( Spaceobject *object );
	
private:
	Vector_2 *particles[MAXHOLE];
	Sprite *hole_sprite;
	
};

/************************************************************************
 *									*
 * Wormhole								*	
 *									*
 ************************************************************************/
class Wormhole : public Spaceobject
{
public:
	Wormhole( double x=0, double y=0 );

	~Wormhole();
		
	void draw();

	void hit( Spaceobject *object );
	
private:
	double exit_x, exit_y;
	double particles[MAXWORM];
	Vector_2 *start_particles[MAXWORM/15];

};

/************************************************************************
 *									*
 * Explosion								*	
 *									*
 ************************************************************************/
class Explosion : public Spaceobject
{
public:
	Explosion( double x=0, double y=0 );

	~Explosion();
	
	bool is_active() const;

	void activate( double x, double y );
	
	void draw();

	void hit( Spaceobject *object );

private:
	bool exploding;
	Sprite *explosion_sprite;

};

/************************************************************************
 *									*
 * Shoot								*	
 *									*
 ************************************************************************/
class Shoot : public Spaceobject
{
public:
	Shoot( double x=0, double y=0 );

	~Shoot();
	
	bool is_active();

	bool will_be_a_Hit( int player_id, double factor, Vector_2 start, Vector_2 direction, Galaxy *galaxy );

	void activate( Vector_2 start, Vector_2 vector );

	void destroy();

	bool move( Galaxy *galaxy );

	void draw();

	void draw_hint( Vector_2 start, Vector_2 direction, Galaxy *galaxy );

	void hit( Spaceobject *object );

private:
	bool is_exploding;
	int moving_time;
	Vector_2 last_shootPos;
	int pre_calculated_Steps;

	struct { 
		int x, y;
	} pre_calculated_Pos[MAXPRECALC];
	
	Sprite *laser_sprite,
		*laserback_sprite;

	Explosion *explosion;

	void calculate_ShootPath( Vector_2 start, Vector_2 direction, Galaxy *galaxy );

};

/************************************************************************
 *									*
 * Flying Saucer							*	
 *									*
 ************************************************************************/
class Ufo : public Spaceobject
{
public:
	Ufo( double x, double y );

	~Ufo();

	bool is_dead() const;
	
	bool is_Computer() const;

	int get_Energy() const;

	void reset();

	void set_Human();
	
	void set_Computer();
	
	void activate();
	
	void deactivate();

	void move_Up();

	void move_Down();

	void inc_ShootAngle();

	void dec_ShootAngle();

	void inc_ShootPower();

	Shoot *shoot();
	
	bool calculate_Computer_Move( Galaxy *galaxy, int factor );

	void draw();
	
	void draw_hint( Galaxy *galaxy );

	void hit( Spaceobject *object );

private:
	int player_id;
	bool is_human,
		is_active,
		is_locked;
	int shield_strength;
	double shoot_power,
		shoot_angle;
	
	enum { NONE, THINKING, SHOOTING } computer_mode;
	Shoot *ufo_shoot;
	Sprite *ufo_sprite, 
		*circle_sprite, 
		*thinking_sprite,
		*shooting_sprite;
	
	static int current_playerid;

	void draw_Targetmode();

	void draw_Computermode();
};

/************************************************************************
 *									*
 * Galaxy								*	
 *									*
 ************************************************************************/
class Galaxy 
{
public:
	Galaxy( int max, int seed );
	
	~Galaxy();

	double get_ShootX() const;
	
	double get_ShootY() const;
	
	bool is_ShootActive() const;
	
	bool is_Imploding() const;

	bool is_Ufo_In_Area( int player_id, double x, double y, double factor );
	
	void set_Ufos( Ufo **ufos, int max );

	void set_Shoot( Shoot *shoot );

	void kill_all_Shoots();
	
	bool has_collision( Spaceobject *object );
	
	bool check_collision( double x, double y, double width, bool spacing=false );
	
	bool create( int max, int seed );
	
	void calculate_nextPos( Vector_2 &position, Vector_2 &direction );

	bool animate();
	
	void draw();
	
private:
	bool is_imploding;
	int objects_in_galaxy;
	int ufos_in_galaxy;
	Spaceobject *objects[MAXPLANETS];
	Ufo **ufos;
	Shoot *shoot;

	struct {
		double x, y;
	} animate_position[MAXPLANETS];

	bool animate_BigBang();
};

#endif
