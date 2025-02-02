// HosekWilkieSkyGen.cpp: 定义控制台应用程序的入口点。
//

#include <Image\ImageRawData.h>
#include <Foundations\Vector.h>
#include <LightAndAttenuation\Spectrum.h>

extern "C" {
#include "ArHosekSkyModel.h"
}

//#define RGB_MODE


#define M_PI 3.1415
#define M_PI_2 M_PI / 2
#define DEG_TO_RAD M_PI/180
#define RAD_TO_DEG 180/M_PI

inline float angleBetween ( float thetav , float phiv , float theta , float phi ) 
{
	float cosGamma = sinf ( thetav ) * sinf ( theta ) * cosf ( phi - phiv ) + cosf ( thetav ) * cosf ( theta );
	return acosf ( cosGamma );
}


inline float clamp ( float value , float min , float max )
{
	if (value < min)
		return min;
	if (value > max)
		return max;

	return value;
}


inline float max ( float a , float b )
{
	if (a > b)
		return a;
	else
		return b;
}


static float  g_turbidity = 4.0f;          // between 1 and 32
static float  g_elevation = 0.785398f;     // sun elevation in radians (45 degrees)
static float  g_azimuth = 0.0f;          // sun azimuth in radians (0.0 degrees)
static bool   g_normalize = true;
static bool   g_tonemap = true;
static bool   g_gammaCorrect = true;

static Vector g_groundAlbedo = Vector ( 0.9 , 0.9 , 0.9 );  // ground albedo, sRGB


int main()
{

	float solarElevation = clamp ( g_elevation , 0.0f , float ( M_PI_2 ) );
	float sunTheta = float ( M_PI_2 - solarElevation );
	float sunPhi = 0.0f;

#ifdef RGB_MODE
	ArHosekSkyModelState* skyState[ 3 ] = {
		arhosek_rgb_skymodelstate_alloc_init ( g_turbidity, g_groundAlbedo.x, solarElevation ),
		arhosek_rgb_skymodelstate_alloc_init ( g_turbidity, g_groundAlbedo.y, solarElevation ),
		arhosek_rgb_skymodelstate_alloc_init ( g_turbidity, g_groundAlbedo.z, solarElevation )
	};
#endif // RGB_MODE

#ifndef RGB_MODE
	ArHosekSkyModelState* skyState[ 3 ] = {
		arhosek_xyz_skymodelstate_alloc_init ( g_turbidity, g_groundAlbedo.x, solarElevation ),
		arhosek_xyz_skymodelstate_alloc_init ( g_turbidity, g_groundAlbedo.y, solarElevation ),
		arhosek_xyz_skymodelstate_alloc_init ( g_turbidity, g_groundAlbedo.z, solarElevation )
};
#endif // !RGB_MODE


	size_t width = 1024;
	size_t height = width/2;

	ImageRawData myImage = ImageRawData ( width , height );

	float maxSample = 0.00001f;

	for (size_t y = 0; y < height; y++)
	{
		float v = ( y + 0.5f ) / height;
		float theta = float ( M_PI * v );
		if (theta > M_PI_2)
			continue;
		
		for (size_t x = 0; x < width; x++)
		{
			float u = ( x + 0.5f ) / width;
			float phi = float ( -2.0 * M_PI * u + M_PI + g_azimuth );

			float gamma = angleBetween ( theta , phi , sunTheta , sunPhi );

			double color[ 3 ];

			color[ 0 ] = arhosek_tristim_skymodel_radiance ( skyState[ 0 ] , theta , gamma , 0 );
			color[ 1 ] = arhosek_tristim_skymodel_radiance ( skyState[ 0 ] , theta , gamma , 1 );
			color[ 2 ] = arhosek_tristim_skymodel_radiance ( skyState[ 0 ] , theta , gamma , 2 );

			Spectrum skyColor(color);

			maxSample = max ( maxSample , skyColor[1]);

			myImage.PixelValue ( x , y ) = ( skyColor );
		}
	}

	float hdrScale = 1.0f / ( g_normalize ? maxSample : maxSample / 16.0f );

	if (g_normalize)
		maxSample /= float ( 4.0 * M_PI / 683.0 );


	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			myImage.PixelValue ( x , y ) *= hdrScale;
		}
	}

	arhosekskymodelstate_free ( skyState[ 0 ] );
	arhosekskymodelstate_free ( skyState[ 1 ] );
	arhosekskymodelstate_free ( skyState[ 2 ] );

	string filename = "test";

#ifdef RGB_MODE
	myImage.SaveImage ( filename + ".png" , true );
	myImage.SaveAsHDR ( filename + ".hdr" );
#endif // RGB_MODE

#ifndef RGB_MODE
	myImage.SaveImage ( filename + "_xyz.png" , true, true , true);
	myImage.SaveAsHDR ( filename + "_xyz.hdr" );
#endif // RGB_MODE

	printf ( "Information\n" );
	printf ( "    Max radiance:    %.2f W/(m^2.sr.nm)\n" , maxSample );
	printf ( "    Max luminance:   %.2f nt\n" , maxSample * 683.0f );
	printf ( "    Max illuminance: %.2f lx\n" , maxSample * 683.0f * 2.0 * M_PI );
	printf ( "\n" );

	printf ( "Rendering parameters\n" );
	printf ( "    Luminance multiplier: %.2f\n" , ( 1.0f / hdrScale ) * 683.0f );
	printf ( "\n" );

    return 0;
}

