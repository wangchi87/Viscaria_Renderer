/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:16:20
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:16:20
 * @Description: All rights reserved
 */
#include "Core/Scene.h"
#include "Camera/Camera.h"
#include "Image/ImageRawData.h"
#include "LightTransportationSim/integrator.h"
#include "Shapes/geometry.h"
#include "LightAndAttenuation/MicroFacetModel.h"
#include "SceneFileParser.h"
#include <stdio.h>
#include <omp.h>

#pragma once


class RenderRunner
{
public:
	Scene myScene;

	RenderRunner ();
	~RenderRunner ();

	void InitialiseRenderer (
		const SceneFileParser * sfp
	);

	void Run ();

private:

	size_t width;
	size_t height;
	size_t samplePerPixel;

	Camera * myCamera;
	ImageRawData myImage;

	ImageRawData * thread_image;
	size_t thread_num;

	LightTransSim * integrator;

	string output_filename;

	void MergeThreadImageBuffer ();

	void InitialiseImageAndCamera (
		size_t width_t,
		size_t height_t,
		size_t sample,
		double fovx,
		double fovy,
		Pnt3D eyePoint,
		Pnt3D imagePoint,
		Vector upDirection
	);

	void InitialiseThreadData ();
};

