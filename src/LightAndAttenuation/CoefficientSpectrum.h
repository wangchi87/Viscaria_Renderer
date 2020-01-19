/*
 * @Author: Chi Wang
 * @Date: 2018-09-27 16:17:47
 * @LastEditors: Chi Wang
 * @LastEditTime: 2018-09-27 16:17:47
 * @Description: All rights reserved
 */
#pragma once


#define LOOP_OVER_COEFFICIENT_SPECTRUM(_i) for (size_t _i = 0; _i < nSamples; _i++)


template< size_t nSamples >
class CoefficientSpectrum
{
public:
	double spec[ nSamples ];

	// this cause serious bugs in memory allocation 
	//	double *spec;

	//	PolarisedLight *spec_pol;

	CoefficientSpectrum ();

	~CoefficientSpectrum ();

	CoefficientSpectrum ( const double * value );

	void resetZero ();

	void resetOne ();

	double operator[] ( size_t i ) const;

	// ******* operation on Spectrum ****** 

	CoefficientSpectrum operator = ( const CoefficientSpectrum<nSamples> & s2 );

	CoefficientSpectrum operator += ( const CoefficientSpectrum<nSamples> & s2 );

	CoefficientSpectrum operator + ( const CoefficientSpectrum<nSamples> & s2 );

	// ******* operation on numbers ******
	CoefficientSpectrum operator *= ( const double f );

	CoefficientSpectrum operator /= ( const double f );

	CoefficientSpectrum operator * ( const double f );

	CoefficientSpectrum operator / ( const double f );

	void print ();

	bool isEmpty () const;

	bool HasNaNs ();

	double Length ();

	bool Normalise ();
};

template< size_t nSamples >
inline CoefficientSpectrum<nSamples> operator * ( const CoefficientSpectrum<nSamples> & s, const double & f )
{
	CoefficientSpectrum<nSamples> new_spectrum;
	LOOP_OVER_COEFFICIENT_SPECTRUM( i )
		new_spectrum.spec[ i ] = s.spec[ i ] * f;
	return new_spectrum;
}

template< size_t nSamples >
// for non-polarised mode
inline CoefficientSpectrum<nSamples> Spectrum_Multi ( const CoefficientSpectrum<nSamples> & s1, const CoefficientSpectrum<nSamples> & s2 )
{
	CoefficientSpectrum<nSamples> new_spectrum;
	LOOP_OVER_COEFFICIENT_SPECTRUM( i )
		new_spectrum.spec[ i ] = s1[ i ] * s2[ i ];
	return new_spectrum;
}


typedef CoefficientSpectrum<3> RGB;

