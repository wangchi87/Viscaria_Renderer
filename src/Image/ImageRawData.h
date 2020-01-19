/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:17:30
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:17:30
 * @Description: All rights reserved
 */
#include "Foundations/Vector.h"
#include "LightAndAttenuation/Spectrum.h"
#include "FreeImageInterface.h"
#include <fstream>
#include <algorithm>
#pragma once


class ImageRawData
{

	size_t width;
	size_t height;

	Spectrum ** rawData;

public:
	ImageRawData (
		size_t w,
		size_t h
	);

	ImageRawData operator + ( const ImageRawData image1 );

	Spectrum & PixelValue ( size_t w , size_t h );

	size_t GetImageWidth ();
	size_t GetImageHeight();


	// Saving HDR
	void SaveAsHDR ( const string aFilename ) const;

	void SaveAsPPM ( const string aFilename ) const;

	void SaveImage( const string aFilename, bool need_XYZ_to_RGB = true , bool need_tonemap = true , bool need_gamma = false ) const;

	void PixelNormalise ();

	ImageRawData ();
	~ImageRawData ();
};


inline ImageRawData image_fireflies_filter (
	ImageRawData * image_array,
	size_t array_num );


inline double tonemap ( const double x )
{
	const double a = 2.51;
	const double b = 0.03;
	const double c = 2.43;
	const double d = 0.59;
	const double e = 0.14;

	return ( x * ( x * a + b ) ) / ( x * ( x * c + d ) + e );
}

inline Spectrum XYZ_to_RGB ( const Spectrum& x )
{
	double spec[ 3 ];

	spec[ 0 ] = x.spec[ 0 ];
	spec[ 1 ] = x.spec[ 1 ];
	spec[ 2 ] = x.spec[ 2 ];

	double out[ 3 ];

	out[ 0 ] = spec[ 0 ] * 3.2404542f - spec[ 1 ] * 1.537150f - spec[ 2 ] * 0.498535f;
	out[ 1 ] = -spec[ 0 ] * 0.969256f + spec[ 1 ] * 1.8760108f + spec[ 2 ] * 0.041556f;
	out[ 2 ] = spec[ 0 ] * 0.055648f - spec[ 1 ] * 0.204043f + spec[ 2 ] * 1.0572252f;

	return Spectrum ( out );
}

inline Spectrum tonemapACES ( const Spectrum& x )
{
	double spec[ 3 ];

	spec[ 0 ] = x.spec[ 0 ];
	spec[ 1 ] = x.spec[ 1 ];
	spec[ 2 ] = x.spec[ 2 ];

	spec[ 0 ] = tonemap ( spec[ 0 ] );
	spec[ 1 ] = tonemap ( spec[ 1 ] );
	spec[ 2 ] = tonemap ( spec[ 2 ] );

	return Spectrum ( spec );
}