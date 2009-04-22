/************************************************************************
 *
 * vector2d
 * Patrick Gerdsmeier 2004, <patrick@gerdsmeier.net>
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
#include <cmath>

#include "vector2d.hpp"

const double PI=3.1415;

Vector2d::Vector2d( double a, double b, Vectortype typ )
{
	switch (typ) {
		
		case P:
			length=a;
			angle=b;
			x=length*cos( angle );
			y=length*sin( angle );
			infinite=false;
			break;
		case K:
			x=a;
			y=b;
			length=sqrt( x*x+y*y );
			angle=atan2( y,x );
			infinite=false;
			break;
		case U:
			x=0;
			y=0;
			infinite=true;
	}
}

bool Vector2d::isInfinite() { return infinite; }

double Vector2d::getX() { return x; }

double Vector2d::getY() { return y; }

double Vector2d::getLength() { return length; }

double Vector2d::getAngle() { return angle; }

bool Vector2d::operator==( Vector2d v )
{
	return ( x==v.x && y==v.y );
}

bool Vector2d::operator!=( Vector2d v )
{
	return !( *this==v );
}

Vector2d& Vector2d::operator+=( Vector2d v )
{
	x+=v.getX();
	y+=v.getY();
	length=sqrt( x*x+y*y );
	angle=atan2( y,x );
	
	return *this;
}

Vector2d Vector2d::operator+( Vector2d v )
{
	Vector2d r=*this;
	
	return r+=v;
}

Vector2d& Vector2d::operator-=( Vector2d v )
{
	x-=v.getX();
	y-=v.getY();
	length=sqrt( x*x+y*y );
	angle=atan2( y,x );
	
	return *this;
}

Vector2d Vector2d::operator-( Vector2d v )
{
	Vector2d r=*this;
	
	return r-=v;
}

double Vector2d::distance( Vector2d v )
{
	double dx = x-v.getX();
	double dy = y-v.getY();

	return sqrt( dx*dx+dy*dy );
}	

double Vector2d::projectOn( double angle )
{
	return length*cos( angle-this->angle );
}

Vector2d Vector2d::newLength( double len )
{
	return len==0 ? Vector2d( 0, 0, K ) : Vector2d( x/length*len, y/length*len, K );
}

Vector2d Vector2d::newVectorTo( double len, double angle )
{
	return Vector2d( x+len*cos(angle), y+len*sin(angle), K );
}	

double Vector2d::angleDifference( double angle1, double angle2 )
{
	double d=fabs(angle1-angle2);

	if ( d>2*PI ) d=d-2*PI;
	if ( d>PI ) d=2*PI-d;

	return d;
}

Vector2d Vector2d::rayCrossPoint( Vector2d v1, double angle1, Vector2d v2, double angle2 )
{
	double x1=v1.getX();
	double y1=v1.getY();
	double x2=v2.getX();
	double y2=v2.getY();
	double rx1=cos(angle1);
	double ry1=sin(angle1);
	double rx2=cos(angle2);
	double ry2=sin(angle2);
	double detA=ry1*rx2-rx1*ry2;

	if ( detA==0 ) return Vector2d(0,0,U);

	double dx=x2-x1;
	double dy=y2-y1;
	double lambda=( dy*rx2-dx*ry2 )/detA;

	return Vector2d( x1+lambda*rx1, y1+lambda*ry1, K );
}	

std::ostream& operator<<( std::ostream& s, Vector2d v )
{
	if (v.isInfinite()) return s << "INFINITE";
	
	return s << "x:" << v.getX() << " y:" << v.getY() << " l:" << v.getLength() << " w:" << ( v.getAngle()*180.0/PI );
}

/*
int main()
{
	Vector2d a=Vector2d(100, 100, K);
	Vector2d b=Vector2d(10, 2, P);
	Vector2d d=a+b;
	std::cout << "A=" << a << " B=" << b << std::endl;
	std::cout << "A-B: " << a.distance(b) << std::endl;
	b=a.newLength(100);
	std::cout << "B=" << b << std::endl;
	Vector2d cross=Vector2d::rayCrossPoint( a, 1, b, 2);
	std::cout << "Kreuzpunkt 1=" << cross << std::endl;
	cross=Vector2d::rayCrossPoint( a, 2, b, 2);
	std::cout << "Kreuzpunkt 2=" << cross << std::endl;
}
*/
