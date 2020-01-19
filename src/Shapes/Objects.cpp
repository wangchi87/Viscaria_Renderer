#include "Objects.h"


Objects::Objects ( void )
{
}


Objects::~Objects ( void )
{
}


bool Plane::intersect ( const Ray & ray, double & pathLength ) const
{
	double d0 = Dot( normal, ray.direction );
	if ( d0 != 0 )
	{
		double t = ( ( Dot( normal, pos - ray.origin ) ) / d0 );
		if ( t > eps )
		{
			pathLength = t;
			return true;
		}
		else
		{
			pathLength = 0;
			return false;
		}
	}
	return false;
}

void Plane::getNormalDirection ( const Vector & p0, Vector & normal ) const
{
	normal = this->normal;
}

Vector Plane::samplePointOnObject () const
{
	return pos; // to be modified
}

bool Sphere::intersect ( const Ray & ray, double & pathLength ) const
{
	Vector dir_to_center = center - ray.origin;

	double distance = dir_to_center.Length ();

	// special case: ray is cast from INSIDE of the sphere
	if( distance < radius )
	{
		dir_to_center = Normalize ( dir_to_center );

		double cos_theta = Dot ( ray.direction , dir_to_center );

		double abs_cos_theta = abs(cos_theta);

		double project_radius = distance * sqrt ( 1 - abs_cos_theta * abs_cos_theta );

		double temp = sqrt ( radius*radius - project_radius * project_radius );

		
		pathLength = cos_theta > 0 ? 
							temp + sqrt ( distance * distance - project_radius * project_radius )
						:	temp - sqrt ( distance * distance - project_radius * project_radius );

		return true;
	}

	// normal case: ray is cast from OUTSIDE of the sphere
	if ( Dot( dir_to_center, ray.direction ) < 0 )
	{
		pathLength = 0;
		return false;
	}

	

	dir_to_center = Normalize( dir_to_center );

	double cos_theta = AbsDot( ray.direction, dir_to_center );

	double project_radius = distance * sqrt( 1 - cos_theta * cos_theta );

	if ( project_radius > radius )
	{
		pathLength = 0;
		return false;
	}

	double tca = distance * cos_theta;
	double thc = sqrt( radius * radius - project_radius * project_radius );

	double t0, t1;
	t0 = tca - thc;
	t1 = tca + thc;

	if ( t0 > t1 )
		std::swap( t0, t1 );

	if ( t0 < 0 )
	{
		t0 = t1; // if t0 is negative, let's use t1 instead 
		if ( t0 < 0 )
			return false; // both t0 and t1 are negative 
	}

	pathLength = t0;

	return true;
}

void Sphere::getNormalDirection ( const Vector & p0, Vector & normal ) const
{
	normal = Normalize( p0 - center );
}

Vector Sphere::samplePointOnObject () const
{
	// http://mathworld.wolfram.com/SpherePointPicking.html

	double phi;
	double theta;

	phi = getUniformRandomNumber( 0.0, 2 * M_PI );
	theta = getUniformRandomNumber( 0.0, M_PI );

	double x, y, z;

	z = cos( theta );
	x = sqrt( 1 - z * z ) * cos( phi );
	y = sqrt( 1 - z * z ) * sin( phi );

	x *= radius;
	y *= radius;
	z *= radius;

	Pnt3D sampledPoint( center.x + x, center.y + y, center.z + z );

	return sampledPoint;
}

double Sphere::PDF_in_solid_angle ( Vector sampledPointOnLight, Vector pointOnObject, Vector lightDir ) const
{
	Vector normal = Normalize( sampledPointOnLight - center );

	double cos_term = AbsDot( lightDir, normal );

	double distance_square = DistanceSquareBetweenPoints( sampledPointOnLight, pointOnObject );

	return distance_square / ( area * cos_term );
}

bool Disk::intersect ( const Ray & ray, double & pathLength ) const
{
	double d0 = Dot( normal, ray.direction );
	if ( d0 != 0 )
	{
		double t = ( ( Dot( normal, center - ray.origin ) ) / d0 );

		Pnt3D intersectionPoint;

		intersectionPoint = MovePoint( ( ray ).origin, ( ray ).direction, t );

		double distance = DistanceBetweenPoints( intersectionPoint, center );

		if ( t > eps && distance < radius )
		{
			pathLength = t;
			return true;
		}
		else
		{
			pathLength = 0;
			return false;
		}
	}
	return false;
}

void Disk::getNormalDirection ( const Vector & p0, Vector & normal ) const
{
	normal = this->normal;
}

Vector Disk::samplePointOnObject () const
{
	TransformBetweenGlobalAndLocal trans;
	trans.InitTransformMatrix( normal );

	double theta = getUniformRandomNumber( 0.0, 2 * M_PI );

	double x, y, z;

	z = 0;
	x = sqrt( radius ) * cos( theta );
	y = sqrt( radius ) * sin( theta );

	Pnt3D sampledPoint( x, y, z );

	sampledPoint = trans.Local2World( sampledPoint );

	sampledPoint = sampledPoint + center;

	return sampledPoint; // to be modified
}

double Disk::PDF_in_solid_angle ( Vector sampledPointOnLight, Vector pointOnObject, Vector lightDir ) const
{
	
	double cos_term = AbsDot( lightDir, normal );

	double distance_square = DistanceSquareBetweenPoints( sampledPointOnLight, pointOnObject );

	return distance_square / ( area * cos_term );
}

