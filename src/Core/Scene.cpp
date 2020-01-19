#include "Scene.h"


Scene::Scene ()
{
	device = rtcNewDevice( nullptr );

	error_handler( nullptr, rtcDeviceGetError( device ) );

	/* set error handler */
	rtcDeviceSetErrorFunction2( device, error_handler, nullptr );


	// works for embree 2.13.0, left for reference

	//	error_handler ( rtcDeviceGetError ( device ) );
	/* set error handler */
	//	rtcDeviceSetErrorFunction2 ( device , error_handler );

	scene_bre = rtcDeviceNewScene(
		device,
		RTC_SCENE_STATIC | RTC_SCENE_ROBUST,
		RTC_INTERSECT1 );

	ObjTotalNum = 0;
}


Scene::~Scene ()
{
}

void Scene::AddObject ( Objects * obj )
{
	obj->objID = ObjTotalNum;
	ObjTotalNum++;

	if ( obj->shapeFlag != obj_triangleMesh )
		self_defined_objs.push_back( obj );

	if ( obj->shapeFlag > obj_mesh)
	{
		// obj embreeID is identical to index of bre_objs
		bre_objs.push_back( obj );
		// insert into scene_bre
		obj->toEmbreeObj( scene_bre );
	}
	if ( obj->emitter == true )
		emissive_objs.push_back( obj );
}

void Scene::Commit_Embree_Scene ()
{
	rtcCommit( scene_bre );
}

Objects * Scene::AquireOneOfEmissiveObjects (double &pdf) const
{
	int length = emissive_objs.size();
	int index = getUniformRandomNumber( 0.0, length ); // might cause error
	if ( index == length )
		index--;
	pdf = 1 / length;
	return emissive_objs[ index ];
}

bool Scene::IntersectionWithScene (
	const Ray & ray,
	Intersection & closestIntersectionPoint
) const
{
	// move the start point a little bit away
	Pnt3D startPoint = ray.origin + ray.direction * eps * 100000;

	// avoid self-intesection
	Ray newRay( startPoint, ray.direction );

	double minT = inf;
	Objects * closestObject = nullptr;
	bool hasIntersection = false;

	// ********** find intersection with self-defined obj first   ******
	for ( size_t i = 0; i < self_defined_objs.size(); i++ )
	{
		//Objects * obj = & self_defined_objs[ i ];
		double t = 0;

		bool intersected = self_defined_objs[ i ]->intersect( newRay, t );
		if ( intersected == true )
		{
			if ( t < minT && t > eps )
			{
				hasIntersection = true;
				minT = t;
				closestObject = self_defined_objs[ i ];
			}
		}
	}

	// ********** then do Embree intersection test **********

	RTCRay ray_bre;
	ray_bre = newRay.toEmbreeRay();
	rtcIntersect( scene_bre, ray_bre );

	bool breHasIntersection = ( ray_bre.geomID != RTC_INVALID_GEOMETRY_ID );


	if ( hasIntersection == false
		&& breHasIntersection == true
	)
	{
		closestIntersectionPoint = Intersection (
			newRay ,
			ray_bre.tfar ,
			ray_bre.primID ,
			ray_bre.u,
			ray_bre.v,
			bre_objs[ ray_bre.geomID ],
			Normalize( Vector( ray_bre.Ng[ 0 ], ray_bre.Ng[ 1 ], ray_bre.Ng[ 2 ] ) )
		);
		return true;
	}

	if ( hasIntersection == true
		&& minT > eps
		&& minT < inf
		&& breHasIntersection == true
		&& closestObject != nullptr )
	{
		if ( minT > ray_bre.tfar )
			closestIntersectionPoint = Intersection(
				newRay,
				ray_bre.tfar,
				ray_bre.primID,
				ray_bre.u ,
				ray_bre.v ,
				bre_objs[ ray_bre.geomID ],
				Normalize( Vector( ray_bre.Ng[ 0 ], ray_bre.Ng[ 1 ], ray_bre.Ng[ 2 ] ) )
			);
		else
			closestIntersectionPoint = Intersection(
				newRay,
				minT,
				closestObject
			);
		return true;
	}

	if ( hasIntersection == true
		&& minT > eps
		&& minT < inf
		&& breHasIntersection == false
		&& closestObject != nullptr )
	{
		closestIntersectionPoint = Intersection(
			newRay,
			minT,
			closestObject
		);
		return true;
	}
	closestIntersectionPoint.clear();
	return false;
}

