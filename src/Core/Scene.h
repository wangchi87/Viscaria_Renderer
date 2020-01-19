/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:16:24
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:16:24
 * @Description: All rights reserved
 */
#include <vector>
#include "Shapes/geometry.h"
#include "Intersection.h"
#include "Foundations/RandomNumber.h"


#pragma once

using namespace std;

/**
 * \brief scene structure 
 */
class Scene
{
	// maintain self-defined objs
	vector<Objects *> self_defined_objs;

	// maintain emissive objs
	vector<Objects *> emissive_objs;

	// maintain Embree objs
	vector<Objects *> bre_objs;

	RTCDevice device;
	RTCScene scene_bre;

	unsigned ObjTotalNum;
public:
	Scene ();
	~Scene ();

	void AddObject ( Objects * obj );

	void Commit_Embree_Scene ();

	Objects * AquireOneOfEmissiveObjects (double &pdf) const;

	bool IntersectionWithScene (
		const Ray & ray,
		Intersection & closestIntersectionPoint
		//Objects			* obj_Itself
	) const;

	bool Occluded (
		Pnt3D From,
		     Pnt3D To,
		     Objects * endObj
	) const;

	int GetEmbreeObjsCounts ()
	{
		return bre_objs.size ();
	}

	Objects* GetEmbreeObject ( int idx )
	{
		return bre_objs[ idx ];
	}
};

void error_handler ( 
	void * userPtr, 
	const RTCError code, 
	const char * str = nullptr 
);

