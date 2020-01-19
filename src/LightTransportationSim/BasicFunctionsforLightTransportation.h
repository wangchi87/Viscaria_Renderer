/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:18:20
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:18:20
 * @Description: All rights reserved
 */
#pragma once
#include "Core/Ray.h"
#include "Core/Scene.h"
#include "LightAndAttenuation/Spectrum.h"

#define DebugMode false

// ********** basic inline functions for Light transporation simulation *****
// 1. GeometricTerm
// 2. BalanceHeuristic
// 3. EstimateLightSampling
// 4. EstimateBRDFSampling

inline double GeometricTerm (
	const Ray & ray,
	const Vector & normal,
	const Intersection & next_intersection )
{
	double cos_1 = AbsDot( ray.direction, normal );

	Vector normal_next_intersection;
	next_intersection.object->getNormalDirection( next_intersection.intersectionPoint, normal_next_intersection );

	double cos_2 = AbsDot( ray.direction, normal_next_intersection );

	double distance = DistanceSquareBetweenPoints( ray.origin, next_intersection.intersectionPoint );

	return cos_1 * cos_2 / distance;
}

inline double GeometricTerm (
	const Ray & ray,
	const Vector & normal,
	const Pnt3D & next_intersection_point,
	const Vector & normal_next_intersection )
{
	double cos_1 = AbsDot( ray.direction, normal );

	double cos_2 = AbsDot( ray.direction, normal_next_intersection );

	double distance = DistanceSquareBetweenPoints( ray.origin, next_intersection_point );

	return cos_1 * cos_2 / distance;
}

inline float BalanceHeuristic (
	int nf,
	float fPdf,
	int ng,
	float gPdf )
{
	if ( fPdf == 0 && gPdf == 0 )
	{
		printf( "Both PDFs are zero \n" );
		return 0;
	}

	return ( nf * fPdf ) / ( nf * fPdf + ng * gPdf );
}

inline void BalanceHeuristic (
	int nf, double fPdf,
	int ng, double gPdf,
	double & weight_f,
	double & weight_g )
{
	if ( fPdf == 0 && gPdf == 0 )
	{
		weight_f = 0;
		weight_g = 0;
	}
	weight_f = ( nf * fPdf ) / ( nf * fPdf + ng * gPdf );
	weight_g = ( nf * gPdf ) / ( nf * fPdf + ng * gPdf );
}

inline Spectrum GetIntersectionTexture ( const Intersection * intersectionInfo )
{
	Objects * obj = intersectionInfo->object;
	Spectrum textureColor;
	textureColor.resetOne ();
	if (obj->shapeFlag == obj_importedMesh && obj->texture.HasTexture ())
	{
		Spectrum textureColor = obj->texture.QueryTextureColor ( intersectionInfo->primID , intersectionInfo->hit_u , intersectionInfo->hit_v );
	}
	return textureColor;
}


inline void BRDFSampleNextDirection (
	const Ray & current_ray,
	const Intersection * intersectionInfo,
	Attenuation & attenuationFactor,
	Vector & sampledNextDirection,
	double & brdf_pdf
)
{
	Objects * obj = intersectionInfo->object;
	BRDFModels * brdf = obj->brdf_model;

	Vector geometryNormalDirection = intersectionInfo->normal;

	attenuationFactor =
		brdf->SampleBRDF (
			current_ray.direction ,
			sampledNextDirection ,
			geometryNormalDirection ,
			brdf_pdf
		);

	// apply material color
	attenuationFactor.Scaled_by_Spectrum_Value ( obj->material_type->color );

	// apply texture color
	Spectrum textureColor = GetIntersectionTexture ( intersectionInfo );
	attenuationFactor.Scaled_by_Spectrum_Value ( textureColor );
}

