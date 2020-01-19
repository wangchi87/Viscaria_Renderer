#include "CoefficientSpectrum.h"
#include <cstdio>


template< size_t nSamples >
CoefficientSpectrum<nSamples>::CoefficientSpectrum ()
{
	//	spec = new double[ SPECTRUM_NUM ];
	LOOP_OVER_COEFFICIENT_SPECTRUM ( i )
		spec[ i ] = 0;

	//	if ( POLARISATION_MODE )
	//		spec_pol = new PolarisedLight[ SPECTRUM_NUM ];
}

template< size_t nSamples >
CoefficientSpectrum<nSamples>::~CoefficientSpectrum ()
{
}

template< size_t nSamples >
CoefficientSpectrum<nSamples>::CoefficientSpectrum ( const double * value )
{
	LOOP_OVER_COEFFICIENT_SPECTRUM ( i )
		spec[ i ] = value[ i ];

	//	if (POLARISATION_MODE)
	//		spec_pol = new PolarisedLight[ SPECTRUM_NUM ];
}


template< size_t nSamples >
void CoefficientSpectrum<nSamples>::resetZero ()
{
	LOOP_OVER_COEFFICIENT_SPECTRUM ( i )
		spec[ i ] = 0;
}

template< size_t nSamples >
void CoefficientSpectrum<nSamples>::resetOne ()
{
	LOOP_OVER_COEFFICIENT_SPECTRUM ( i )
		spec[ i ] = 1;
}

template< size_t nSamples >
double CoefficientSpectrum<nSamples>::operator[] ( size_t i ) const
{
	if ( i < nSamples )
		return spec[ i ];
	else
	{
		printf( "index out of boundary \n" );
		return -1;
	}
}


template< size_t nSamples >
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>
::operator= ( const CoefficientSpectrum<nSamples> & s2 )
{
	LOOP_OVER_COEFFICIENT_SPECTRUM ( i )
		spec[ i ] = s2.spec[ i ];
	return *this;
}

template< size_t nSamples >
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>
::operator+= ( const CoefficientSpectrum<nSamples> & s2 )
{
	LOOP_OVER_COEFFICIENT_SPECTRUM ( i )
		spec[ i ] += s2.spec[ i ];
	return *this;
}

template< size_t nSamples >
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>
::operator+ ( const CoefficientSpectrum<nSamples> & s2 )
{
	CoefficientSpectrum new_spectrum;
	LOOP_OVER_COEFFICIENT_SPECTRUM ( i )
		new_spectrum.spec[ i ] = this->spec[ i ] + s2.spec[ i ];
	return new_spectrum;
}

template< size_t nSamples >
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>
::operator*= ( const double f )
{
	LOOP_OVER_COEFFICIENT_SPECTRUM ( i )
		spec[ i ] *= f;
	return *this;
}

template< size_t nSamples >
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>
::operator/= ( const double f )
{
	LOOP_OVER_COEFFICIENT_SPECTRUM ( i )
		spec[ i ] /= f;
	return *this;
}

template< size_t nSamples >
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>
::operator* ( const double f )
{
	CoefficientSpectrum new_spectrum;
	LOOP_OVER_COEFFICIENT_SPECTRUM ( i )
		new_spectrum.spec[ i ] = this->spec[ i ] * f;
	return new_spectrum;
}

template< size_t nSamples >
CoefficientSpectrum<nSamples> CoefficientSpectrum<nSamples>
::operator/ ( const double f )
{
	CoefficientSpectrum new_spectrum;
	LOOP_OVER_COEFFICIENT_SPECTRUM ( i )
		new_spectrum.spec[ i ] = this->spec[ i ] / f;
	return new_spectrum;
}

template< size_t nSamples >
void CoefficientSpectrum<nSamples>::print ()
{
	LOOP_OVER_COEFFICIENT_SPECTRUM ( i )
		printf( "%f ", spec[ i ] );
	printf( "\n" );
}

template< size_t nSamples >
bool CoefficientSpectrum<nSamples>::isEmpty () const
{
	LOOP_OVER_COEFFICIENT_SPECTRUM ( i )
		if ( spec[ i ] == 0 )
			return true;
	return false;
}

template< size_t nSamples >
bool CoefficientSpectrum<nSamples>::HasNaNs ()
{
	LOOP_OVER_COEFFICIENT_SPECTRUM ( i )
		if ( _isnan( spec[ i ] ) )
			return true;
	return false;
}

template< size_t nSamples >
double CoefficientSpectrum<nSamples>::Length ()
{
	double length = 0;
	LOOP_OVER_COEFFICIENT_SPECTRUM ( i )
		length += spec[ i ] * spec[ i ];
	length = sqrt( length );
	return length;
}

template< size_t nSamples >
bool CoefficientSpectrum<nSamples>::Normalise ()
{
	double length = Length();
	LOOP_OVER_COEFFICIENT_SPECTRUM ( i )
		spec[ i ] /= length;
	return false;
}

