/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:17:53
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:17:53
 * @Description: All rights reserved
 */
#pragma once

#include "BRDFModels.h"
#include "MicroSurface.h"

class MicroFacetModel: public BRDFModels
{
public:
	MicroSurface * surface;

	MicroFacetModel ();
	~MicroFacetModel ();
	MicroFacetModel ( bool is_GGX, double roughness );
};

class TorranceSparrow : public MicroFacetModel
{
public:
	Spectrum refraction_index_n;
	Spectrum refraction_index_k;

	TorranceSparrow () : MicroFacetModel()
	{
	};

	TorranceSparrow (
		bool is_GGX,
		double roughness,
		const Spectrum & n,
		const Spectrum & k
	)
		: MicroFacetModel( is_GGX, roughness )
	{
		Spectrum_s_Init_s( n, refraction_index_n );
		Spectrum_s_Init_s( k, refraction_index_k );
		//		refraction_index_n.Init_Spectrum( n );
		//		refraction_index_k.Init_Spectrum( k );
	}

	void PrintBRDF ()
	{
		printf( "TS BRDF" );
	}

	double PDF (
		Vector in,
		Vector normal,
		Vector out )
	override;

	void BRDF (
		Vector in,
		Vector normal,
		Vector out,
		Attenuation & value,
		double & pdf )
	override;

	Attenuation SampleBRDF (
		Vector inWorld,
		Vector & outWorld,
		Vector normal,
		double & pdf_out )
	override;
};

