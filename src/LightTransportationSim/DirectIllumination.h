/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:18:23
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:18:23
 * @Description: All rights reserved
 */
#include "LightTransSim.h"

#pragma once

class DirectIllumination : public LightTransSim
{
public:
	DirectIllumination ()
	{
	};

	Spectrum Integration (
		const Scene * thisScene,
		const Ray & ray
	) override;
};


inline void EstimateDirectIllumination (
	const Scene * thisScene,
	const Intersection * intersectionInfo,
	const Ray & ray,
	Spectrum & intensity
)
{
	intensity += EstimateLightSampling( thisScene, intersectionInfo, ray );

	intensity += EstimateBRDFSampling( thisScene, intersectionInfo, ray );
}

