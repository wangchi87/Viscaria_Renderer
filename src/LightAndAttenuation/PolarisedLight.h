/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:18:03
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:18:03
 * @Description: All rights reserved
 */
#pragma once
#include "Foundations/Vector.h"
#include "Polarization.h"


class PolarisedLight
{
	StokesVector sv;
	ReferenceFrame ref_frame;
public:
	PolarisedLight ();

	PolarisedLight ( const ReferenceFrame & rf );

	PolarisedLight (
		const StokesVector & sv_vector,
		const ReferenceFrame & rf );

	PolarisedLight (
		const StokesVector & sv_vector,
		const Vector & dir,
		const Vector & n );

	void Set_StokesVector ( const StokesVector & sv_vector );

	void Set_Reference_Frame ( const ReferenceFrame & rf );

	void Set_Reference_Frame ( const Vector & dir, const Vector & n );

	ReferenceFrame get_ReferenceFrame () const;

	StokesVector get_StokesVector () const;

	double get_intensity () const;

	void Print ();

	PolarisedLight operator += ( const PolarisedLight & pl );

	PolarisedLight operator + ( const PolarisedLight & pl );

	PolarisedLight operator = ( const PolarisedLight & pl );

	PolarisedLight operator *= ( const double d );

	void Align_with_New_ReferenceFrame ( const ReferenceFrame & ref );
};

