/************************************************************************
 *
 * vector2d
 * Patrick Gerdsmeier 2004, <patrick@gerdsmeier.net>
 *
 * Implementiert eine 2D-Vektor Klasse. Vektoren koennen
 * mit ihren karthesisch oder polar initialisiert werden:
 *
 * Vector2d v1=Vector2d( 100, 100, K );
 * Vector2d v2=Vector2d( 100, 100, P );
 * 
 * Vektoren koennen addiert oder subtrahiert werden:
 * 
 * Vector2d v3=v1+v2;
 * v2-=v1;
 *
 * Vektoren koennen auf Gleichheit ueberprueft werden:
 *
 * if (v1!=v2) exit(1);
 *
 * Weiterhin existieren Funktionen rund um Vektoren.
 *
 ************************************************************************/

#ifndef __VECTOR2D_H__
#define __VECTOR2D_H__

#include <iostream>

enum Vectortype { K, P, U };

class Vector2d
{
public:
	Vector2d( double, double, Vectortype );

	bool isInfinite();
	double getX();
	double getY();
	double getLength();
	double getAngle();
	
	bool operator==( Vector2d );
	bool operator!=( Vector2d );
	
	Vector2d& operator+=( Vector2d );
	Vector2d operator+( Vector2d );
	Vector2d& operator-=( Vector2d );
	Vector2d operator-( Vector2d );
	
	double distance( Vector2d );
	double projectOn( double );
	Vector2d newLength( double );
	Vector2d newVectorTo( double, double );

	static double angleDifference( double, double );
	static Vector2d rayCrossPoint( Vector2d, double, Vector2d, double );
	
	void print();

private:
	double x, y, length, angle;
	bool infinite;
}; 

	std::ostream& operator<<( std::ostream&, Vector2d );
	

#endif
