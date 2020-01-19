/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:13:47
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:16:06
 * @Description: All rights reserved
 */
#include "Shapes/Objects.h"
#pragma once


inline void makeNormalFaceOutside ( const Vector in, Vector & n )
{
	if ( Dot( in, n ) > 0 )
		n = -n;
	return;
}

class Intersection
{
public:
	~Intersection ();

public:
	Intersection ()
	{
		t = eps;
		object = nullptr;
		intersectionPoint = Vector( 0, 0, 0 );
		normal = Vector( 0, 0, 0 );
		primID = -1;
	}

	Intersection ( const Ray & ray, double t_, Objects * object_ );

	Intersection ( const Ray & ray, double t_, unsigned long int hitPrimID , float u, float v, Objects * object_, Vector nor );

	void clear ()
	{
		t = eps;
		object = nullptr;
		intersectionPoint = Vector( 0, 0, 0 );
		normal = Vector( 0, 0, 0 );
	}

	operator bool ();

	Vector normal;
	double t;
	Vector intersectionPoint;
	unsigned long int primID;
	float hit_u , hit_v;
	Objects * object;
};

