#include "Triangle.h"

bool Triangle::intersect ( const Ray & ray, double & pathLength ) const
{
	// Fast Minimum Storage Ray Triangle Intersection
	// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

	Vector P, Q, T;

	Vector edge1, edge3;
	edge1 = tri_edge.edges[ 0 ];
	edge3 = - tri_edge.edges[ 2 ];

	double det, inv_det, u, v;
	double t;

	//Begin calculating determinant - also used to calculate u parameter
	P = Cross( ray.direction, edge3 );

	//if determinant is near zero, ray lies in plane of triangle or ray is parallel to plane of triangle
	det = Dot( edge1, P );

	//NOT CULLING
	if ( det > -eps && det < eps )
		return 0;

	inv_det = 1.f / det;

	//calculate distance from V1 to ray origin
	T = ray.origin - tri_vertex.vertex[ 0 ];

	//Calculate u parameter and test bound
	u = Dot( T, P ) * inv_det;
	//The intersection lies outside of the triangle
	if ( u < 0.f || u > 1.f ) return 0;

	//Prepare to test v parameter
	Q = Cross( T, edge1 );

	//Calculate V parameter and test bound
	v = Dot( ray.direction, Q ) * inv_det;

	//The intersection lies outside of the triangle
	if ( v < 0.f || u + v > 1.f ) return 0;

	t = Dot( edge3, Q ) * inv_det;

	if ( t > eps )
	{
		//ray intersection
		pathLength = t;
		return true;
	}

	// No hit, no win
	return false;
}

void Triangle::getNormalDirection ( const Vector & p0, Vector & normal ) const
{
	normal = this->normal;
}

Vector Triangle::samplePointOnObject () const
{
	return Pnt3D( 0, 0, 0 );
}

double Triangle::PDF_in_solid_angle ( Vector sampledPointOnLight, Vector pointOnObject, Vector lightDir ) const
{
	double cos_term = AbsDot( lightDir, normal );

	double distance_square = DistanceSquareBetweenPoints( sampledPointOnLight, pointOnObject );

	return distance_square / ( area * cos_term );
}

int Triangle::toEmbreeObj ( RTCScene scene_i )
{
	return -1;
}

void TriangleMesh::RotX ( float theta )
{
	for ( int i = 0; i < num_of_vertex; i++ )
		Vertex_RotX( ver_array[ i ], theta );

	// update normal
}

void TriangleMesh::RotY ( float theta )
{
	for ( int i = 0; i < num_of_vertex; i++ )
		Vertex_RotY( ver_array[ i ], theta );

	// update normal
}

void TriangleMesh::RotZ ( float theta )
{
	for ( int i = 0; i < num_of_vertex; i++ )
		Vertex_RotZ( ver_array[ i ], theta );

	// update normal
}

void TriangleMesh::Move ( float a, float b, float c )
{
	for ( int i = 0; i < num_of_vertex; i++ )
		Vertex_Move( ver_array[ i ], a, b, c );
}

void TriangleMesh::Scale ( float a, float b, float c )
{
	for ( int i = 0; i < num_of_vertex; i++ )
	{
		Vertex_ScaleX( ver_array[ i ], a );
		Vertex_ScaleY( ver_array[ i ], b );
		Vertex_ScaleZ( ver_array[ i ], c );
	}
	Updata_Object_Area();
}

void TriangleMesh::Scale ( float a )
{
	for ( int i = 0; i < num_of_vertex; i++ )
	{
		Vertex_ScaleX( ver_array[ i ], a );
		Vertex_ScaleY( ver_array[ i ], a );
		Vertex_ScaleZ( ver_array[ i ], a );
	}
	Updata_Object_Area();
}

void TriangleMesh::PrintInfo ()
{
	printf( "GemID %d \n", embreeID );
	for ( int i = 0; i < num_of_vertex; i++ )
	{
		printf( "Vertex %d : %f %f %f \n",
		        i,
		        ver_array[ i ].x,
		        ver_array[ i ].y,
		        ver_array[ i ].z
		);
	}
	printf( "\n " );
}

