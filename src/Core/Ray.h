/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:16:16
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:16:16
 * @Description: All rights reserved
 */
#include "Foundations/Vector.h"
#include <embree2\rtcore.h>
#include <embree2\rtcore_ray.h>

#pragma once
class Ray
{
public:
	Pnt3D origin;
	Vector direction;
	bool insideObject;
	//double	currentRefractionIndex;
public:
	Ray ( void );

	Ray ( Pnt3D o, Vector d )
	{
		origin = o;
		direction = d;
		insideObject = false;

		// initial the ray in air
		// currentRefractionIndex = 1.0; 
		Normalize( direction );
	}

	void rayInsideObject ()
	{
		insideObject = true;
	}

	void rayOutsideObject ()
	{
		insideObject = false;
	}

	void PrintRay ()
	{
		printf( "Ray Info : \n" );
		origin.Print();
		direction.Print();
		printf( "\n" );
	}

	~Ray ( void );

	RTCRay toEmbreeRay () const;
};

