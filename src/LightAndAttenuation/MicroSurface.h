/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:17:58
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:17:58
 * @Description: All rights reserved
 */
#pragma once
#include "Foundations/Vector.h"
#include "Foundations/RandomNumber.h"

inline int X_plus ( double x )
{
	return ( x > 0 ) ? 1 : 0;
}

class MicroSurface
{
public:
	double alpha_x;
	double alpha_y;

	MicroSurface ();
	~MicroSurface ();

	MicroSurface ( double alp_x, double alp_y )
	{
		alpha_x = alp_x;
		alpha_y = alp_y;
	}

	MicroSurface ( double alp_x )
	{
		alpha_x = alp_x;
		alpha_y = alp_x;
	}

	// wh_local is local normal of micro surface
	// wh_global is normal of MACRO surface
	virtual double D_wh (
		const Vector * wh_local,
		const Vector * wh_global )
	{
		return 0;
	}

	virtual double D_wh (
		const Vector * wh_local,
		const Vector * wh_global,
		double & pdf_wh )
	{
		double D = D_wh( wh_local, wh_global );
		pdf_wh = AbsDot( *wh_local, *wh_global ) * D;

		if ( pdf_wh < eps && pdf_wh > 0 )
		{
			pdf_wh = 0;
		}

		return D;
	}

	// sample visible D_wh
	// overrided by Beckmann_Eric model
	virtual double D_wh (
		const Vector * wi_local,
		const Vector * wh_local,
		const Vector * wh_global,
		double & pdf_wh )
	{
		return D_wh( wh_local, wh_global, pdf_wh );
	}

	virtual double PDF ( const Vector * wh_local, const Vector * wh_global )
	{
		double dot_mn = AbsDot( *wh_local, *wh_global );
		return D_wh( wh_local, wh_global ) * dot_mn;
	}

	virtual Vector sample_wh ()
	{
		return Vector( 0, 0, 0 );
	}

	virtual Vector sample_wh (
		const Vector & wi_global,
		const Vector & wh_global )
	{
		return Vector( 0, 0, 0 );
	}

	virtual double masking (
		const Vector * w,
		const Vector * wh_local,
		const Vector * wh_global )
	{
		return 1;
	}

	virtual double masking (
		const Vector * wi,
		const Vector * wo,
		const Vector * wh_local,
		const Vector * wh_global )
	{
		return masking( wo, wh_local, wh_global );
	}

	virtual double G ( const Vector & wo, const Vector & wi ) const
	{
		return 1;
	}
};

// Beckmann model from Walter's egsr2017 paper
class Beckmann : public MicroSurface
{
public:
	Beckmann ( double alp_x, double alp_y )
		: MicroSurface( alp_x, alp_y )
	{
	}

	Beckmann ( double alp_x )
		: MicroSurface( alp_x )
	{
	}

	double D_wh ( const Vector * wh_local, const Vector * wh_global ) override;

	Vector sample_wh () override;

	Vector sample_wh (
		const Vector & wi_global,
		const Vector & wh_global )
	override;

	double masking (
		const Vector * w,
		const Vector * wh_local,
		const Vector * wh_global )
	override;
};

// GGX model from Walter's egsr2017 paper
// note that GGX model introduce high variance at grazing incident angle
// discussed by Eric Heitz's egsr 2014 paper
class GGX : public MicroSurface
{
public:
	GGX ( double alp_x, double alp_y )
		: MicroSurface( alp_x, alp_y )
	{
	}

	GGX ( double alp_x )
		: MicroSurface( alp_x )
	{
	}

	double D_wh ( const Vector * wh_local, const Vector * wh_global ) override;

	Vector sample_wh () override;

	Vector sample_wh (
		const Vector & wi_global,
		const Vector & wh_global )
	override;

	double masking (
		const Vector * w,
		const Vector * wh_local,
		const Vector * wh_global )
	override;
};

class V_cavity : public MicroSurface
{
public:
	V_cavity ( double alp_x, double alp_y )
		: MicroSurface( alp_x, alp_y )
	{
	}

	V_cavity ( double alp_x )
		: MicroSurface( alp_x )
	{
	}

	double D_wh ( const Vector * wh_local, const Vector * wh_global ) override;

	Vector sample_wh () override;

	Vector sample_wh (
		const Vector & wi_global,
		const Vector & wh_global )
	override;

	double masking (
		const Vector * w,
		const Vector * wh_local,
		const Vector * wh_global )
	override;
};


class Beckmann_Eric : public MicroSurface
{
public:
	Beckmann_Eric ( double alp_x, double alp_y )
		: MicroSurface( alp_x, alp_y )
	{
	}

	Beckmann_Eric ( double alp_x )
		: MicroSurface( alp_x )
	{
	}

	double D_wh ( const Vector * wh_local, const Vector * wh_global ) override;

	Vector sample_wh (
		const Vector & wi_global,
		const Vector & wh_global )
	override;

	double masking (
		const Vector * wi,
		const Vector * wo,
		const Vector * wh_local,
		const Vector * wh_global )
	override;

	static void BeckmannSample11 (
		double cosThetaI,
		double U1,
		double U2,
		double * slope_x,
		double * slope_y );

	static Vector BeckmannSample (
		const Vector & wi,
		double alpha_x,
		double alpha_y,
		double U1,
		double U2 );

	double Lambda ( const Vector & w ) const;

	double D_wh (
		const Vector * wi_local,
		const Vector * wh_local,
		const Vector * wh_global,
		double & pdf_wh )
	override;

	double G1 ( const Vector & w ) const;

	double G (
		const Vector & wo,
		const Vector & wi )
	const;
};