void TriangleMesh::getNormalDirection ( const Vector & p0, Vector & normal ) const
{
	printf( "\n getNormalDirection in TriangleMesh is not implemented" );
}

Vector TriangleMesh::samplePointOnObject () const
{
	printf( "\n samplePointOnObject in TriangleMesh is not implemented" );
	return Pnt3D( 0, 0, 0 );
}

double TriangleMesh::PDF_in_solid_angle ( Vector sampledPointOnLight, Vector pointOnObject, Vector lightDir ) const
{
	// undefined
	printf( "PDF in TriangleMesh is not implemented" );
	return 0;
}

int TriangleMesh::toEmbreeObj ( RTCScene scene_i )
{
	unsigned int mesh = rtcNewTriangleMesh(
		scene_i,
		RTC_GEOMETRY_STATIC,
		num_of_triangle,
		num_of_vertex
	);

	Vertex * vertices = ( Vertex * ) rtcMapBuffer( scene_i, mesh, RTC_VERTEX_BUFFER );

	for ( int i = 0; i < num_of_vertex; i++ )
	{
		vertices[ i ] = ver_array[ i ];
	}

	rtcUnmapBuffer( scene_i, mesh, RTC_VERTEX_BUFFER );


	Triangle_index_bre * triangles = ( Triangle_index_bre * ) rtcMapBuffer( scene_i, mesh, RTC_INDEX_BUFFER );

	for ( int i = 0; i < num_of_triangle; i++ )
	{
		triangles[ i ] = tri_array[ i ];
	}

	rtcUnmapBuffer( scene_i, mesh, RTC_INDEX_BUFFER );

	embreeID = mesh;
	return mesh;
}

double TriangleMesh::GetTriangleArea ( size_t index )
{
	size_t p1 = tri_array[ index ].v0;
	size_t p2 = tri_array[ index ].v1;
	size_t p3 = tri_array[ index ].v2;

	Vertex v1 = ver_array[ p1 ];
	Vertex v2 = ver_array[ p2 ];
	Vertex v3 = ver_array[ p3 ];

	double tri_area = 0;

	double a = 0, b = 0, c = 0, s = 0;
	a = Vertex_distance( v1, v2 );
	b = Vertex_distance( v2, v3 );
	c = Vertex_distance( v1, v3 );
	s = 0.5 * ( a + b + c );
	tri_area = sqrt( s * ( s - a ) * ( s - b ) * ( s - c ) );

	return tri_area;
}

void TriangleMesh::Updata_Object_Area ()
{
	area = 0;

	for ( size_t i = 0; i < num_of_triangle; i++ )
		area += GetTriangleArea( i );

	//	return area;
}

void Square::initSquareXY ()
{
	ver_array[ 0 ].x = -1.f;
	ver_array[ 0 ].y = -1.f;
	ver_array[ 0 ].z = -0.f;
	ver_array[ 1 ].x = -1.f;
	ver_array[ 1 ].y = +1.f;
	ver_array[ 1 ].z = +0.f;
	ver_array[ 2 ].x = +1.f;
	ver_array[ 2 ].y = +1;
	ver_array[ 2 ].z = -0.f;
	ver_array[ 3 ].x = +1.f;
	ver_array[ 3 ].y = -1.f;
	ver_array[ 3 ].z = +0.f;

	tri_array[ 0 ].v0 = 0;
	tri_array[ 0 ].v1 = 2;
	tri_array[ 0 ].v2 = 1;
	tri_array[ 1 ].v0 = 0;
	tri_array[ 1 ].v1 = 3;
	tri_array[ 1 ].v2 = 2;
}

