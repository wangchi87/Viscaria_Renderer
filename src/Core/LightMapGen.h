/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:15:24
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:15:24
 * @Description: All rights reserved
 */
#pragma once
#include "Core/Scene.h"
#include "LightTransportationSim/PhotonTracer.h"
#include "SceneFileParser.h"
#include <stdio.h>
#include <omp.h>

class LightMapGen
{
public:
	Scene myScene;

	size_t photonNum;
	
	LightMapGen ();
	~LightMapGen ();

	void InitialiseGenerator ( const SceneFileParser * sfp);

	void Run ();

	void SetPhotonNum ( size_t num );

private:
	PhotonTracer tracer;
};