/**
 * \brief occlusion test
 * \param From		usually intesected point
 * \param To		usually sampled point on light source
 * \param endObj	usually obj pointer to light source
 * \return 
 */
bool Scene::Occluded (
	Pnt3D From,
	     Pnt3D To,
	     Objects * endObj
) const
{
	Vector rayDirection = To - From;

	rayDirection = Normalize( rayDirection );

	// move the start point a little bit away to avoid self-intesection
	Pnt3D startPoint = From + rayDirection * eps * 100000;

	// how many units the light goes from start point to end point
	double pathLength = ( To - startPoint ).Length();

	Ray ray( startPoint, rayDirection );

	// conduct intesection in Embree
	RTCRay ray_bre;
	ray_bre = ray.toEmbreeRay ();
	ray_bre.geomID = RTC_INVALID_GEOMETRY_ID;
	rtcIntersect ( scene_bre , ray_bre );

	// ***************** if there is NO self-defined object in the scene  **************
	if (self_defined_objs.size () == 0)
	{
		// for registed embree object, if intesected obj ID is identical
		// with endObj, the sampled point('TO' param) is visiable.
		if (ray_bre.geomID != endObj->embreeID)
			return true;
		else
			return false;
	}

	// ***************** if there is self-defined object in the scene  *****************


	// closest embree object is in front of self defined object
	if (abs ( pathLength - ray_bre.tfar ) > 10 * eps && pathLength > ray_bre.tfar)
		return true;


	// if embree object is behind self defined object, we need take care 
	// of the possibility that the sampled point is not the closest point on the sampled object
	// for example sampled point is on the back side of the sphere or cube, in the view of the ray.
	// so, we have to detect the closest intesection again with self-defined object, to ensure the 
	// visibility of the sampled point.

	double t = 0;
	bool intersected = false;
	for (size_t i = 0; i < self_defined_objs.size (); i++)
	{
		intersected = self_defined_objs[ i ]->intersect ( ray , t );

		if (intersected == true && abs ( pathLength - t ) > 10 * eps)
		{
			// In this case: abs ( pathLength - t ) > 10 * eps, 
			// the new intesection point 
			// is NOT the same one with 'TO' point.

			// And then, if t is smaller that pathLength
			// the TO point is occluded.
			if (pathLength - t > eps && t > eps)
			{
				return true;
			}

		}
	}

	return false;
}

/* error reporting function */
void error_handler ( void * userPtr, const RTCError code, const char * str )
{
	if ( code == RTC_NO_ERROR )
		return;

	printf( "Embree: " );
	switch ( code )
	{
	case RTC_UNKNOWN_ERROR: printf( "RTC_UNKNOWN_ERROR" );
		break;
	case RTC_INVALID_ARGUMENT: printf( "RTC_INVALID_ARGUMENT" );
		break;
	case RTC_INVALID_OPERATION: printf( "RTC_INVALID_OPERATION" );
		break;
	case RTC_OUT_OF_MEMORY: printf( "RTC_OUT_OF_MEMORY" );
		break;
	case RTC_UNSUPPORTED_CPU: printf( "RTC_UNSUPPORTED_CPU" );
		break;
	case RTC_CANCELLED: printf( "RTC_CANCELLED" );
		break;
	default: printf( "invalid error code" );
		break;
	}
	if ( str )
	{
		printf( " (" );
		while ( *str ) putchar( *str++ );
		printf( ")\n" );
	}
	exit( 1 );
}

