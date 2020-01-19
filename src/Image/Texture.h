/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:17:33
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:17:33
 * @Description: All rights reserved
 */
#pragma once
#include"ImagePort.h"
#include <vector>
#include <array>
using namespace std;

class Texture
{
	string filename;
	float * uv_map;
	unsigned long int uv_map_size;
	
	bool hasTexture;

public:
	ImagePort imagePort;

	Texture ();
	~Texture ();

	void InitUVMap (vector<float>& uv);

	Spectrum QueryTextureColor( unsigned long int primID, float prim_u , float prim_v );

	void GetImageUVCoord ( unsigned long int primID, float prim_u , float prim_v, float & img_u, float & img_v );

	void LoadTextureFile ( const string & filename );

	void RecordPhoton ( Photon p , unsigned long int primID , float prim_u , float prim_v );

	bool HasTexture ();
};

