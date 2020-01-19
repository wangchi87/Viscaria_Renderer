/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:18:29
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:18:29
 * @Description: All rights reserved
 */
#pragma once

#include "BasicFunctionsforLightTransportation.h"


class LightTransSim
{
public:

	LightTransSim ();
	virtual ~LightTransSim ();

	virtual Spectrum Integration(
		const Scene* thisScene,
		const Ray& ray);;
};


class PathTracer : public LightTransSim
{
public:
	size_t maxDepth;

	PathTracer ();

	PathTracer (
		size_t maxdep
	)
	{
		maxDepth = maxdep;
	}

	Spectrum Integration (
		const Scene * thisScene,
		const Ray & ray,
		size_t currentDepth
	) const;

	// with direct illumination
	// this one actually does not do the right things on DI with MIS
	// left only for reference
	Spectrum PathTracer::IntegrationWithDI (
		const Scene * thisScene,
		const Ray & ray,
		size_t currentDepth
	) const;

	/**
	 * \brief				Path tracing
	 * \param thisScene		scene
	 * \param ray			camera ray 
	 * \return 
	 */
	Spectrum Integration (
		const Scene * thisScene,
		const Ray & ray
	) override;
};

