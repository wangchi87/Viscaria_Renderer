#include "PhotonTracer.h"

PhotonTracer::PhotonTracer ()
{
}

PhotonTracer::~PhotonTracer ()
{
}

void PhotonTracer::Tracer ( const Scene & myScene )
{
	Objects *lightSource; 
	double choose_light_pdf;
	lightSource = myScene.AquireOneOfEmissiveObjects ( choose_light_pdf );

	Pnt3D pointOnLight;
	pointOnLight = lightSource->samplePointOnObject ();

	Vector lightDirection;
	
	// sample light direction
	// has to be changed in the future
	lightSource->getNormalDirection ( pointOnLight , lightDirection );

	bool photonAbsorbed = false;
	
	// this brdf_pdf is actually not useful here
	double brdf_pdf = 0;

	Ray photonRay ( pointOnLight , lightDirection );
	Intersection intersectionInfo;

	while (!photonAbsorbed)
	{
		if (!myScene.IntersectionWithScene ( photonRay , intersectionInfo ))
			break;

		Objects * obj = intersectionInfo.object;
		BRDFModels * brdf = obj->brdf_model;

		Vector geometryNormalDirection = intersectionInfo.normal;
		float u1 = getUniformRandomNumber ( 0.0 , 1.0 );

		Photon initialPhoton;

		float reflectance = brdf->getReflectance ( photonRay.direction , geometryNormalDirection );

		if (u1 < reflectance)
		{
			// reflect the photon

			Vector sampledNextDirection ( 0 , 0 , 0 );

			Attenuation attenuationFactor =
				brdf->SampleBRDF (
					photonRay.direction ,
					sampledNextDirection ,
					geometryNormalDirection ,
					brdf_pdf
				);

			photonRay.origin = intersectionInfo.intersectionPoint;
			photonRay.direction = sampledNextDirection;

			if (obj->shapeFlag == obj_importedMesh)
			{
				obj->texture.RecordPhoton ( initialPhoton , intersectionInfo.primID , intersectionInfo.hit_u , intersectionInfo.hit_v );
			}
		}
		else
		{	
			// absorb the photon
			photonAbsorbed = true;
		}
		
	}

}
