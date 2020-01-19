/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:17:39
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:17:39
 * @Description: All rights reserved
 */
#pragma once
#include "Spectrum.h"
#include "PolarisingAttenuation.h"

class Attenuation
{
public:

	// non-polarising attenuation
	Spectrum attenuation_plain;

	// polarising attenuation
	PolarisingAttenuation * attenuation_pol;

	Attenuation ();
	~Attenuation ();

	void Set_Plain_Attenuation ( const Spectrum & s );

	void Set_Polarising_Attenuation ( const PolarisingAttenuation & pol );

	// this function is very different from "Attenuate_Light_Spectrum"
	void Scaled_by_Spectrum_Value (
		const Spectrum & scale );

	Attenuation operator * ( const double f );

	Attenuation operator / ( const double f );

	Attenuation operator * ( const Attenuation & f );

	Attenuation operator + ( const Attenuation & f );

	void operator *= ( const double f );

	void resetZero ();
	void resetOne ();
	bool isEmpty ();
	void print ();
};

// attenuate a light which is represented as spectrum
inline Spectrum Attenuate_Light_Spectrum (
	const Attenuation & att,
	const Spectrum & light )
{
	Spectrum attnuated_spec;

	if ( POLARISATION_MODE )
	LOOP_OVER_SPECTRUM ( i )
		attnuated_spec.spec_pol[ i ] =
			( *att.attenuation_pol ) * light.spec_pol[ i ];
	else
	LOOP_OVER_SPECTRUM ( i )
		attnuated_spec.spec[ i ] =
			light.spec[ i ] * att.attenuation_plain[ i ];

	return attnuated_spec;
}

inline void Attenuate_Light_Spectrum (
	const Attenuation & att,
	const Spectrum & in_spec,
	Spectrum & out_spec )
{
	if ( POLARISATION_MODE )
	LOOP_OVER_SPECTRUM ( i )
		out_spec.spec_pol[ i ] =
			( *att.attenuation_pol ) * in_spec.spec_pol[ i ];
	else
		Spectrum_ss_Mul_s( att.attenuation_plain, in_spec, out_spec );
}


inline void ASSERT_NAN ( const Attenuation & v )
{
#ifdef ASSERTION_ON

	LOOP_OVER_SPECTRUM ( i )
		if (_isnan ( v.attenuation_plain.spec[ i ] ))
			printf ( "Spectrum has nan" );
#endif
}

