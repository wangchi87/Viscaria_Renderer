/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:18:13
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:18:13
 * @Description: All rights reserved
 */
#pragma once
#include "Core/Global_Settings.h"
#include "Foundations/Assertion.h"
#include "PolarisedLight.h"
#include <cmath>
#include <vector>
#include <array>

#define USE_ARRAY

#define LOOP_OVER_SPECTRUM(_i) \
	for (size_t _i = 0; _i < SPECTRUM_NUM; _i++)

//#define LOOP_OVER_SPECTRUM_VECTOR(p) \
//	vector<double>::iterator p;	\
//	for(p = spec.begin(); p != spec.end(); ++p)

class Spectrum
{
public:

#ifdef USE_ARRAY
	array<double , SPECTRUM_NUM> spec;
#endif

#ifndef USE_ARRAY
	vector<double> spec;
#endif

	// this cause serious bugs in memory allocation 
	//	double *spec;

	PolarisedLight * spec_pol;

	Spectrum ();

	~Spectrum ();

	Spectrum ( const double * value );

	void Init_Spectrum ( const Spectrum & s2 );

	void resetZero ();

	void resetOne ();

	double operator[] ( size_t i ) const;

	// ******* operation on Spectrum ****** 

	Spectrum & operator = ( const Spectrum & s2 );

	Spectrum & operator += ( const Spectrum & s2 );

	Spectrum operator + ( const Spectrum & s2 );

	// ******* operation on numbers ******
	Spectrum & operator *= ( const double f );

	Spectrum & operator /= ( const double f );

	Spectrum operator * ( const double f );

	Spectrum operator + ( const double f );

	Spectrum operator / ( const double f );

	void print ();

	bool isEmpty () const;

	bool HasNaNs ();

	double Length ();

	bool Normalise ();
};

// for non-polarised mode
inline Spectrum Spectrum_Multi ( const Spectrum & s1, const Spectrum & s2 )
{
	Spectrum new_spectrum;
	LOOP_OVER_SPECTRUM ( i )
		new_spectrum.spec[ i ] = s1.spec[ i ] * s2.spec[ i ];
	return new_spectrum;
}

inline void Spectrum_ss_Mul_s ( const Spectrum & s1, const Spectrum & s2, Spectrum & out_spectrum )
{
	LOOP_OVER_SPECTRUM ( i )
		out_spectrum.spec[ i ] = s1.spec[ i ] * s2.spec[ i ];
}

inline void Spectrum_ss_Add_s ( const Spectrum & s1, const Spectrum & s2, Spectrum & out_spectrum )
{
	LOOP_OVER_SPECTRUM ( i )
		out_spectrum.spec[ i ] = s1.spec[ i ] + s2.spec[ i ];
}

inline void Spectrum_s_Init_s ( const Spectrum & s1, Spectrum & out_spectrum )
{
	LOOP_OVER_SPECTRUM ( i )
		out_spectrum.spec[ i ] = s1.spec[ i ];
}

inline void Spectrum_d_Mul_s ( const double f, Spectrum & out_spectrum )
{
	LOOP_OVER_SPECTRUM ( i )
		out_spectrum.spec[ i ] *= f;
}

inline void Spectrum_sd_Mul_s ( const Spectrum & s1, const double f, Spectrum & out_spectrum )
{
	LOOP_OVER_SPECTRUM ( i )
		out_spectrum.spec[ i ] = s1.spec[ i ] * f;
}

inline void Spectrum_sd_Div_s ( const Spectrum & s1, const double f, Spectrum & out_spectrum )
{
	LOOP_OVER_SPECTRUM ( i )
		out_spectrum.spec[ i ] = s1.spec[ i ] / f;
}

// operator += 
inline void Spectrum_s_AddTo_s ( const Spectrum & s1, Spectrum & out_spectrum )
{
	LOOP_OVER_SPECTRUM ( i )
		out_spectrum.spec[ i ] += s1.spec[ i ];
}

// operator *=
inline void Spectrum_s_MulTo_s ( const Spectrum & s1, Spectrum & out_spectrum )
{
	LOOP_OVER_SPECTRUM ( i )
		out_spectrum.spec[ i ] *= s1.spec[ i ];
}

inline void Spectrum_d_Div_s ( const double f, Spectrum & out_spectrum )
{
	LOOP_OVER_SPECTRUM ( i )
		out_spectrum.spec[ i ] /= f;
}

inline void Spectrum_Inverse_Gamma_Correction ( Spectrum & spectrum, float gamma )
{
	LOOP_OVER_SPECTRUM ( i )
		spectrum.spec[ i ] = pow ( spectrum.spec[ i ] , gamma );
}

inline void Spectrum_Gamma_Correction ( Spectrum & spectrum, float gamma )
{
	LOOP_OVER_SPECTRUM ( i )
		spectrum.spec[ i ] = pow( spectrum.spec[ i ], 1/gamma);
}

inline void ASSERT_NAN ( const Spectrum & v )
{
#ifdef ASSERTION_ON

	LOOP_OVER_SPECTRUM ( i )
		if (_isnan (spec[ i ]))
			printf ( "Spectrum has nan" );		
#endif
}


inline void Spectrum_test ()
{
	double s1[ 3 ] = { 1,2,3 };
	double s2[ 3 ] = { 10,20,30 };

	Spectrum spec1_spectrum;
	Spectrum spec2_spectrum( s1 );
	Spectrum spec3_spectrum( s2 );

	spec1_spectrum.print();
	spec2_spectrum.print();

	spec1_spectrum = spec2_spectrum;
	spec1_spectrum.print();

	spec1_spectrum = spec2_spectrum + spec3_spectrum;
	spec1_spectrum.print();

	spec1_spectrum = Spectrum_Multi( spec2_spectrum, spec3_spectrum );
	spec1_spectrum.print();

	spec1_spectrum *= 4;
	spec1_spectrum.print();

	spec1_spectrum /= 4;
	spec1_spectrum.print();


	//	double ss2[ 3 ] = { 1,2,3 };
	//	double ss3[ 3 ] = { 10,20,30 };
	//
	//	Spectrum s1;
	//	Spectrum s2 ( ss2 );
	//	Spectrum s3 ( ss3 );
	//
	//
	//	s1 = s2 + s3;
	//	s1.print ( );

	//	spec1_spectrum.print ( );
	//	spec2_spectrum.print ( );
	//
	//	spec1_spectrum = spec2_spectrum;
	//	spec1_spectrum.print ( );
	//
	//	Spectrum_ss_Add_s ( spec2_spectrum , spec3_spectrum , spec1_spectrum );
	//	spec1_spectrum.print ( );
	//
	//	Spectrum_ss_Mul_s ( spec2_spectrum , spec3_spectrum, spec1_spectrum );
	//	spec1_spectrum.print ( );
	//
	//	Spectrum_d_Mul_s ( 4 , spec1_spectrum );
	//
	//	spec1_spectrum.print ( );
	//
	//	Spectrum_d_Div_s(  4 , spec1_spectrum );
	//
	//	spec1_spectrum.print ( );
	//
	//	Spectrum_s_AddTo_s ( spec1_spectrum , spec2_spectrum );
	//	spec2_spectrum.print ( );
}

