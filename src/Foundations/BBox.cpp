#include "BBox.h"

// BBox Method Definitions
BBox Union ( const BBox & b, const Pnt3D & p )
{
	BBox ret = b;
	ret.pMin.x = fmin( b.pMin.x, p.x );
	ret.pMin.y = fmin( b.pMin.y, p.y );
	ret.pMin.z = fmin( b.pMin.z, p.z );
	ret.pMax.x = fmax( b.pMax.x, p.x );
	ret.pMax.y = fmax( b.pMax.y, p.y );
	ret.pMax.z = fmax( b.pMax.z, p.z );
	return ret;
}


BBox Union ( const BBox & b, const BBox & b2 )
{
	BBox ret;
	ret.pMin.x = fmin( b.pMin.x, b2.pMin.x );
	ret.pMin.y = fmin( b.pMin.y, b2.pMin.y );
	ret.pMin.z = fmin( b.pMin.z, b2.pMin.z );
	ret.pMax.x = fmax( b.pMax.x, b2.pMax.x );
	ret.pMax.y = fmax( b.pMax.y, b2.pMax.y );
	ret.pMax.z = fmax( b.pMax.z, b2.pMax.z );
	return ret;
}

