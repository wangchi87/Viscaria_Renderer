#include "LightTransSim.h"


inline void EstimateDirectIllumination (
	const Scene * thisScene,
	const Intersection * intersectionInfo,
	const Ray & ray,
	double & light_pdf,
	Spectrum & lightSourceSamplingContribution
);

LightTransSim::LightTransSim ()
{
}


LightTransSim::~LightTransSim ()
{
}

Spectrum LightTransSim::Integration(const Scene* thisScene, const Ray& ray)
{
	return Spectrum();
}

PathTracer::PathTracer (): maxDepth( 3 )
{
}


Spectrum PathTracer::Integration (
	const Scene * thisScene,
	const Ray & ray,
	size_t currentDepth
) const
{
	// Russian Roulette
	// probility to stop this path
	double rr = 0.01;
	double randnum = getUniformRandomNumber( 0.0, 0.99 );
	if ( randnum < rr && currentDepth > 5 )
		return Spectrum();

	if ( currentDepth == maxDepth )
		return Spectrum();

	Intersection intersectionInfo;

	// check intersection between ray and scene
	if ( ! thisScene->IntersectionWithScene( ray, intersectionInfo ) )
		return Spectrum();

	// if there is an intersection
	Spectrum intensity;
	Spectrum emission;

	Objects * obj = intersectionInfo.object;

	if ( obj->emitter )
	{
		emission = obj->emission;
	}

	BRDFModels * brdf = obj->brdf_model;

	// Get surface (macro) normal direction
	Vector geometryNormalDirection ( 0 , 0 , 0 );
	Vector sampledNextDirection ( 0 , 0 , 0 );

	geometryNormalDirection = intersectionInfo.normal;

	double pdf = 0;

	Attenuation attenuationFactor = 
		brdf->SampleBRDF(
			ray.direction,
			sampledNextDirection,
			geometryNormalDirection,
			pdf
		);

	Ray newRay(
		intersectionInfo.intersectionPoint,
		sampledNextDirection
	);

	Spectrum Li =
		Integration(
			thisScene,
			newRay,
			currentDepth + 1
		);


	if ( !Li.isEmpty() )
	{
		attenuationFactor.Scaled_by_Spectrum_Value( obj->material_type->color );

		double cos_i = AbsDot( sampledNextDirection, geometryNormalDirection );

		intensity +=
			Attenuate_Light_Spectrum( attenuationFactor, Li ) * cos_i / pdf;
	}

	// weight the radiance back after Ruassian Roulette
	intensity = ( currentDepth > 5 ) ? ( intensity / ( 1 - rr ) ) : intensity;

	ASSERT_NAN( intensity );

	return emission + intensity;
}

Spectrum PathTracer::Integration (
	const Scene * thisScene,
	const Ray & ray
)
{
	Spectrum intensity;

	Attenuation throughput;
	throughput.resetOne();

	Intersection intersectionInfo;

	double cos_i = 0;
	double pdf = 1;

	double rr = 1;

	Ray current_ray( ray.origin, ray.direction );
	size_t current_depth = 0;
	while ( !( current_depth == maxDepth ) )
	{
		if ( !thisScene->IntersectionWithScene( current_ray, intersectionInfo ) )
			break;

		Objects * obj = intersectionInfo.object;

		if ( obj->emitter )
		{
			intensity +=
				Attenuate_Light_Spectrum(
					throughput,
					obj->emission
				);
		}

		BRDFModels * brdf = obj->brdf_model;

		// Get surface (macro) normal direction
		Vector geometryNormalDirection;

		geometryNormalDirection = intersectionInfo.normal;

		Vector sampledNextDirection( 0, 0, 0 );
		Attenuation attenuationFactor;

		attenuationFactor = brdf->SampleBRDF(
			current_ray.direction,
			sampledNextDirection,
			geometryNormalDirection,
			pdf
		);

		//		double randnum = getUniformRandomNumber ( 0.0 , 0.99 );

		//		if ( randnum < rr)
		//			break;

		attenuationFactor.Scaled_by_Spectrum_Value( obj->material_type->color );

		cos_i = AbsDot( sampledNextDirection, geometryNormalDirection );

		throughput = throughput * ( attenuationFactor * ( cos_i / ( rr * pdf ) ) );

		// update ray
		current_ray.origin = intersectionInfo.intersectionPoint;
		current_ray.direction = sampledNextDirection;
		current_depth++;
	}

	return intensity;
}


