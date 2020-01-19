/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:17:26
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:17:26
 * @Description: All rights reserved
 */
#pragma once
#include<string>
#include "LightAndAttenuation/Spectrum.h"
#include "FreeImageInterface.h"
#include "ImageRawData.h"
#include "LightAndAttenuation/Photon.h"
using namespace std;


class ImagePort
{
	string filename;

	size_t width;
	size_t height;

	FreeImageInterface imgRead;

	ImageRawData *lightMap;

public:
	ImagePort ();
	~ImagePort ();

	void LoadImage ( const std::string & filename );

	void AllocateLightMap ( int width , int height );

	void AddPhotonToPhotonMap ( Photon p , float u , float v );

	void ConvertUVtoPixelPos ( float u , float v , int & x , int & y );

	Spectrum GetPixelColor ( float u , float v );

	void SaveLightMap ();
};

