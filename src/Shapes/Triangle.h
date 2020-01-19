/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:19:25
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:19:25
 * @Description: All rights reserved
 */
#pragma once
#include "Shapes\Objects.h"
#include "tiny_obj_loader.h"

struct Triangle_Face_index
{
	int index[ 3 ];
};

inline void initTriangleFaceIndex ( int p1, int p2, int p3, Triangle_Face_index & face )
{
	face.index[ 0 ] = p1;
	face.index[ 1 ] = p2;
	face.index[ 2 ] = p3;
}

struct Triangle_Vertex
{
	Pnt3D vertex[ 3 ];
};

inline void initTriangleVertex (
	Pnt3D p1,
	     Pnt3D p2,
	     Pnt3D p3,
	     Triangle_Vertex & tri
)
{
	tri.vertex[ 0 ] = p1;
	tri.vertex[ 1 ] = p2;
	tri.vertex[ 2 ] = p3;
}

struct Triangle_Edges
{
	Vector edges[ 3 ];
};

// clock-wise calculate the edges
inline void GetTriangleEdges (
	Triangle_Vertex ver,
	Triangle_Face_index ind,
	Triangle_Edges & ed
)
{
	ed.edges[ 0 ] = ver.vertex[ ind.index[ 1 ] ] - ver.vertex[ ind.index[ 0 ] ];
	ed.edges[ 1 ] = ver.vertex[ ind.index[ 2 ] ] - ver.vertex[ ind.index[ 1 ] ];
	ed.edges[ 2 ] = ver.vertex[ ind.index[ 0 ] ] - ver.vertex[ ind.index[ 2 ] ];
}

class Triangle : public Objects
{
	Triangle_Vertex tri_vertex;
	Triangle_Face_index tri_face;
	Triangle_Edges tri_edge;
	Vector normal;
public:
	Triangle ()
	{
	};

	Triangle (
		BRDFModels * brdf,
		Material * mat,
		Triangle_Vertex point,
		Triangle_Face_index index
	)
		: Objects( brdf, mat )
	{
		tri_vertex = point;
		tri_face = index;

		GetTriangleEdges( tri_vertex, tri_face, tri_edge );

		normal = Cross( tri_edge.edges[ 0 ], tri_edge.edges[ 1 ] );

		shapeFlag = obj_triangle;

		double theta = acos( AbsDot( tri_edge.edges[ 0 ], tri_edge.edges[ 1 ] ) );

		area = tri_edge.edges[ 0 ].Length() * tri_edge.edges[ 1 ].Length() * sin( theta ) / 2;
	}

	bool intersect (
		const Ray & ray,
		double & pathLength
	) const override;

	void getNormalDirection (
		const Vector & p0,
		Vector & normal
	) const override;

	Vector samplePointOnObject () const override;

	double PDF_in_solid_angle (
		Vector sampledPointOnLight,
		Vector pointOnObject,
		Vector lightDir
	) const override;

	int toEmbreeObj ( RTCScene scene_i );
};


class TriangleMesh : public Objects
{
protected:
	Vertex * ver_array;
	Triangle_index_bre * tri_array;

	unsigned num_of_vertex;
	unsigned num_of_triangle;
public:
	TriangleMesh (
		BRDFModels * brdf,
		Material * mat,
		const Vertex * vertex_list,
		const Triangle_index_bre * triangle_list,
		unsigned triangle_num,
		unsigned vertex_num

	)
		: Objects( brdf, mat )
	{
		num_of_triangle = triangle_num;
		num_of_vertex = vertex_num;

		AllocateMemory();
		for ( unsigned i = 0; i < num_of_vertex; i++ )
			ver_array[ i ] = vertex_list[ i ];

		for ( unsigned i = 0; i < num_of_triangle; i++ )
			tri_array[ i ] = triangle_list[ i ];

		shapeFlag = obj_triangleMesh;
	}

