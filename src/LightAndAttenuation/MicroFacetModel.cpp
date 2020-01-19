#include "MicroFacetModel.h"


MicroFacetModel::MicroFacetModel ()
{
	surface = new GGX( 0.1 );
}


MicroFacetModel::~MicroFacetModel ()
{
}

MicroFacetModel::MicroFacetModel (
	bool is_GGX,
	double roughness )
{
	if ( is_GGX )
		surface = new GGX( roughness );
	else
		surface = new Beckmann_Eric( roughness );
}

double TorranceSparrow::PDF (
	Vector in,
	Vector normal,
	Vector out )
{
	double pdf_out;
	Vector local_normal = Normalize( out - in );
	double cos_theta = AbsDot( in, local_normal );

	double D = surface->D_wh( &in, &local_normal, &normal, pdf_out );

	// turn pdf of local normal to pdf of sampled outgoing direction
	pdf_out = pdf_out / ( 4 * cos_theta );

	return pdf_out;
}

void TorranceSparrow::BRDF (
	Vector in,
	Vector normal,
	Vector out,
	Attenuation & value,
	double & pdf )
{
	Vector local_normal = Normalize( out - in );

	double cos_theta = AbsDot( in, local_normal );
	double theta = Clamp( acos( cos_theta ), 0, M_PI );

	LOOP_OVER_SPECTRUM ( i )
		value.attenuation_plain.spec[ i ] =
			Fresnel_Reflection_Plain( theta,
			                          1, 0,
			                          refraction_index_n.spec[ i ],
			                          refraction_index_k.spec[ i ] );

	double D = 0;
	double G = 1;
	if ( abs( normal.z - 1 ) > eps || abs( in.y - 0 ) > eps )
	{
		Vector Z_axis( 0, 0, 1 );
		Vector normal_BRDF;
		Vector in_BRDF;
		Vector out_BRDF;
		// input vector is not in BRDF coordinate	
		TransformBetweenGlobalAndLocal trans;

		trans.InitTransformMatrix( in, normal );

		normal_BRDF = trans.World2Local( local_normal );
		in_BRDF = trans.World2Local( in );
		out_BRDF = trans.World2Local( out );

		D = surface->D_wh( &in_BRDF, &normal_BRDF, &Z_axis, pdf );
		G = surface->masking( &in_BRDF, &out_BRDF, &normal_BRDF, &Z_axis );
	}
	else
	{
		D = surface->D_wh( &in, &local_normal, &normal, pdf );
		G = surface->masking( &in, &out, &local_normal, &normal );
	}

	// turn pdf of local normal to pdf of sampled outgoing direction
	pdf = pdf / ( 4 * cos_theta );

	double denominator = 4 * AbsDot( in, normal ) * AbsDot( out, normal );

	value *= D * G / ( denominator );
}

Attenuation TorranceSparrow::SampleBRDF (
	Vector inWorld,
	Vector & outWorld,
	Vector normal,
	double & pdf_out )
{
	// detect back facing normal direction
	double same_side = Dot( inWorld, normal );

	if ( same_side > 0 )
		printf( "not at same side" );

	// initialise transformation matrix
	TransformBetweenGlobalAndLocal trans;

	trans.InitTransformMatrix( inWorld, normal );

	Vector local_in = trans.World2Local( inWorld );

	Vector local_normal;
	local_normal = surface->sample_wh( local_in, Vector( 0, 0, 1 ) );

	GetSpecularReflectionDirection( local_in, local_normal, outWorld );

	Attenuation brdfValue;

	BRDF( local_in,
	      Vector( 0, 0, 1 ),
	      outWorld,
	      brdfValue,
	      pdf_out );

	outWorld = trans.Local2World( outWorld );

	return brdfValue;
}

