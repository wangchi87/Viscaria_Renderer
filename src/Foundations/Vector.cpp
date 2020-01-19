#include "Vector.h"


Vector::Vector ()
{
	x = y = z = 0.f;
}

Vector::Vector ( double xx, double yy, double zz ): x( xx ), y( yy ), z( zz )
{
}

void Vector::Print ()
{
	printf( "\n x %f y %f z %f \n", x, y, z );
}

Vector::Vector ( const Vector & v )
{
	x = v.x;
	y = v.y;
	z = v.z;
}

Vector & Vector::operator= ( const Vector & v )
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

Vector Vector::operator+ ( const Vector & v ) const
{
	return Vector( x + v.x, y + v.y, z + v.z );
}

Vector & Vector::operator+= ( const Vector & v )
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector Vector::operator- ( const Vector & v ) const
{
	return Vector( x - v.x, y - v.y, z - v.z );
}

Vector & Vector::operator-= ( const Vector & v )
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vector Vector::operator* ( double f ) const
{
	return Vector( f * x, f * y, f * z );
}

Vector & Vector::operator*= ( double f )
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

Vector Vector::operator/ ( double f ) const
{
	if ( f == 0 )
	{
		return Vector( 0, 0, 0 );
	}

	double inv = 1.f / f;
	return Vector( x * inv, y * inv, z * inv );
}

bool Vector::HasNaNs () const
{
	return isnan( x ) || isnan( y ) || isnan( z );
}

Vector & Vector::operator/= ( double f )
{
	double inv = 1.f / f;
	x *= inv;
	y *= inv;
	z *= inv;
	return *this;
}

Vector Vector::operator- () const
{
	return Vector( -x, -y, -z );
}

double Vector::operator[] ( int i ) const
{
	//		Assert ( i >= 0 && i <= 2 );
	return ( &x )[ i ];
}

double & Vector::operator[] ( int i )
{
	return ( &x )[ i ];
}

double Vector::LengthSquared () const
{
	return x * x + y * y + z * z;
}

double Vector::Length () const
{
	return sqrt( LengthSquared() );
}

bool Vector::operator== ( const Vector & v ) const
{
	return x == v.x && y == v.y && z == v.z;
}

bool Vector::operator!= ( const Vector & v ) const
{
	return x != v.x || y != v.y || z != v.z;
}

bool Vector::isEmpty () const
{
	return Length() == 0;
}

void Vector::resetZero ()
{
	x = 0;
	y = 0;
	z = 0;
}

