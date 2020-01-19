#include "Texture.h"


Texture::Texture ()
{
	uv_map_size = 0;
	hasTexture = false;
}


Texture::~Texture ()
{
	delete uv_map;
}

//copy uv coords to local memory
void Texture::InitUVMap ( vector<float>& uv )
{
	uv_map_size = uv.size ();

	uv_map = new float[ uv_map_size ];

	for (long int i = 0; i < uv_map_size; i++)
		uv_map[ i ] = uv[ i ];
	
	hasTexture = true;
}

Spectrum Texture::QueryTextureColor ( unsigned long int primID, float prim_u, float prim_v )
{

	if (!hasTexture)
	{
		Spectrum spec;
		spec.resetOne ();
		return spec;
	}

	float real_u = 0;
	float real_v = 0;

	GetImageUVCoord ( primID , prim_u , prim_v , real_u , real_v );

	Spectrum color = imagePort.GetPixelColor ( real_u , real_v ) ;

	color = color / 255;
	
	return color;
}

void Texture::GetImageUVCoord ( unsigned long int primID, float prim_u , float prim_v , float & img_u , float & img_v )
{
	unsigned long int idx = primID * 6;

	float u1 , v1 , u2 , v2 , u3 , v3;

	u1 = uv_map[ idx ];
	v1 = uv_map[ idx + 1 ];
	u2 = uv_map[ idx + 2 ];
	v2 = uv_map[ idx + 3 ];
	u3 = uv_map[ idx + 4 ];
	v3 = uv_map[ idx + 5 ];

	img_u = ( 1 - prim_u - prim_v ) * u1 + prim_u * u2 + prim_v * u3;
	img_v = ( 1 - prim_u - prim_v ) * v1 + prim_u * v2 + prim_v * v3;
}

void Texture::LoadTextureFile ( const string & filename )
{
	this->filename = filename;
	imagePort.LoadImage ( filename );
}

void Texture::RecordPhoton ( Photon p , unsigned long int primID , float prim_u , float prim_v )
{
	float real_u = 0;
	float real_v = 0;

	GetImageUVCoord ( primID , prim_u , prim_v , real_u , real_v );

	imagePort.AddPhotonToPhotonMap ( p , real_u , real_v );
}

bool Texture::HasTexture ()
{
	return hasTexture;
}
