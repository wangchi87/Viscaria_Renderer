#include "SceneFileParser.h"


inline void GetGoldIndex ( Spectrum & n, Spectrum & k )
{
	n.spec[ 0 ] = 1.5379;
	k.spec[ 0 ] = 1.8226;
	n.spec[ 1 ] = 0.38089;
	k.spec[ 1 ] = 2.4347;
	n.spec[ 2 ] = 0.17209;
	k.spec[ 2 ] = 3.6818;
}

inline void GetAluminumIndex ( Spectrum & n, Spectrum & k )
{
	n.spec[ 0 ] = 0.41288;
	k.spec[ 0 ] = 4.4691;
	n.spec[ 1 ] = 0.96526;
	k.spec[ 1 ] = 6.3996;
	n.spec[ 2 ] = 1.4941;
	k.spec[ 2 ] = 7.7905;
}

inline void ChooseMaterial (
	char * option,
	Spectrum & n,
	Spectrum & k )
{
	if ( strcmp( option, "GOLD" ) == 0 )
	{
		GetGoldIndex( n, k );
	}
	if ( strcmp( option, "ALU" ) == 0 )
	{
		GetAluminumIndex( n, k );
	}
}

SceneFileParser::SceneFileParser ()
{
}


SceneFileParser::~SceneFileParser ()
{
}

