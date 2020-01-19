#include "ImageRawData.h"


ImageRawData::ImageRawData ( size_t w, size_t h )
{
	width = w;
	height = h;

	rawData = new Spectrum*[width];

	for ( size_t x = 0; x < width; x++ )
	{
		rawData[ x ] = new Spectrum[height];
	}
}

ImageRawData ImageRawData::operator+ ( const ImageRawData image1 )
{
	if ( width != image1.width || height != image1.height )
	{
		printf( "image size does not match! \n" );
		return ( *this );
	}

	for ( size_t i = 0; i < width; i++ )
		for ( size_t j = 0; j < height; j++ )
		{
			rawData[ i ][ j ] = rawData[ i ][ j ] + image1.rawData[ i ][ j ];
		}

	return ( *this );
}

Spectrum & ImageRawData::PixelValue ( size_t w , size_t h )
{
	// TODO: 在此处插入 return 语句
	return rawData[ w ][ h ];
}

size_t ImageRawData::GetImageWidth ()
{
	return width;
}

size_t ImageRawData::GetImageHeight ()
{
	return height;
}

void ImageRawData::SaveAsHDR ( const string aFilename ) const
{
	std::ofstream hdr;
	try
	{
		hdr.open( aFilename, std::ios::binary | std::ofstream::out );
	}
	catch ( std::exception e )
	{
		printf( "has exception in opening hdr file! \n" );
		e.what();
	}

	if ( !hdr )
		printf( "failed to open hdf file" );


	hdr << "#?RADIANCE" << '\n';
	hdr << "# Chi_Render" << '\n';
	hdr << "FORMAT=32-bit_rle_rgbe" << '\n' << '\n';
	hdr << "-Y " << height << " +X " << width << '\n';

	for ( int y = 0; y < height; y++ )
	{
		for ( int x = 0; x < width; x++ )
		{
			typedef unsigned char byte;
			byte rgbe[ 4 ] = { 0,0,0,0 };

			const Spectrum & rgbF = rawData[ x ][ y ];
			float v = std::max( rgbF.spec[ 0 ], std::max( rgbF.spec[ 1 ], rgbF.spec[ 2 ] ) );

			if ( v >= 1e-32f )
			{
				int e;
				v = float( frexp( v, &e ) * 256.f / v );
				rgbe[ 0 ] = byte( rgbF.spec[ 0 ] * v );
				rgbe[ 1 ] = byte( rgbF.spec[ 1 ] * v );
				rgbe[ 2 ] = byte( rgbF.spec[ 2 ] * v );
				rgbe[ 3 ] = byte( e + 128 );
			}

			hdr.write( ( char* ) &rgbe[ 0 ], 4 );
		}
	}

	hdr.close();
}

void ImageRawData::SaveAsPPM ( const string aFilename ) const
{
	FILE * f;

	errno_t err;

	// Open for read (will fail if file "crt_fopen_s.c" does not exist)
	if ( ( err = fopen_s( &f, aFilename.c_str(), "w" ) ) != 0 )
		printf( "The PPM file was not opened\n" );

	fprintf( f, "P3\n%llu %llu\n%d\n ", width, height, 255 );

	for ( int row = 0; row < height; row++ )
	{
		for ( int col = 0; col < width; col++ )
		{
			fprintf( f, "%d %d %d ",
			         MIN ( ( int ) rawData[ col ][ row ].spec[ 0 ] , 255 ),
			         MIN ( ( int ) rawData[ col ][ row ].spec[ 1 ] , 255 ),
			         MIN ( ( int ) rawData[ col ][ row ].spec[ 2 ] , 255 ) );
		}
		fprintf( f, "\n" );
	}
	fclose( f );
}

void ImageRawData::SaveImage ( const string aFilename, bool need_XYZ_to_RGB , bool need_tonemap, bool need_gamma) const
{
	FreeImageInterface output;

	output.AllocateMemory ( width , height );

	for(size_t w = 0; w<width; w++ )
		for (size_t h = 0; h < height; h++)
		{
			Spectrum pixel = rawData[ w ][ h ];

			if (need_XYZ_to_RGB)
				pixel = XYZ_to_RGB ( pixel );

			if (need_tonemap)
				pixel = tonemapACES ( pixel ) * 255;

			output.WritePixelValue ( w , h , pixel[0] , pixel[ 1 ], pixel[ 2 ] );
		}

	if(need_gamma)
		output.ApplyGammaCorrection ( 2.2 );

	output.SaveImage ( aFilename );
}

void ImageRawData::PixelNormalise ()
{
	double max_sample = 0;
	for (size_t w = 0; w<width; w++)
		for (size_t h = 0; h < height; h++)
		{
			if (rawData[ w ][ h ][ 1 ] > max_sample)
				max_sample = rawData[ w ][ h ][ 1 ];
		}

	for (size_t w = 0; w<width; w++)
		for (size_t h = 0; h < height; h++)
		{
			rawData[ w ][ h ] *= 1 / max_sample;
		}
}

ImageRawData::ImageRawData ()
{
}


ImageRawData::~ImageRawData ()
{
	//	free ( rawData );
}

ImageRawData image_fireflies_filter ( ImageRawData * image_array, size_t array_num )
{
	size_t width = image_array[ 0 ].GetImageWidth();
	size_t height = image_array[ 0 ].GetImageHeight();

	ImageRawData average_data( width, height );
	ImageRawData output_data( width, height );

	//	for (int i = 0; i < width; i++)
	//	{
	//		for (int j = 0; j < height; j++)
	//		{
	//			for (int k = 0; k < array_num; k++)
	//				average_data.rawData[ i ][ j ] +=
	//					image_array[ k ].rawData[ i ][ j ] / array_num;
	//		}
	//	}
	//
	//	double * weight;
	//
	//	weight = static_cast<double * >( malloc ( sizeof ( double ) * array_num ) );


	//	for (int i = 0; i < width; i++)
	//	{
	//		for (int j = 0; j < height; j++)
	//		{
	//			double total = 0;
	//			for (int k = 0; k < array_num; k++)
	//			{
	//				weight[ i ] = ( average_data.rawData[ i ][ j ] - image_array[ k ].rawData[ i ][ j ] ).Length ();
	//				total += weight[ i ];
	//			}
	//
	//			for (int k = 0; k < array_num; k++)
	//			{
	//				weight[ i ] /= total;
	//				//				output_data.rawData[ i ][ j ] +=
	//				//					image_array[ k ].rawData[ i ][ j ] * weight[ i ];
	//				output_data.rawData[ i ][ j ] += 
	//					image_array[ k ].rawData[ i ][ j ].Length() > average_data.rawData[ i ][ j ].Length() 
	//						? average_data.rawData[ i ][ j ]
	//						: image_array[ k ].rawData[ i ][ j ];
	//			}
	//		}
	//	}

	return output_data;
}

