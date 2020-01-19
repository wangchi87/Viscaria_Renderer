#include "RenderRunner.h"


RenderRunner::RenderRunner ()
{
}


RenderRunner::~RenderRunner ()
{
	delete [] thread_image;
}


void RenderRunner::InitialiseImageAndCamera (
	size_t widthImage,
	size_t heightImage,
	size_t sample,
	double fovx,
	double fovy,
	Pnt3D eyePoint,
	Pnt3D imagePoint,
	Vector upDirection
)
{
	width			= widthImage;
	height			= heightImage;
	samplePerPixel	= sample;

	myImage		= ImageRawData( width, height );

	myCamera	= new PinholeCamera(
		width,
		height,
		fovx,
		fovy,
		eyePoint,
		imagePoint,
		upDirection
	);
}

void RenderRunner::InitialiseRenderer ( const SceneFileParser * sfp )
{
	double camera_fovy = sfp->camera_fov * sfp->image_height / sfp->image_width;

	InitialiseImageAndCamera (
		sfp->image_width ,
		sfp->image_height ,
		sfp->image_sample_per_pixel ,
		sfp->camera_fov ,
		camera_fovy ,
		sfp->cameraPos ,
		sfp->cameraLookAt ,
		sfp->camera_up
	);
	
	//create integrator
	if ( strcmp( sfp->integratorName, "PATH_TRACER_MIS" ) == 0 )
	{
		integrator = new PathTracer_MIS( sfp->path_max_depth );
	}
	if ( strcmp( sfp->integratorName, "PATH_TRACER" ) == 0 )
	{
		integrator = new PathTracer( sfp->path_max_depth );
	}
	if ( strcmp( sfp->integratorName, "DIRECT_ILLUMINATION" ) == 0 )
	{
		integrator = new DirectIllumination();
	}

	output_filename = sfp->output_filename;

	myScene.Commit_Embree_Scene ();

	InitialiseThreadData ();
}

void RenderRunner::InitialiseThreadData ()
{
	int core_num = omp_get_num_procs();

	printf( "Number of cores on the machine: %d \n", core_num );

	thread_num = core_num < samplePerPixel ? core_num : samplePerPixel;

	// left for debug use
	thread_num = 8;

	// ******* initialise thread_image buffer *************
	thread_image = new ImageRawData[ thread_num ];

	for ( size_t i = 0; i < thread_num; i++ )
		thread_image[ i ] = ImageRawData( width, height );
}

void RenderRunner::Run ()
{
	size_t report_num = width / 10;

	for ( int i = 0; i < width; i++ )
	{
		if ( i % report_num == 0 )
			printf_s( "Finish %d of %llu \n", i, width );

		for ( int j = 0; j < height; j++ )
		{
			#pragma omp parallel for num_threads(thread_num)
			for ( int t = 0; t < samplePerPixel; t++ )
			{
				Spectrum pixelValuePerThreadImage;
				Ray primaryRay;

				// generate primary ray
				myCamera->CastingRayFromCamera( i, j, primaryRay );

				pixelValuePerThreadImage = integrator->Integration( &myScene, primaryRay ) / samplePerPixel;

				size_t thread_id = omp_get_thread_num();

				// splat value to thread image
				thread_image[ thread_id ].PixelValue(i,j) += pixelValuePerThreadImage;
			}
		}
	}

	MergeThreadImageBuffer();
}

void RenderRunner::MergeThreadImageBuffer ()
{
	for ( size_t i = 0; i < thread_num; i++ )
	{
		myImage = myImage + thread_image[ i ];
	}

	myImage.PixelNormalise ();

	myImage.SaveAsHDR ( output_filename + ".hdr" );
	myImage.SaveImage ( output_filename + ".png" , false , true , true );
}


// ***************************** LEFT for REFERENCE ****************************

