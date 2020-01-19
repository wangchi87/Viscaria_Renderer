/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:19:12
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:19:12
 * @Description: All rights reserved
 */
#pragma once
#include "Shapes\Triangle.h"

class Cube :
	public TriangleMesh
{
public:

	Cube (
		BRDFModels * brdf,
		Material * mat )
		: TriangleMesh( brdf, mat, 12, 8 )
	{
		shapeFlag = obj_triangleMesh;

		ver_array[ 0 ].x = -1;
		ver_array[ 0 ].y = -1;
		ver_array[ 0 ].z = -1;
		ver_array[ 1 ].x = +1;
		ver_array[ 1 ].y = -1;
		ver_array[ 1 ].z = -1;
		ver_array[ 2 ].x = +1;
		ver_array[ 2 ].y = +1;
		ver_array[ 2 ].z = -1;
		ver_array[ 3 ].x = -1;
		ver_array[ 3 ].y = +1;
		ver_array[ 3 ].z = -1;
		ver_array[ 4 ].x = -1;
		ver_array[ 4 ].y = -1;
		ver_array[ 4 ].z = +1;
		ver_array[ 5 ].x = +1;
		ver_array[ 5 ].y = -1;
		ver_array[ 5 ].z = +1;
		ver_array[ 6 ].x = +1;
		ver_array[ 6 ].y = +1;
		ver_array[ 6 ].z = +1;
		ver_array[ 7 ].x = -1;
		ver_array[ 7 ].y = +1;
		ver_array[ 7 ].z = +1;

		// bottom
		tri_array[ 0 ].v0 = 0;
		tri_array[ 0 ].v1 = 1;
		tri_array[ 0 ].v2 = 3;
		tri_array[ 1 ].v0 = 3;
		tri_array[ 1 ].v1 = 1;
		tri_array[ 1 ].v2 = 2;

		// top
		tri_array[ 2 ].v0 = 4;
		tri_array[ 2 ].v1 = 5;
		tri_array[ 2 ].v2 = 7;
		tri_array[ 3 ].v0 = 7;
		tri_array[ 3 ].v1 = 5;
		tri_array[ 3 ].v2 = 6;

		// left
		tri_array[ 4 ].v0 = 0;
		tri_array[ 4 ].v1 = 3;
		tri_array[ 4 ].v2 = 4;
		tri_array[ 5 ].v0 = 4;
		tri_array[ 5 ].v1 = 3;
		tri_array[ 5 ].v2 = 7;

		// right
		tri_array[ 6 ].v0 = 1;
		tri_array[ 6 ].v1 = 2;
		tri_array[ 6 ].v2 = 5;
		tri_array[ 7 ].v0 = 5;
		tri_array[ 7 ].v1 = 2;
		tri_array[ 7 ].v2 = 6;

		// front
		tri_array[ 8 ].v0 = 0;
		tri_array[ 8 ].v1 = 1;
		tri_array[ 8 ].v2 = 4;
		tri_array[ 9 ].v0 = 4;
		tri_array[ 9 ].v1 = 1;
		tri_array[ 9 ].v2 = 5;

		// back
		tri_array[ 10 ].v0 = 3;
		tri_array[ 10 ].v1 = 2;
		tri_array[ 10 ].v2 = 7;
		tri_array[ 11 ].v0 = 7;
		tri_array[ 11 ].v1 = 2;
		tri_array[ 11 ].v2 = 6;
	}

	~Cube ();
};

