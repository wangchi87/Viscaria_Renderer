/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:17:16
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:17:16
 * @Description: All rights reserved
 */
#pragma once
#include <stdio.h>
#include <cmath>
#include "Assertion.h"
#include "Inline_functions.h"

#define M_PI 3.1415926
#define isnan _isnan
#define inf 1e9
#define eps 1e-6


class Vector
{
public:
	// Vector Public Methods
	Vector ();

	Vector ( double xx, double yy, double zz );

	void Print ();

	// The default versions of these are fine for release builds; for debug
	// we define them so that we can add the Assert checks.
	Vector ( const Vector & v );

	Vector & operator= ( const Vector & v );

	Vector operator+ ( const Vector & v ) const;

	Vector & operator+= ( const Vector & v );

	Vector operator- ( const Vector & v ) const;

	Vector & operator-= ( const Vector & v );

	Vector operator* ( double f ) const;

	Vector & operator*= ( double f );

	Vector operator/ ( double f ) const;

	bool HasNaNs () const;

	Vector & operator/= ( double f );

	Vector operator- () const;

	double operator[] ( int i ) const;

	double & operator[] ( int i );

	double LengthSquared () const;

	double Length () const;


	bool operator== ( const Vector & v ) const;

	bool operator!= ( const Vector & v ) const;

	bool isEmpty () const;

	void resetZero ();
	// Vector Public Data
	double x, y, z;
};


#define Pnt3D		Vector

//#define RGBColors	Vector


inline Vector Clamp ( Vector & v, double low, double high )
{
	return Vector(
		Clamp( v[ 0 ], low, high ),
		Clamp( v[ 1 ], low, high ),
		Clamp( v[ 2 ], low, high ) );
}

inline Vector operator* ( double f, const Vector & v )
{
	return v * f;
}

// input v1 v2 must be normalised
inline double Dot ( const Vector & v1, const Vector & v2 )
{
	return Clamp( v1.x * v2.x + v1.y * v2.y + v1.z * v2.z, -1, 1 );
}

//inline RGBColors RGBColorsMulti(const RGBColors &v1, const RGBColors &v2) {
//
//	return RGBColors (v1.x * v2.x , v1.y * v2.y , v1.z * v2.z);
//}

inline Vector VectorMulti ( const Vector & v1, const Vector & v2 )
{
	return Vector( v1.x * v2.x, v1.y * v2.y, v1.z * v2.z );
}

inline Vector MovePoint ( const Vector & originPoint, const Vector & direction, double t )
{
	return Vector(
		originPoint.x + t * direction.x,
		originPoint.y + t * direction.y,
		originPoint.z + t * direction.z
	);
}

inline double AbsDot ( const Vector & v1, const Vector & v2 )
{
	return fabs( Dot( v1, v2 ) );
}


inline Vector Cross ( const Vector & v1, const Vector & v2 )
{
	double v1x = v1.x, v1y = v1.y, v1z = v1.z;
	double v2x = v2.x, v2y = v2.y, v2z = v2.z;
	return Vector( ( v1y * v2z ) - ( v1z * v2y ),
	               ( v1z * v2x ) - ( v1x * v2z ),
	               ( v1x * v2y ) - ( v1y * v2x ) );
}


inline Vector Normalize ( const Vector & v )
{
	return v / v.Length();
}


inline void CoordinateSystem ( const Vector & v1, Vector * v2, Vector * v3 )
{
	if ( abs( v1.x ) > abs( v1.y ) )
	{
		double invLen = 1.f / sqrt( v1.x * v1.x + v1.z * v1.z );
		*v2 = Vector( -v1.z * invLen, 0.f, v1.x * invLen );
	}
	else
	{
		double invLen = 1.0 / sqrt( v1.y * v1.y + v1.z * v1.z );
		*v2 = Vector( 0.f, v1.z * invLen, -v1.y * invLen );
	}
	*v3 = Cross( v1, *v2 );
}

