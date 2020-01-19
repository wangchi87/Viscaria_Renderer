/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:17:13
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:17:13
 * @Description: All rights reserved
 */
#pragma once
#include "Matrix.h"

class TransformBetweenGlobalAndLocal
{
	Matrix33 local2world;
	Matrix33 world2local;
public:
	TransformBetweenGlobalAndLocal ();
	~TransformBetweenGlobalAndLocal ();

	void InitTransformMatrix ( Vector in, Vector normal )
	{
		// incident light will be on XOZ plane in local coordinate system
		Vector local_z = Normalize( normal );
		Vector local_y = Normalize( Cross( in, normal ) );
		Vector local_x = Normalize( Cross( normal, local_y ) );

		// world_x = 1,0,0
		// world_y = 0,1,0
		// world_z = 0,0,1

		local2world.initWithVector( &local_x, &local_y, &local_z );
		world2local.initWithTranVector( &local_x, &local_y, &local_z );
	}


	void InitTransformMatrix ( Vector normal )
	{
		// incident light will be on XOZ plane in local coordinate system
		Vector local_z = Normalize( normal );

		Vector local_y;
		Vector local_x;

		CoordinateSystem( local_z, &local_x, &local_y );

		// world_x = 1,0,0
		// world_y = 0,1,0
		// world_z = 0,0,1

		local2world.initWithVector( &local_x, &local_y, &local_z );
		world2local.initWithTranVector( &local_x, &local_y, &local_z );
	}

	Vector Local2World ( const Vector v )
	{
		Vector tmp = local2world.multiply( &v );
		return tmp;
	}

	Vector World2Local ( const Vector v )
	{
		Vector tmp = world2local.multiply( &v );
		return tmp;
	}
};

