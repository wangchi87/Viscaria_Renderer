#include "Ray.h"


Ray::Ray ( void )
{
}


Ray::~Ray ( void )
{
}

RTCRay Ray::toEmbreeRay () const
{
	RTCRay ray;

	ray.org[ 0 ] = origin.x;
	ray.org[ 1 ] = origin.y;
	ray.org[ 2 ] = origin.z;

	ray.dir[ 0 ] = direction.x;
	ray.dir[ 1 ] = direction.y;
	ray.dir[ 2 ] = direction.z;


	//ray.dir = direction_rtc;
	ray.tnear = 0.0f;
	ray.tfar = inf;
	ray.geomID = RTC_INVALID_GEOMETRY_ID;
	ray.primID = RTC_INVALID_GEOMETRY_ID;
	ray.mask = -1;
	ray.time = 0;

	return ray;
}