inline Vector SphericalDirection ( double sintheta,
                                   double costheta, double phi )
{
	return Vector( sintheta * cos( phi ),
	               sintheta * sin( phi ),
	               costheta );
}

inline Vector SphericalDirection ( double theta, double phi )
{
	return Vector( sin( theta ) * cos( phi ),
	               sin( theta ) * sin( phi ),
	               cos( theta ) );
}

inline Vector SphericalDirection ( double sintheta, double costheta,
                                   double phi, const Vector & x,
                                   const Vector & y, const Vector & z )
{
	return sintheta * cos( phi ) * x +
		sintheta * sin( phi ) * y + costheta * z;
}

inline double SphericalTheta ( const Vector & v )
{
	Vector x = Normalize( v );

	return acos( fabs( x.z ) );
}


inline double SphericalPhi ( const Vector & v )
{
	double p = atan2( v.y, v.x );
	return ( p < 0.f ) ? p + 2.f * M_PI : p;
}

template< class T >
inline T Sqr ( const T & x )
{
	return x * x;
}


inline double length ( Vector v )
{
	return sqrt( v.x * v.x + v.y * v.y + v.z * v.z );
}

inline double CosBetweenVector ( const Vector & v1, const Vector & v2 )
{
	return ( v1.x * v2.x + v1.y * v2.y + v1.z * v2.z ) / ( length( v1 ) * length( v2 ) );
}

inline double DistanceBetweenPoints ( Pnt3D p1, Pnt3D p2 )
{
	return sqrt( ( p1.x - p2.x ) * ( p1.x - p2.x )
		+ ( p1.y - p2.y ) * ( p1.y - p2.y )
		+ ( p1.z - p2.z ) * ( p1.z - p2.z )
	);
}

inline double DistanceSquareBetweenPoints ( Pnt3D p1, Pnt3D p2 )
{
	return sqrt( ( p1.x - p2.x ) * ( p1.x - p2.x )
		+ ( p1.y - p2.y ) * ( p1.y - p2.y )
		+ ( p1.z - p2.z ) * ( p1.z - p2.z )
	);
}

inline void ASSERT_NAN ( Vector v )
{
#ifdef ASSERTION_ON
	if (_isnan ( v.x ) || _isnan ( v.y ) || _isnan ( v.z ))
		printf ( "vector has nan" );
#endif
}


inline double CosTheta ( const Vector & w )
{
	return w.z;
}

inline double AbsCosTheta ( const Vector & w )
{
	return fabsf( w.z );
}

inline double Sin2Theta ( const Vector & w )
{
	return fmax( 0.f, 1.f - CosTheta( w ) * CosTheta( w ) );
}

inline double Cos2Theta ( const Vector & w )
{
	return MIN( 1 , CosTheta ( w )*CosTheta ( w ) );
}


inline double SinTheta ( const Vector & w )
{
	return sqrtf( Sin2Theta( w ) );
}

inline double DistanceSquared ( const Vector & p1, const Vector & p2 )
{
	return ( p1 - p2 ).LengthSquared();
}


inline double CosPhi ( const Vector & v )
{
	double sin_theta = SinTheta( v );
	return ( sin_theta == 0 ) ? 1 : Clamp( v.x / sin_theta, -1, 1 );
}

inline double SinPhi ( const Vector & v )
{
	double sin_theta = SinTheta( v );
	return ( sin_theta == 0 ) ? 1 : Clamp( v.y / sin_theta, -1, 1 );
}


inline double TanTheta ( const Vector & w )
{
	return SinTheta( w ) / CosTheta( w );
}

inline double Tan2Theta ( const Vector & w )
{
	return Sin2Theta( w ) / Cos2Theta( w );
}


inline double Cos2Phi ( const Vector & w )
{
	return CosPhi( w ) * CosPhi( w );
}

inline double Sin2Phi ( const Vector & w )
{
	return SinPhi( w ) * SinPhi( w );
}

