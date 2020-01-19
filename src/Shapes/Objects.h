/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:19:21
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:19:21
 * @Description: All rights reserved
 */
#include "Foundations/Vector.h"
#include "LightAndAttenuation/BRDFModels.h"
#include "LightAndAttenuation/Material.h"
#include "LightAndAttenuation/Spectrum.h"
#include "Core/Ray.h"
#include "Foundations/TransformBetweenGlobalAndLocal.h"
#include "..\Image\Texture.h"
#include <embree2\rtcore.h>
#include <embree2\rtcore_ray.h>

#pragma once

enum
{
	// object shape label
	obj_nothing = 0,
	obj_sphere = 1,
	obj_plane = 2,
	obj_disk = 3,
	obj_triangle = 4,

	// obj_mesh only used as boundary
	obj_mesh = 100,

	obj_triangleMesh = 101,
	obj_importedMesh = 102
};

class Objects
{
public:
	bool emitter;
	bool isDeltaEmitter;
	BRDFModels * brdf_model;
	Material * material_type;
	Spectrum emission;
	size_t shapeFlag;
	Texture texture;

	unsigned embreeID; // embree ID
	unsigned objID; // ID in the scene

	double area;

	Objects (
		BRDFModels * brdf,
		Material * mat
	)
	{
		emitter = false;
		isDeltaEmitter = false;
		brdf_model = brdf;
		material_type = mat;
		embreeID = -1;
		objID = -1;

		area = 0;
	}

	void SetEmissionValue ( Spectrum emissionValue )
	{
		emitter = true;
		emission = emissionValue;
	}

	void SetDeltaEmitter ( bool delta )
	{
		isDeltaEmitter = delta;
	}

	// if 0 is return, no intersection happen
	virtual bool intersect (
		const Ray & ray,
		double & pathLength ) const
	{
		return false;
	};

	virtual void getNormalDirection (
		const Pnt3D & p0,
		Vector & normal ) const
	{
	};

	virtual int toEmbreeObj ( RTCScene scene_i )
	{
		return -1;
	};

	virtual Pnt3D samplePointOnObject () const
	{
		return Pnt3D( 0, 0, 0 );
	};

	
	/**
	 * \brief PDF in solid angle form
	 * \param sampledPointOnLight 
	 * \param pointOnObject 
	 * \param lightDir 
	 * \return 
	 */
	virtual double PDF_in_solid_angle (
		Pnt3D sampledPointOnLight,
		Pnt3D pointOnObject,
		Vector lightDir ) const
	{
		return 0;
	}

	virtual void Updata_Object_Area ()
	{
	}

	/**
	 * \brief PDF in area form
	 * \return 
	 */
	virtual double PDF () const
	{
		return 1 / area;
	}

	virtual void RotX ( float theta )
	{
	};

	virtual void RotY ( float theta )
	{
	};

	virtual void RotZ ( float theta )
	{
	};

	virtual void Move ( float a, float b, float c )
	{
	};

	virtual void Scale ( float a, float b, float c )
	{
	};

	virtual void Scale ( float a )
	{
	};

	Objects ( void );
	~Objects ( void );
};


class Plane : public Objects
{
public:
	Vector normal;
	Pnt3D pos;

	Plane (
		BRDFModels * brdf,
		Material * mat,
		Pnt3D p,
		Vector n
	)
		: Objects( brdf, mat )
	{
		pos = p;
		normal = n;
		shapeFlag = obj_plane;
	}

	Plane (
		Pnt3D p,
		     Vector n
	)
	{
		pos = p;
		normal = n;
		shapeFlag = obj_plane;
	};

	bool intersect (
		const Ray & ray,
		double & pathLength
	) const override;

	void getNormalDirection (
		const Vector & p0,
		Vector & normal
	) const override;

	Vector samplePointOnObject () const override;
};

class Sphere : public Objects
{
public:
	Pnt3D center; // center
	double radius; // radius

	Sphere (
		BRDFModels * brdf,
		Material * mat,
		double r,
		Pnt3D c
	)
		: Objects( brdf, mat )
	{
		center = c;
		radius = r;
		shapeFlag = obj_sphere;

		area = 4 * M_PI * radius * radius;
	}

	Sphere (
		BRDFModels * brdf,
		Material * mat
	)
		: Objects( brdf, mat )
	{
		center = Pnt3D( 0, 0, 0 );
		radius = 1;
		shapeFlag = obj_sphere;

		area = 4 * M_PI * radius * radius;
	}

