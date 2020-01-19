#include "PathTracer_MIS.h"


PathTracer_MIS::PathTracer_MIS ()
{
}


PathTracer_MIS::~PathTracer_MIS ()
{
}

Spectrum PathTracer_MIS::Integration (
	const Scene * thisScene,
	const Ray & ray )
{
	Spectrum intensity;

	Attenuation throughput;
	throughput.resetOne();

	Intersection intersectionInfo;

	double cos_i = 0;

	// russian roulette
	double rr = 1;

	double brdf_pdf = 0;
	double light_pdf = 0;

	double MIS_weight = 1;

	Ray current_ray( ray.origin, ray.direction );
	Ray old_ray( ray.origin, ray.direction );
	size_t current_depth = 0;

	Spectrum lightSourceSamplingContribution;

	Intersection last_intersectionInfo;

	while ( !( current_depth == maxDepth ) && ! throughput.isEmpty() )
	{
		if ( !thisScene->IntersectionWithScene( current_ray, intersectionInfo ) )
			break;

		Objects * obj = intersectionInfo.object;

		if ( obj->emitter )
		{
			// estimate BRDF sampling
			if ( current_depth > 0 )
			{
				// MIS
				light_pdf = obj->PDF_in_solid_angle (
					last_intersectionInfo.intersectionPoint,
					intersectionInfo.intersectionPoint,
					current_ray.direction
				);
				MIS_weight = BalanceHeuristic( 1, brdf_pdf, 1, light_pdf );
			}

			intensity +=
				Attenuate_Light_Spectrum(
					throughput,
					obj->emission
				) * MIS_weight;
		}

		// **************  start to estimate Direct illummination *****************

		lightSourceSamplingContribution.resetZero();


		if ( !obj->emitter )
		{
			lightSourceSamplingContribution =
				EstimateLightSampling(
					thisScene,
					& intersectionInfo,
					current_ray
				);
			intensity += Attenuate_Light_Spectrum(
				throughput,
				lightSourceSamplingContribution
			);
		}

		// ********************* sample next direction *****************

		Vector geometryNormalDirection = intersectionInfo.normal;
		Vector sampledNextDirection ( 0 , 0 , 0 );
		Attenuation attenuationFactor;

		BRDFSampleNextDirection (
			current_ray ,
			&intersectionInfo ,
			attenuationFactor ,
			sampledNextDirection ,
			brdf_pdf
		);
		
		// russian roulette
		//		double rand = getUniformRandomNumber ( 0.0 , 1 );
		//
		//		if ( rand < rr && rand > eps)
		//				break;

		cos_i = AbsDot(
			sampledNextDirection,
			geometryNormalDirection );

		if ( brdf_pdf == 0 )
			break;

		throughput = throughput * ( attenuationFactor * ( cos_i / ( rr * brdf_pdf ) ) );

		// update ray
		current_ray.origin = intersectionInfo.intersectionPoint;
		current_ray.direction = sampledNextDirection;

		last_intersectionInfo = intersectionInfo;

		current_depth++;
	}
	ASSERT_NAN( intensity );

	return intensity;
}

