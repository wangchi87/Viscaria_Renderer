/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:18:34
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:18:34
 * @Description: All rights reserved
 */
#include "Core/Ray.h"
#include "Core/Scene.h"
#include "LightAndAttenuation/Spectrum.h"
#include "Shapes/geometry.h"
#include "LightAndAttenuation/Photon.h"


class PhotonTracer
{
public:
	PhotonTracer ();
	~PhotonTracer ();

	void Tracer ( const Scene &myScene );
};