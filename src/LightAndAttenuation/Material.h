/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:17:50
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:17:50
 * @Description: All rights reserved
 */
#include "Foundations/Vector.h"
#include "LightAndAttenuation/Spectrum.h"
#pragma once

enum
{
	// object property label
	TRANSPARENT = 101,
	NON_TRANSPARENT = 100
};

class Material
{
public:
	Spectrum color;
	double n;
	double k;

	size_t transparentObject;

	Material ( const Spectrum & cl )
	{
		//		color = cl * 2;
		//
		//		color.print ( );
		//		color *= 3;

		color.Init_Spectrum( cl );

		transparentObject = NON_TRANSPARENT;
	}

	Material ( const Spectrum & cl, double nn, double kk )
	{
		color = cl;
		n = nn;
		k = kk;
		transparentObject = NON_TRANSPARENT;
	}

	void setObjectTransparent ()
	{
		transparentObject = TRANSPARENT;
	}

	Material ( void );
	~Material ( void );
};

