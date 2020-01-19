#include "DirectIllumination.h"


Spectrum DirectIllumination::Integration (
	const Scene * thisScene,
	const Ray & ray )
{
	Intersection intersectionInfo;

	// check intersection between ray and scene
	if ( !thisScene->IntersectionWithScene( ray, intersectionInfo ) )
		return Spectrum();

	Spectrum intensity;

	Objects * obj = intersectionInfo.object;

	if ( obj->emitter )
	{
		intensity += obj->emission;
		return intensity;
	}

	// **************** evaluate direction illumination ******
	if (!obj->emitter)
	{
		Ray currentRay = ray;
		
		Attenuation throughput;
		
		// find a non-delta surface
		while (obj->brdf_model->surfaceFlag == surface_delta)
		{
			BRDFModels * brdf = obj->brdf_model;
			Vector geometryNormalDirection = intersectionInfo.normal;
			Vector sampledNextDirection ( 0 , 0 , 0 );
			Attenuation attenuationFactor;
			double brdf_pdf = 1;

			BRDFSampleNextDirection (
				currentRay ,
				&intersectionInfo ,
				attenuationFactor ,
				sampledNextDirection,
				brdf_pdf
			);

			float cos_i = AbsDot (
				sampledNextDirection ,
				geometryNormalDirection );
			throughput = throughput * ( attenuationFactor * ( cos_i / ( brdf_pdf ) ) );

			currentRay.origin = intersectionInfo.intersectionPoint;
			currentRay.direction = sampledNextDirection;

			if (!thisScene->IntersectionWithScene ( currentRay , intersectionInfo ))
				return Spectrum ();
		}
		
		EstimateDirectIllumination (
			thisScene ,
			&intersectionInfo ,
			currentRay ,
			intensity
		);

		intensity = Attenuate_Light_Spectrum (
			throughput ,
			intensity
		);
	}

	//intensity.print ();
	return intensity;
}

