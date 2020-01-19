#include "ImagePort.h"


ImagePort::ImagePort ()
{
	
}


ImagePort::~ImagePort ()
{
	/*for (size_t x = 0; x < width; x++)
	{
		delete[] pixelValue[x];
	}
	delete [] pixelValue;*/
}

void ImagePort::LoadImage ( const std::string & filename )
{
	this->filename = filename;
	imgRead.LoadImage ( filename );
	width = imgRead.GetWidth ();
	height = imgRead.GetHeight ();

	AllocateLightMap ( width , height );
}

void ImagePort::AllocateLightMap ( int width , int height )
{
	lightMap = new ImageRawData (
		width ,
		height
	);
}

void ImagePort::AddPhotonToPhotonMap ( Photon p, float u , float v )
{
	int x , y;
	ConvertUVtoPixelPos ( u , v , x , y );

	double photon[ 3 ];
	photon[ 0 ] = ( double ) p.energy_r;
	photon[ 1 ] = ( double ) p.energy_g;
	photon[ 2 ] = ( double ) p.energy_b;

	Spectrum pixelValue ( photon );

	lightMap->PixelValue( x ,height - 1 -y ) += pixelValue;
}

void ImagePort::ConvertUVtoPixelPos ( float u , float v , int & x , int & y )
{
	x = ( int ) ( u * width );
	y = ( int ) ( v * height);
}


Spectrum ImagePort::GetPixelColor ( float u , float v )
{
	double color[ 3 ];

	int x , y;
	ConvertUVtoPixelPos ( u , v , x , y );

	imgRead.GetPixelValue ( x , y , color[ 0 ] , color[ 1 ] , color[ 2 ] );

	return Spectrum ( color );
}

void ImagePort::SaveLightMap ()
{
	int dotpos = filename.find ( "." );
	string prefix;

	prefix = filename.substr ( 0 , dotpos );
	prefix = "lightmap_" + prefix;

	string jpgName = prefix + ".jpg";
	lightMap->SaveImage ( jpgName.c_str () , false, false, false );

	string hdrName = prefix + ".hdr";
	lightMap->SaveAsHDR ( hdrName.c_str() );
}








