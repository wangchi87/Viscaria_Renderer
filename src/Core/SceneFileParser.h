/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:16:26
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:16:26
 * @Description: All rights reserved
 */
#pragma once
#include "Global_Settings.h"
#include "Scene.h"
#include "LightAndAttenuation/MicroFacetModel.h"


class SceneFileParser
{
public:

	bool polarisation_support;
	size_t spectrum_num;

	char output_filename[ 50 ];

	char integratorName[ 30 ];
	size_t path_max_depth = 3;
	size_t lighmap_sample_count = 10000;

	size_t image_width = 0;
	size_t image_height = 0;
	size_t image_sample_per_pixel = 0;

	Pnt3D cameraPos;
	Pnt3D cameraLookAt;
	Vector camera_up;
	double camera_fov = M_PI / 8;

	SceneFileParser ();
	~SceneFileParser ();

	void ParseSceneFile (
		const char * sceneFile,
		Scene & myScene );
};