void SceneFileParser::ParseSceneFile (
	const char * sceneFile,
	Scene & myScene )
{
	ifstream file;
	file.open( sceneFile, ios::in );

	if ( !file )
	{
		printf( "Failed to read Scene File! \n" );
		return;
	}

	char option[ 50 ];

	size_t obj_num = 0;

	while ( !file.eof() )
	{
		file >> option;
		if ( strcmp( option, "OUTPUT_FILE_NAME" ) == 0 )
		{
			file >> output_filename;
		}

		if ( strcmp( option, "IMAGESIZE" ) == 0 )
		{
			file >> image_width;
			file >> image_height;
		}

		if ( strcmp( option, "SAMPLE_PER_PIXEL" ) == 0 )
		{
			file >> image_sample_per_pixel;
		}

		if ( strcmp( option, "INTEGRATOR" ) == 0 )
		{
			file >> integratorName;
			if ( strcmp( integratorName, "PATH_TRACER_MIS" ) == 0 )
			{
				file >> path_max_depth;
			}
			if ( strcmp( integratorName, "PATH_TRACER" ) == 0 )
			{
				file >> path_max_depth;
			}
			if ( strcmp( integratorName, "DIRECT_ILLUMINATION" ) == 0 )
			{
			}
		}

		if ( strcmp( option, "CAMERA_POS_LOOKAT_AND_UP" ) == 0 )
		{
			file >> cameraPos.x;
			file >> cameraPos.y;
			file >> cameraPos.z;

			file >> cameraLookAt.x;
			file >> cameraLookAt.y;
			file >> cameraLookAt.z;

			file >> camera_up.x;
			file >> camera_up.y;
			file >> camera_up.z;
		}

		if ( strcmp( option, "CAMERA_FOV" ) == 0 )
		{
			file >> camera_fov;
			camera_fov = M_PI / camera_fov;
		}

		if ( strcmp( option, "OBJECT_NUMBER" ) == 0 )
		{
			file >> obj_num;
		}

		if ( strcmp( option, "BEGIN_OBJ" ) == 0 )
		{
			Objects * obj = nullptr;

			Material * mat = nullptr;
			BRDFModels * brdf = nullptr;
			Spectrum * emission = nullptr;

			file >> option;

			while ( ! strcmp( option, "END_OBJ" ) == 0 )
			{
				if ( strcmp( option, "MATERTIAL" ) == 0 )
				{
					file >> option;

					if ( strcmp( option, "LAMBERTIAN" ) == 0 )
					{
						double albedo;
						file >> albedo;
						brdf = new Lambertian( albedo );
					}

					if ( strcmp( option, "SPECULAR" ) == 0 )
					{
						Spectrum r_n, r_k;
						file >> option;
						ChooseMaterial( option, r_n, r_k );
						brdf = new SpecularReflection( r_n, r_k );
					}

					if ( strcmp( option, "MEASURED_BRDF" ) == 0 )
					{
						double roughness = 0.2;
						file >> roughness;

						char brdf_file[ 50 ];
						file >> brdf_file;

						brdf = new MeasuredBRDF( brdf_file, roughness );
					}

					if ( strcmp( option, "TS" ) == 0 )
					{
						bool is_GGX = false;
						char model[ 50 ];
						file >> model;
						if ( strcmp( model, "GGX" ) == 0 )
						{
							is_GGX = true;
						}
						double roughness = 0.1;
						file >> roughness;
						Spectrum r_n, r_k;
						file >> option;
						ChooseMaterial( option, r_n, r_k );
						brdf = new TorranceSparrow( is_GGX, roughness, r_n, r_k );
					}
				}

				if ( strcmp( option, "COLOR" ) == 0 )
				{
					double color[3];
					file >> color[ 0 ];
					file >> color[ 1 ];
					file >> color[ 2 ];

					// normalise reflected color into [0, 1]
					color[ 0 ] = color[ 0 ] / 255;
					color[ 1 ] = color[ 1 ] / 255;
					color[ 2 ] = color[ 2 ] / 255;

					Spectrum ref = Spectrum( color );
					ref.Normalise();

					mat = new Material( ref );
					//					brdf->initRefColors ( ref );
				}

				if ( strcmp( option, "SHAPE" ) == 0 )
				{
					if ( brdf == NULL || mat == NULL )
					{
						printf( "Define MATERIAL and BRDF REFLECTANCE before SHAPE" );
						break;
					}
					file >> option;
					if ( strcmp( option, "SQUARE" ) == 0 )
					{
						obj = new Square(
							brdf,
							mat
						);
					}

					if ( strcmp( option, "CUBE" ) == 0 )
					{
						obj = new Cube(
							brdf,
							mat
						);
					}

					if ( strcmp( option, "SPHERE" ) == 0 )
					{
						obj = new Sphere(
							brdf,
							mat
						);
					}

					if ( strcmp( option, "PLY" ) == 0 )
					{
						char ply_file[50];
						file >> ply_file;

						obj = new Mesh(
							brdf,
							mat,
							ply_file
						);
					}
				}

				if ( strcmp( option, "LIGHTSOURCE" ) == 0 )
				{
					double color[ 3 ];
					file >> color[ 0 ];
					file >> color[ 1 ];
					file >> color[ 2 ];

					emission = new Spectrum( color );
					obj->SetEmissionValue( *emission );
				}

				if ( strcmp( option, "ROTX" ) == 0 )
				{
					double angle;
					file >> angle;
					angle = angle * MATH_DEG_TO_RAD;
					obj->RotX( angle );
				}

				if ( strcmp( option, "ROTY" ) == 0 )
				{
					double angle;
					file >> angle;
					angle = angle * MATH_DEG_TO_RAD;
					obj->RotY( angle );
				}

				if ( strcmp( option, "ROTZ" ) == 0 )
				{
					double angle;
					file >> angle;
					angle = angle * MATH_DEG_TO_RAD;
					obj->RotZ( angle );
				}

				if ( strcmp( option, "SCALE" ) == 0 )
				{
					double x, y, z;
					file >> x;
					file >> y;
					file >> z;
					obj->Scale( x, y, z );
				}

				if ( strcmp( option, "MOVE" ) == 0 )
				{
					double x, y, z;
					file >> x;
					file >> y;
					file >> z;
					obj->Move( x, y, z );
				}

				file >> option;
			}
			option[ 0 ] = '\0';
			myScene.AddObject( obj );
		}

		if ( strcmp( option, "SPECTRUM_NUMBER" ) == 0 )
		{
			//			file >> SPECTRUM_NUM;
		}

		if ( strcmp( option, "POLARISATION_MODE" ) == 0 )
		{
			POLARISATION_MODE = true;
			printf( "Polarisation Enabled \n" );
		}

		if ( strcmp( option, "LIGHTMAP_SAMPLE_NUMBER" ) == 0 )
		{
			file >> lighmap_sample_count;
		}

		if ( strcmp( option, "OUTPUT_FILE_NAME" ) == 0 )
		{
		}
	}

	printf( "Spectrum Number %llu \n", SPECTRUM_NUM );
}