// only evaluate light sampling with MIS
inline Spectrum EstimateLightSampling (
	const Scene * thisScene,
	const Intersection * intersectionInfo,
	const Ray & ray )
{
	Objects * obj = intersectionInfo->object;
	BRDFModels * brdf = obj->brdf_model;

	Vector newRayDirection;
	Vector normalDirection;

	normalDirection = intersectionInfo->normal;

	Attenuation attenuationFactor;
	bool delta_brdf_hitEmissive = false;
	bool occludedByObjects = true;

	double light_pdf = 0;
	double brdf_pdf = 0;
	double weight = 1;

	Spectrum lightSourceSamplingContribution;

	if ( brdf->surfaceFlag == surface_delta )
		return lightSourceSamplingContribution;


	// ***************    light source sampling evaluation   **********************

	Objects * light;
	double choose_light_pdf = 1;
	light = thisScene->AquireOneOfEmissiveObjects( choose_light_pdf );

	Pnt3D pointOnLight;
	pointOnLight = light->samplePointOnObject ();
	occludedByObjects = thisScene->Occluded (
		intersectionInfo->intersectionPoint ,
		pointOnLight ,
		light
	);

	if ( occludedByObjects == false && brdf->surfaceFlag != surface_delta )
	{
		newRayDirection = Normalize( pointOnLight - intersectionInfo->intersectionPoint );

		brdf->BRDF(
			ray.direction,
			normalDirection,
			newRayDirection,
			attenuationFactor,
			brdf_pdf
		);

		attenuationFactor.Scaled_by_Spectrum_Value( obj->material_type->color );

		if (obj->shapeFlag == obj_importedMesh && obj->texture.HasTexture())
		{
			Spectrum textureColor = obj->texture.QueryTextureColor ( intersectionInfo->primID, intersectionInfo->hit_u, intersectionInfo->hit_v );
			// textureColor.print ();
			attenuationFactor.Scaled_by_Spectrum_Value ( textureColor );
		}

		// light pdf in angular form
		light_pdf = light->PDF_in_solid_angle (
			pointOnLight,
			intersectionInfo->intersectionPoint,
			newRayDirection );

		weight = BalanceHeuristic ( 1 , light_pdf , 1 , brdf_pdf );

		double cos_i = AbsDot ( newRayDirection , intersectionInfo->normal );

		lightSourceSamplingContribution +=
			Attenuate_Light_Spectrum (
				attenuationFactor ,
				light->emission
			) * ( cos_i * weight / light_pdf );
	}
	else
	{
		if ( DebugMode )
		{
			printf( "Direct illimination can not sample a point on light source" );
		}
	}

	return lightSourceSamplingContribution/ choose_light_pdf;
}

// only evaluate BRDF sampling with MIS
inline Spectrum EstimateBRDFSampling (
	const Scene * thisScene,
	const Intersection * intersectionInfo,
	const Ray & ray )
{
	// ***************    BRDF sampling evaluation   **********************
	Objects * obj = intersectionInfo->object;
	BRDFModels * brdf = obj->brdf_model;

	Vector newRayDirection;
	Vector normalDirection;

	normalDirection = intersectionInfo->normal;

	Attenuation attenuationFactor;
	bool delta_brdf_hitEmissive = false;
	bool occludedByObjects = true;

	double light_pdf = 0;
	double brdf_pdf = 0;
	double weight = 1;

	Spectrum brdfSamplingContribution;

	attenuationFactor = brdf->SampleBRDF(
		ray.direction,
		newRayDirection,
		normalDirection,
		brdf_pdf
	);

	Ray newRay( intersectionInfo->intersectionPoint, newRayDirection );

	Intersection newIntersectionInfo;

	if ( thisScene->IntersectionWithScene( newRay, newIntersectionInfo ) )
		if ( newIntersectionInfo.object->emitter == true )
		{
			attenuationFactor.Scaled_by_Spectrum_Value( obj->material_type->color );

			if (obj->shapeFlag == obj_importedMesh && obj->texture.HasTexture ())
			{
				Spectrum textureColor = obj->texture.QueryTextureColor ( newIntersectionInfo.primID, newIntersectionInfo.hit_u, newIntersectionInfo.hit_v );
				// textureColor.print ();
				attenuationFactor.Scaled_by_Spectrum_Value ( textureColor );
			}

			//double cos_i = AbsDot( ray.direction, normalDirection );

			double cos_i = AbsDot ( newRayDirection , normalDirection );

			light_pdf = newIntersectionInfo.object->PDF_in_solid_angle (
				intersectionInfo->intersectionPoint,
				newIntersectionInfo.intersectionPoint,
				newRayDirection
			);
			weight = BalanceHeuristic( 1, brdf_pdf, 1, light_pdf );

			brdfSamplingContribution =
				Attenuate_Light_Spectrum(
					attenuationFactor,
					newIntersectionInfo.object->emission
				) * ( cos_i * weight / brdf_pdf );
		}
	return brdfSamplingContribution;
}

