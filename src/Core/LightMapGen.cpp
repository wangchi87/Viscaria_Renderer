#include "LightMapGen.h"



LightMapGen::LightMapGen ()
{
	photonNum = 1000;
}


LightMapGen::~LightMapGen ()
{
}

void LightMapGen::InitialiseGenerator ( const SceneFileParser * sfp )
{
	photonNum = sfp->lighmap_sample_count;
	myScene.Commit_Embree_Scene ();
}

void LightMapGen::Run ()
{
	size_t core_num = omp_get_num_procs ();

	printf ( "Number of cores on the machine: %d \n" , core_num );

	// left for debug use
	size_t thread_num = 8;

	#pragma omp parallel for num_threads(thread_num)
	for (long long i = 0; i < photonNum; i++)
	{
		tracer.Tracer ( myScene );
	}

	for (int i = 0; i < myScene.GetEmbreeObjsCounts (); i++)
	{
		Objects * obj = myScene.GetEmbreeObject ( i );

		if (obj->shapeFlag == obj_importedMesh)
		{
			obj->texture.imagePort.SaveLightMap ();
		}
	}


}

void LightMapGen::SetPhotonNum ( size_t num )
{
	photonNum = num;
}
