#include "Intersection.h"


Intersection::~Intersection ()
{
}

Intersection::Intersection (
	const Ray & ray,
	double t_,
	Objects * object_ )
{
	t = t_;
	object = object_;
	intersectionPoint = MovePoint( ( ray ).origin, ( ray ).direction, t );
	object->getNormalDirection( intersectionPoint, normal );
	makeNormalFaceOutside( ray.direction, normal );
	primID = ULONG_MAX;
}

Intersection::Intersection (
	const Ray & ray,
	double t_,
	unsigned long int hitPrimID,
	float u , float v ,
	Objects * object_,
	Vector nor )
{
	t = t_;
	object = object_;
	primID = hitPrimID;
	hit_u = u;
	hit_v = v;
	intersectionPoint = MovePoint( ray.origin, ray.direction, t );
	normal = nor;
	makeNormalFaceOutside( ray.direction, normal );

}

Intersection::operator bool ()
{
	return object != nullptr;
}

