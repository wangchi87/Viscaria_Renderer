/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:19:18
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:19:18
 * @Description: All rights reserved
 */
#pragma once
#include "Shapes\Triangle.h"
#include "tinyply/tinyply.h"
#include <fstream>
#include <sstream>
#include <vector>

class Mesh :
	public TriangleMesh
{
	
public:
	Mesh (
		BRDFModels * brdf,
		Material * mat,
		const Vertex * vertex_list,
		const Triangle_index_bre * triangle_list,
		unsigned triangle_num,
		unsigned vertex_num

	) : TriangleMesh(
		brdf,
		mat,
		vertex_list,
		triangle_list,
		triangle_num,
		vertex_num )
	{
		shapeFlag = obj_importedMesh;
	}

	Mesh (
		BRDFModels * brdf,
		Material * mat,
		unsigned triangle_num,
		unsigned vertex_num

	) : TriangleMesh(
		brdf,
		mat,
		triangle_num,
		vertex_num )
	{
		shapeFlag = obj_importedMesh;
	}

	Mesh (
		BRDFModels * brdf,
		Material * mat,
		const std::string & filename
	) : TriangleMesh( brdf, mat, 2, 4 )
	{
		tinyPLY( filename );
		shapeFlag = obj_importedMesh;
	}

	// obj does not work now ....
	void LoadObjFile ();

	void tinyPLY ( const std::string & filename );

	Mesh ();
	~Mesh ();
};

