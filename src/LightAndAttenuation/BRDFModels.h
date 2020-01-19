/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:17:42
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:17:42
 * @Description: All rights reserved
 */
#include "Foundations/Vector.h"
#include "Physics/Fresnel.h"
#include "Foundations/Matrix.h"
#include "Foundations/RandomNumber.h"
#include "Foundations/TransformBetweenGlobalAndLocal.h"
#include "Material.h"
#include "Foundations/KdTree.h"
#include "Foundations/ReadFloatFile.h"
#include "MicroSurface.h"

#include "LightAndAttenuation/Attenuation.h"

#include <map>
using std::map;

#pragma once

enum
{
	surface_not_specified = -1,
	surface_SpecularRef = 0,
	surface_SpecularTrans = 1,
	surface_delta = 2,
	surface_Lambertian = 3,
	surface_Glossy = 4,
	surface_Measured = 5
};

class BRDFModels
{
public:
	BRDFModels ( void );

	virtual ~BRDFModels ( void );

	short surfaceFlag;

	Spectrum color;

	virtual void PrintBRDF ()
	{
	}

	// calculate BRDF value
	virtual void BRDF (
		Vector in,
		Vector out,
		Attenuation & value )
	{
	}

	// calculate BRDF value
	// and corresponding pdf
	virtual void BRDF (
		Vector in,
		Vector normal,
		Vector out,
		Attenuation & value,
		double & pdf )
	{
	}

	// sample outgoing direction
	virtual Attenuation SampleBRDF (
		Vector in,
		Vector & out,
		Vector normal )
	{
		return Attenuation();
	}

	// sample outgoing direction
	// and also return corresponding pdf
	virtual Attenuation SampleBRDF (
		Vector in,
		Vector & out,
		Vector normal,
		double & pdf_out )
	{
		return Attenuation();
	}

	virtual double PDF (
		Vector in,
		Vector normal,
		Vector out )
	{
		return 0;
	}

	void initRefColors ( const Spectrum & m )
	{
		color = m;
	}

	virtual float getReflectance (
		Vector in ,
		Vector normal
	)
	{
		return 0.8;
	}
};

class SpecularReflection : public BRDFModels
{
public:
	// index of refraction
	Spectrum refractionIndex_n;
	Spectrum refractionIndex_k;

	SpecularReflection ( Spectrum n1, Spectrum k1 ) //: BRDFModels( in, out, n )
	;
	void BRDF (
		Vector in,
		Vector out,
		Attenuation & value
	) override;

	void BRDF (
		Vector in,
		Vector normal,
		Vector out,
		Attenuation & value,
		double & pdf )
	override;

	void PrintBRDF ();

	Attenuation SampleBRDF (
		Vector in,
		Vector & out,
		Vector normal,
		double & pdf_out )
	override;

	Attenuation SampleBRDF (
		Vector in,
		Vector & out,
		Vector normal )
	override;

	double PDF (
		Vector in,
		Vector normal,
		Vector out )
	override;
};

class SpecularRefraction : public BRDFModels
{
public:

	// index of refraction
	Spectrum refractionIndex_n;
	Spectrum refractionIndex_k;

	bool TIR;

	SpecularRefraction ( Spectrum n1, Spectrum k1 ) //: BRDFModels( in, out, n )
	;

	void BRDF (
		Vector in,
		Vector normal,
		Attenuation & value )
	override;

	void BRDF (
		Vector in,
		Vector normal,
		Vector out,
		Attenuation & value,
		double & pdf )
	override;

	Attenuation SampleBRDF (
		Vector in,
		Vector & out,
		Vector normal,
		double & pdf_out )
	override;

	Attenuation SampleBRDF (
		Vector in,
		Vector & out,
		Vector normal )
	override;

	double PDF (
		Vector in,
		Vector normal,
		Vector out )
	override;
};

class Lambertian : public BRDFModels
{
public:
	double reflectance;

	Lambertian () //: BRDFModels( in, out, n )
	{
		reflectance = 0.9;
		surfaceFlag = surface_Lambertian;
	}

	Lambertian ( double albedo ) //: BRDFModels( in, out, n )
	{
		reflectance = albedo;
		surfaceFlag = surface_Lambertian;
	}

	void BRDF (
		Vector in,
		Vector out,
		Attenuation & value )
	override;

	void BRDF (
		Vector in,
		Vector normal,
		Vector out,
		Attenuation & value,
		double & pdf ) override;

	Attenuation SampleBRDF (
		Vector in,
		Vector & out,
		Vector normal,
		double & pdf_out )
	override;

	Attenuation SampleBRDF (
		Vector in,
		Vector & out,
		Vector normal )
	override;

	double PDF (
		Vector in,
		Vector normal,
		Vector out )
	override;

	void PrintBRDF ()
	{
		printf( "Lambertian BRDF" );
	}

	float getReflectance (
		Vector in ,
		Vector normal )
		override;
	
};


struct IrregIsotropicBRDFSample
{
	IrregIsotropicBRDFSample (
		const Pnt3D & pp,
		const Spectrum & vv )
		: p( pp ) , v( vv )
	{
	}

	IrregIsotropicBRDFSample ()
	{
	}

	Pnt3D p;
	Spectrum v;
};

class MeasuredBRDF : public BRDFModels
{
public:
	MicroSurface * surface;

	double roughness;

	const KdTree<IrregIsotropicBRDFSample> * thetaPhiData;

	MeasuredBRDF () //: BRDFModels( in, out, n )
	{
		surfaceFlag = surface_Measured;
		roughness = 0.2;
	}

	MeasuredBRDF (
		const char * filename,
		double roughness_parameter
	) //: BRDFModels( in, out, n )
	;

	void BRDF (
		Vector in,
		Vector out,
		Attenuation & value )
	override;

	void BRDF (
		Vector in,
		Vector normal,
		Vector out,
		Attenuation & value,
		double & pdf )
	override;

	Attenuation SampleBRDF (
		Vector in,
		Vector & out,
		Vector normal )
	override;

	Attenuation SampleBRDF (
		Vector in,
		Vector & out,
		Vector normal,
		double & pdf_out )
	override;

	double PDF (
		Vector in,
		Vector normal,
		Vector out )
	override;
};

Pnt3D inline BRDFRemap (
	const Vector & wo,
	const Vector & wi )
{
	double cosi = CosTheta( wi ), coso = CosTheta( wo );
	double sini = SinTheta( wi ), sino = SinTheta( wo );
	double phii = SphericalPhi( wi ), phio = SphericalPhi( wo );
	double dphi = phii - phio;
	if ( dphi < 0. ) dphi += 2.f * M_PI;
	if ( dphi > 2.f * M_PI ) dphi -= 2.f * M_PI;
	if ( dphi > M_PI ) dphi = 2.f * M_PI - dphi;
	return Pnt3D( sini * sino, dphi / M_PI, cosi * coso );
}


struct IrregIsoProc
{
	// IrregIsoProc Public Methods
	IrregIsoProc ()
	{
		sumWeights = 0.f;
		nFound = 0;
	}

	void operator() (
		const Pnt3D & p,
		const IrregIsotropicBRDFSample & sample,
		double d2,
		double & maxDist2 )
	{
		double weight = expf( -100.f * d2 );

		Spectrum temp;

		Spectrum_sd_Mul_s( sample.v, weight, temp );

		Spectrum_s_AddTo_s( temp, v );

		//		v += sample . v * weight;
		sumWeights += weight;
		++nFound;
	}

	Spectrum v;
	double sumWeights;
	int nFound;
};

