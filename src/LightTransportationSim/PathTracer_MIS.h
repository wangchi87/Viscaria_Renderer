/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:18:32
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:18:32
 * @Description: All rights reserved
 */
#include "LightTransSim.h"

#pragma once


class PathTracer_MIS : public PathTracer
{
public:
	PathTracer_MIS ();
	~PathTracer_MIS ();

	PathTracer_MIS (
		size_t maxdep
	) : PathTracer( maxdep )
	{
	}

	/**
	* \brief				Path tracing with multiple importance sampling
	* \param thisScene		scene
	* \param ray			camera ray
	* \return
	*/
	Spectrum Integration (
		const Scene * thisScene,
		const Ray & ray
	) override;
};