	Sphere (
		bool isEmitter,
		double r,
		Pnt3D c
	)
	{
		center = c;
		radius = r;
		shapeFlag = obj_sphere;

		area = 4 * M_PI * radius * radius;
	};

	bool intersect (
		const Ray & ray,
		double & pathLength
	) const override;

	void getNormalDirection (
		const Vector & p0,
		Vector & normal
	) const override;

	Vector samplePointOnObject () const override;

	void Move ( float a, float b, float c ) override
	{
		center.x += a;
		center.y += b;
		center.z += c;
	}

	void Scale ( float a, float b, float c ) override
	{
		radius *= a;

		Updata_Object_Area();
	};

	double PDF_in_solid_angle (
		Vector sampledPointOnLight,
		Vector pointOnObject,
		Vector lightDir
	) const override;

	void Updata_Object_Area () override
	{
		area = 4 * M_PI * radius * radius;
	}
};

class Disk : public Objects
{
public:
	Vector normal;
	Pnt3D center;
	double radius;

	Disk (
		BRDFModels * brdf,
		Material * mat,
		Pnt3D p,
		double r,
		Vector n
	)
		: Objects( brdf, mat )
	{
		center = p;
		normal = n;
		radius = r;
		shapeFlag = obj_disk;

		area = 2 * M_PI * radius * radius;
	}

	Disk (
		Pnt3D p,
		     double r,
		     Vector n
	)
	{
		center = p;
		radius = r;
		normal = n;
		shapeFlag = obj_disk;

		area = 2 * M_PI * radius * radius;
	};

	bool intersect (
		const Ray & ray,
		double & pathLength
	) const override;

	void getNormalDirection (
		const Vector & p0,
		Vector & normal
	) const override;

	Vector samplePointOnObject () const override;

	void Move ( float a, float b, float c ) override
	{
		center.x += a;
		center.y += b;
		center.z += c;
	}

	void Scale ( float a, float b, float c ) override
	{
		radius *= a;

		Updata_Object_Area();
	};

	double PDF_in_solid_angle (
		Vector sampledPointOnLight,
		Vector pointOnObject,
		Vector lightDir
	) const override;

	void Updata_Object_Area () override
	{
		area = 2 * M_PI * radius * radius;
	}
};

struct Vertex
{
	float x, y, z, a;
};

struct Triangle_index_bre
{
	int v0, v1, v2;
};

struct Quad_index_bre
{
	int v0, v1, v2, v3;
};

inline void print_vertex ( Vertex ver )
{
	printf( "Vertex %f %f %f \n", ver.x, ver.y, ver.z );
}


inline void Vertex_RotX ( Vertex & ver, float theta )
{
	//	float a = ver.x;
	float b = ver.y;
	float c = ver.z;

	//	ver.x = a;
	ver.y = cos( theta ) * b - sin( theta ) * c;
	ver.z = cos( theta ) * c + sin( theta ) * b;
}

inline void Vertex_RotY ( Vertex & ver, float theta )
{
	float a = ver.x;
	//	float b = ver.y;
	float c = ver.z;

	ver.x = cos( theta ) * a + sin( theta ) * c;
	//	ver.y = b;
	ver.z = cos( theta ) * c - sin( theta ) * a;;
}

inline void Vertex_RotZ ( Vertex & ver, float theta )
{
	float a = ver.x;
	float b = ver.y;
	//	float c = ver.z;

	ver.x = cos( theta ) * a - sin( theta ) * b;
	ver.y = cos( theta ) * b + sin( theta ) * a;
	//	ver.z = c;
}

inline void Vertex_Move ( Vertex & ver, float a, float b, float c )
{
	ver.x += a;
	ver.y += b;
	ver.z += c;
}

inline void Vertex_ScaleX ( Vertex & ver, float a )
{
	ver.x *= a;
}

inline void Vertex_ScaleY ( Vertex & ver, float a )
{
	ver.y *= a;
}

inline void Vertex_ScaleZ ( Vertex & ver, float a )
{
	ver.z *= a;
}

inline double Vertex_distance ( const Vertex & v1, const Vertex & v2 )
{
	return sqrt( pow( ( v1.x - v2.x ), 2 ) + pow( ( v1.y - v2.y ), 2 ) + pow( ( v1.z - v2.z ), 2 ) );
}

