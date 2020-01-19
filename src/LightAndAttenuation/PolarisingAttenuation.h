/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:18:06
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:18:06
 * @Description: All rights reserved
 */
#pragma once
#include "Polarization.h"
#include "PolarisedLight.h"
#include "Physics/Fresnel.h"

class PolarisingAttenuation
{
public:
	MuellerMatrix mm;
	ReferenceFrame entry_rf;
	ReferenceFrame exit_rf;

	PolarisingAttenuation ();
	~PolarisingAttenuation ();

	PolarisingAttenuation (
		const MuellerMatrix & mm_matrix,
		const ReferenceFrame & rf_in,
		const ReferenceFrame & rf_out
	);

	PolarisedLight operator * ( const PolarisedLight & pl_in );

	PolarisingAttenuation operator * ( const double d );

	PolarisingAttenuation operator = ( const PolarisingAttenuation & pa );

	PolarisingAttenuation operator *= ( const PolarisingAttenuation & atten_input );

	PolarisingAttenuation operator * ( const PolarisingAttenuation & atten_input );

	void Set_Mueller_Matrix ( const MuellerMatrix & mm_matrix );

	void Set_Reference_Frame (
		const ReferenceFrame & rf_in,
		const ReferenceFrame & rf_out );

	void InitFresnelPolarisingAttenuation (
		const double & A,
		const double & B,
		const double & C,
		const double & S,
		const Vector & in_dir,
		const Vector & out_dir,
		const Vector & surface_normal );

	bool isValid ();
};

inline int sign_fun ( double x )
{
	if ( x > 0 )
		return 1;
	return 0;
}

inline void Polarising_Fresnel_Reflection_Attenuation (
	const Vector & in_dir,
	const Vector & out_dir,
	const Vector & surface_normal,
	double n1,
	double k1,
	double n2,
	double k2,
	PolarisingAttenuation & attenuation
)
{
	Vector half_vector = out_dir - in_dir;
	half_vector = Normalize( half_vector );

	double theta_fresnel = acos( AbsDot( out_dir, half_vector ) );

	// calculate Mueller matrix for Fresnel reflection
	double rs;
	double rp;
	double phaseS;
	double phaseP;
	double Fs;
	double Fp;

	Fresnel_Reflection(
		theta_fresnel,
		n1, k1,
		n2, k2,
		rs, rp,
		phaseS,
		phaseP,
		Fs,
		Fp
	);

	double A, B, C, S;
	A = ( Fs + Fp ) / 2;
	B = ( Fs - Fp ) / 2;
	C = cos( phaseS - phaseP ) * sqrt( Fs * Fp );
	S = sin( phaseS - phaseP ) * sqrt( Fs * Fp );

	attenuation.InitFresnelPolarisingAttenuation(
		A, B, C, S,
		in_dir,
		out_dir,
		surface_normal );
}

inline void Polarising_Torrance_Sparrow_Model (
	const PolarisedLight & in_polarised_light,
	PolarisedLight & out_polarised_light,
	const Vector & in_dir,
	const Vector & out_dir,
	const Vector & surface_normal,
	const double roughness,
	double n1,
	double k1,
	double n2,
	double k2
)
{
	Vector half_vector = out_dir - in_dir;
	half_vector = Normalize( half_vector );

	double mn = Dot( surface_normal, half_vector );
	double theta_fresnel = acos( AbsDot( out_dir, half_vector ) );
	double theta_mn = acos( mn );

	double cos_theta_i = AbsDot( in_dir, surface_normal );
	double cos_theta_o = AbsDot( out_dir, surface_normal );


	// ggx normal distribution and shadowing function
	double D = roughness * roughness * sign_fun( mn ) /
		( M_PI * pow( mn, 4 ) * pow( ( roughness * roughness + tan( theta_mn ) * tan( theta_mn ) ), 2 ) );

	double G = sign_fun( mn / cos( theta_fresnel ) ) * 2 /
		( 1 + sqrt( pow( ( roughness * roughness + tan( theta_mn ) * tan( theta_mn ) ), 2 ) ) );


	PolarisingAttenuation attenuation;

	Polarising_Fresnel_Reflection_Attenuation(
		in_dir, out_dir, surface_normal,
		n1, k1,
		n2, k2,
		attenuation
	);

	attenuation = attenuation * ( D * G / 4 * ( cos_theta_i * cos_theta_o ) );

	out_polarised_light = attenuation * in_polarised_light;
}

