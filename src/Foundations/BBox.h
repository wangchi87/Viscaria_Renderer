/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:16:46
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:16:46
 * @Description: All rights reserved
 */
#pragma once

#include "Vector.h"

// integrated from PBRT

inline double Lerp ( double t, double v1, double v2 )
{
	return ( 1.f - t ) * v1 + t * v2;
}

class BBox
{
public:
	// BBox Public Methods
	BBox ()
	{
		pMin = Pnt3D( INFINITY, INFINITY, INFINITY );
		pMax = Pnt3D( -INFINITY, -INFINITY, -INFINITY );
	}

	BBox ( const Pnt3D & p ) : pMin( p ) , pMax( p )
	{
	}

	BBox ( const Pnt3D & p1, const Pnt3D & p2 )
	{
		pMin = Pnt3D( fmin( p1.x, p2.x ), fmin( p1.y, p2.y ), fmin( p1.z, p2.z ) );
		pMax = Pnt3D( fmax( p1.x, p2.x ), fmax( p1.y, p2.y ), fmax( p1.z, p2.z ) );
	}

	friend BBox Union ( const BBox & b, const Pnt3D & p );
	friend BBox Union ( const BBox & b, const BBox & b2 );

	bool Overlaps ( const BBox & b ) const
	{
		bool x = ( pMax.x >= b.pMin.x ) && ( pMin.x <= b.pMax.x );
		bool y = ( pMax.y >= b.pMin.y ) && ( pMin.y <= b.pMax.y );
		bool z = ( pMax.z >= b.pMin.z ) && ( pMin.z <= b.pMax.z );
		return ( x && y && z );
	}

	bool Inside ( const Pnt3D & pt ) const
	{
		return ( pt.x >= pMin.x && pt.x <= pMax.x &&
			pt.y >= pMin.y && pt.y <= pMax.y &&
			pt.z >= pMin.z && pt.z <= pMax.z );
	}

	void Expand ( double delta )
	{
		pMin -= Vector( delta, delta, delta );
		pMax += Vector( delta, delta, delta );
	}

	double SurfaceArea () const
	{
		Vector d = pMax - pMin;
		return 2.f * ( d.x * d.y + d.x * d.z + d.y * d.z );
	}

	double Volume () const
	{
		Vector d = pMax - pMin;
		return d.x * d.y * d.z;
	}

	int MaximumExtent () const
	{
		Vector diag = pMax - pMin;
		if ( diag.x > diag.y && diag.x > diag.z )
			return 0;
		else if ( diag.y > diag.z )
			return 1;
		else
			return 2;
	}

	const Pnt3D & operator[] ( int i ) const;
	Pnt3D & operator[] ( int i );
	Pnt3D Lerp ( double tx, double ty, double tz ) const
	{
		return Pnt3D( ::Lerp( tx, pMin.x, pMax.x ), ::Lerp( ty, pMin.y, pMax.y ),
		              ::Lerp( tz, pMin.z, pMax.z ) );
	}

	Vector Offset ( const Pnt3D & p ) const
	{
		return Vector( ( p.x - pMin.x ) / ( pMax.x - pMin.x ),
		               ( p.y - pMin.y ) / ( pMax.y - pMin.y ),
		               ( p.z - pMin.z ) / ( pMax.z - pMin.z ) );
	}


	bool operator== ( const BBox & b ) const
	{
		return b.pMin == pMin && b.pMax == pMax;
	}

	bool operator!= ( const BBox & b ) const
	{
		return b.pMin != pMin || b.pMax != pMax;
	}

	// BBox Public Data
	Pnt3D pMin, pMax;
};