void Square::initSquareXZ ()
{
	ver_array[ 0 ].x = -1.f;
	ver_array[ 0 ].y = -0.f;
	ver_array[ 0 ].z = -1.f;
	ver_array[ 1 ].x = -1.f;
	ver_array[ 1 ].y = +0.f;
	ver_array[ 1 ].z = +1.f;
	ver_array[ 2 ].x = +1.f;
	ver_array[ 2 ].y = +0;
	ver_array[ 2 ].z = +1.f;
	ver_array[ 3 ].x = +1.f;
	ver_array[ 3 ].y = -0.f;
	ver_array[ 3 ].z = -1.f;

	tri_array[ 0 ].v0 = 0;
	tri_array[ 0 ].v1 = 2;
	tri_array[ 0 ].v2 = 1;
	tri_array[ 1 ].v0 = 0;
	tri_array[ 1 ].v1 = 3;
	tri_array[ 1 ].v2 = 2;
}

void Square::initSquareYZ ()
{
	ver_array[ 0 ].x = -0.f;
	ver_array[ 0 ].y = -1.f;
	ver_array[ 0 ].z = -1.f;
	ver_array[ 1 ].x = -0.f;
	ver_array[ 1 ].y = -1.f;
	ver_array[ 1 ].z = +1.f;
	ver_array[ 2 ].x = +0.f;
	ver_array[ 2 ].y = +1;
	ver_array[ 2 ].z = +1.f;
	ver_array[ 3 ].x = +0.f;
	ver_array[ 3 ].y = +1.f;
	ver_array[ 3 ].z = -1.f;

	tri_array[ 0 ].v0 = 0;
	tri_array[ 0 ].v1 = 2;
	tri_array[ 0 ].v2 = 1;
	tri_array[ 1 ].v0 = 0;
	tri_array[ 1 ].v1 = 3;
	tri_array[ 1 ].v2 = 2;
}

void Square::RotX ( float theta )
{
	TriangleMesh::RotX( theta );
	Updata_normal();
}

void Square::RotY ( float theta )
{
	TriangleMesh::RotY( theta );
	Updata_normal();
}

void Square::RotZ ( float theta )
{
	TriangleMesh::RotY( theta );
	Updata_normal();
}

void Square::Updata_normal ()
{
	Vector u(
		ver_array[ 1 ].x - ver_array[ 0 ].x,
		ver_array[ 1 ].y - ver_array[ 0 ].y,
		ver_array[ 1 ].z - ver_array[ 0 ].z );

	Vector v(
		ver_array[ 3 ].x - ver_array[ 0 ].x,
		ver_array[ 3 ].y - ver_array[ 0 ].y,
		ver_array[ 3 ].z - ver_array[ 0 ].z );

	normal = Normalize( Cross( u, v ) );
}

double Square::PDF_in_solid_angle ( Vector sampledPointOnLight, Vector pointOnObject, Vector lightDir ) const
{
	double cos_term = AbsDot( lightDir, normal );

	double distance_square = DistanceSquareBetweenPoints( sampledPointOnLight, pointOnObject );

	return distance_square / ( area * cos_term );
}

Pnt3D Square::samplePointOnObject () const
{
	Vector u(
		ver_array[ 1 ].x - ver_array[ 0 ].x,
		ver_array[ 1 ].y - ver_array[ 0 ].y,
		ver_array[ 1 ].z - ver_array[ 0 ].z );

	Vector v(
		ver_array[ 3 ].x - ver_array[ 0 ].x,
		ver_array[ 3 ].y - ver_array[ 0 ].y,
		ver_array[ 3 ].z - ver_array[ 0 ].z );

	double r1 = getUniformRandomNumber( 0, 1 );
	double r2 = getUniformRandomNumber( 0, 1 );

	Vector samplePointVector = u * r1 + v * r2;

	Pnt3D p(
		samplePointVector.x + ver_array[ 0 ].x,
		samplePointVector.y + ver_array[ 0 ].y,
		samplePointVector.z + ver_array[ 0 ].z );
	return p;
}

void Square::getNormalDirection ( const Vector & p0, Vector & normal ) const
{
	normal = this->normal;
}