	TriangleMesh (
		BRDFModels * brdf,
		Material * mat,
		unsigned triangle_num,
		unsigned vertex_num

	): Objects( brdf, mat )
	{
		num_of_triangle = triangle_num;
		num_of_vertex = vertex_num;
		shapeFlag = obj_triangleMesh;
		AllocateMemory();
	}

	void RotX ( float theta ) override;
	void RotY ( float theta ) override;
	void RotZ ( float theta ) override;
	void Move ( float a, float b, float c ) override;
	void Scale ( float a, float b, float c ) override;
	void Scale ( float a ) override;

	void PrintInfo ();
	void getNormalDirection ( const Pnt3D & p0, Vector & normal ) const;

	void AllocateMemory ()
	{
		ver_array = ( Vertex * ) malloc( num_of_vertex * sizeof ( Vertex) );
		tri_array = ( Triangle_index_bre * ) malloc( num_of_triangle * sizeof ( Triangle_index_bre) );
	}

	void ResetMemory (
		unsigned triangle_num,
		unsigned vertex_num
	)
	{
		ver_array = NULL;
		tri_array = NULL;

		num_of_triangle = triangle_num;
		num_of_vertex = vertex_num;

		AllocateMemory();
	}

	void SetMesh (
		const Vertex * vertex_list,
		const Triangle_index_bre * triangle_list )
	{
		for ( unsigned i = 0; i < num_of_vertex; i++ )
			ver_array[ i ] = vertex_list[ i ];

		for ( unsigned i = 0; i < num_of_triangle; i++ )
			tri_array[ i ] = triangle_list[ i ];
	}

	Vector samplePointOnObject () const override;

	double PDF_in_solid_angle (
		Vector sampledPointOnLight,
		Vector pointOnObject,
		Vector lightDir
	) const override;

	int toEmbreeObj ( RTCScene scene_i );

	double GetTriangleArea ( size_t index );

	void Updata_Object_Area () override;
};

class Square : public TriangleMesh
{
	Vector normal;
public:
	Square (
		BRDFModels * brdf,
		Material * mat,
		const Vertex * vertex_list,
		const Triangle_index_bre * triangle_list,
		unsigned triangle_num,
		unsigned vertex_num

	): TriangleMesh(
		brdf,
		mat,
		vertex_list,
		triangle_list,
		triangle_num,
		vertex_num )
	{
		initNormal();

		shapeFlag = obj_triangleMesh;
	}

	Square (
		BRDFModels * brdf,
		Material * mat
	) : TriangleMesh( brdf, mat, 2, 4 )
	{
		initSquareXY();

		initNormal();

		shapeFlag = obj_triangleMesh;
	}


	void initSquareXY ();

	void initSquareXZ ();

	void initSquareYZ ();

	void RotX ( float theta ) override;

	void RotY ( float theta ) override;

	void RotZ ( float theta ) override;

	void Updata_normal ();

	double PDF_in_solid_angle (
		Vector sampledPointOnLight,
		Vector pointOnObject,
		Vector lightDir
	) const override;

	Pnt3D samplePointOnObject () const override;

	void getNormalDirection ( const Pnt3D & p0, Vector & normal ) const;

	void initNormal ()
	{
		Vector u(
			ver_array[ 1 ].x - ver_array[ 0 ].x,
			ver_array[ 1 ].y - ver_array[ 0 ].y,
			ver_array[ 1 ].z - ver_array[ 0 ].z );

		Vector v(
			ver_array[ 3 ].x - ver_array[ 0 ].x,
			ver_array[ 3 ].y - ver_array[ 0 ].y,
			ver_array[ 3 ].z - ver_array[ 0 ].z );

		float x2 = u.Length();
		float y2 = v.Length();

		area = x2 * y2;

		normal = Normalize( Cross( u, v ) );
	}


	//	int toEmbreeObj ( RTCScene scene_i );
};