//void RenderRunner::AddingObjects()
//{
//	Material	* white_mat = new Material ( Normalize ( RGBColors ( 1 , 1 , 1 ) ) );
//	
//	//myScene = new Scene ( );
//	// we use left-handed coordinate system 
//
//	//back wall
//	Pnt3D		plane1_pos		= Pnt3D ( -15 , 0 , 0 );
//	Vector		plane1_normal	= Normalize ( Vector ( 1 , 0 , 0 ) );
//	RGBColors	plane1_emission = RGBColors ( 500 , 500 ,500 );
//
//	Material	* plane1_mat	= new Material( Normalize( RGBColors (4, 1, 1)) );
//	BRDFModels	* plane1_brdf	= new Lambertian();
//
//	Objects * plane1 = new Plane( 
//		plane1_brdf,
//		plane1_mat,
//		plane1_pos, 
//		plane1_normal
//	);
//	
////	plane1->SetEmissionValue(plane1_emission);
//	
//	
//
//	// right wall
//	Pnt3D		plane2_pos		= Pnt3D ( 0 , -20 , 0 );
//	Vector		plane2_normal	= Normalize ( Vector ( 0 , 1 , 0 ) );
//	RGBColors	plane2_emission = RGBColors ( 200 , 200 , 300 );
//
//	Material	* plane2_mat	= new Material( Normalize ( RGBColors (1, 5, 5) ) );
//	BRDFModels	* plane2_brdf	= new Lambertian();
//
//	Objects * plane2 = new Plane (
//		plane2_brdf,
//		plane2_mat,
//		plane2_pos ,
//		plane2_normal
//	);
////	plane2->SetEmissionValue ( plane2_emission );
//
//	//left wall
//	Pnt3D		plane3_pos		= Pnt3D ( 0 , 20 , 0 );
//	Vector		plane3_normal	= Normalize ( Vector ( 0 , -1 , 0 ) );
//	RGBColors	plane3_emission = RGBColors ( 2000 , 1000 , 3000 );
//
//	Material	* plane3_mat	= new Material ( Normalize( RGBColors ( 8 , 8 , 3 )) );
//	BRDFModels	* plane3_brdf	= new SpecularReflection ( 3.24 , 7.39 ); //new Lambertian ();
//
//	Objects * plane3 = new Plane (
//		plane3_brdf ,
//		plane3_mat ,
//		plane3_pos ,
//		plane3_normal
//	);
//
////	plane3->SetEmissionValue ( plane3_emission );
//
//	//bottom wall
//	Pnt3D		plane4_pos		= Pnt3D ( 0 , 0 , -10 );
//	Vector		plane4_normal	= Normalize ( Vector ( 0 , 0 , 1 ) );
//	RGBColors	plane4_emission = RGBColors ( 2000 , 1000 , 3000 );
//
//	Material	* plane4_mat	= new Material ( Normalize ( RGBColors ( 5 , 1 , 5 ) ));
//	BRDFModels	* plane4_brdf	= new Lambertian ();
//
//	Objects * plane4 = new Plane (
//		plane4_brdf ,
//		plane4_mat ,
//		plane4_pos ,
//		plane4_normal
//	);
////	plane4->SetEmissionValue ( plane4_emission );
//
//
////  top wall
//	Pnt3D		plane5_pos		= Pnt3D ( 0 , 0 , 15 );
//	Vector		plane5_normal	= Normalize ( Vector ( 0 , 0 , -1 ) );
//	RGBColors	plane5_emission = RGBColors ( 1000 , 1000 , 1000 );
//
//	Material *		plane5_mat = new Material ( Normalize( RGBColors ( 1 , 4 , 1 )) );
//	BRDFModels *	plane5_brdf = new Lambertian ();
//
//	Objects * plane5 = new Plane (
//		plane5_brdf ,
//		plane5_mat ,
//		plane5_pos ,
//		plane5_normal
//	);
//	//plane5->SetEmissionValue ( plane5_emission );
//
//	Pnt3D		sphere1_pos		= Pnt3D ( -0 , 0 , 8 );
//	Material*	sphere1_mat		= new Material ( Normalize( RGBColors ( 3 , 5 , 1 )) );
//	BRDFModels*	sphere1_brdf	= new Lambertian ();
//
//	Objects * sphere1 = new Sphere (
//		sphere1_brdf ,
//		sphere1_mat ,
//		4 ,
//		sphere1_pos
//	);
//
//	RGBColors	sphere1_emission = RGBColors ( 500 , 500 , 500 );
////
////	sphere1->SetEmissionValue ( sphere1_emission );
//
//	Pnt3D		sphere2_pos		= Pnt3D ( 0 , 7 , -0 );
//	Material*	sphere2_mat		= new Material ( Normalize ( RGBColors ( 1 , 1 , 1 ) ) );
//	BRDFModels*	sphere2_brdf	= new SpecularReflection ( 3.24, 7.39 );
//
//	Objects * sphere2 = new Sphere (
//		sphere2_brdf ,
//		sphere2_mat ,
//		5 ,
//		sphere2_pos
//	);
//	Pnt3D		sphere3_pos		= Pnt3D ( -2 , -7 ,  -0 );
//	Material*	sphere3_mat		= new Material ( Normalize ( RGBColors ( 1,1,1 )) );
//	BRDFModels*	sphere3_brdf	= new SpecularReflection ( 3.24 , 7.39 ); //new SpecularRefraction ( 1.5 , 0 ); //new Lambertian ();
//
//	Objects * sphere3 = new Sphere (
//		sphere3_brdf ,
//		sphere3_mat ,
//		5 ,
//		sphere3_pos
//	);
//
//	Pnt3D		sphere4_pos = Pnt3D ( 8 , 8 , 7 );
//	Material*	sphere4_mat = new Material ( Normalize ( RGBColors ( 1 , 1 , 1 ) ) );
//	BRDFModels*	sphere4_brdf = new Lambertian (); //new SpecularRefraction ( 1.5 , 0 ); //new Lambertian ();
//
//	Objects * sphere4 = new Sphere (
//		sphere4_brdf ,
//		sphere4_mat ,
//		5 ,
//		sphere4_pos
//	);
//
////	RGBColors	sphere3_emission = RGBColors ( 500 , 500 , 500 );
//
////	sphere3->SetEmissionValue ( sphere3_emission );
//
//	Pnt3D		disk1_center	= Pnt3D ( 13 , -8 , 7 );
//	Vector		disk1_normal	= Normalize ( Vector ( 1 , 0 , 0 ) );
////	RGBColors	disk1_emission	 = RGBColors ( 1000 , 1000 , 1000 );
//
//	Material	* disk1_mat  = new Material ( Normalize ( RGBColors ( 1 , 1 , 1 ) ) );
//	BRDFModels	* disk1_brdf = new Lambertian ();
//
//	Objects * disk1 = new Disk (
//		disk1_brdf ,
//		disk1_mat ,
//		disk1_center ,
//		5,
//		disk1_normal 
//	);
//
////	disk1->SetEmissionValue ( disk1_emission );
//
//	Pnt3D		disk2_center = Pnt3D ( 0 , -0 , -7 );
//	Vector		disk2_normal = Normalize ( Vector ( 1 , 1 , 7 ) );
//	RGBColors	disk2_emission = RGBColors ( 1000 , 1000 , 1000 );
//
//	Material	* disk2_mat  = new Material ( Normalize ( RGBColors ( 1 , 1 , 1 ) ) );
//	BRDFModels	* disk2_brdf = new SpecularReflection ( 1 , 0 );
//
//	Objects * disk2 = new Disk (
//		disk2_brdf ,
//		disk2_mat ,
//		disk2_center ,
//		8 ,
//		disk2_normal
//	);
//	disk2->SetEmissionValue ( disk2_emission );
//	
//	
//	Triangle_Vertex tri1;
//	Pnt3D triangle_v1 ( 10 , 5 , 13 );
//	Pnt3D triangle_v2 ( 10 , 10,  7 );
//	Pnt3D triangle_v3 ( 10 , 0 ,  7 );
//	initTriangleVertex ( triangle_v1 , triangle_v2 , triangle_v3 , tri1 );
//
//	Triangle_Face_index tri1_face;
//	initTriangleFaceIndex ( 0 , 1 , 2 , tri1_face );
//
//	Material	* triangle1_mat = new Material ( Normalize ( RGBColors ( 4 , 1 , 1 ) ) );
//	BRDFModels	* triangle1_brdf = new Lambertian ();
//
//	Objects * triangle1 = new Triangle (
//		triangle1_brdf ,
//		triangle1_mat ,
//		tri1,
//		tri1_face
//	);
//
//
//
//	//Objects * plane6 = new Plane (
//		//true ,
//		//plane2_brdf ,
//		//plane2_mat ,
//		//plane6_pos ,
//		//plane5_normal
//	//);
//	
////	myScene.AddObject ( sphere1 );
////	myScene.AddObject ( sphere2 );
////	myScene.AddObject ( sphere3 );
//////	myScene.AddObject ( sphere4 );
////	myScene.AddObject ( plane1 );
////	myScene.AddObject ( plane2 );
////	myScene.AddObject ( plane3 );
////	myScene.AddObject ( plane4 );
////	myScene.AddObject ( plane5 );
////	myScene.AddObject ( disk1 );
//	//myScene.AddObject ( triangle1 );
//
//
//
//	Vertex vertices[ 4 ], v1[4];
//	Triangle_index_bre triangles[ 2 ];
//
//	vertices[ 0 ].y = -10.f; vertices[ 0 ].x = -0.f; vertices[ 0 ].z = -10.f;
//	vertices[ 1 ].y = -10.f; vertices[ 1 ].x = -0.f; vertices[ 1 ].z = +10.f;
//	vertices[ 2 ].y = +10.f; vertices[ 2 ].x = -0.f; vertices[ 2 ].z = -10.f;
//	vertices[ 3 ].y = +10.f; vertices[ 3 ].x = -0.f; vertices[ 3 ].z = +10.f;
//
//	v1[ 0 ].y = 10.f; v1[ 0 ].x = -10.f; v1[ 0 ].z = -10.f;
//	v1[ 1 ].y = 10.f; v1[ 1 ].x = -10.f; v1[ 1 ].z = +10.f;
//	v1[ 2 ].y = 10.f; v1[ 2 ].x = 10.f; v1[ 2 ].z = -10.f;
//	v1[ 3 ].y = 10.f; v1[ 3 ].x = 10.f; v1[ 3 ].z = +10.f;
//
//
//	triangles[ 0 ].v0 = 0; triangles[ 0 ].v1 = 2; triangles[ 0 ].v2 = 1;
//	triangles[ 1 ].v0 = 1; triangles[ 1 ].v1 = 2; triangles[ 1 ].v2 = 3;
//
//
//	Material	* triangleMesh1_mat = new Material ( Normalize ( RGBColors ( 4 , 3 , 1 ) ) );
//	BRDFModels	* triangleMesh1_brdf = new Lambertian ();
//
//	TriangleMesh * tm = new TriangleMesh (
//		triangleMesh1_brdf ,
//		triangleMesh1_mat ,
//		vertices ,
//		triangles ,
//		2 ,
//		4 );
//	tm->Move ( 0 , 0 , 0 );
////	myScene.AddObject ( tm );
////	tm->PrintInfo ();
//
//
//
//
//
//	Material	* wallBack_mat = new Material ( Normalize ( RGBColors ( 135, 206 ,250 ) ) );
//	BRDFModels	* wallBack_brdf = new Lambertian ();
//
//	Square * wallBack = new Square (
//		wallBack_brdf ,
//		wallBack_mat
//	);
////	wallBack->PrintInfo ();
//	wallBack->initSquareYZ ();
//	wallBack->Scale ( 500 );
//	wallBack->PrintInfo ();
//	wallBack->Move ( -200 , 0 , 0 );
//
//
//
//
//
//	Square * wallLeft = new Square (
//		plane1_brdf ,
//		white_mat
//	);
//	
//	wallLeft->initSquareXZ ();
//	wallLeft->Scale ( 500 );
//	//wallLeft->RotZ ( 45 * MATH_DEG_TO_RAD );
////	wallLeft->PrintInfo ();
//	wallLeft->Move ( 0 , 130 , 0 );
//	wallLeft->PrintInfo ();
//
//
//
//
//	Square * wallRight = new Square (
//		plane2_brdf ,
//		white_mat
//	);
//	wallRight->initSquareXZ ();
//	wallRight->Scale ( 500 );
//	wallRight->Move ( 0 , -130 , 0 );
//
//
//
//
//	Square * wallTop = new Square (
//		plane3_brdf ,
//		plane3_mat
//
//	);
//
//	RGBColors	wallTop_emission = RGBColors ( 500 , 500 , 500 );
//	//
//	wallTop->SetEmissionValue ( wallTop_emission );
////	wallLeft->SetEmissionValue ( wallTop_emission );
//	wallTop->Scale ( 500 );
//	wallTop->Move ( 0 , 0 , 150 );
//
//
//
//	Square * wallBottom = new Square (
//		plane4_brdf ,
//		plane5_mat
//	);
//	wallBottom->Scale ( 500 );
//	wallBottom->Move ( 0 , 0 , -10 );
//
////	wallBottom->Scale ( 15 , 15 , 1 );
////	wallBottom->RotY ( 120 * MATH_DEG_TO_RAD );
////	wallBottom->Move ( 0 , 0 , -0 );
////	wallBottom->SetEmissionValue ( wallTop_emission );
//
//	myScene.AddObject ( wallBack );
//	myScene.AddObject ( wallLeft ); 
//	myScene.AddObject ( wallRight );
//	myScene.AddObject ( wallTop );
//	myScene.AddObject ( wallBottom );
////
////	wallBack->PrintInfo ();
////	wallLeft->PrintInfo ();
////	wallRight->PrintInfo ();
////	wallTop->PrintInfo ();
////	wallBottom->PrintInfo ();
//
//	Cube * cube1 = new Cube (
//		plane5_brdf ,
//		plane1_mat
//	);
//	cube1->RotZ ( 40 * MATH_DEG_TO_RAD );
//	cube1->Scale ( 3 );
//	cube1->Move ( 0 , 0 , -6 );
////	myScene.AddObject ( cube1 );
//	cube1->PrintInfo ();
//
//
//	Cube * cube2 = new Cube (
//		sphere2_brdf ,
//		plane2_mat
//	);
//	cube2->RotZ ( 40 * MATH_DEG_TO_RAD );
//	cube2->RotY ( 45 * MATH_DEG_TO_RAD );
//	cube2->Scale ( 3 );
//	cube2->Move ( 4 , -7 , 1 );
////	myScene.AddObject ( cube2 );
//	cube2->PrintInfo ();
//
//	
////	Mesh * mesh = new Mesh (
////		plane1_brdf ,
////		plane2_mat,
////		"bunny.ply"
////	);
//////	mesh->LoadObjFile ();
//////	mesh->LoadPLYFile ();
//////	mesh->tinyPLY ("bunny.ply");
////	mesh->RotX ( 90 * MATH_DEG_TO_RAD );
////	mesh->RotZ ( 120 * MATH_DEG_TO_RAD );
////	mesh->Scale ( 140 );
////	mesh->Move ( -5 , 25 , -15 );
//////	mesh->PrintInfo ();
////	myScene.AddObject ( mesh );
//
//	Mesh * Bear_s = new Mesh (
//		plane2_brdf ,
//		plane3_mat,
//		"BEAR_stand.ply" 
//	);
//
////	Bear_s->tinyPLY ( "BEAR_stand.ply" );
//	Bear_s->RotX ( 90 * MATH_DEG_TO_RAD );
//	Bear_s->RotZ ( 135 * MATH_DEG_TO_RAD );
//	Bear_s->Scale ( 12 );
//	Bear_s->Move ( 0 , -26, -10 );
////		mesh->PrintInfo ();
//	Material * bear_mat = new Material ( Normalize ( RGBColors ( 128, 42, 42 ) ) );
//
////	Mesh * mesh2 = new Mesh (
////		plane2_brdf ,
////		bear_mat ,
////		"Dragon.ply"
////	);
////
//////	mesh2->tinyPLY ( "Dragon.ply" );
////	mesh2->RotZ ( 135 * MATH_DEG_TO_RAD );
////	//	mesh2->RotZ ( 90 * MATH_DEG_TO_RAD );
////	mesh2->Scale ( 0.7 );
////	mesh2->Move ( 0 , -15 , -10 );
//
//	//		mesh->PrintInfo ();
//	Material * cat_mat = new Material ( Normalize ( RGBColors ( 255, 130, 71 ) ) );
//
//	Mesh * cat = new Mesh (
//		plane2_brdf ,
//		cat_mat ,
//		"cat.ply"
//	);
//
////	cat->tinyPLY ( "cat.ply" );
//	cat->RotX ( 90 * MATH_DEG_TO_RAD );
//	cat->RotZ ( 45 * MATH_DEG_TO_RAD );
//	cat->Scale ( 20 );
//	cat->Move ( 10 , 4 , -5 );
//	Material * tree_mat = new Material ( Normalize ( RGBColors ( 34, 139, 34 ) ) );
//	Mesh * tree = new Mesh (
//		plane2_brdf ,
//		tree_mat,
//		"Tree_small.ply" 
//	);
//
////	tree->tinyPLY ( "Tree_small.ply" );
//	tree->RotX ( 90 * MATH_DEG_TO_RAD );
//	tree->RotZ ( 45 * MATH_DEG_TO_RAD );
//	tree->Scale ( 10 );
//	tree->Move ( 5 , 26 , -9 );
//
////	myScene.AddObject ( mesh );
////	myScene.AddObject ( mesh2 );
//	myScene.AddObject ( Bear_s );
//	myScene.AddObject ( cat );
//	myScene.AddObject ( tree );
//
//	myScene.Commit_Embree_Scene ();
//}


